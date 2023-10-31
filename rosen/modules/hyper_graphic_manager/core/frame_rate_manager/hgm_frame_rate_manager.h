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

#ifndef HGM_FRAME_RATE_MANAGER_H
#define HGM_FRAME_RATE_MANAGER_H

#include <unordered_map>
#include <vector>

#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "screen_manager/screen_types.h"
#include "hgm_one_shot_timer.h"
#include "hgm_command.h"
#include "hgm_screen.h"
#include "hgm_core.h"

namespace OHOS {
namespace Rosen {
struct FrameRateRangeData {
    ScreenId screenId = INVALID_SCREEN_ID;
    FrameRateRange rsRange;
    std::unordered_map<pid_t, FrameRateRange> multiAppRange;
    bool forceUpdateFlag = false;
};

class HgmFrameRateManager {
public:
    HgmFrameRateManager() {}

    void UniProcessData(const FrameRateRangeData& data);
    int32_t CalModifierPreferred(const HgmModifierProfile &hgmModifierProfile);
    std::shared_ptr<HgmOneShotTimer> GetScreenTimer(ScreenId screenId) const;
    void ResetScreenTimer(ScreenId screenId) const;
    void StartScreenTimer(ScreenId screenId, int32_t interval,
        std::function<void()> resetCallback, std::function<void()> expiredCallback);
    void Reset();
    void SetTimerExpiredCallback(std::function<void()> expiredCallback)
    {
        expiredCallback_ = expiredCallback;
    }
private:
    void CalcRefreshRate(const ScreenId id, const FrameRateRange& range);
    void ExecuteSwitchRefreshRate(const ScreenId id);
    uint32_t GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range);
    std::pair<float, float> applyDimension(
        SpeedTransType speedTransType, float xSpeed, float ySpeed, sptr<HgmScreen> hgmScreen);

    uint32_t currRefreshRate_ = -1;
    uint32_t rsFrameRate_ = -1;
    std::unordered_map<pid_t, uint32_t> multiAppFrameRate_;
    std::function<void()> expiredCallback_;
    std::unordered_map<ScreenId, std::shared_ptr<HgmOneShotTimer>> screenTimerMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_FRAME_RATE_MANAGER_H