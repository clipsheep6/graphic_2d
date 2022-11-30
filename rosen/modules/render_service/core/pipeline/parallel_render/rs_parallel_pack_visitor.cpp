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

#include "rs_parallel_pack_visitor.h"
#include "pipline/rs_base_render_node.h"
#include "pipline/rs_surface_render_node.h"
#include "pipline/rs_display_render_node.h"
#include "pipline/parallel_render/rs_parallel_render_manager.h"
#include "pipline/rs_uni_render_visitor.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSParallelPackVisitor::RSParallelPackVisitor(RSUniRenderVisitor &visitor)
{
    partialRenderType_ = RSSystemPropeties::GetUniPartialRenderEnable();
    isPartialRenderEnable_ = (partialRenderType_ != PartialRenderType::DISABLED);
    isDirtyRegionDfxEnable_ = (RSSystemProperties::GetDirtyRegionDebugType() == DirtyRegionDebufType::EGL_DAMAGE);
    isTargetDirtyRegionDfxEnable_ = RSSystemProperties::GetTargetDirtyRegionEnaled(dfxTargetSurfaceNames_);
    if (isDirtyRegionDfxEnable_ && isTargetDirtyRegionDfxEnable_) {
        isDirtyRegionDfxEnable_ = false;
    }
    isOpDropped_ = isPartialRenderEnable_ && (partialRenderType_ != PartialRenderType::SET_DAMAGE) &&
        (!isDirtyRegionDfxEnable_ && !isTargetDirtyRegionDfxEnable_);
    doAnimate_ = visitor.GetAnimateState();
}

void RSParallelPackVisitor::ProcessBaseRenderNode(RSBaseRenderNode &node)
{
    for(auto &child : node.GetSortedChildren()) {
        child->Process(shared_form_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSParallelPackVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    isSecurityDisplay_ = node.GetSercurityDisplay();
    ProcessBaseRenderNode(node);
}

void RSParallelPackVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_TRACE_Name("SecurityLayer Skip");
        return;
    }
    const auto  &property = node.GetRenderProperties();
    if (!property.GetVisible()) {
        RS_LOGD("RSParallelPackVisitor::ProcessSurfaceRenderNode: %" PRIu64 " invisible", node.GetId());
        return;
    }
    if (!node.GetOcclusionVisiable() && !doAnimate_ && RSSystemProperties::GetOcclusionEnable()) {
        RS_TRACE_Name("Occlusion Skip");
        return;
    }
    if (isOpDropped_ && node.isAppWindow()) {
        if (!node.SubNodeNeedDraw(node.GetOldDirty(), partialRenderType_)) {
            RS_TRACE_Name("QuikReject Skip");
            RS_LOGD("RSParallelPackVisitor::ProcessSurfaceRenderNode skip: %s", node.GetName().c_str());
            return;
        }
    }
    auto surfaceNode = std::static_pointer_case<RSSurfaceNode>(node.GetSortedChildren().front());
    if (isOpDropped_ && (surfaceNode != nullptr) && surfaceNode->IsAppWindow() &&
        !surfaceNode->SubNodeDraw(surfaceNode->GetDstRect(), partialRenderType_)) {
            RS_TRACE_Name("Parallel QuikReject Skip");
            return;
    }
    RSParellelRenderManager::Instance()->PaceRenderTask(node);
}
} //namespace Rosen
} //namespace OHOS
