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

#include "hgm_core.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unistd.h>

#include "hgm_log.h"
#include "platform/common/rs_system_properties.h"
#include "frame_rate_report.h"
#include "sandbox_utils.h"

namespace OHOS::Rosen {
HgmCore& HgmCore::Instance()
{
    static HgmCore instance;
    if (instance.IsInit()) {
        return instance;
    }
    if (instance.Init() == false) {
        HGM_LOGI("HgmCore initialization failed");
    }

    return instance;
}

HgmCore::HgmCore()
{
    HGM_LOGI("Construction of Hgmcore");
}

bool HgmCore::Init()
{
    if (isInit_) {
        return true;
    }

    if (!isEnabled_) {
        HGM_LOGE("HgmCore Hgm is desactivated");
        return false;
    }

    if (InitXmlConfig() != EXEC_SUCCESS) {
        HGM_LOGE("HgmCore falied to parse");
        return false;
    }

    int newRateMode = static_cast<int32_t>(RSSystemProperties::GetHgmRefreshRateModesEnabled());
    if (newRateMode == 0) {
        HGM_LOGI("HgmCore No customer refreshrate mode found, set to xml default");
        if (mParsedConfigData_ == nullptr) {
            HGM_LOGE("HgmCore failed to get parsed data");
        } else {
            customFrameRateMode_ = static_cast<RefreshRateMode>(std::stoi(mParsedConfigData_->defaultRefreshRateMode_));
        }
    } else {
        HGM_LOGI("HgmCore No customer refreshrate mode found: %{public}d", newRateMode);
        customFrameRateMode_ = static_cast<RefreshRateMode>(newRateMode);
    }
    SetModeBySettingConfig();
    isInit_ = true;
    HGM_LOGI("HgmCore initialization success!!!");
    return isInit_;
}

int32_t HgmCore::InitXmlConfig()
{
    // parse xml configuration to hgm
    HGM_LOGD("HgmCore is parsing xml configuration");
    if (!mParser_) {
        mParser_ = std::make_unique<XMLParser>();
    }
    if (mParser_->LoadConfiguration(CONFIG_FILE) != EXEC_SUCCESS) {
        HGM_LOGE("HgmCore failed to load xml configuration file");
        return XML_FILE_LOAD_FAIL;
    }
    if (mParser_->Parse() != EXEC_SUCCESS) {
        HGM_LOGE("HgmCore failed to parse xml configuration");
        return XML_GET_ROOT_FAIL;
    }

    // parse ccm component
    if (mParser_->ParseComponentData() != EXEC_SUCCESS) {
        HGM_LOGE("HgmCore failed to parse xml configuration from ccm");
    }

    if (!mParsedConfigData_) {
        mParsedConfigData_ = mParser_->GetParsedData();
    }

    return EXEC_SUCCESS;
}

int32_t HgmCore::SetCustomRateMode(RefreshRateMode mode)
{
    customFrameRateMode_ = mode;
    return EXEC_SUCCESS;
}

int32_t HgmCore::SetModeBySettingConfig()
{
    if (!mParsedConfigData_) {
        HGM_LOGW("HgmCore no parsed xml configuration found, failed to apply refreshrate mode");
        return HGM_ERROR;
    }

    std::string settingTag = std::to_string(customFrameRateMode_);
    auto mapIter = mParsedConfigData_->customerSettingConfig_.find(settingTag);
    if (mapIter == mParsedConfigData_->customerSettingConfig_.end()) {
        HGM_LOGW("HgmCore failed to find strategy for customer setting : %{public}d", customFrameRateMode_);
        return HGM_ERROR;
    }

    std::string strat = mParsedConfigData_->customerSettingConfig_[settingTag];
    auto strategy = mParsedConfigData_->detailedStrategies_.find(strat);
    if (strategy == mParsedConfigData_->detailedStrategies_.end()) {
        HGM_LOGW("HgmCore failed to find strategy detail for strat : %{public}s", strat.c_str());
        return HGM_ERROR;
    }
    int32_t rateToSwitch = std::stoi(mParsedConfigData_->detailedStrategies_[strat].max);
    int32_t rateFloor = std::stoi(mParsedConfigData_->detailedStrategies_[strat].min);

    HGM_LOGW("HgmCore switching to rate: %{public}d via refreshrate mode, range min: %{public}d, max: %{public}d",
        rateToSwitch, rateFloor, rateToSwitch);
    if (rateToSwitch <= 0 || rateFloor <= 0) {
        HGM_LOGW("HgmCore get an illegal rate via parsed config data : %{public}d", rateToSwitch);
        return HGM_ERROR;
    }

    rateToSwitch = RequestBundlePermission(rateToSwitch);
    for (auto &screen : screenList_) {
        int32_t setRange = screen->SetRefreshRateRange(
            static_cast<uint32_t>(rateFloor), static_cast<uint32_t>(rateToSwitch));
        if (customFrameRateMode_ == HGM_REFRESHRATE_MODE_AUTO) {
            rateToSwitch = OLED_60_HZ;
            HGM_LOGI("HgmCore auto mode, set refreshrate 60HZ");
        }
        int32_t setThisScreen = SetScreenRefreshRate(screen->GetId(), 0, rateToSwitch);
        if (setThisScreen < EXEC_SUCCESS || setRange != EXEC_SUCCESS) {
            HGM_LOGW("HgmCore failed to apply refreshrate mode to screen : " PUBU64 "", screen->GetId());
            return HGM_ERROR;
        }
    }
    std::unordered_map<pid_t, uint32_t> rates;
    rates[GetRealPid()] = rateToSwitch;
    FRAME_TRACE::FrameRateReport::GetInstance().SendFrameRates(rates);
    return EXEC_SUCCESS;
}

int32_t HgmCore::RequestBundlePermission(int32_t rate)
{
    if (rate <= OLED_60_HZ) {
        return rate;
    }

    // black_list conatrol at 90hz, return 60 if in the list
    if (customFrameRateMode_ == HGM_REFRESHRATE_MODE_MEDIUM || customFrameRateMode_ == HGM_REFRESHRATE_MODE_HIGH) {
        auto bundle = mParsedConfigData_->bundle_black_list_.find(currentBundleName_);
        if (bundle != mParsedConfigData_->bundle_black_list_.end()) {
            return OLED_60_HZ;
        }
        return rate;
    }

    return rate;
}

int32_t HgmCore::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    // set the screen to the desired refreshrate
    HGM_LOGD("HgmCore setting screen " PUBU64 " to the rate of %{public}d", id, rate);
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGW("HgmCore failed to get screen of : " PUBU64 "", id);
        return HGM_ERROR;
    }

    if (rate <= 0) {
        HGM_LOGW("HgmCore refuse an illegal framerate: %{public}d", rate);
        return HGM_ERROR;
    }
    sceneId = static_cast<int32_t>(screenSceneSet_.size());
    int32_t modeToSwitch = screen->SetActiveRefreshRate(sceneId, static_cast<uint32_t>(rate));
    if (modeToSwitch < 0) {
        return modeToSwitch;
    }

    std::lock_guard<std::mutex> lock(modeListMutex_);

    // the rate is accepted and passed to a list, will be applied by hardwarethread before sending the composition
    HGM_LOGI("HgmCore the rate of %{public}d is accepted, the target mode is %{public}d", rate, modeToSwitch);
    if (modeListToApply_ == nullptr) {
        HGM_LOGD("HgmCore modeListToApply_ is invalid, buiding a new mode list");
        modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
    }
    auto modeList = modeListToApply_.get();
    (*modeList)[id] = modeToSwitch;
    return modeToSwitch;
}

