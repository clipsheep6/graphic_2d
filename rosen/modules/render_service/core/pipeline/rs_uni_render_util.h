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
#include "surface.h"
#include "sync_fence.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "common/rs_obj_abs_geometry.h"

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
    static void SrcRectScaleDown(BufferDrawParam& params, const RSSurfaceRenderNode& node);
    static BufferDrawParam CreateBufferDrawParam(const RSSurfaceRenderNode& node, bool forceCPU);
    static BufferDrawParam CreateBufferDrawParam(const RSDisplayRenderNode& node, bool forceCPU);
    static BufferDrawParam CreateLayerBufferDrawParam(const LayerInfoPtr& layer, bool forceCPU);
    static void DrawCachedImage(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas, sk_sp<SkImage> image);
    static Occlusion::Region AlignedDirtyRegion(const Occlusion::Region& dirtyRegion, int32_t alignedBits = 32);
    static int GetRotationFromMatrix(SkMatrix matrix);
    static void AssignWindowNodes(const std::shared_ptr<RSDisplayRenderNode>& node, uint64_t focusNodeId,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
};
}
}
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H
