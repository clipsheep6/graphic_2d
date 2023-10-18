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
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#endif
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
    RSPreComposeElement(ScreenInfo& info, RenderContext* renderContext,
        std::shared_ptr<RSPaintFilterCanvas> canvas, int32_t id);
    ~RSPreComposeElement();
    void SetParams(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
        std::shared_ptr<RSUniRenderVisitor> visitor);
    void Init();
    void Deinit();
    void Reset();
    void UpdateDirtyRegion();
    void UpdateImage();
    void UpdateNodes(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    void UpdateOcclusion(Occlusion::Region& accumulatedRegion,
        VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap);
    bool IsDirty();
    void GetHwcNodes(std::vector<SurfaceDirtyMgrPair>& prevHwcEnabledNodes);
    Occlusion::Region GetDirtyRegion();
    Occlusion::Region GetVisibleDirtyRegion();
    void SetGlobalDirtyRegion(Occlusion::Region& globalRegion);
    bool ProcessNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas);
    bool isUpdateImageEnd() {
        return isDone_;
    }
    void StartDraw() {
        needDraw_ = true;
    }

private:
    void SetNewNodeList(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList);
    ScreenInfo screenInfo_;
    RenderContext* renderContext_;
    std::shared_ptr<RSPaintFilterCanvas> mainCanvas_;
    std::shared_ptr<RSUniRenderVisitor> displayVisitor_;
    std::shared_ptr<RSUniRenderVisitor> visitor_;
    int32_t id_;
    std::unordered_set<NodeId> nodeIds_;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> surfaceNodeList_;
    bool isDone_ = true;
    bool needDraw_ = false;
    std::shared_ptr<RSPreComposeRegionManager> regionManager_;
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
    EGLSurface pbufferSurface_ = EGL_NO_SURFACE;
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> CreateShareGrContext();
#endif
    sk_sp<SkSurface> CreateSkSurface();
    sk_sp<SkSurface> CreateSkSurfaceWithMainCanvas();
    sk_sp<SkSurface> cacheSurface_ = nullptr;
#ifdef RS_ENABLE_GL
    GrBackendTexture cacheTexture_;
#endif
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_PRE_COMPOSE_ELEMENT_H