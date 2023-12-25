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

#ifndef RS_DECIDE_HARDWARE_DISABLE_H
#define RS_DECIDE_HARDWARE_DISABLE_H

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
    std::weak_ptr<RSSurfaceRenderNode> surfaceNode;
};
class RSB_EXPORT RSDecideHardwareDisable {
public:
    static RSDecideHardwareDisable* Instance();
    bool IsHardwareDisabledBySort(const RSSurfaceRenderNode& node);
    void UpdateSurfaceBufferSortVector();
    void UpdateSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);
    void DeleteSurfaceNode(NodeId nodeId);
    void UpdateVSyncCnt();

private:
    RSDecideHardwareDisable() = default;
    ~RSDecideHardwareDisable() noexcept = default;
    static bool SortVectorCmp(const std::pair<NodeId, double>& p1, const std::pair<NodeId, double>& p2);
    bool IsInHwcWhiteList(const std::string& nodeName);

    std::unordered_map<NodeId, std::pair<std::queue<uint32_t>, SortElements>> surfaceBufferUpdateMap_;
    std::vector<std::pair<NodeId, double>> surfaceBufferSortVec_;
    uint32_t vsyncCnt_ = 0;
    uint32_t curVSyncHwcWhiteCnt_ = 0;
};
}
}


#endif