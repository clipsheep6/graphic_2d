/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_BASE_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_BASE_RENDER_NODE_H

#include <atomic>
#include <list>
#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
namespace OHOS {
namespace Rosen {
class RSContext;
class RSNodeVisitor;
class RSCommand;

class RSB_EXPORT RSBaseRenderNode : public std::enable_shared_from_this<RSBaseRenderNode> {
public:
    enum NodeDirty : uint8_t {
        CLEAN                  = 0x00,
        HIERARCHY_DIRTY        = 0x01,
        MODIFIER_DIRTY         = 0x04,
        PROPERTY_DIRTY         = 0x08,
        CONTEXT_VARIABLE_DIRTY = 0x10,
        CONTENT_DIRTY          = 0x20,
        TRANSITION_DIRTY       = 0x40,
        GENERIC_DIRTY          = 0xFF,
    };

    using WeakPtr = std::weak_ptr<RSBaseRenderNode>;
    using SharedPtr = std::shared_ptr<RSBaseRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::BASE_NODE;
    virtual RSRenderNodeType GetType() const
    {
        return Type;
    }

    explicit RSBaseRenderNode(NodeId id, std::weak_ptr<RSContext> context = {}) : id_(id), context_(context) {};
    explicit RSBaseRenderNode(NodeId id, bool isOnTheTree = false, std::weak_ptr<RSContext> context = {}) : id_(id),
        isOnTheTree_(isOnTheTree), context_(context) {};
    virtual ~RSBaseRenderNode() = default;

    void AddChild(SharedPtr child, int index = -1);
    void MoveChild(SharedPtr child, int index);
    void RemoveChild(SharedPtr child, bool skipTransition = false);
    void ClearChildren();
    void RemoveFromTree(bool skipTransition = false);

    // Add/RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens)
    void AddCrossParentChild(const SharedPtr& child, int32_t index = -1);
    void RemoveCrossParentChild(const SharedPtr& child, const WeakPtr& newParent);

    virtual void CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node,
                                std::vector<RSBaseRenderNode::SharedPtr>& vec,
                                bool isUniRender);
    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor);
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor);

    // attention: current all base node's dirty ops causing content dirty
    // if there is any new dirty op, check it
    virtual bool IsContentDirty() const;
    virtual void SetContentDirty();

    // return if any animation is running
    virtual std::pair<bool, bool> Animate(int64_t timestamp)
    {
        return { false, false };
    }

    WeakPtr GetParent() const;
    virtual void ResetParent();

    NodeId GetId() const
    {
        return id_;
    }

    void SetIsOnTheTree(bool flag);
    bool IsOnTheTree() const
    {
        return isOnTheTree_;
    }

    // return children and disappeared children, not guaranteed to be sorted by z-index
    const std::list<SharedPtr>& GetChildren();
    // return children and disappeared children, sorted by z-index
    // NOTE: caller should call ApplyModifiers() of all children before calling this function
    const std::list<SharedPtr>& GetSortedChildren();

    void ResetSortedChildren()
    {
        isChildrenSorted_ = false;
    }

    uint32_t GetChildrenCount() const
    {
        return children_.size();
    }

    void DumpTree(int32_t depth, std::string& ou) const;

    virtual bool HasDisappearingTransition(bool recursive = true) const
    {
        if (recursive == false) {
            return false;
        } else {
            auto parent = GetParent().lock();
            return parent ? parent->HasDisappearingTransition(true) : false;
        }
    }

    void SetTunnelHandleChange(bool change)
    {
        isTunnelHandleChange_ = change;
    }

    bool GetTunnelHandleChange() const
    {
        return isTunnelHandleChange_;
    }

    // type-safe reinterpret_cast
    template<typename T>
    bool IsInstanceOf() const
    {
        constexpr auto targetType = static_cast<uint32_t>(T::Type);
        return (static_cast<uint32_t>(GetType()) & targetType) == targetType;
    }

    bool IsInstanceOf(RSRenderNodeType type) const
    {
        auto targetType = static_cast<uint32_t>(type);
        return (static_cast<uint32_t>(GetType()) & targetType) == targetType;
    }
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(std::shared_ptr<RSBaseRenderNode> node)
    {
        return node ? node->ReinterpretCastTo<T>() : nullptr;
    }
    template<typename T>
    std::shared_ptr<T> ReinterpretCastTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }

    bool HasChildrenOutOfRect() const
    {
        return hasChildrenOutOfRect_;
    }

    void UpdateChildrenOutOfRectFlag(bool flag)
    {
        hasChildrenOutOfRect_ = flag;
    }

    inline void ResetHasRemovedChild()
    {
        hasRemovedChild_ = false;
    }

    inline bool HasRemovedChild() const
    {
        return hasRemovedChild_;
    }

    inline void ResetChildrenRect()
    {
        childrenRect_ = RectI();
    }

    inline RectI GetChildrenRect() const
    {
        return childrenRect_;
    }

    bool ChildHasFilter() const
    {
        return childHasFilter_;
    }

    void SetChildHasFilter(bool childHasFilter)
    {
        childHasFilter_ = childHasFilter;
    }

    // accumulate all valid children's area
    void UpdateChildrenRect(const RectI& subRect);

    void SetDirty(uint8_t bitMask = NodeDirty::GENERIC_DIRTY);
    bool IsDirty(uint8_t bitMask = NodeDirty::GENERIC_DIRTY) const;

protected:
    uint8_t GetDirtyFlag() const;
    void ResetDirty(uint8_t filter = NodeDirty::GENERIC_DIRTY);

    void DumpNodeType(std::string& out) const;

    const std::weak_ptr<RSContext> GetContext() const
    {
        return context_;
    }
    virtual void OnTreeStateChanged() {}

    static void SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId);
private:
    NodeId id_;

    WeakPtr parent_;
    void SetParent(WeakPtr parent);
    bool isOnTheTree_ = false;

    std::list<WeakPtr> children_;
    std::list<std::pair<SharedPtr, uint32_t>> disappearingChildren_;

    std::list<SharedPtr> fullChildrenList_;
    void GenerateFullChildrenList();
    void SortChildren();

    const std::weak_ptr<RSContext> context_;
    uint8_t dirtyStatus_ = NodeDirty::GENERIC_DIRTY;
    friend class RSRenderPropertyBase;
    friend class RSRenderTransition;
    std::atomic<bool> isTunnelHandleChange_ = false;
    // accumulate all children's region rect for dirty merging when any child has been removed
    bool hasRemovedChild_ = false;
    bool hasChildrenOutOfRect_ = false;
    RectI childrenRect_;
    bool childHasFilter_ = false;  // only collect children filter status
    bool isChildrenSorted_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_BASE_RENDER_NODE_H
