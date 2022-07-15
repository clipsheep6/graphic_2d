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

#include "command/rs_node_command.h"

namespace OHOS {
namespace Rosen {
void RSNodeCommandHelper::ClearModifiers(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->ClearModifiers();
    }
}

void RSNodeCommandHelper::AddModifier(RSContext& context, NodeId nodeId,
    const std::shared_ptr<RSRenderModifier>& modifier)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->AddModifier(modifier);
    }
}

void RSNodeCommandHelper::RemoveModifier(RSContext& context, NodeId nodeId, PropertyId propertyId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    if (node) {
        node->RemoveModifier(propertyId);
    }
}
} // namespace Rosen
} // namespace OHOS