int32_t HgmCore::SetRateAndResolution(ScreenId id, int32_t sceneId, int32_t rate, int32_t width, int32_t height)
{
    // reserved
    return HGM_ERROR;
}

int32_t HgmCore::SetRefreshRateMode(RefreshRateMode refreshRateMode)
{
    HGM_LOGD("HgmCore set refreshrate mode to : %{public}d", refreshRateMode);
    // change refreshrate mode by setting application
    if (SetCustomRateMode(refreshRateMode) != EXEC_SUCCESS) {
        return HGM_ERROR;
    }

    // apply the refreshrate mode to the screen
    if (SetModeBySettingConfig() != EXEC_SUCCESS) {
        return HGM_ERROR;
    }

    return EXEC_SUCCESS;
}

int32_t HgmCore::AddScreen(ScreenId id, int32_t defaultMode, ScreenSize& screenSize)
{
    // add a physical screen to hgm during hotplug event
    HGM_LOGI("HgmCore adding screen : " PUBI64 "", id);
    bool removeId = std::any_of(screenIds_.begin(), screenIds_.end(),
        [id](const ScreenId screen) { return screen == id; });
    if (removeId) {
        if (RemoveScreen(id) != EXEC_SUCCESS) {
            HGM_LOGW("HgmCore failed to remove the existing screen, not adding : " PUBI64 "", id);
            return HGM_BASE_REMOVE_FAILED;
        }
    }

    sptr<HgmScreen> newScreen = new HgmScreen(id, defaultMode, screenSize);

    std::lock_guard<std::mutex> lock(listMutex_);
    screenList_.push_back(newScreen);
    screenIds_.push_back(id);

    int32_t screenNum = GetScreenListSize();
    HGM_LOGI("HgmCore num of screen is %{public}d", screenNum);
    return EXEC_SUCCESS;
}

