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
#include "parameters.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class HgmFrameRateManager {
public:
    HgmFrameRateManager() {}

    void UpdateFrameRateRange(ScreenId id, FrameRateRange range);
    void FindAndSendRefreshRate();
    void ResetFrameRateRangeMap();
    void DecideSurfaceDrawingFrameRate(NodeId surfaceNodeId,
        ScreenId screenId, FrameRateRange range);

private:
    std::unordered_map<ScreenId, FrameRateRange> screenIdToFrameRateRange_;
    std::unordered_map<ScreenId, std::vector<uint32_t>> screenIdToSupportedRefreshRates_;
    std::unordered_map<ScreenId, uint32_t> screenIdToLCDRefreshRates_;
    std::unordered_map<NodeId, int> drawingFrameRateMap_; // RSSurfaceRenderNode id - frameRate
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_FRAME_RATE_MANAGER_H