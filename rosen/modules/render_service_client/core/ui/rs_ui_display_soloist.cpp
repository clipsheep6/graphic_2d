/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ui/rs_ui_display_soloist.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

using namespace OHOS::Rosen;

// class RSC_EXPORT SoloistId

std::shared_ptr<SoloistId> SoloistId::Create()
{
    std::shared_ptr<SoloistId> soloistId = std::make_shared<SoloistId>();
    return soloistId;
}

SoloistIdType SoloistId::GenerateId()
{
    static std::atomic<SoloistIdType> currentId_ = 0;
    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    return currentId;
}

SoloistIdType SoloistId::GetId() const
{
    return id_;
}

SoloistId::SoloistId() : id_(GenerateId) {}

SoloistId::~SoloistId() {}

// class RSC_EXPORT RSDisplaySoloist

void RSDisplaySoloist::OnVsync(TimestampType timestamp, void* client)
{
    auto soloist = static_cast<RSDisplaySoloist*>(client);
    if (soloist && soloist->useExclusiveThread_ &&
        subStatus_ == ActiveStatus::ACTIVE) {
        soloist->VsyncCallbackInner(timestamp);
    }
}

void RSDisplaySoloist::VsyncCallbackInner(TimestampType timestamp)
{
    if (subStatus_ != ActiveStatus::ACTIVE) {
        SetFrameRateLinkerEnable(false);
    }

    if (JudgeWhetherSkip(timestamp)) {
        if (callback_.first) {
            callback_.first(timestamp_, targetTimestamp_, callback_.second);
        }
    }

    if (callback_.first) {
        RequestNextVSync();
    }
    FlushFrameRate(frameRateRange_.preferred_);
}

void RSDisplaySoloist::RequestNextVSync()
{
    if (!subReceiver_ || !hasInitVsyncReceiver_) {
        if (!subVsyncHandler_) {
            subVsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(
                AppExecFwk::EventRunner::Create(std::string(instanceId_)+"_SubDisplaySoloist"));
        }
        auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
        frameRateLinker_ = OHOS::Rosen::RSFrameRateLinker::Create();
        while (!subReceiver_) {
            subReceiver_ = rsClient.CreateVSyncReceiver(std::to_string(instanceId_)+"_SubDisplaySoloist",
                frameRateLinker_->GetId(), subVsyncHandler_);
        }
        subReceiver_->Init();
    }

    if (subReceiver_) {
        subReceiver_->RequestNextVSync();
    }

    return;
}

void RSDisplaySoloist::FlushFrameRate(int32_t rate)
{
    if (frameRateLinker_ && frameRateLinker_->IsEnable()) {
        FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, rate};
        frameRateLinker_->UpdateFrameRateRangeImme(range);
    }
}

void RSDisplaySoloist::SetSubFrameRateLinkerEnable(bool enabled)
{
    if (frameRateLinker_) {
        if (!enabled) {
            FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, 0};
            frameRateLinker_->UpdateFrameRateRangeImme(range);
        }
        frameRateLinker_->SetEnable(enabled);
    }
}

bool RSDisplaySoloist::JudgeWhetherSkip(TimestampType timestamp)
{
    int32_t vsyncRate = GetVSyncRate();
    drawFPS_ = FindMatchedRefreshRate(vsyncRate, frameRateLinker_.preferred_);
    if (drawFPS_ < frameRateRange_.min_ || drawFPS_ > frameRateRange_.max_) {
        drawFPS_ = SearchMatchedRate(frameRateRange_, vsyncRate);
    }

    if (drawFPS_ == 0) {
        return false;
    }

    int32_t curRate = vsyncRate / drawFPS_;
    int64_t curVsyncPeriod = GetVSyncPeriod();
    timestamp_ = timestamp;
    targetTimestamp_ = timestamp + curRate * curVsyncPeriod;

    if (curRate != curRate_) {
        curCnt_ = 0;
    }
    curRate_ = curRate;

    bool isSkip = false;
    if (curCnt_ == 0) {
        isSkip = true;
    }

    if (curRate_ && (curRate_ - curCnt_) == 1) {
        curCnt_ = -1;
    }
    curCnt_++;

    return isSkip;
}

