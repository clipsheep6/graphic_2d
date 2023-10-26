/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RS_PRE_COMPOSE_MANAGER_H
#define RS_PRE_COMPOSE_MANAGER_H

#include <list>
#include <memory>
#include <refbase.h>
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_pre_compose_group.h"
#include "pipeline/rs_display_render_node.h"


namespace OHOS {
namespace Rosen {
class RSPreComposeManager : public RefBase {
public:
    static sptr<RSPreComposeManager> GetInstance();
    void UpdateLastAndCurrentVsync();
    void StartCurrentVsync(std::shared_ptr<RSDisplayRenderNode>& node,
        std::shared_ptr<RSUniRenderVisitor> visitor, uint64_t focusNodeId);
    void UpdateNodesByLastVsync(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    void UpdateOcclusionByLastVsync(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
        Occlusion::Region& accumulatedRegion, VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap);
    bool LastVsyncIsDirty();
    void GetLastHwcNodes(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
        std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair>& prevHwcEnabledNodes);
    Occlusion::Region GetLastVisibleDirtyRegion();
    Occlusion::Region GetLastVisibleDirtyRegionWithGpuNodes();
    bool ProcessLastVsyncNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas,
        uint32_t threadIndex);
    void SetBufferAge(int32_t bufferAge);
    void UpdateGlobalDirtyByLastVsync(std::shared_ptr<RSDirtyRegionManager> dirtyManager);

private:
    RSPreComposeManager() = default;
    ~RSPreComposeManager() = default;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> GetSurfaceNodesFromDisplay(
        std::shared_ptr<RSDisplayRenderNode>& displayNode, uint64_t focusNodeId, uint64_t& leashFocusId);
    static inline sptr<RSPreComposeManager> instance_ = nullptr;
    std::unordered_set<NodeId> nodeIds_;
    std::shared_ptr<RSPreComposeGroup> rsPreComposeGroup_;
    std::shared_ptr<RSPreComposeGroup> MakeGroup(std::shared_ptr<RSDisplayRenderNode>& node);
    // void GetAllSurfaceNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>> surfaceNodeList,
    //     std::unorderd_set<NodeId>& nodeIds, std::vector<RSBaseRenderNode::SharedPtr>& surfaceNodes);
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_PRE_COMPOSE_MANAGER_H