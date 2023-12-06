/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef RS_DECIDE_HARDWARE_ENABLE_H
#define RS_DECIDE_HARDWARE_ENABLE_H

#include <utility>
#include <queue>
#include <unordered_map>
#include "common/rs_macros.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
struct SortElements {
    uint64_t frequecy;
    uint64_t recent;
}
class RSB_EXPORT RSDecideHardwareEnable {
public:
    static RSDecideHardwareEnable* Instance();
    bool IsShouldHardwareEnable(NodeId nodeId) const;
    void UpdateSurfaceBufferSortVector();
    void UpdateSurfaceNode(NodeId nodeId);
    void DeleteSurfaceNode(NodeId nodeId);
    void UpdateVSyncCnt();

private:
    RSDecideHardwareEnable() = default;
    ~RSDecideHardwareEnable() noexcept = default;
    static bool SortVectorCmp(const std::pair<NodeId, double>& p1, const std::pair<NodeId, double>& p2);

    std::unordered_map<NodeId, std::pair<std::queue<uint32_t>, SortElements>> surfaceBufferUpdateMap_;
    std::vector<std::pair<NodeId, double>> surfaceBufferSortVec_;
    uint32_t vsyncCnt_ = 0;
}
}
}


#endif