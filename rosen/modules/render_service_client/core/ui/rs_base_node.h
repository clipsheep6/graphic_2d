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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_BASE_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_BASE_NODE_H

#include <memory>
#include <unistd.h>
#include <vector>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSTransitionEffect;

class RS_EXPORT RSBaseNode : public std::enable_shared_from_this<RSBaseNode> {
public:
    using WeakPtr = std::weak_ptr<RSBaseNode>;
    using SharedPtr = std::shared_ptr<RSBaseNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::BASE_NODE;

    virtual ~RSBaseNode();

    virtual void AddChild(SharedPtr child, int index);
    virtual void RemoveChild(SharedPtr child);
    // Add/RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens)
    void AddCrossParentChild(SharedPtr child, int index);
    void RemoveCrossParentChild(SharedPtr child, NodeId newParentId);
    void RemoveFromTree();
    virtual void ClearChildren();

    NodeId GetId() const
    {
        return id_;
    }

    virtual RSUINodeType GetType() const
    {
        return RSUINodeType::BASE_NODE;
    }

    virtual FollowType GetFollowType() const
    {
        return FollowType::NONE;
    }

    bool IsInstanceOf(RSUINodeType type) const;
    template<typename T>
    bool IsInstanceOf() const;

    // type-safe reinterpret_cast
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(const std::shared_ptr<RSBaseNode>& node)
    {
        return node ? node->ReinterpretCastTo<T>() : nullptr;
    }
    template<typename T>
    std::shared_ptr<T> ReinterpretCastTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }
    virtual std::string DumpNode(int depth) const;
protected:
    static bool isUniRenderEnabled_;
    bool isRenderServiceNode_;

    RSBaseNode(bool isRenderServiceNode);
    RSBaseNode(const RSBaseNode&) = delete;
    RSBaseNode(const RSBaseNode&&) = delete;
    RSBaseNode& operator=(const RSBaseNode&) = delete;
    RSBaseNode& operator=(const RSBaseNode&&) = delete;

    virtual void OnAddChildren() {}
    virtual void OnRemoveChildren() {}
    SharedPtr GetParent();

    void SetId(const NodeId& id)
    {
        id_ = id;
    }

    bool IsRenderServiceNode() const
    {
        return isUniRenderEnabled_ || isRenderServiceNode_;
    }

private:
    static NodeId GenerateId();
    NodeId id_;

    NodeId parent_ = 0;
    std::vector<NodeId> children_;
    void SetParent(NodeId parent);
    void RemoveChildById(NodeId childId);

    friend class RSUIDirector;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_BASE_NODE_H
