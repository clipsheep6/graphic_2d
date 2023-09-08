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

#ifndef HGM_CORE_H
#define HGM_CORE_H

#include <functional>
#include <cinttypes>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include <event_handler.h>

#include "hgm_screen.h"
#include "hgm_frame_rate_tool.h"
#include "xml_parser.h"
#include "hgm_one_shot_timer.h"

namespace OHOS::Rosen {
class HgmCore final {
public:
    static HgmCore& Instance();

    std::vector<ScreenId> GetScreenIds() const
    {
        return screenIds_;
    }

    bool IsEnabled() const
    {
        return isEnabled_;
    }

    bool IsInit() const
    {
        return isInit_;
    }

    int32_t GetScreenListSize() const
    {
        return screenList_.size();
    }

    ScreenId GetActiveScreenId() const
    {
        return activeScreenId_;
    }

    // set refresh rates
    int32_t SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);
    static int32_t SetRateAndResolution(ScreenId id, int32_t sceneId, int32_t rate, int32_t width, int32_t height);
    int32_t SetRefreshRateMode(RefreshRateMode refreshRateMode);
    int32_t SetDefaultRefreshRateMode();

    // screen interface
    int32_t AddScreen(ScreenId id, int32_t defaultMode);
    int32_t RemoveScreen(ScreenId id);
    int32_t AddScreenInfo(ScreenId id, int32_t width, int32_t height, uint32_t rate, int32_t mode, int32_t group);
    int32_t RefreshBundleName(const std::string& name);
    uint32_t GetScreenCurrentRefreshRate(ScreenId id) const;
    sptr<HgmScreen> GetScreen(ScreenId id) const;
    std::vector<uint32_t> GetScreenSupportedRefreshRates(ScreenId id);
    std::vector<int32_t> GetScreenComponentRefreshRates(ScreenId id);
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> GetModesToApply();
    int32_t AddScreenProfile(ScreenId id, int32_t width, int32_t height, int32_t phyWidth, int32_t phyHeight);
    int32_t RemoveScreenProfile(ScreenId id);
    int32_t CalModifierPreferred(const HgmModifierProfile &hgmModifierProfile) const;
    void SetActiveScreenId(ScreenId id);
    std::shared_ptr<HgmOneShotTimer> GetScreenTimer(ScreenId screenId) const;
    void ResetScreenTimer(ScreenId screenId) const;
    void InsertAndStartScreenTimer(ScreenId screenId, int32_t interval,
        std::function<void()> resetCallback, std::function<void()> expiredCallback);
private:
    HgmCore();
    ~HgmCore() = default;
    HgmCore(const HgmCore&) = delete;
    HgmCore(const HgmCore&&) = delete;
    HgmCore& operator=(const HgmCore&) = delete;
    HgmCore& operator=(const HgmCore&&) = delete;

    bool Init();
    int32_t InitXmlConfig();
    int32_t SetCustomRateMode(RefreshRateMode mode);
    int32_t SetModeBySettingConfig();
    int32_t RequestBundlePermission(int32_t rate);

    bool isEnabled_ = true;
    bool isInit_ = false;
    static constexpr char CONFIG_FILE[] = "/system/etc/graphic/hgm_policy_config.xml";
    std::unique_ptr<XMLParser> mParser_;
    std::shared_ptr<ParsedConfigData> mParsedConfigData_ = nullptr;

    RefreshRateMode customFrameRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    std::vector<ScreenId> screenIds_;
    std::vector<sptr<HgmScreen>> screenList_;
    mutable std::mutex listMutex_;

    mutable std::mutex modeListMutex_;
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> modeListToApply_ = nullptr;

    std::string currentBundleName_;
    std::shared_ptr<HgmFrameRateTool> hgmFrameRateTool_ = nullptr;
    ScreenId activeScreenId_ = 0;
    std::unordered_map<ScreenId, std::shared_ptr<HgmOneShotTimer>> screenTimerMap_;
};
} // namespace OHOS::Rosen
#endif // HGM_CORE_H
