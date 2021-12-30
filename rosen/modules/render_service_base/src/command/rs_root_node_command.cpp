/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "command/rs_root_node_command.h"

#include "pipeline/rs_root_render_node.h"

namespace OHOS {
namespace Rosen {

void RootNodeCommandHelper::Create(RSContext& context, NodeId id)
{
    auto node = std::make_shared<RSRootRenderNode>(id);
    context.GetNodeMap().RegisterRenderNode(node);
}

void RootNodeCommandHelper::Attach(RSContext& context, NodeId id, uintptr_t surfaceProducer, int width, int height)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        node->AttachSurface(surfaceProducer, width, height);
        if (surfaceProducer == 0) {
            context.GetGlobalRootRenderNode()->RemoveChild(node);
        } else {
            context.GetGlobalRootRenderNode()->AddChild(node);
        }
    }
}

void RootNodeCommandHelper::AttachRSSurface(
    RSContext& context, NodeId id, std::shared_ptr<RSSurface> rsSurface, int width, int height)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        node->AttachRSSurface(rsSurface, width, height);
        context.GetGlobalRootRenderNode()->AddChild(node);
    }
}

} // namespace Rosen
} // namespace OHOS
