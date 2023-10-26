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

#ifndef RS_PRE_COMPOSE_REGION_MANAGER_H
#define RS_PRE_COMPOSE_REGION_MANAGER_H

#include <list>
#include <memory>
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "common/rs_occlusion_region.h"
#include "transaction/rs_occlusion_data.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/core/SkRegion.h"
#endif

namespace OHOS {
namespace Rosen {
struct NodeInfo {
    uint64_t id;
    uint8_t type;
    Occlusion::Region region;
};
class RSPreComposeRegionManager {
public:
    RSPreComposeRegionManager();
    ~RSPreComposeRegionManager();
    void UpdateDirtyRegion(std::vector<RSBaseRenderNode::SharedPtr>& curAllNodes);
    void GetOcclusion(Occlusion::Region& accumulatedRegion,
        VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap,
        std::vector<NodeInfo>& visNodes);
    bool IsDirty();
    Occlusion::Region GetVisibleDirtyRegion();
    RectI GetNodeChangeDirtyRect();
#ifndef USE_ROSEN_DRAWING
    SkRegion& GetClipRegion() {
        return region_;
    }
#else
    Drawing::Region& GetClipRegion() {
        return region_;
    }
#endif
private:
    std::shared_ptr<RSDirtyRegionManager> dirtyManager_;
    std::vector<RSBaseRenderNode::SharedPtr> curAllNodes_;
    std::vector<NodeId> lastSurfaceIds_;
    std::vector<NodeId> curSurfaceIds_;
    VisibleData curVisVec_;
    std::map<uint32_t, bool> curVisMap_;
    Occlusion::Region accumulatedRegion_;
    std::vector<NodeInfo> visNodes_;
    std::unordered_map<NodeId, RectI> lastSurfaceVisiableRegion_;
    std::unordered_map<NodeId, RectI> curSurfaceVisiableRegion_;
    Occlusion::Region allVisibleDirtyRegion_;
    std::vector<RectI> dirtyRects_;
    bool isDirty_ = false;
    bool isNodeChange_ = false;
#ifndef USE_ROSEN_DRAWING
    SkRegion region_;
#else
    Drawing::Region region_;
#endif
    bool CheckNodeChange();
    void GetChangedNodeRegion();
    bool CheckSurfaceVisiable(OcclusionRectISet& occlusionSurfaces, std::shared_ptr<RSSurfaceRenderNode> curSurface);
    void CalcOcclusion();
    void CalcGlobalDirtyRegion();
    void CalcGlobalDirtyRegionByNodeChange();
    void CalcGlobalDirtyRegionByContainer();
    void MergeDirtyHistory();
    void MergeVisibleDirtyRegion();
    void SetSurfaceGlobalDirtyRegion();
    void SetSurfaceBelowDirtyRegion();
    void CalcVisiableDirtyRegion();
    void CalcDirtyRectsWithDirtyRegion();
    void CalcClipRects();
};
} // namespace Rosen
} // namespace OHOS
 #endif // RS_PRE_COMPOSE_REGION_MANAGER_H