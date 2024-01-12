/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hgm_screen.h"

#include <cmath>
#include <map>
#include "hgm_log.h"

namespace OHOS::Rosen {
HgmScreen::HgmScreen() {}

HgmScreen::HgmScreen(ScreenId id, int32_t mode, ScreenSize& screenSize)
    : id_(id), activeModeId_(mode), width_(screenSize.width), height_(screenSize.height),
    phyWidth_(screenSize.phyWidth), phyHeight_(screenSize.phyHeight)
{
    auto screenLength = sqrt(pow(screenSize.width, 2) + pow(screenSize.height, 2));
    auto phyScreenLength = sqrt(pow(screenSize.phyWidth, 2) + pow(screenSize.phyHeight, 2));
    if (phyScreenLength != 0) {
        ppi_ = screenLength / (phyScreenLength / INCH_2_MM);
    }
    if (screenSize.phyWidth != 0) {
        xDpi_ = screenSize.width / (screenSize.phyWidth / INCH_2_MM);
    }
    if (screenSize.phyHeight != 0) {
        yDpi_ = screenSize.height / (screenSize.phyHeight / INCH_2_MM);
    }
}

HgmScreen::~HgmScreen() {}

uint32_t HgmScreen::GetActiveRefreshRate() const
{
    auto profilePtr = GetModeViaId(activeModeId_);
    if (!profilePtr) {
        HGM_LOGI("HgmScreen current mode is not supported, failed to get refreshrate");
        return RATE_NOT_SUPPORTED;
    }

    return profilePtr->GetRate();
}

int32_t HgmScreen::SetActiveRefreshRate(int32_t sceneId, uint32_t rate)
{
    // check if this screen supports the requested rate
    if (supportedRefreshRates_.find(rate) == supportedRefreshRates_.end()) {
        HGM_LOGE("HgmScreen refreshrate %{public}u is not supported, abandoned", rate);
        return HGM_ERROR;
    }

    // decide if this rate should be accepted
    if (!IfSwitchToRate(sceneId, rate)) {
        return HGM_ERROR;
    }

    int32_t modeIdToApply = GetModeIdViaRate(rate);
    if (modeIdToApply >= 0) {
        SetActiveModeId(modeIdToApply);
    }
    return modeIdToApply;
}

int32_t HgmScreen::SetRateAndResolution(int32_t sceneId, uint32_t rate, int32_t width, int32_t height)
{
    if (!IfSwitchToRate(sceneId, rate)) {
        return HGM_ERROR;
    }

    int32_t modeIdToApply = GetModeIdViaResolutionAndRate(width, height, rate);
    if (modeIdToApply >= 0) {
        SetActiveModeId(modeIdToApply);
    }
    return modeIdToApply;
}

int32_t HgmScreen::SetRefreshRateRange(uint32_t minRate, uint32_t maxRate)
{
    minRefreshRateRange_ = minRate;
    maxRefreshRateRange_ = maxRate;
    return EXEC_SUCCESS;
}

int32_t HgmScreen::AddScreenModeInfo(int32_t width, int32_t height, uint32_t rate, int32_t modeId)
{
    if (supportedModeIds_.find(modeId) == supportedModeIds_.end()) {
        supportedModeIds_.emplace(modeId);
    } else {
        return HGM_SCREEN_MODE_EXIST;
    }

    if (supportedRefreshRates_.find(rate) == supportedRefreshRates_.end()) {
        supportedRefreshRates_.emplace(rate);
    }

    auto newProfile = std::make_shared<ScreenProfileExt>(width, height, rate, modeId, 0);
    screenModeInfos_.emplace_back(newProfile);
    return EXEC_SUCCESS;
}

int32_t HgmScreen::AddScreenModeInfoExt(const RSScreenModeInfoExt& screenModeInfoExt)
{
    if (supportedModeIds_.find(screenModeInfoExt.GetScreenModeId()) == supportedModeIds_.end()) {
        supportedModeIds_.emplace(screenModeInfoExt.GetScreenModeId());
    } else {
        return HGM_SCREEN_MODE_EXIST;
    }

    if (supportedRefreshRates_.find(screenModeInfoExt.GetScreenRefreshRate()) == supportedRefreshRates_.end()) {
        supportedRefreshRates_.emplace(screenModeInfoExt.GetScreenRefreshRate());
    }

    auto newProfile = std::make_shared<ScreenProfileExt>(
        screenModeInfoExt.GetScreenWidth(),
        screenModeInfoExt.GetScreenHeight(),
        screenModeInfoExt.GetScreenRefreshRate(),
        screenModeInfoExt.GetScreenModeId(),
        screenModeInfoExt.GetGroupId());
    screenModeInfos_.emplace_back(newProfile);

    UpdateScreenMaterialType();
    return EXEC_SUCCESS;
}

void HgmScreen::SetActiveModeId(int32_t modeId)
{
    HGM_LOGI("HgmScreen setting activeModeId to %{public}d", modeId);
    activeModeId_ = modeId;
}

std::shared_ptr<HgmScreen::ScreenProfileExt> HgmScreen::GetModeViaId(int32_t id) const
{
    for (auto mode : screenModeInfos_) {
        if (mode->GetModeId() != id) {
            continue;
        }
        return mode;
    }

    HGM_LOGW("HgmScreen failed to get activeMode via modeId : %{public}d", id);
    return nullptr;
}

bool HgmScreen::IfSwitchToRate(int32_t sceneId, uint32_t rate) const
{
    // decides if a refreshrate switch will be accepted or not
    auto profilePtr = GetModeViaId(activeModeId_);
    bool ifSwitch = false;

    if (!profilePtr) {
        return ifSwitch;
    }

    if (rate == profilePtr->GetRate() || sceneId < 0) {
        HGM_LOGI("HgmScreen Set framerate to %{public}u is rejected!!!", rate);
        return ifSwitch;
    }

    ifSwitch = true;
    HGM_LOGI("HgmScreen Set framerate to %{public}u is accepted", rate);
    return ifSwitch;
}

int32_t HgmScreen::GetModeIdViaRate(uint32_t rate) const
{
    // get the corresponding mode id with the given refreshrate, using the current resolution
    auto supportedRates = supportedRefreshRates_.find(rate);
    if (supportedRates == supportedRefreshRates_.end()) {
        HGM_LOGE("HgmScreen freshrate %{public}u is not supported by any modeId", rate);
        return HGM_ERROR;
    }

    auto profilePtr = GetModeViaId(activeModeId_);
    if (!profilePtr) {
        HGM_LOGW("HgmScreen activeMode is not among supported modes");
        return HGM_ERROR;
    }

    HGM_LOGI("HgmScreen getting a new mode with the resolution of current mode : %{public}d", activeModeId_);
    int32_t mode = GetModeIdViaResolutionAndRate(profilePtr->GetWidth(), profilePtr->GetHeight(), rate);
    return mode;
}

int32_t HgmScreen::GetModeIdViaResolutionAndRate(int32_t width, int32_t height, uint32_t rate) const
{
    // get the corresponding mode id with the given resolution and refreshrate
    for (const auto& mode : screenModeInfos_) {
        if (mode->GetRate() != rate || mode->GetWidth() != width || mode->GetHeight() != height) {
            continue;
        }
        int32_t modeIdFound = mode->GetModeId();
        HGM_LOGI("HgmScreen mode : %{public}d is found for w : %{public}d, h : %{public}d, rate : %{public}u",
            modeIdFound, width, height, rate);
        return modeIdFound;
    }

    HGM_LOGW("HgmScreen NO mode is found for w : %{public}d, h : %{public}d, rate : %{public}u",
        width, height, rate);
    return HGM_ERROR;
}

void HgmScreen::UpdateScreenMaterialType()
{
    std::map<uint32_t, uint32_t> groupInfoCnt;
    for (auto &mode : screenModeInfos_) {
        auto iter = groupInfoCnt.find(mode->GetGroupId());
        if (iter == groupInfoCnt.end()) {
            groupInfoCnt[mode->GetGroupId()] = 1;
        } else {
            iter->second++;
        }
    }

    if (groupInfoCnt.size() == 1) {
        screenMaterialType_ = ScreenMaterialType::LTPO2;
    } else if (std::any_of(groupInfoCnt.begin(), groupInfoCnt.end(),
        [](const std::pair<uint32_t, uint32_t>& val) { return val.second > 1;})) {
        screenMaterialType_ = ScreenMaterialType::LTPO1;
    } else {
        screenMaterialType_ = ScreenMaterialType::LTPS;
    }
    HGM_LOGI("InitScreenMaterialType screen%{public}u:%{public}s", static_cast<uint32_t>(id_),
        screenMaterialType_ == ScreenMaterialType::LTPO1 ? "LTPO1" :
        screenMaterialType_ == ScreenMaterialType::LTPO2 ? "LTPO2" :
        screenMaterialType_ == ScreenMaterialType::LTPS ? "LTPS" :
        "UnKnown");
}
} // namespace OHOS::Rosen