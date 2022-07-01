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

#include "ui/rs_base_node.h"

#include <algorithm>
#include <sstream>

#include "command/rs_base_node_command.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {

NodeId RSBaseNode::GenerateId()
{
    static pid_t pid_ = getpid();
    static std::atomic<uint32_t> currentId_ = 0;

    ++currentId_;
    if (currentId_ == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Node Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((NodeId)pid_ << 32) | currentId_;
}

bool RSBaseNode::isUniRenderEnabled_ = false;

void RSBaseNode::InitUniRenderEnabled()
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        isUniRenderEnabled_ = RSSystemProperties::GetUniRenderEnabled();
        ROSEN_LOGI("RSBaseNode::InitUniRenderEnabled:%d", isUniRenderEnabled_);
    }
}

RSBaseNode::RSBaseNode(bool isRenderServiceNode) : isRenderServiceNode_(isRenderServiceNode), id_(GenerateId())
{
    InitUniRenderEnabled();
}

RSBaseNode::~RSBaseNode()
{
    RemoveFromTree();
    RSNodeMap::MutableInstance().UnregisterNode(id_);
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeDestroy>(id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSBaseNode::AddChild(SharedPtr child, int index)
{
    if (child == nullptr) {
        ROSEN_LOGE("RSBaseNode::AddChild, child is nullptr");
        return;
    }
    NodeId childId = child->GetId();
    if (child->parent_ != 0) {
        child->RemoveFromTree();
    }

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }
    child->SetParent(id_);
    child->OnAddChildren();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(id_, childId, index);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::RemoveChild(SharedPtr child)
{
    if (child == nullptr || child->parent_ != id_) {
        ROSEN_LOGI("RSBaseNode::RemoveChild, child is nullptr");
        return;
    }
    NodeId childId = child->GetId();
    RemoveChildById(childId);
    child->OnRemoveChildren();
    child->SetParent(0);

    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveChild>(id_, childId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::AddCrossParentChild(SharedPtr child, int index)
{
    // AddCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // so this child will not remove from the old parent.
    if (child == nullptr) {
        ROSEN_LOGE("RSBaseNode::AddCrossScreenChild, child is nullptr");
        return;
    }
    if (!this->IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("RSBaseNode::AddCrossScreenChild, only displayNode support AddCrossScreenChild");
        return;
    }
    NodeId childId = child->GetId();

    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.push_back(childId);
    } else {
        children_.insert(children_.begin() + index, childId);
    }
    child->SetParent(GetId());
    child->OnAddChildren();
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddCrossParentChild>(GetId(), childId, index);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSBaseNode::RemoveCrossParentChild(SharedPtr child, NodeId newParentId)
{
    // RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // set the newParentId to rebuild the parent-child relationship.
    if (child == nullptr) {
        ROSEN_LOGI("RSBaseNode::RemoveCrossScreenChild, child is nullptr");
        return;
    }
    if (!this->IsInstanceOf<RSDisplayNode>()) {
        ROSEN_LOGE("RSBaseNode::RemoveCrossScreenChild, only displayNode support RemoveCrossScreenChild");
        return;
    }
    NodeId childId = child->GetId();
    RemoveChildById(childId);
    child->OnRemoveChildren();
    child->SetParent(newParentId);

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSBaseNodeRemoveCrossParentChild>(GetId(), childId, newParentId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSBaseNode::RemoveChildById(NodeId childId)
{
    auto itr = std::find(children_.begin(), children_.end(), childId);
    if (itr != children_.end()) {
        children_.erase(itr);
    }
}

void RSBaseNode::RemoveFromTree()
{
    if (auto parentPtr = RSNodeMap::Instance().GetNode(parent_)) {
        if (parentPtr == nullptr) {
            ROSEN_LOGD("RSBaseNode::RemoveFromTree, parent is nullptr, parent id: [%llu]", parent_);
            return;
        }
        parentPtr->RemoveChildById(id_);
        OnRemoveChildren();
        SetParent(0);
    }
    // always send Remove-From-Tree command
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveFromTree>(id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::ClearChildren()
{
    for (auto child : children_) {
        if (auto childPtr = RSNodeMap::Instance().GetNode(child)) {
            if (childPtr == nullptr) {
                ROSEN_LOGD("RSBaseNode::ClearChildren, child is nullptr, child id: [%llu]", child);
                continue;
            }
            childPtr->SetParent(0);
        }
    }
    children_.clear();

    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(id_);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), id_);
    }
}

void RSBaseNode::SetParent(NodeId parentId)
{
    parent_ = parentId;
}

RSBaseNode::SharedPtr RSBaseNode::GetParent()
{
    return RSNodeMap::Instance().GetNode(parent_);
}

std::string RSBaseNode::DumpNode(int depth) const
{
    std::stringstream ss;
    auto it = RSUINodeTypeStrs.find(GetType());
    if (it == RSUINodeTypeStrs.end()) {
        return "";
    }
    ss << it->second << "[" << std::to_string(id_) << "] child[";
    for (auto child : children_) {
        ss << std::to_string(child) << " ";
    }
    ss << "]";
    return ss.str();
}

template<typename T>
bool RSBaseNode::IsInstanceOf()
{
    constexpr uint32_t targetType = static_cast<uint32_t>(T::Type);
    return (static_cast<uint32_t>(GetType()) & targetType) == targetType;
}

// explicit instantiation with all render node types
template bool RSBaseNode::IsInstanceOf<RSBaseNode>();
template bool RSBaseNode::IsInstanceOf<RSDisplayNode>();
template bool RSBaseNode::IsInstanceOf<RSNode>();
template bool RSBaseNode::IsInstanceOf<RSSurfaceNode>();
template bool RSBaseNode::IsInstanceOf<RSCanvasNode>();
template bool RSBaseNode::IsInstanceOf<RSRootNode>();

} // namespace Rosen
} // namespace OHOS
