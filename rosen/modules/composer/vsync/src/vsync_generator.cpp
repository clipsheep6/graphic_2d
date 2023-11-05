/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vsync_generator.h"
#include <cstdint>
#include <mutex>
#include <scoped_bytrace.h>
#include <sched.h>
#include <sys/resource.h>
#include <string>
#include "vsync_log.h"

namespace OHOS {
namespace Rosen {
namespace impl {
namespace {
static int64_t GetSysTimeNs()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

// 1.5ms
constexpr int64_t maxWaleupDelay = 1500000;
constexpr int32_t THREAD_PRIORTY = -6;
constexpr int32_t SCHED_PRIORITY = 2;
constexpr int64_t errorThreshold = 500000;
constexpr int32_t MAX_REFRESHRATE_DEVIATION = 5; // ±5Hz

static void SetThreadHighPriority()
{
    setpriority(PRIO_PROCESS, 0, THREAD_PRIORTY);
    struct sched_param param = {0};
    param.sched_priority = SCHED_PRIORITY;
    sched_setscheduler(0, SCHED_FIFO, &param);
}
}

std::once_flag VSyncGenerator::createFlag_;
sptr<OHOS::Rosen::VSyncGenerator> VSyncGenerator::instance_ = nullptr;

sptr<OHOS::Rosen::VSyncGenerator> VSyncGenerator::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        auto vsyncGenerator = new VSyncGenerator();
        instance_ = vsyncGenerator;
    });

    return instance_;
}

void VSyncGenerator::DeleteInstance() noexcept
{
    instance_ = nullptr;
}

VSyncGenerator::VSyncGenerator()
    : period_(0), phase_(0), referenceTime_(0), wakeupDelay_(0),
      pulse_(0), currRefreshRate_(0), referenceTimeOffset_(0), referenceTimeOffsetPulseNum_(8) // default 8
{
    vsyncThreadRunning_ = true;
    thread_ = std::thread(std::bind(&VSyncGenerator::ThreadLoop, this));
    pthread_setname_np(thread_.native_handle(), "VSyncGenerator");
}

VSyncGenerator::~VSyncGenerator()
{
    {
        std::unique_lock<std::mutex> locker(mutex_);
        vsyncThreadRunning_ = false;
    }
    if (thread_.joinable()) {
        con_.notify_all();
        thread_.join();
    }
}

void VSyncGenerator::ThreadLoop()
{
    // set thread priorty
    SetThreadHighPriority();

    int64_t occurTimestamp = 0;
    int64_t nextTimeStamp = 0;
    int64_t occurReferenceTime = 0;
    while (vsyncThreadRunning_ == true) {
        std::vector<Listener> listeners;
        {
            std::unique_lock<std::mutex> locker(mutex_);
            UpdateVSyncModeLocked();
            occurReferenceTime = referenceTime_;
            if (period_ == 0) {
                if (vsyncThreadRunning_ == true) {
                    con_.wait(locker);
                }
                continue;
            }
            occurTimestamp = GetSysTimeNs();
            nextTimeStamp = ComputeNextVSyncTimeStamp(occurTimestamp, occurReferenceTime);
            if (nextTimeStamp == INT64_MAX) {
                if (vsyncThreadRunning_ == true) {
                    con_.wait(locker);
                }
                continue;
            } else if (vsyncMode_ == VSYNC_MODE_LTPO) {
                bool isTimingCorrect = CheckTimingCorrect(occurTimestamp, occurReferenceTime, nextTimeStamp);
                if (isTimingCorrect) {
                    bool modelChanged = UpdateChangeDataLocked(nextTimeStamp);
                    if (modelChanged) {
                        continue;
                    }
                }
            }
        }

        bool isWakeup = false;
        if (occurTimestamp < nextTimeStamp) {
            std::unique_lock<std::mutex> lck(waitForTimeoutMtx_);
            auto err = waitForTimeoutCon_.wait_for(lck, std::chrono::nanoseconds(nextTimeStamp - occurTimestamp));
            if (err == std::cv_status::timeout) {
                isWakeup = true;
            } else {
                ScopedBytrace func("VSyncGenerator::ThreadLoop::Continue");
                continue;
            }
        }
        {
            std::unique_lock<std::mutex> locker(mutex_);
            int64_t newOccurTimestamp = GetSysTimeNs();
            if (isWakeup) {
                UpdateWakeupDelay(newOccurTimestamp, nextTimeStamp);
            }
            if (vsyncMode_ == VSYNC_MODE_LTPO) {
                listeners = GetListenerTimeouted(occurTimestamp, occurReferenceTime);
            } else {
                listeners = GetListenerTimeouted(newOccurTimestamp, occurReferenceTime);
            }
        }
        ScopedDebugTrace func("GenerateVsyncCount:" + std::to_string(listeners.size()) +
                              ", period:" + std::to_string(period_) +
                              ", currRefreshRate_:" + std::to_string(currRefreshRate_) +
                              ", vsyncMode_:" + std::to_string(vsyncMode_));
        for (uint32_t i = 0; i < listeners.size(); i++) {
            listeners[i].callback_->OnVSyncEvent(listeners[i].lastTime_, period_, currRefreshRate_, vsyncMode_);
        }
    }
}

