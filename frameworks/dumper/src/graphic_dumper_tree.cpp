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

#include "graphic_dumper_tree.h"

#include "graphic_dumper_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperTree" };
} // namespace

TreeNodePtr TreeNode::GetSetNode(const std::string s)
{
    if (nodeMap_ == nullptr) {
        nodeMap_ = std::make_unique<TreeNodeMap>();
    }
    if (!HasNode(s)) {
        (*nodeMap_)[s] = std::make_shared<TreeNode>();
        (*nodeMap_)[s]->tag_ = s;
    }
    return (*nodeMap_)[s];
}

bool TreeNode::HasNode(const std::string s)
{
    return nodeMap_->find(s) != nodeMap_->end();
}

bool TreeNode::IsEmptyNode()
{
    if (nodeMap_->empty() && listenerIds_.empty()) {
        return true;
    }
    return false;
}

void TreeNode::EraseNode(const std::string &s)
{
    (*nodeMap_)[s] = nullptr;
    nodeMap_->erase(s);
}

std::string TreeNode::GetTag()
{
    return tag_;
}

void TreeNode::SetValue(const std::string s)
{
    value_ = s;
}

std::string TreeNode::GetValue()
{
    return value_;
}

void TreeNode::AddListenerId(uint32_t &listenerId)
{
    GDLOGFI("");
    listenerIds_.push_back(listenerId);
}

void TreeNode::RemoveListenerId(uint32_t &listenerId)
{
    GDLOGFI("%{public}u", listenerId);
    for (auto iter = listenerIds_.begin(); iter != listenerIds_.end(); ++iter) {
        GDLOGFI("%{public}u <==> %{public}u", (*iter), listenerId);
        if (*iter == listenerId) {
            GDLOGFI("");
            listenerIds_.erase(iter--);
        }
    }
}

std::vector<uint32_t> TreeNode::GetListenerIds() const
{
    GDLOGFI("");
    return listenerIds_;
}

void TreeNode::Foreach(TreeNodeVisitFunc func) const
{
    GDLOGFI("");
    if (nodeMap_ == nullptr) {
        return;
    }
    for (const auto& [k, v] : (*nodeMap_)) {
        GDLOGFI("%{public}s", k.c_str());
        func(v);
    }
}
} // namespace OHOS
