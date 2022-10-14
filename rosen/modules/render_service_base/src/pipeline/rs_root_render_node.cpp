/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_root_render_node.h"

#include "platform/drawing/rs_surface.h"
#include "transaction/rs_transaction_proxy.h"
#include "visitor/rs_node_visitor.h"
#include <surface.h>

namespace OHOS {
namespace Rosen {
RSRootRenderNode::RSRootRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSCanvasRenderNode(id, context), dirtyManager_(std::make_shared<RSDirtyRegionManager>()) {}

RSRootRenderNode::~RSRootRenderNode() {}

void RSRootRenderNode::AttachRSSurfaceNode(NodeId surfaceNodeId)
{
    surfaceNodeId_ = surfaceNodeId;
}

int32_t RSRootRenderNode::GetSuggestedBufferWidth() const
{
    return suggestedBufferWidth_;
}

int32_t RSRootRenderNode::GetSuggestedBufferHeight() const
{
    return suggestedBufferHeight_;
}

void RSRootRenderNode::UpdateSuggestedBufferSize(int32_t width, int32_t height)
{
    suggestedBufferHeight_ = height;
    suggestedBufferWidth_ = width;
}

std::shared_ptr<RSSurface> RSRootRenderNode::GetSurface()
{
    return rsSurface_;
}

NodeId RSRootRenderNode::GetRSSurfaceNodeId()
{
    return surfaceNodeId_;
}

std::shared_ptr<RSDirtyRegionManager> RSRootRenderNode::GetDirtyManager() const
{
    return dirtyManager_;
}

void RSRootRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareRootRenderNode(*this);
}

void RSRootRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessRootRenderNode(*this);
}
} // namespace Rosen
} // namespace OHOS
