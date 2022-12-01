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
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/parallel_render/rs_parallel_render_manager.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSParallelPackVisitor::RSParallelPackVisitor(RSUniRenderVisitor &visitor)
{
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    isPartialRenderEnabled_ = (partialRenderType_ != PartialRenderType::DISABLED);
    isOpDropped_ = isPartialRenderEnabled_ && (partialRenderType_ != PartialRenderType::SET_DAMAGE);
    doAnimate_ = visitor.GetAnimateState();
}

void RSParallelPackVisitor::ProcessBaseRenderNode(RSBaseRenderNode &node)
{
    for (auto &child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSParallelPackVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    isSecurityDisplay_ = node.GetSecurityDisplay();
    ProcessBaseRenderNode(node);
}

void RSParallelPackVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_TRACE_NAME("SecurityLayer Skip");
        return;
    }
    const auto  &property = node.GetRenderProperties();
    if (!property.GetVisible()) {
        RS_LOGD("RSParallelPackVisitor::ProcessSurfaceRenderNode: %" PRIu64 " invisible", node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        RS_TRACE_NAME("Occlusion Skip");
        return;
    }
    if (isOpDropped_ && node.IsAppWindow()) {
        if (!node.SubNodeNeedDraw(node.GetOldDirty(), partialRenderType_)) {
            RS_TRACE_NAME("QuickReject Skip");
            RS_LOGD("RSParallelPackVisitor::ProcessSurfaceRenderNode skip: %s", node.GetName().c_str());
            return;
        }
    }
    auto surfaceNode = std::static_pointer_cast<RSSurfaceRenderNode>(node.GetSortedChildren().front());
    if (isOpDropped_ && (surfaceNode != nullptr) && surfaceNode->IsAppWindow() &&
        !surfaceNode->SubNodeNeedDraw(surfaceNode->GetDstRect(), partialRenderType_)) {
            RS_TRACE_NAME("Parallel QuickReject Skip");
            return;
    }
    RSParallelRenderManager::Instance()->PackRenderTask(node);
}
} // namespace Rosen
} // namespace OHOS
