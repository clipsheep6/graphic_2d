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

#ifndef RS_PRE_COMPOSE_ELEMENT_H
#define RS_PRE_COMPOSE_ELEMENT_H

#include <list>
#include "pipeline/rs_surface_render_node.h"
#include "event_handler.h"
#include "render_context/render_context.h"
#include "screen_manager/rs_screen_manager.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_pre_compose_region_manager.h"

namespace OHOS {
namespace Rosen {
class RSPreComposeElement {
public:
    RSPreComposeElement(ScreenInfo& info, int32_t id, std::shared_ptr<RSPreComposeRegionManager> regionManager);
    ~RSPreComposeElement();
    void SetParams(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
        std::shared_ptr<RSUniRenderVisitor> visitor, uint64_t focusNodeId, uint64_t leashFocusId);
    void Init();
    void Deinit();
    void Reset();
    void UpdateDirtyRegion();
    void UpdateImage();
    void UpdateNodes(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    void UpdateOcclusion(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
        Occlusion::Region& accumulatedRegion, VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap,
        std::shared_ptr<RSPreComposeElement> next);
    bool IsDirty();
    void GetHwcNodes(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
        std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair>& prevHwcEnabledNodes);
    Occlusion::Region GetDirtyVisibleRegion();
    Occlusion::Region GetVisibleDirtyRegionWithGpuNodes();
    bool ProcessNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas, uint32_t threadIndex);
    bool IsUpdateImageEnd();
    void StartCalculateAndDrawImage();

private:
    void SetNewNodeList(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList);
    void ClipRect();
    void UpdateHwcNodes();
    void DrawGpuNodes(std::shared_ptr<RSPaintFilterCanvas>& canvas, uint32_t threadIndex);
    void UpdateCanvasMatrix(std::shared_ptr<RSPaintFilterCanvas>& canvas,
        std::shared_ptr<RSSurfaceRenderNode> node, Occlusion::Region& surfaceDirtyRegion);
    void CalVisDirtyRegion();
    ScreenInfo screenInfo_;
    RenderContext* renderContext_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> mainCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::shared_ptr<RSUniRenderVisitor> displayVisitor_;
    std::shared_ptr<RSUniRenderVisitor> visitor_;
    int32_t id_;
    std::unordered_set<NodeId> nodeIds_;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> surfaceNodeList_;
    std::vector<RSBaseRenderNode::SharedPtr> allSurfaceNodes_;
    bool isDone_ = true;
    bool needDraw_ = false;
    bool isDirty_ = false;
    bool getHwcNodesDone_ = false;
    bool isVisDirtyRegionUpdate_ = false;
    int32_t width_;
    int32_t height_;
    uint64_t focusNodeId_ = 0;
    uint64_t leashFocusId_ = 0;
    uint64_t lastOcclusionId_ = 0;
    VisibleData curVisVec_;
    std::map<uint32_t, bool> curVisMap_;
    Occlusion::Region accumulatedRegion_;
    std::vector<NodeInfo> visNodes_;
    std::shared_ptr<RSPreComposeRegionManager> regionManager_;
    RectI dirtyRect_;
    Occlusion::Region dirtyRegion_;
    Occlusion::Region visDirtyRegion_;
    Occlusion::Region visRegion_;
    Occlusion::Region aboveRegion_;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> appWindowNodes_;
    std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair> hardwareNodes_;
    std::vector<std::pair<std::shared_ptr<RSSurfaceRenderNode>, Occlusion::Region>> gpuNodes_;
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
    void CreateShareEglContext();
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> CreateShareGrContext();
    sk_sp<GrDirectContext> grContext_;
#endif
    sk_sp<SkSurface> CreateSkSurface();
    sk_sp<SkSurface> cacheSurface_ = nullptr;
#ifdef RS_ENABLE_GL
    GrBackendTexture cacheTexture_;
#endif
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_PRE_COMPOSE_ELEMENT_H