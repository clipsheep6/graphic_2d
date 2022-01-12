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
#include <cstddef>
#include <iostream>
#include "command/rs_display_node_command.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_display_render_node.h"
#include <cinttypes>
namespace OHOS {
namespace Rosen {

void DisplayNodeCommandHelper::Create(RSContext& context, NodeId id, const RSDisplayNodeConfig& config)
{
    std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto& nodeMap = context.GetNodeMap();
    auto nodeList = context.GetGlobalRootRenderNode()->GetChildren();
    uint64_t mirrorId = 0;
    for (unsigned i = 0; i < nodeList.size(); ++i) {
        auto c = nodeList[i].lock();
        if (c != nullptr) {
            mirrorId = c->GetId();
            break;
        }
    }
    nodeMap.RegisterRenderNode(node);
    context.GetGlobalRootRenderNode()->AddChild(node);
    if (config.isMirrored) {
        auto mirrorSourceNode = nodeMap.GetRenderNode<RSDisplayRenderNode>(mirrorId);
        if (mirrorSourceNode == nullptr) {
            //TODO: ERROR
            return;
        }
        auto displayNode = RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(node);
        displayNode->SetMirrorSource(mirrorSourceNode);
    }
}

void DisplayNodeCommandHelper::SetScreenId(RSContext& context, NodeId id, uint64_t screenId)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSDisplayRenderNode>(id)) {
        node->SetScreenId(screenId);
    }
}

} // namespace Rosen
} // namespace OHOS
