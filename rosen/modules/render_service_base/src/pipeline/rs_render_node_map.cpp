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

#include "pipeline/rs_render_node_map.h"

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSRenderNodeMap::RSRenderNodeMap()
{
    // add animation fallback node
    renderNodeMap_.emplace(0, new RSCanvasRenderNode(0));
}

void RSRenderNodeMap::RegisterRenderNode(const std::shared_ptr<RSBaseRenderNode>& nodePtr)
{
    if (nodePtr == nullptr) {
        ROSEN_LOGE("RSRenderNodeMap::RegisterRenderNode: nodePtr is nullptr");
        return;
    }
    NodeId id = nodePtr->GetId();
    if (renderNodeMap_.find(id) != renderNodeMap_.end()) {
        return;
    }
    renderNodeMap_.emplace(id, nodePtr);
}

void RSRenderNodeMap::RegisterAliasNode(NodeId aliasId, NodeId targetId)
{
    auto nodePtr = GetRenderNode(targetId);
    if (nodePtr == nullptr) {
        ROSEN_LOGE("RSRenderNodeMap::RegisterAliasNode: target node does not exist");
        return;
    }
    aliasNodeMap_.emplace(aliasId, nodePtr);
}

void RSRenderNodeMap::UnregisterRenderNode(NodeId id)
{
    // std::unordered_map::erase() will return 0 if the key is not found.
    // if no node is found in renderNodeMap_, try to erase in aliasNodeMap_
    renderNodeMap_.erase(id) || aliasNodeMap_.erase(id);
}

void RSRenderNodeMap::FilterNodeByPid(pid_t pid)
{
    ROSEN_LOGI("RSRenderNodeMap::FilterNodeByPid removing all nodes belong to pid %d", pid);

    auto filter = [pid](const RSBaseRenderNode::SharedPtr& nodePtr) -> bool {
        if (nodePtr == nullptr) {
            return true;
        }
        // filter by matching higher 32 bits of node id
        if (static_cast<pid_t>(nodePtr->GetId() >> 32) != pid) {
            return false;
        }
        nodePtr->RemoveFromTree();
        return true;
    };

    // remove all nodes belong to given pid
    std::__libcpp_erase_if_container(
        renderNodeMap_, [&filter](const auto& pair) -> bool { return filter(pair.second); });
    std::__libcpp_erase_if_container(
        aliasNodeMap_, [&filter](const auto& pair) -> bool { return filter(pair.second.lock()); });
}

template<>
const std::shared_ptr<RSBaseRenderNode> RSRenderNodeMap::GetRenderNode(NodeId id) const
{
    auto itr = renderNodeMap_.find(id);
    if (itr != renderNodeMap_.end()) {
        return itr->second;
    }
    auto aliasItr = aliasNodeMap_.find(id);
    if (aliasItr != aliasNodeMap_.end()) {
        return aliasItr->second.lock();
    }
    return nullptr;
}

const std::shared_ptr<RSRenderNode> RSRenderNodeMap::GetAnimationFallbackNode() const
{
    return std::static_pointer_cast<RSRenderNode>(renderNodeMap_.at(0));
}

} // namespace Rosen
} // namespace OHOS