bool VSyncGenerator::ChangeListenerOffsetInternal()
{
    if (changingPhaseOffset_.cb == nullptr) {
        return true;
    }
    auto it = listeners_.begin();
    for (; it < listeners_.end(); it++) {
        if (it->callback_ == changingPhaseOffset_.cb) {
            break;
        }
    }
    int64_t phaseOffset = pulse_ * changingPhaseOffset_.phaseByPulseNum;
    if (it != listeners_.end()) {
        it->phase_ = phaseOffset;
    }

    it = listenersRecord_.begin();
    for (; it < listenersRecord_.end(); it++) {
        if (it->callback_ == changingPhaseOffset_.cb) {
            break;
        }
    }
    if (it == listenersRecord_.end()) {
        return false;
    }
    it->callback_->OnPhaseOffsetChanged(phaseOffset);
    changingPhaseOffset_ = {}; // reset
    return true;
}

bool VSyncGenerator::ChangeListenerRefreshRatesInternal()
{
    if (changingRefreshRates_.cb == nullptr) {
        return true;
    }
    auto it = listenersRecord_.begin();
    for (; it < listenersRecord_.end(); it++) {
        if (it->callback_ == changingRefreshRates_.cb) {
            break;
        }
    }
    if (it == listenersRecord_.end()) {
        return false;
    }
    it->callback_->OnConnsRefreshRateChanged(changingRefreshRates_.refreshRates);
    // reset
    changingRefreshRates_.cb = nullptr;
    changingRefreshRates_.refreshRates.clear();
    changingRefreshRates_ = {};
    return true;
}

void VSyncGenerator::UpdateWakeupDelay(int64_t occurTimestamp, int64_t nextTimeStamp)
{
    // 63, 1 / 64
    wakeupDelay_ = ((wakeupDelay_ * 63) + (occurTimestamp - nextTimeStamp)) / 64;
    wakeupDelay_ = wakeupDelay_ > maxWaleupDelay ? maxWaleupDelay : wakeupDelay_;
}

int64_t VSyncGenerator::ComputeNextVSyncTimeStamp(int64_t now, int64_t referenceTime)
{
    int64_t nextVSyncTime = INT64_MAX;
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], now, referenceTime);
        if (t < nextVSyncTime) {
            nextVSyncTime = t;
        }
    }

    return nextVSyncTime;
}

bool VSyncGenerator::CheckTimingCorrect(int64_t now, int64_t referenceTime, int64_t nextVSyncTime)
{
    bool isTimingCorrect = false;
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], now, referenceTime);
        if ((t - nextVSyncTime < errorThreshold) && (listeners_[i].phase_ == 0)) {
            isTimingCorrect = true;
        }
    }
    return isTimingCorrect;
}

bool VSyncGenerator::UpdateChangeDataLocked(int64_t nextVSyncTime)
{
    bool modelChanged = false;

    // update generate refreshRate
    if (needChangeGeneratorRefreshRate_) {
        currRefreshRate_ = changingGeneratorRefreshRate_;
        period_ = pulse_ * (VSYNC_MAX_REFRESHRATE / currRefreshRate_);
        referenceTime_ = nextVSyncTime;
        changingGeneratorRefreshRate_ = 0; // reset
        needChangeGeneratorRefreshRate_ = false;
        modelChanged = true;
    }

    // update phaseOffset
    if (needChangePhaseOffset_) {
        bool offsetChangedSucceed = ChangeListenerOffsetInternal();
        if (offsetChangedSucceed) {
            needChangePhaseOffset_ = false;
            modelChanged = true;
        }
    }

    // update VSyncConnections refreshRates
    if (needChangeRefreshRates_) {
        bool refreshRatesChangedSucceed = ChangeListenerRefreshRatesInternal();
        if (refreshRatesChangedSucceed) {
            needChangeRefreshRates_ = false;
            modelChanged = true;
        }
    }

    return modelChanged;
}