int32_t HgmCore::RemoveScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lock(listMutex_);
    // delete a screen during a hotplug event
    HGM_LOGD("HgmCore deleting the screen : " PUBU64 "", id);
    for (auto screen = screenIds_.begin(); screen != screenIds_.end(); ++screen) {
        if (*screen == id) {
            screenIds_.erase(screen);
            break;
        }
    }
    for (auto screen = screenList_.begin(); screen != screenList_.end(); ++screen) {
        if ((*screen)->GetId() == id) {
            screenList_.erase(screen);
            break;
        }
    }
    return EXEC_SUCCESS;
}

int32_t HgmCore::AddScreenInfo(ScreenId id, int32_t width, int32_t height, uint32_t rate, int32_t mode)
{
    // add a supported screen mode to the screen
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGW("HgmCore failed to get screen of : " PUBU64 "", id);
        return HGM_NO_SCREEN;
    }

    if (screen->AddScreenModeInfo(width, height, rate, mode) == EXEC_SUCCESS) {
        return EXEC_SUCCESS;
    }

    HGM_LOGW("HgmCore failed to add screen mode info of screen : " PUBU64 "", id);
    return HGM_SCREEN_PARAM_ERROR;
}

int32_t HgmCore::RefreshBundleName(const std::string& name)
{
    if (name == currentBundleName_) {
        return EXEC_SUCCESS;
    }

    currentBundleName_ = name;

    if (customFrameRateMode_ == HGM_REFRESHRATE_MODE_AUTO) {
        return EXEC_SUCCESS;
    }

    int resetResult = SetRefreshRateMode(customFrameRateMode_);
    if (resetResult == EXEC_SUCCESS) {
        HGM_LOGI("HgmCore reset current refreshrate mode: %{public}d due to bundlename: %{public}s",
            customFrameRateMode_, currentBundleName_.c_str());
    }
    return EXEC_SUCCESS;
}

uint32_t HgmCore::GetScreenCurrentRefreshRate(ScreenId id) const
{
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGW("HgmCore failed to find screen " PUBU64 "", id);
        return static_cast<uint32_t>(EXEC_SUCCESS);
    }

    return screen->GetActiveRefreshRate();
}

int32_t HgmCore::GetCurrentRefreshRateMode() const
{
    int32_t currentRefreshRateMode = static_cast<int32_t>(customFrameRateMode_);
    return currentRefreshRateMode;
}

sptr<HgmScreen> HgmCore::GetScreen(ScreenId id) const
{
    std::lock_guard<std::mutex> lock(listMutex_);
    for (auto screen = screenList_.begin(); screen != screenList_.end(); ++screen) {
        if ((*screen)->GetId() == id) {
            return *screen;
        }
    }

    return nullptr;
}

std::vector<uint32_t> HgmCore::GetScreenSupportedRefreshRates(ScreenId id)
{
    auto screen = GetScreen(id);
    if (!screen) {
        HGM_LOGW("HgmCore failed to find screen " PUBU64 "", id);
        return std::vector<uint32_t>(static_cast<uint32_t>(EXEC_SUCCESS));
    }

    auto supportedRates = screen->GetSupportedRates();
    std::vector<uint32_t> retVec;
    retVec.assign(supportedRates.begin(), supportedRates.end());
    return retVec;
}

std::vector<int32_t> HgmCore::GetScreenComponentRefreshRates(ScreenId id)
{
    if (!mParsedConfigData_) {
        HGM_LOGW("HgmCore no parsed component data, returning default value");
        return std::vector<int32_t>(static_cast<uint32_t>(EXEC_SUCCESS));
    }

    std::vector<int32_t> retVec;
    for (const auto& [rate, _] : mParsedConfigData_->refreshRateForSettings_) {
        retVec.emplace_back(std::stoi(rate));
        HGM_LOGE("HgmCore Adding component rate: %{public}d", std::stoi(rate));
    }
    return retVec;
}

std::unique_ptr<std::unordered_map<ScreenId, int32_t>> HgmCore::GetModesToApply()
{
    std::lock_guard<std::mutex> lock(modeListMutex_);
    return std::move(modeListToApply_);
}

void HgmCore::SetActiveScreenId(ScreenId id)
{
    activeScreenId_ = id;
}

void HgmCore::StartScreenScene(SceneType sceceType)
{
    screenSceneSet_.insert(sceceType);
}

void HgmCore::StopScreenScene(SceneType sceceType)
{
    screenSceneSet_.erase(sceceType);
}

int32_t HgmCore::GetScenePreferred() const
{
    if (screenSceneSet_.find(SceneType::SCREEN_RECORD) != screenSceneSet_.end()) {
        return 60; // 60 means screen record scene preferred
    }
    return 0;
}

sptr<HgmScreen> HgmCore::GetActiveScreen() const
{
    if (activeScreenId_ == INVALID_SCREEN_ID) {
        HGM_LOGE("HgmScreen activeScreenId_ noset");
        return nullptr;
    }
    return GetScreen(activeScreenId_);
}
} // namespace OHOS::Rosen