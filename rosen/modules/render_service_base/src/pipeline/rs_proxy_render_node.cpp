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

#include "pipeline/rs_proxy_render_node.h"

#include "command/rs_surface_node_command.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSProxyRenderNode::RSProxyRenderNode(
    NodeId id, std::weak_ptr<RSSurfaceRenderNode> target, NodeId targetId, std::weak_ptr<RSContext> context)
    : RSRenderNode(id, context), target_(target), targetId_(targetId)
{}

RSProxyRenderNode::~RSProxyRenderNode()
{
    // clear target node context properties
}

void RSProxyRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareProxyRenderNode(*this);
}

void RSProxyRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->ProcessProxyRenderNode(*this);
}

void RSProxyRenderNode::SetContextMatrix(const SkMatrix& matrix)
{
    if (contextMatrix_ == matrix) {
        return;
    }
    contextMatrix_ = matrix;
    if (auto target = target_.lock()) {
        target->SetContextMatrix(matrix, false);
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextMatrix>(targetId_, matrix);
    SendCommandFromRT(command, GetId());
}

void RSProxyRenderNode::SetContextAlpha(float alpha)
{
    if (contextAlpha_ == alpha) {
        return;
    }
    contextAlpha_ = alpha;
    if (auto target = target_.lock()) {
        target->SetContextAlpha(alpha, false);
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextAlpha>(targetId_, alpha);
    SendCommandFromRT(command, GetId());
}

void RSProxyRenderNode::SetContextClipRegion(SkRect clipRegion)
{
    if (contextClipRect_ == clipRegion) {
        return;
    }
    contextClipRect_ = clipRegion;
    if (auto target = target_.lock()) {
        target->SetContextClipRegion(clipRegion, false);
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextClipRegion>(targetId_, clipRegion);
    SendCommandFromRT(command, GetId());
}

void RSProxyRenderNode::ResetContextAlpha()
{
    // reset context alpha, makes sure next call to SetContextAlpha will set the correct value
    contextAlpha_ = -1.0f;
}
} // namespace Rosen
} // namespace OHOS