bool RSDisplaySoloist::IsCommonDivisor(int32_t expectedRate, int32_t vsyncRate)
{
    if (expectedRate == 0 || vsyncRate == 0) {
        return false;
    }

    int32_t n =  vsyncRate / expectedRate;
    if (expectedRate * n == vsyncRate) {
        return true;
    }
    return false;
}

std::vector<int32_t> RSDisplaySoloist::FindRefreshRateFactors(int32_t refreshRate)
{
    std::vector<int32_t> refreshRateFactors;
    for (int32_t i = 1; i * i <= refreshRate; ++i) {
        if (refreshRate % i == 0) {
            refreshRateFactors.emplace_back(i);
            if (i != refreshRate / i) {
                refreshRateFactors.emplace_back(refreshRate / i);
            }
        }
    }
    sort(refreshRateFactors.begin(), refreshRateFactors.end());
    return refreshRateFactors;
}

void RSDisplaySoloist::FindAllRefreshRateFactors()
{
    std::set<int32_t> allFactors;
    for (const auto& refreshRate : REFRESH_RATE_LIST) {
        std::vector<int32_t> factors = FindRefreshRateFactors(refreshRate);
        allFactors.insert(factors.begin(), factors.end());
    }
    REFRESH_RATE_FACTORS.clear();
    std::copy(allFactors.begin, allFactors.end(), std::back_inserter(REFRESH_RATE_FACTORS));
    return;
}

int32_t RSDisplaySoloist::FindAccurateRefreshRate(int32_t approximateRate)
{
    if (REFRESH_RATE_FACTORS.empty()) {
        ROSEN_LOGE("%{public}s REFRESH_RATE_FACTORS is Empty.", __func__);
        return 0;
    }
    auto it = std::lower_bound(REFRESH_RATE_FACTORS.begin(), REFRESH_RATE_FACTORS.end(), approximateRate);
    if (it == REFRESH_RATE_FACTORS.begin()) {
        return *it;
    } else if (it == REFRESH_RATE_FACTORS.end()) {
        return *(it - 1);
    }
    return std::abs(*it - targetRate) < std::abs(*(it - 1) - targetRate) ? *it : *(it - 1);
}

int32_t RSDisplaySoloist::FindMatchedRefreshRate(int32_t vsyncRate, int32_t targetRate)
{
    if (targetRate == 0 || targetRate > vsyncRate) {
        return vsyncRate;
    }

    if (IsCommonDivisor(targetRate, vsyncRate)) {
        return targetRate;
    }

    if (!RATE_TO_FACTORS.count(vsyncRate)) {
        RATE_TO_FACTORS[vsyncRate] = FindRefreshRateFactors(vsyncRate);
    }

    std::vector<int32_t> refreshRateFactors = RATE_TO_FACTORS[vsyncRate];
    if (refreshRateFactors.empty()) {
        return 0;
    }
    auto it = std::lower_bound(refreshRateFactors.begin(), refreshRateFactors.end(), targetRate);
    if (it == refreshRateFactors.begin()) {
        return *it;
    } else if (it == refreshRateFactors.end()) {
        return *(it - 1);
    }
    return std::abs(*it - targetRate) < std::abs(*(it - 1) - targetRate) ? *it : *(it - 1);
}

int32_t RSDisplaySoloist::SearchMatchedRate(const FrameRateRange& frameRateRange,
    int32_t vsyncRate, int32_t iterCount)
{
    if (vsyncRate != 0 && iterCount >= vsyncRate) {
        return FindMatchedRefreshRate(vsyncRate, frameRateRange.preferred_);
    }

    if (iterCount == 0 || vsyncRate == 0) {
        return vsyncRate;
    }

    int32_t expectedRate = vsyncRate / iterCount;
    if (frameRateRange.min_ <= expectedRate &&
        frameRateRange.max_ >= expectedRate) {
        return FindMatchedRefreshRate(vsyncRate, expectedRate);
    }

    return SearchMatchedRate(frameRateRange, vsyncRate, ++iterCount);
}

int64_t RSDisplaySoloist::GetVSyncPeriod()
{
    int64_t period = 0;
    if (subReceiver_) {
        subReceiver_->GetVSyncPeriod(period);
    }

    return period;
}

