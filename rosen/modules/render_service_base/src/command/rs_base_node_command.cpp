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

#include "command/rs_base_node_command.h"

#include "pipeline/rs_base_render_node.h"
#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {

void BaseNodeCommandHelper::Destroy(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetMutableNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    auto& map = context.pendingSyncNodes_;
    RS_LOGE("ZJH Destroy 0, nodeid:%{public}llu,  %{public}ld, %{public}d", nodeId, node.use_count(), map.size());
    if (node == nullptr) {
        return;
    }
    auto parent = node->GetParent().lock();
    node->ClearChildren();
    RS_LOGE("ZJH Destroy 1, nodeid:%{public}llu,  %{public}ld", nodeId, node.use_count());
    node->RemoveFromTree();
    RS_LOGE("ZJH Destroy 2, nodeid:%{public}llu,  %{public}ld", nodeId, node.use_count());
    nodeMap.UnregisterRenderNode(node->GetId());
    RS_LOGE("ZJH Destroy 3, nodeid:%{public}llu,  %{public}ld %{public}d", nodeId, node.use_count(), !!parent);
    if (parent) {
        parent->RemoveChildFromFulllist(node->GetId());
    }
    RS_LOGE("ZJH Destroy 4, nodeid:%{public}llu,  %{public}ld", nodeId, node.use_count());
    map.clear();
    node.reset();
    RS_LOGE("ZJH Destroy 5, nodeid:%{public}llu,  %{public}ld", nodeId, node.use_count());
}

void BaseNodeCommandHelper::AddChild(RSContext& context, NodeId nodeId, NodeId childNodeId, int32_t index)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    auto child = nodeMap.GetRenderNode(childNodeId);
    if (node && child) {
        node->AddChild(child, index);
    }
}

void BaseNodeCommandHelper::MoveChild(RSContext& context, NodeId nodeId, NodeId childNodeId, int32_t index)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    auto child = nodeMap.GetRenderNode(childNodeId);
    if (node && child) {
        node->MoveChild(child, index);
    }
}

void BaseNodeCommandHelper::RemoveChild(RSContext& context, NodeId nodeId, NodeId childNodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    auto child = nodeMap.GetRenderNode(childNodeId);
    if (node && child) {
        node->RemoveChild(child);
    }
}

void BaseNodeCommandHelper::AddCrossParentChild(RSContext& context, NodeId id, NodeId childId, int32_t index)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(id);
    auto child = nodeMap.GetRenderNode(childId);
    if (node && child) {
        node->AddCrossParentChild(child, index);
    }
}

void BaseNodeCommandHelper::RemoveCrossParentChild(RSContext& context, NodeId nodeId, NodeId childNodeId,
    NodeId newParentId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    auto child = nodeMap.GetRenderNode(childNodeId);
    auto newParent = nodeMap.GetRenderNode(newParentId);
    if (node && child && newParent) {
        node->RemoveCrossParentChild(child, newParent);
    }
}

void BaseNodeCommandHelper::RemoveFromTree(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    if (node == nullptr) {
        return;
    }
    node->RemoveFromTree();
}

void BaseNodeCommandHelper::ClearChildren(RSContext& context, NodeId nodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto node = nodeMap.GetRenderNode(nodeId);
    if (node) {
        node->ClearChildren();
    }
}
} // namespace Rosen
} // namespace OHOS