void VSyncGenerator::UpdateVSyncModeLocked()
{
    if (pendingVsyncMode_ != VSYNC_MODE_INVALID) {
        vsyncMode_ = pendingVsyncMode_;
        pendingVsyncMode_ = VSYNC_MODE_INVALID;
    }
}

int64_t VSyncGenerator::ComputeListenerNextVSyncTimeStamp(const Listener& listener, int64_t now, int64_t referenceTime)
{
    int64_t lastVSyncTime = listener.lastTime_ + wakeupDelay_;
    if (now < lastVSyncTime) {
        now = lastVSyncTime;
    }

    now -= referenceTime;
    int64_t phase = phase_ + listener.phase_;
    now -= phase;
    if (now < 0) {
        now = -period_;
    }
    int64_t numPeriod = now / period_;
    int64_t nextTime = (numPeriod + 1) * period_ + phase;
    nextTime += referenceTime;

    // 3 / 5 just empirical value
    if ((vsyncMode_ == VSYNC_MODE_LTPS) && (nextTime - listener.lastTime_ < (3 * period_ / 5))) {
        nextTime += period_;
    }

    nextTime -= wakeupDelay_;
    return nextTime;
}

std::vector<VSyncGenerator::Listener> VSyncGenerator::GetListenerTimeouted(int64_t now, int64_t referenceTime)
{
    std::vector<VSyncGenerator::Listener> ret;
    int64_t nowTimestamp = now;
    if (vsyncMode_ == VSYNC_MODE_LTPS) {
        nowTimestamp -= period_;
    }

    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], nowTimestamp, referenceTime);
        if (t - GetSysTimeNs() < errorThreshold) {
            listeners_[i].lastTime_ = t;
            ret.push_back(listeners_[i]);
        }
    }
    return ret;
}

