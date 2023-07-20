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

#include "hgm_frame_rate_manager.h"

#include "hgm_core.h"
#include "hgm_log.h"

namespace OHOS {
namespace Rosen {
void HgmFrameRateManager::UpdateFrameRateRange(ScreenId id, FrameRateRange range)
{
    if (!range.IsValid()) {
        return;
    }
    screenIdToFrameRateRange_[id] = range;
}

void HgmFrameRateManager::FindAndSendRefreshRate()
{
    for (auto idToRange : screenIdToFrameRateRange_) {
        ScreenId id = idToRange.first;
        auto& instance = HgmCore::Instance();

        // Get supported refreshRates
        auto& refreshRateMap = screenIdToSupportedRefreshRates_;
        if (refreshRateMap.find(id) == refreshRateMap.end()) {
            refreshRateMap[id] = instance.GetScreenSupportedRefreshRates(id);
            std::sort(refreshRateMap[id].begin(), refreshRateMap[id].end());
        }
        auto& refreshRates = refreshRateMap[id];

        // Find current refreshRate by FrameRateRange. For example:
        // 1. FrameRateRange[min, max, preferred] is [24, 48, 48], supported refreshRates
        // of current screen are {30, 60, 90}, the result should be 30, not 60.
        // 2. FrameRateRange[min, max, preferred] is [150, 150, 150], supported refreshRates
        // of current screen are {30, 60, 90}, the result will be 90.
        uint32_t currRefreshRate = 0;
        FrameRateRange range = screenIdToFrameRateRange_[id];
        auto iter = std::lower_bound(refreshRates.begin(), refreshRates.end(), range.preferred_);
        uint32_t pos = iter - refreshRates.begin();
        if (pos < refreshRates.size()) {
            if (refreshRates[pos] > static_cast<uint32_t>(range.max_) && pos > 0 &&
                refreshRates[pos - 1] >= static_cast<uint32_t>(range.min_) &&
                refreshRates[pos - 1] <= static_cast<uint32_t>(range.max_)) {
                currRefreshRate = refreshRates[pos - 1];
            } else {
                currRefreshRate = refreshRates[pos];
            }
        } else {
            currRefreshRate = refreshRates.back(); // preferred fps >= biggest supported refreshRate
        }

        // Send RefreshRate
        if (!refreshRateSwitch) {
            HGM_LOGD("HgmFrameRateManager: refreshRateSwitch is off, currRefreshRate is %{public}d", currRefreshRate);
            return;
        }
        if (currRefreshRate != instance.GetScreenCurrentRefreshRate(id)) {
            HGM_LOGD("HgmFrameRateManager: current refreshRate is %{public}d", currRefreshRate);
            int status = instance.SetScreenRefreshRate(id, 0, currRefreshRate);
            if (status != EXEC_SUCCESS) {
                HGM_LOGE("HgmFrameRateManager: failed to set refreshRate %{public}d, screenId %{public}d",
                    currRefreshRate, static_cast<int>(id));
            }
        }
    }
}

void HgmFrameRateManager::ResetFrameRateRangeMap()
{
    screenIdToFrameRateRange_.clear();
}
} // namespace Rosen
} // namespace OHOS