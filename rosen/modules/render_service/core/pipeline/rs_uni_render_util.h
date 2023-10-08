/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H

#include <list>
#include <mutex>
#include <set>
#include <unordered_map>
#include "surface.h"
#include "sync_fence.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node_map.h"
#include "common/rs_obj_abs_geometry.h"

#ifdef USE_ROSEN_DRAWING
#include "utils/matrix.h"
#endif

namespace OHOS {
namespace Rosen {
class RSUniRenderUtil {
public:
    // merge history dirty region of current display node and its child surfacenode(app windows)
    // for mirror display, call this function twice will introduce additional dirtyhistory in dirtymanager
    static void MergeDirtyHistory(std::shared_ptr<RSDisplayRenderNode>& node, int32_t bufferAge,
        bool useAlignedDirtyRegion = false);

    /* we want to set visible dirty region of each surfacenode into DamageRegionKHR interface, hence
     * occlusion is calculated.
     * make sure this function is called after merge dirty history
     */
    static Occlusion::Region MergeVisibleDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
        bool useAlignedDirtyRegion = false);
    static bool HandleSubThreadNode(RSRenderNode& node, RSPaintFilterCanvas& canvas);
    static bool HandleCaptureNode(RSRenderNode& node, RSPaintFilterCanvas& canvas);
    static void SrcRectScaleDown(BufferDrawParam& params, const RSSurfaceRenderNode& node);
    static BufferDrawParam CreateBufferDrawParam(const RSSurfaceRenderNode& node,
        bool forceCPU, uint32_t threadIndex = UNI_MAIN_THREAD_INDEX);
    static BufferDrawParam CreateBufferDrawParam(const RSDisplayRenderNode& node, bool forceCPU);
    static BufferDrawParam CreateLayerBufferDrawParam(const LayerInfoPtr& layer, bool forceCPU);
    static bool IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info);
#ifndef USE_ROSEN_DRAWING
    static Occlusion::Region AlignedDirtyRegion(const Occlusion::Region& dirtyRegion, int32_t alignedBits = 32);
    static int GetRotationFromMatrix(SkMatrix matrix);
#else
    static Occlusion::Region AlignedDirtyRegion(const Occlusion::Region& dirtyRegion, int32_t alignedBits = 32);
    static int GetRotationFromMatrix(Drawing::Matrix matrix);
#endif
    static void AssignWindowNodes(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes, uint64_t focusNodeId = 0,
        DeviceType deviceType = DeviceType::PHONE);
    static void ClearSurfaceIfNeed(const RSRenderNodeMap& map, const std::shared_ptr<RSDisplayRenderNode>& displayNode,
        std::set<std::shared_ptr<RSBaseRenderNode>>& oldChildren, DeviceType deviceType = DeviceType::PHONE);
    static void ClearCacheSurface(RSRenderNode& node, uint32_t threadIndex, bool isClearCompletedCacheSurface = true);
    static void CacheSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& oldSubThreadNodes,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
    // use floor value of translateX and translateY in matrix of canvas to avoid jittering
    static void FloorTransXYInCanvasMatrix(RSPaintFilterCanvas& canvas);
private:
    static void AssignMainThreadNode(std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
        const std::shared_ptr<RSSurfaceRenderNode>& node);
    static void AssignSubThreadNode(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes,
        const std::shared_ptr<RSSurfaceRenderNode>& node, DeviceType deviceType = DeviceType::PHONE,
        uint64_t focusNodeId = 0);
    static void SortSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
    static void HandleHardwareNode(const std::shared_ptr<RSSurfaceRenderNode>& node);
#ifndef USE_ROSEN_DRAWING
    static void PostReleaseSurfaceTask(sk_sp<SkSurface>&& surface, uint32_t threadIndex);
#else
    static void PostReleaseSurfaceTask(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex);
#endif
};
}
}
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H
