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
#include <scoped_bytrace.h>
#include <sched.h>
#include <sys/resource.h>
#include <string>
#include "vsync_log.h"
#include "vsync_type.h"
#include "vsync_sampler.h"

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
    : period_(0), phase_(0), referenceTime_(0), wakeupDelay_(0), pulse_(0), refreshRate_(0),
      shouldUpdateVsyncInfo_(false), referenceTimeOffset_(0)
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
    int64_t occurRefrenceTime = 0;
    while (vsyncThreadRunning_ == true) {
        std::vector<Listener> listeners;
        {
            std::unique_lock<std::mutex> locker(mutex_);
            occurRefrenceTime = referenceTime_;
            if (period_ == 0) {
                if (vsyncThreadRunning_ == true) {
                    con_.wait(locker);
                }
                continue;
            }
            occurTimestamp = GetSysTimeNs();
            nextTimeStamp = ComputeNextVSyncTimeStamp(occurTimestamp, occurRefrenceTime);
            if (nextTimeStamp == INT64_MAX) {
                if (vsyncThreadRunning_ == true) {
                    con_.wait(locker);
                }
                continue;
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
            occurTimestamp = GetSysTimeNs();
            if (isWakeup) {
                UpdateWakeupDelay(occurTimestamp, nextTimeStamp);
            }
            if (shouldUpdateVsyncInfo_) {
                ScopedBytrace shouldUpdateVsyncInfo("shouldUpdateVsyncInfo_ listeners.size = " + std::to_string(listeners.size()));
                period_ = pulse_ * (VSYNC_MAX_REFRESHRATE / refreshRate_);
                referenceTime_ = occurTimestamp - period_;
                shouldUpdateVsyncInfo_ = false;
            }
            listeners = GetListenerTimeouted(occurTimestamp, referenceTime_);
        }
        ScopedBytrace func(
            "GenerateVsyncCount:" + std::to_string(listeners.size()) + ", period:" + std::to_string(period_));
        // VLOGE("sgbdebug period_ = " VPUBI64, period_);
        for (uint32_t i = 0; i < listeners.size(); i++) {
            listeners[i].callback_->OnVSyncEvent(listeners[i].lastTime_, period_, refreshRate_);
        }
    }
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

int64_t VSyncGenerator::ComputeListenerNextVSyncTimeStamp(const Listener& listener, int64_t now, int64_t referenceTime)
{
    ScopedBytrace phase_trace1("ComputeListenerNextVSyncTimeStamp now:" + std::to_string(now) + ",referenceTime:" + std::to_string(referenceTime) + ",period_:" + std::to_string(period_));
    int64_t lastVSyncTime = listener.lastTime_ + wakeupDelay_;
    if (now < lastVSyncTime) {
        ScopedBytrace lasttime("lastVSyncTime:" + std::to_string(lastVSyncTime));
        now = lastVSyncTime;
    }

    now -= referenceTime;
    int64_t phase = phase_ + listener.phase_;
    now -= phase;
    if (now < 0) {
        now -= period_;
    }
    int64_t numPeriod = now / period_;
    int64_t nextTime = (numPeriod + 1) * period_ + phase;
    nextTime += referenceTime;

    // 3 / 5 just empirical value
    if (nextTime - listener.lastTime_ < (1 * period_ / 5)) {
        nextTime += period_;
    }

    nextTime -= wakeupDelay_;
    ScopedBytrace phase_trace2("ComputeListenerNextVSyncTimeStamp phase:" + std::to_string(phase) + ",nextTime:" + std::to_string(nextTime));
    return nextTime;
}

std::vector<VSyncGenerator::Listener> VSyncGenerator::GetListenerTimeouted(int64_t now, int64_t referenceTime)
{
    std::vector<VSyncGenerator::Listener> ret;
    int64_t onePeriodAgo = now - period_;

    ScopedBytrace func("GetListenerTimeouted now:" + std::to_string(now) + ",onePeriodAgo:" + std::to_string(onePeriodAgo));
    for (uint32_t i = 0; i < listeners_.size(); i++) {
        int64_t t = ComputeListenerNextVSyncTimeStamp(listeners_[i], onePeriodAgo, referenceTime);
        if (t < now || (t - now < errorThreshold)) {
            listeners_[i].lastTime_ = t;
            ret.push_back(listeners_[i]);
        }
    }
    return ret;
}

VsyncError VSyncGenerator::UpdateMode(int64_t period, int64_t phase, int64_t referenceTime)
{
    VLOGE("VSyncGenerator::UpdateMode period = " VPUBI64 ", phase = " VPUBI64, period, phase);
    std::lock_guard<std::mutex> locker(mutex_);
    if (period > 0) {
        period_ = period;
        UpdatePulseLocked(period);
    }
    phase_ = phase;
    if (referenceTime > 0) {
        // TODO：这里暂时按LTPO屏幕的方式校准，后续补充区分LTPO的屏幕和LTPS的屏幕的方式
        // 按pulse整数倍的偏移来校准generator的referenceTime_
        if (pulse_ > 0) {
            referenceTimeOffset_ = round((double)((referenceTime - referenceTime_) % period) / pulse_) * pulse_;
            referenceTime_ = referenceTime - referenceTimeOffset_;
        } else {
            referenceTime_ = referenceTime;
        }
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
    con_.notify_all();
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

void VSyncGenerator::UpdatePulseLocked(int64_t period)
{
    int32_t actualRefreshRate = round(1.0/((double)period/1000000000.0)); // 1.0s == 1000000000.0ns
    VLOGE("VSyncGenerator::UpdatePulseLocked actualRefreshRate = %{public}d", actualRefreshRate);
    int32_t refreshRate = actualRefreshRate;
    int32_t diff = 0;
    // 在actualRefreshRate附近找到一个能被VSYNC_MAX_REFRESHRATE整除的刷新率作为训练pulse的参考刷新率
    while ((abs(refreshRate - actualRefreshRate) < MAX_REFRESHRATE_DEVIATION) &&
           (VSYNC_MAX_REFRESHRATE % refreshRate != 0)) {
        if (diff < 0) {
            diff = -diff;
        } else {
            diff = -diff - 1;
        }
        refreshRate = actualRefreshRate + diff;
        // VLOGE("VSyncGenerator::UpdatePulseLocked diff = %{public}d, refreshRate:%{public}d, actualRefreshRate:%{public}d, (VSYNC_MAX_REFRESHRATE %% refreshRate):%{public}d, period:" VPUBI64, diff, refreshRate, actualRefreshRate, (VSYNC_MAX_REFRESHRATE % refreshRate), period);
    }
    // VLOGE("VSyncGenerator::UpdatePulseLocked VSYNC_MAX_REFRESHRATE = %{public}d, refreshRate = %{public}d, (VSYNC_MAX_REFRESHRATE %% refreshRate) = %{public}d", VSYNC_MAX_REFRESHRATE, refreshRate, (VSYNC_MAX_REFRESHRATE % refreshRate));
    if (VSYNC_MAX_REFRESHRATE % refreshRate != 0) {
        VLOGE("VSyncGenerator::UpdatePulseLocked Recognize an unsupported refresh rate: %{public}d, update pulse failed.", actualRefreshRate);
        return;
    }
    pulse_ = period / (VSYNC_MAX_REFRESHRATE / refreshRate);
    {
        // 校准时不更新刷新率和周期，仅调试用
        if (refreshRate_ <= 0) {
            refreshRate_ = refreshRate;
        }
        period_ = pulse_ * (VSYNC_MAX_REFRESHRATE / refreshRate_);
    }
    // VLOGE("VSyncGenerator::UpdatePulseLocked pulse_ = " VPUBI64, pulse_);
}

VsyncError VSyncGenerator::SetGeneratorRefreshRate(int32_t refreshRate)
{
    ScopedBytrace func("VSyncGenerator::SetGeneratorRefreshRate:" + std::to_string(refreshRate));
    std::lock_guard<std::mutex> locker(mutex_);
    if (pulse_ == 0) {
        VLOGE("pulse is not ready!!!");
        return VSYNC_ERROR_API_FAILED;
    }
    if (VSYNC_MAX_REFRESHRATE % refreshRate != 0) {
        VLOGE("not support this refresh rate: %{public}d", refreshRate);
        return VSYNC_ERROR_NOT_SUPPORT;
    }
    // period_ = pulse_ * (VSYNC_MAX_REFRESHRATE / refreshRate); // 改为不立刻更新周期，只更新刷新率，通过shouldUpdateVsyncInfo_变量通知分发线程去修改period_
    refreshRate_ = refreshRate;
    shouldUpdateVsyncInfo_ = true;
    // 清空fenceTime
    CreateVSyncSampler()->ResetErrorLocked();
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