int32_t RSDisplaySoloist::GetVSyncRate()
{
    int64_t vsyncPeriod = GetVSyncPeriod();
    if (vsyncPeriod == 0) {
        RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
        vsyncPeriod = soloistManager.GetVSyncPeriod();
    }
    int32_t approximateRate = static_cast<int32_t>(std::ceil((1.0f * 1e9) / vsyncPeriod));
    std::call_once(COMPUTE_FACTORS_FLAG, [this] () { FindAllRefreshRateFactors(); });
    int32_t rate = FindAccurateRefreshRate(approximateRate);
    SetVSyncRate(rate);
    return rate;
}

bool RSDisplaySoloist::SetVSyncRate(int32_t vsyncRate)
{
    if (vsyncRate < 0) {
        return false;
    }

    if (sourceVsyncRate_ == vsyncRate) {
        return false;
    }
    sourceVsyncRate_ = vsyncRate;
    return true;
}

// RSDisplaySoloistManager

RSDisplaySoloistManager& RSDisplaySoloistManager::GetInstance() noexcept
{
    static RSDisplaySoloistManager soloistManager;
    soloistManager.InitVsyncReceiver();
    return soloistManager;
}

bool RSDisplaySoloistManager::InitVsyncReceiver()
{
    if (hasInitVsyncReceiver_) {
        return false;
    }

    if (!vsyncHandler_) {
        vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(
            AppExecFwk::EventRunner::Create("MainDisplaySoloist"));
    }
    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    frameRateLinker_ = OHOS::Rosen::RSFrameRateLinker::Create();
    while (!receiver_) {
        receiver_ = rsClient.CreateVSyncReceiver("MainDisplaySoloist",
            frameRateLinker_->GetId(), vsyncHandler_);
    }

    receiver_->Init();
    hasInitVsyncReceiver_ = true;
    managerStatus_ = ActiveStatus::ACTIVE;

    return true;
}

void RSDisplaySoloistManager::OnVsync(TimestampType timestamp, void* client)
{
    auto soloist = static_cast<RSDisplaySoloist*>(client);
    if (soloist) {
        soloist->VsyncCallbackInner(timestamp);
    }
}

void RSDisplaySoloistManager::VsyncCallbackInner(TimestampType timestamp)
{
    if (managerStatus_ != ActiveStatus::ACTIVE) {
        SetFrameRateLinkerEnable(false);
        return;
    }

    for (auto it = idToSoloistMap_.begin(); it != idToSoloistMap_.end();) {
        if (it->second->subStatus_ == ActiveStatus::NEED_REMOVE) {
            it = idToSoloistMap_.erase(it);
        } else {
            ++it;
        }
    }

    bool isNeedRequestVSync = false;
    frameRateRange_.Reset();

    for (const auto& [id, displaySoloist] : idToSoloistMap_) {
        if (displaySoloist->useExclusiveThread_) {
            displaySoloist->RequestNextVSync();
            continue;
        }

        if (displaySoloist->JudgeWhetherSkip(timestamp) &&
            displaySoloist->subStatus_ == ActiveStatus::ACTIVE) {
            if (displaySoloist->callback_.first) {
                displaySoloist->callback_.first(displaySoloist->timestamp_, displaySoloist->targetTimestamp_,
                                                displaySoloist->callback_.second);
            }
            if (displaySoloist->frameRateRange_.IsValid()) {
                frameRateRange_.Merge(displaySoloist->frameRateRange_);
            }
        }
        isNeedRequestVSync |= (displaySoloist->subStatus_ == ActiveStatus::ACTIVE);
    }

    if (isNeedRequestVSync && managerStatus_ == ActiveStatus::ACTIVE) {
        RequestNextVSync();
        FlushFrameRate(frameRateRange_.preferred_);
    } else {
        FlushFrameRate(0);
    }
}

bool RSDisplaySoloistManager::RequestNextVSync()
{
    if (receiver_ == nullptr) {
        ROSEN_LOGE("%{public}s, VSyncReceiver is null.", __func__);
        return false;
    }

    receiver_->RequestNextVSync(managerFrameCallback_);
    return true;
}