VsyncError VSyncGenerator::UpdateMode(int64_t period, int64_t phase, int64_t referenceTime)
{
    if (period < 0 || referenceTime < 0) {
        VLOGE("wrong parameter, period:" VPUBI64 ", referenceTime:" VPUBI64, period, referenceTime);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    phase_ = phase;
    if ((pendingVsyncMode_ == VSYNC_MODE_LTPO) || (vsyncMode_ == VSYNC_MODE_LTPO)) {
        int32_t refreshRate = JudgeRefreshRateLocked(period);
        if (refreshRate == 0) {
            return VSYNC_ERROR_NOT_SUPPORT;
        }
        referenceTimeOffset_ = referenceTimeOffsetPulseNum_ * pulse_;
        int64_t pendingReferenceTime = referenceTime - referenceTimeOffset_;
        if ((pendingReferenceTime >= referenceTime_) && ((currRefreshRate_ == refreshRate) || currRefreshRate_ == 0)) {
            referenceTime_ = pendingReferenceTime;
            period_ = period != 0 ? period : period_;
        }
    } else {
        referenceTime_ = referenceTime;
        period_ = period != 0 ? period : period_;
    }
    con_.notify_all();
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::AddListener(int64_t phase, const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    Listener listener;
    listener.phase_ = phase;
    listener.callback_ = cb;
    // just correct period / 2 time
    listener.lastTime_ = GetSysTimeNs() - period_ / 2 + phase_;

    listeners_.push_back(listener);

    int i = 0;
    for (; i < listenersRecord_.size(); i++) {
        if (listener.callback_ == listenersRecord_[i].callback_) {
            break;
        }
    }
    if (i == listenersRecord_.size()) {
        listenersRecord_.push_back(listener);
    }
    con_.notify_all();
    waitForTimeoutCon_.notify_all();
    return VSYNC_ERROR_OK;
}

int32_t VSyncGenerator::JudgeRefreshRateLocked(int64_t period)
{
    if (period <= 0) {
        return 0;
    }
    int32_t actualRefreshRate = round(1.0/((double)period/1000000000.0)); // 1.0s == 1000000000.0ns
    int32_t refreshRate = actualRefreshRate;
    int32_t diff = 0;
    // 在actualRefreshRate附近找一个能被VSYNC_MAX_REFRESHRATE整除的刷新率作为训练pulse的参考刷新率
    while ((abs(refreshRate - actualRefreshRate) < MAX_REFRESHRATE_DEVIATION) &&
           (VSYNC_MAX_REFRESHRATE % refreshRate != 0)) {
        if (diff < 0) {
            diff = -diff;
        } else {
            diff = -diff - 1;
        }
        refreshRate = actualRefreshRate + diff;
    }
    if (VSYNC_MAX_REFRESHRATE % refreshRate != 0) {
        VLOGE("Not Support this refresh rate: %{public}d, update pulse failed.", actualRefreshRate);
        return 0;
    }
    pulse_ = period / (VSYNC_MAX_REFRESHRATE / refreshRate);
    return refreshRate;
}

VsyncError VSyncGenerator::ChangeGeneratorRefreshRateModel(const ListenerRefreshRateData &listenerRefreshRates,
                                                           const ListenerPhaseOffsetData &listenerPhaseOffset,
                                                           uint32_t generatorRefreshRate)
{
    ScopedBytrace func("ChangeGeneratorRefreshRateModel:" + std::to_string(generatorRefreshRate));
    std::lock_guard<std::mutex> locker(mutex_);
    if (vsyncMode_ != VSYNC_MODE_LTPO) {
        return VSYNC_ERROR_NOT_SUPPORT;
    }
    if (pulse_ == 0) {
        VLOGE("pulse is not ready!!!");
        return VSYNC_ERROR_API_FAILED;
    }
    if ((generatorRefreshRate <= 0 || (VSYNC_MAX_REFRESHRATE % generatorRefreshRate != 0))) {
        VLOGE("Not support this refresh rate: %{public}u", generatorRefreshRate);
        return VSYNC_ERROR_NOT_SUPPORT;
    }

    changingRefreshRates_ = listenerRefreshRates;
    needChangeRefreshRates_ = true;

    changingPhaseOffset_ = listenerPhaseOffset;
    needChangePhaseOffset_ = true;

    changingGeneratorRefreshRate_ = generatorRefreshRate;
    needChangeGeneratorRefreshRate_ = true;

    waitForTimeoutCon_.notify_all();
    return VSYNC_ERROR_OK;
}

int64_t VSyncGenerator::GetVSyncPulse()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return pulse_;
}

int64_t VSyncGenerator::GetReferenceTimeOffset()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return referenceTimeOffset_;
}

VsyncError VSyncGenerator::SetVSyncMode(VSyncMode vsyncMode)
{
    ScopedBytrace func("SetVSyncMode:" + std::to_string(vsyncMode));
    std::lock_guard<std::mutex> locker(mutex_);
    pendingVsyncMode_ = vsyncMode;
    return VSYNC_ERROR_OK;
}

VSyncMode VSyncGenerator::GetVSyncMode()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return vsyncMode_;
}

VsyncError VSyncGenerator::SetVSyncPhaseByPulseNum(int32_t phaseByPulseNum)
{
    std::lock_guard<std::mutex> locker(mutex_);
    referenceTimeOffsetPulseNum_ = phaseByPulseNum;
    referenceTimeOffset_ = phaseByPulseNum * pulse_;
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::RemoveListener(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb)
{
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    bool removeFlag = false;
    auto it = listeners_.begin();
    for (; it < listeners_.end(); it++) {
        if (it->callback_ == cb) {
            listeners_.erase(it);
            removeFlag = true;
            break;
        }
    }
    if (!removeFlag) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    con_.notify_all();
    return VSYNC_ERROR_OK;
}

VsyncError VSyncGenerator::ChangePhaseOffset(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb, int64_t offset)
{
    std::lock_guard<std::mutex> locker(mutex_);
    if (cb == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    auto it = listeners_.begin();
    for (; it < listeners_.end(); it++) {
        if (it->callback_ == cb) {
            break;
        }
    }
    if (it != listeners_.end()) {
        it->phase_ = offset;
    } else {
        return VSYNC_ERROR_INVALID_OPERATING;
    }
    return VSYNC_ERROR_OK;
}

bool VSyncGenerator::IsEnable()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return period_ > 0;
}
} // namespace impl
sptr<VSyncGenerator> CreateVSyncGenerator()
{
    return impl::VSyncGenerator::GetInstance();
}

void DestroyVSyncGenerator()
{
    impl::VSyncGenerator::DeleteInstance();
}
}
}
