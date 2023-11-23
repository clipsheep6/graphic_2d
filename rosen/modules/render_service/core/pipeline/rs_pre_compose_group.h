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

#ifndef RS_PRE_COMPOSE_GROUP_H
#define RS_PRE_COMPOSE_GROUP_H

#include <list>
#include "pipeline/rs_surface_render_node.h"
#include "event_handler.h"
#include "render_context/render_context.h"
#include "screen_manager/rs_screen_manager.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_pre_compose_element.h"
#include "pipeline/rs_pre_compose_region_manager.h"

namespace OHOS {
namespace Rosen {
class RSPreComposeGroup {
public:
    RSPreComposeGroup();
    ~RSPreComposeGroup();
    void UpdateLastAndCurrentVsync();
    void Init(ScreenInfo& info);
    void StartCurrentVsync(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
        std::shared_ptr<RSUniRenderVisitor> visitor, uint64_t focusNodeId, uint64_t leashFocusId);
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
    void UpdateGlobalDirtyByLastVsync(std::shared_ptr<RSDirtyRegionManager>& dirtyManager);
    void UpdateAppWindowNodesByLastVsync(std::vector<std::shared_ptr<RSSurfaceRenderNode>>& appWindowNodes);
    void SetLastVsyncHardwareForcedDisabled(bool forcedDisabled);
    void Close();

private:
    void PushHistory(Occlusion::Region& region);
    Occlusion::Region GetHistory(uint32_t index);
    Occlusion::Region MergeHistory(uint32_t age, Occlusion::Region& dirtyRegion);
    void CreateShareEglContext();
    void CreateShareGrContext();
    RenderContext *renderContext_ = nullptr;
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
#else
    std::shared_ptr<Drawing::GPUContext> grContext_ = nullptr;
#endif
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<RSPreComposeElement> current_ = nullptr;
    std::shared_ptr<RSPreComposeElement> last_ = nullptr;
    ScreenInfo screenInfo_;
    int32_t elementCount_ = 0;
    std::shared_ptr<RSPreComposeRegionManager> regionManager_;
    std::vector<Occlusion::Region> dirtyHistory_;
    Occlusion::Region visDirtyRegion_;
    int32_t historyHead_ = -1;
    uint32_t bufferAge_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_PRE_COMPOSE_GROUP_H