void RSDisplaySoloistManager::Start(SoloistIdType id)
{
    if (!idToSoloistMap_.count(id)) {
        idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
    }
    managerStatus_ = ActiveStatus::ACTIVE;
    idToSoloistMap_[id]->subStatus_ = ActiveStatus::ACTIVE;
    idToSoloistMap_[id]->SetSubFrameRateLinkerEnable(true);
    idToSoloistMap_[id]->RequestNextVSync();
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d.", __func__, id);
    return;
}

void RSDisplaySoloistManager::Stop(SoloistIdType id)
{
    if (!idToSoloistMap_.count(id)) {
        return;
    }
    idToSoloistMap_[id]->subStatus_ = ActiveStatus::INACTIVE;
    idToSoloistMap_[id]->SetSubFrameRateLinkerEnable(false);
    idToSoloistMap_[id]->RequestNextVSync();
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d.", __func__, id);
    return;
}

void RSDisplaySoloistManager::RemoveSoloist(SoloistIdType id)
{
    if (idToSoloistMap_.count(id)) {
        idToSoloistMap_[id]->subStatus_ = ActiveStatus::NEED_REMOVE;
        idToSoloistMap_[id]->SetSubFrameRateLinkerEnable(false);
        idToSoloistMap_[id]->RequestNextVSync();
    }
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d.", __func__, id);
    return;
}

void RSDisplaySoloistManager::InsertOnVsyncCallback(SoloistIdType id, DisplaySoloistOnFrameCallback cb, void* data)
{
    if (!idToSoloistMap_.count(id)) {
        idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
    }
    idToSoloistMap_[id]->callback_ = { cb, data };
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d.", __func__, id);
    return;
}

void RSDisplaySoloistManager::InsertFrameRateRange(SoloistIdType id, FrameRateRange frameRateRange)
{
    if (!idToSoloistMap_.count(id)) {
        idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
    }
    idToSoloistMap_[id]->frameRateRange_ = frameRateRange;
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d expected:%{public}d.", __func__, id, frameRateRange_.preferred_);
    return;
}

void RSDisplaySoloistManager::InsertUseExclusiveThreadFlag(SoloistIdType id, bool useExclusiveThread)
{
    if (!idToSoloistMap_.count(id)) {
        idToSoloistMap_[id] = std::make_shared<RSDisplaySoloist>(id);
    }
    idToSoloistMap_[id]->useExclusiveThread_ = useExclusiveThread;
    ROSEN_LOGD("%{public}s, SoloistId:%{public}d useExclusiveThread:%{public}d.", __func__, id, useExclusiveThread);
    return;
}

void RSDisplaySoloistManager::FlushFrameRate(int32_t rate)
{
    if (frameRateLinker_ && frameRateLinker_->IsEnable()) {
        FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, rate};
        frameRateLinker_->UpdateFrameRateRangeImme(range);
    }
}

void RSDisplaySoloistManager::SetMainFrameRateLinkerEnable(bool enabled)
{
    if (frameRateLinker_) {
        if (!enabled) {
            FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, 0};
            frameRateLinker_->UpdateFrameRateRangeImme(range);
            managerStatus_ = ActiveStatus::INACTIVE;
        } else {
            managerStatus_ = ActiveStatus::ACTIVE;
        }
        frameRateLinker_->SetEnable(enabled);
    }

    for (const auto& [id, displaySoloist] : idToSoloistMap_) {
        displaySoloist->SetSubFrameRateLinkerEnable(enabled);
    }

    RequestNextVSync();
}

int64_t RSDisplaySoloistManager::GetVSyncPeriod()
{
    int64_t period = 0;
    if (receiver_ == nullptr) {
        ROSEN_LOGE("%{public}s, VSyncReceiver is null.", __func__);
        return period;
    }

    receiver_->GetVSyncPeriod(period);
    return period;
}

bool RSDisplaySoloistManager::SetVSyncRate(int32_t vsyncRate)
{
    if (vsyncRate < 0) {
        return false;
    }

    if (sourceVsyncRate_ == vsyncRate) {
        return false;
    }
    sourceVsyncRate_ = vsyncRate;
    return true;
}

int32_t RSDisplaySoloistManager::GetVsyncRate() const
{
    return sourceVsyncRate_;
}

RSDisplaySoloistManager::~RSDisplaySoloistManager() {}

