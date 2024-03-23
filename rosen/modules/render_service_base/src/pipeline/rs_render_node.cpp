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

#include "pipeline/rs_render_node.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <set>

#include "rs_trace.h"
#include "animation/rs_render_animation.h"
#include "common/rs_obj_abs_geometry.h"
#include "benchmarks/file_utils.h"
#include "common/rs_optional_trace.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "property/rs_property_drawable.h"
#include "property/rs_property_trace.h"
#include "transaction/rs_transaction_proxy.h"
#include "visitor/rs_node_visitor.h"
#ifdef DDGR_ENABLE_FEATURE_OPINC
#include "rs_auto_cache.h"
#endif

#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#ifdef RS_ENABLE_VK
namespace {
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        return UINT32_MAX;
    }
    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton();
    VkPhysicalDevice physicalDevice = vkContext.GetPhysicalDevice();

    VkPhysicalDeviceMemoryProperties memProperties;
    vkContext.vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return UINT32_MAX;
}

void SetVkImageInfo(std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo,
    const VkImageCreateInfo& imageInfo)
{
    vkImageInfo->imageTiling = imageInfo.tiling;
    vkImageInfo->imageLayout = imageInfo.initialLayout;
    vkImageInfo->format = imageInfo.format;
    vkImageInfo->imageUsageFlags = imageInfo.usage;
    vkImageInfo->levelCount = imageInfo.mipLevels;
    vkImageInfo->currentQueueFamily = VK_QUEUE_FAMILY_EXTERNAL;
    vkImageInfo->ycbcrConversionInfo = {};
    vkImageInfo->sharingMode = imageInfo.sharingMode;
}

OHOS::Rosen::Drawing::BackendTexture MakeBackendTexture(uint32_t width, uint32_t height,
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM)
{
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkImageCreateInfo imageInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {width, height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton();
    VkDevice device = vkContext.GetDevice();
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    if (vkContext.vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        return {};
    }

    VkMemoryRequirements memRequirements;
    vkContext.vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (allocInfo.memoryTypeIndex == UINT32_MAX) {
        return {};
    }

    if (vkContext.vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        return {};
    }

    vkContext.vkBindImageMemory(device, image, memory, 0);

    OHOS::Rosen::RsVulkanMemStat& memStat = vkContext.GetRsVkMemStat();
    auto time = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
    std::string timeStamp = std::to_string(static_cast<uint64_t>(time.time_since_epoch().count()));
    memStat.InsertResource(timeStamp, static_cast<uint64_t>(memRequirements.size));
    OHOS::Rosen::Drawing::BackendTexture backendTexture(true);
    OHOS::Rosen::Drawing::TextureInfo textureInfo;
    textureInfo.SetWidth(width);
    textureInfo.SetHeight(height);

    std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo =
        std::make_shared<OHOS::Rosen::Drawing::VKTextureInfo>();
    vkImageInfo->vkImage = image;
    vkImageInfo->vkAlloc.memory = memory;
    vkImageInfo->vkAlloc.size = memRequirements.size;
    vkImageInfo->vkAlloc.statName = timeStamp;

    SetVkImageInfo(vkImageInfo, imageInfo);
    textureInfo.SetVKTextureInfo(vkImageInfo);
    backendTexture.SetTextureInfo(textureInfo);
    return backendTexture;
}
} // un-named
#endif

namespace OHOS {
namespace Rosen {
void RSRenderNode::OnRegister(const std::weak_ptr<RSContext>& context)
{
    context_ = context;
    renderContent_->type_ = GetType();
    renderContent_->GetMutableRenderProperties().backref_ = weak_from_this();
    SetDirty(true);
}

bool RSRenderNode::IsPureContainer() const
{
    auto& drawCmdModifiers_ = renderContent_->drawCmdModifiers_;
    return (drawCmdModifiers_.empty() && !GetRenderProperties().isDrawn_ && !GetRenderProperties().alphaNeedApply_);
}

bool RSRenderNode::IsContentNode() const
{
    auto& drawCmdModifiers_ = renderContent_->drawCmdModifiers_;
    return ((drawCmdModifiers_.size() == 1 && drawCmdModifiers_.count(RSModifierType::CONTENT_STYLE)) ||
               drawCmdModifiers_.empty()) &&
           !GetRenderProperties().isDrawn_;
}

namespace {
const std::set<RSModifierType> GROUPABLE_ANIMATION_TYPE = {
    RSModifierType::ALPHA,
    RSModifierType::ROTATION,
    RSModifierType::SCALE,
};
const std::set<RSModifierType> CACHEABLE_ANIMATION_TYPE = {
    RSModifierType::BOUNDS,
    RSModifierType::FRAME,
};
const std::set<RSModifierType> BASIC_GEOTRANSFORM_ANIMATION_TYPE = {
    RSModifierType::TRANSLATE,
    RSModifierType::SCALE,
};
}

RSRenderNode::RSRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : id_(id), context_(context), isTextureExportNode_(isTextureExportNode)
{}

RSRenderNode::RSRenderNode(
    NodeId id, bool isOnTheTree, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : isOnTheTree_(isOnTheTree), id_(id), context_(context), isTextureExportNode_(isTextureExportNode)
{}

void RSRenderNode::AddChild(SharedPtr child, int index)
{
    // sanity check, avoid loop
    if (child == nullptr || child->GetId() == GetId()) {
        return;
    }
    // if child already has a parent, remove it from its previous parent
    if (auto prevParent = child->GetParent().lock()) {
        prevParent->RemoveChild(child);
    }

    // Set parent-child relationship
    child->SetParent(weak_from_this());
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.emplace_back(child);
    } else {
        children_.emplace(std::next(children_.begin(), index), child);
    }

    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    // A child is not on the tree until its parent is on the tree
    if (isOnTheTree_) {
        child->SetIsOnTheTree(true, instanceRootNodeId_, firstLevelNodeId_, drawingCacheRootId_);
    }
    SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::SetContainBootAnimation(bool isContainBootAnimation)
{
    isContainBootAnimation_ = isContainBootAnimation;
    isFullChildrenListValid_ = false;
}

void RSRenderNode::MoveChild(SharedPtr child, int index)
{
    if (child == nullptr || child->GetParent().lock().get() != this) {
        return;
    }
    auto it = std::find_if(children_.begin(), children_.end(),
        [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSRenderNode>(ptr, child); });
    if (it == children_.end()) {
        return;
    }

    // Reset parent-child relationship
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        children_.emplace_back(child);
    } else {
        children_.emplace(std::next(children_.begin(), index), child);
    }
    children_.erase(it);
    SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::RemoveChild(SharedPtr child, bool skipTransition)
{
    if (child == nullptr) {
        return;
    }
    // break parent-child relationship
    auto it = std::find_if(children_.begin(), children_.end(),
        [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSRenderNode>(ptr, child); });
    if (it == children_.end()) {
        return;
    }
    // avoid duplicate entry in disappearingChildren_ (this should not happen)
    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    // if child has disappearing transition, add it to disappearingChildren_
    if (skipTransition == false && child->HasDisappearingTransition(true)) {
        ROSEN_LOGD("RSRenderNode::RemoveChild %{public}" PRIu64 " move child(id %{public}" PRIu64 ") into"
            " disappearingChildren", GetId(), child->GetId());
        // keep shared_ptr alive for transition
        uint32_t origPos = static_cast<uint32_t>(std::distance(children_.begin(), it));
        disappearingChildren_.emplace_back(child, origPos);
    } else {
        child->ResetParent();
    }
    children_.erase(it);
    if (child->GetBootAnimation()) {
        SetContainBootAnimation(false);
    }
    if (!children_.empty() || !disappearingChildren_.empty()) {
        isFullChildrenListValid_ = false;
    } else { // directly clear children list
        isFullChildrenListValid_ = true;
        isChildrenSorted_ = true;
        std::atomic_store_explicit(&fullChildrenList_, EmptyChildrenList, std::memory_order_release);
    }
}

void RSRenderNode::SetIsOnTheTree(bool flag, NodeId instanceRootNodeId, NodeId firstLevelNodeId, NodeId cacheNodeId)
{
    // We do not need to label a child when the child is removed from a parent that is not on the tree
    if (flag == isOnTheTree_) {
        return;
    }
    isNewOnTree_ = flag && !isOnTheTree_;
    isOnTheTree_ = flag;
    if (isOnTheTree_) {
        instanceRootNodeId_ = instanceRootNodeId;
        firstLevelNodeId_ = firstLevelNodeId;
        OnTreeStateChanged();
    } else {
        OnTreeStateChanged();
        instanceRootNodeId_ = instanceRootNodeId;
        firstLevelNodeId_ = firstLevelNodeId;
    }
    // if node is marked as cacheRoot, update subtree status when update surface
    // in case prepare stage upper cacheRoot cannot specify dirty subnode
    if (cacheNodeId != INVALID_NODEID) {
        drawingCacheRootId_ = cacheNodeId;
    }

    for (auto& weakChild : children_) {
        auto child = weakChild.lock();
        if (child == nullptr) {
            continue;
        }
        child->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    }

    for (auto& [child, _] : disappearingChildren_) {
        child->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    }
}

void RSRenderNode::UpdateChildrenRect(const RectI& subRect)
{
    if (!subRect.IsEmpty()) {
        if (childrenRect_.IsEmpty()) {
            // init as not empty subRect in case join RectI enlarging area
            childrenRect_ = subRect;
        } else {
            childrenRect_ = childrenRect_.JoinRect(subRect);
        }
    }
}

void RSRenderNode::AddCrossParentChild(const SharedPtr& child, int32_t index)
{
    // AddCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // so this child will not remove from the old parent.
    if (child == nullptr) {
        return;
    }

    // Set parent-child relationship
    child->SetParent(weak_from_this());
    if (index < 0 || index >= static_cast<int32_t>(children_.size())) {
        children_.emplace_back(child);
    } else {
        children_.emplace(std::next(children_.begin(), index), child);
    }

    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    // A child is not on the tree until its parent is on the tree
    if (isOnTheTree_) {
        child->SetIsOnTheTree(true, instanceRootNodeId_, firstLevelNodeId_, drawingCacheRootId_);
    }
    SetContentDirty();
    isFullChildrenListValid_ = false;
}

void RSRenderNode::RemoveCrossParentChild(const SharedPtr& child, const WeakPtr& newParent)
{
    // RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens),
    // set the newParentId to rebuild the parent-child relationship.
    if (child == nullptr) {
        return;
    }
    // break parent-child relationship
    auto it = std::find_if(children_.begin(), children_.end(),
        [&](WeakPtr& ptr) -> bool { return ROSEN_EQ<RSRenderNode>(ptr, child); });
    if (it == children_.end()) {
        return;
    }
    // avoid duplicate entry in disappearingChildren_ (this should not happen)
    disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    // if child has disappearing transition, add it to disappearingChildren_
    if (child->HasDisappearingTransition(true)) {
        ROSEN_LOGD("RSRenderNode::RemoveChild %{public}" PRIu64 " move child(id %{public}" PRIu64 ")"
            " into disappearingChildren", GetId(), child->GetId());
        // keep shared_ptr alive for transition
        uint32_t origPos = static_cast<uint32_t>(std::distance(children_.begin(), it));
        disappearingChildren_.emplace_back(child, origPos);
    } else {
        child->SetParent(newParent);
        // attention: set new parent means 'old' parent has removed this child
        hasRemovedChild_ = true;
    }
    children_.erase(it);
    SetContentDirty();
    if (!children_.empty() || !disappearingChildren_.empty()) {
        isFullChildrenListValid_ = false;
    } else { // directly clear children list
        isFullChildrenListValid_ = true;
        isChildrenSorted_ = true;
        std::atomic_store_explicit(&fullChildrenList_, EmptyChildrenList, std::memory_order_release);
    }
}

void RSRenderNode::RemoveFromTree(bool skipTransition)
{
    auto parentPtr = parent_.lock();
    if (parentPtr == nullptr) {
        return;
    }
    auto child = shared_from_this();
    parentPtr->RemoveChild(child, skipTransition);
    if (skipTransition == false) {
        return;
    }
    // force remove child from disappearingChildren_ and clean sortChildren_ cache
    parentPtr->disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
    parentPtr->isFullChildrenListValid_ = false;
    child->ResetParent();
}

void RSRenderNode::ClearChildren()
{
    if (children_.empty()) {
        return;
    }
    // Cache the parent's transition state to avoid redundant recursively check
    bool parentHasDisappearingTransition = HasDisappearingTransition(true);
    uint32_t pos = 0;
    for (auto& childWeakPtr : children_) {
        auto child = childWeakPtr.lock();
        if (child == nullptr) {
            ++pos;
            continue;
        }
        // avoid duplicate entry in disappearingChildren_ (this should not happen)
        disappearingChildren_.remove_if([&child](const auto& pair) -> bool { return pair.first == child; });
        if (parentHasDisappearingTransition || child->HasDisappearingTransition(false)) {
            // keep shared_ptr alive for transition
            disappearingChildren_.emplace_back(child, pos);
        } else {
            child->ResetParent();
        }
        ++pos;
    }
    children_.clear();
    SetContentDirty();
    if (!disappearingChildren_.empty()) {
        isFullChildrenListValid_ = false;
    } else { // directly clear children list
        isFullChildrenListValid_ = true;
        isChildrenSorted_ = true;
        std::atomic_store_explicit(&fullChildrenList_, EmptyChildrenList, std::memory_order_release);
    }
}

void RSRenderNode::SetParent(WeakPtr parent)
{
    UpdateSubSurfaceCnt(parent.lock(), parent_.lock());
    parent_ = parent;
    if (isSubSurfaceEnabled_) {
        AddSubSurfaceNode(shared_from_this(), parent.lock());
    }
}

void RSRenderNode::ResetParent()
{
    if (auto parentNode = parent_.lock()) {
        if (isSubSurfaceEnabled_) {
            auto it = std::find_if(parentNode->disappearingChildren_.begin(), parentNode->disappearingChildren_.end(),
                [childPtr = shared_from_this()](const auto& pair) -> bool { return pair.first == childPtr; });
            if (it == parentNode->disappearingChildren_.end()) {
                RemoveSubSurfaceNode(shared_from_this(), parentNode);
            }
        }
        parentNode->hasRemovedChild_ = true;
        parentNode->SetContentDirty();
        UpdateSubSurfaceCnt(nullptr, parentNode);
    }
    parent_.reset();
    SetIsOnTheTree(false);
    OnResetParent();
}

bool RSRenderNode::IsFirstLevelSurfaceNode()
{
    if (!this->IsInstanceOf<RSSurfaceRenderNode>()) {
        return false;
    }
    auto parentNode = parent_.lock();
    while (parentNode && !parentNode->IsInstanceOf<RSDisplayRenderNode>()) {
        auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentNode);
        if (node != nullptr && (node->IsLeashOrMainWindow())) {
            return false;
        }
        parentNode = parentNode->GetParent().lock();
    }
    return true;
}

bool RSRenderNode::SubSurfaceNodeNeedDraw(PartialRenderType opDropType)
{
    for (auto &nodes : subSurfaceNodes_) {
        for (auto &node : nodes.second) {
            const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node.lock());
            if (surfaceNode != nullptr &&
                (surfaceNode->SubNodeNeedDraw(surfaceNode->GetOldDirtyInSurface(), opDropType) ||
                surfaceNode->SubSurfaceNodeNeedDraw(opDropType))) {
                return true;
            }
        }
    }
    return false;
}

void RSRenderNode::AddSubSurfaceNode(SharedPtr child, SharedPtr parent)
{
    if (parent->subSurfaceNodes_.find(child->GetId()) != parent->subSurfaceNodes_.end()) {
        return;
    }
    std::vector<WeakPtr> subSurfaceNodes;
    if (child->IsInstanceOf<RSSurfaceRenderNode>() &&
        (RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child)->IsMainWindowType() ||
        RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child)->IsLeashWindow())) {
        subSurfaceNodes.push_back(child);
    } else {
        for (auto &nodes : child->subSurfaceNodes_) {
            subSurfaceNodes.insert(subSurfaceNodes.end(), nodes.second.begin(), nodes.second.end());
        }
    }
    if (subSurfaceNodes.size() == 0) {
        return;
    }

    auto childNode = child;
    auto parentNode = parent;
    while (parentNode && !parentNode->IsInstanceOf<RSDisplayRenderNode>()) {
        auto id = childNode->GetId();
        if (parentNode->subSurfaceNodes_.find(id) == parentNode->subSurfaceNodes_.end()) {
            parentNode->subSurfaceNodes_.insert({id, subSurfaceNodes});
        } else {
            parentNode->subSurfaceNodes_[id].insert(parentNode->subSurfaceNodes_[id].end(),
                subSurfaceNodes.begin(), subSurfaceNodes.end());
        }
        std::sort(parentNode->subSurfaceNodes_[id].begin(), parentNode->subSurfaceNodes_[id].end(),
            [](const auto &first, const auto &second) {
            return
                first.lock()->GetRenderProperties().GetPositionZ() <
                second.lock()->GetRenderProperties().GetPositionZ();
        });
        if (parentNode->IsInstanceOf<RSSurfaceRenderNode>()) {
            break;
        }
        childNode = parentNode;
        parentNode = parentNode->GetParent().lock();
    }
}

void RSRenderNode::RemoveSubSurfaceNode(SharedPtr child, SharedPtr parent)
{
    if (parent->subSurfaceNodes_.find(child->GetId()) == parent->subSurfaceNodes_.end()) {
        return;
    }
    auto subSurfaceNodes = parent->subSurfaceNodes_[child->GetId()];
    parent->subSurfaceNodes_.erase(child->GetId());
    auto childNode = parent;
    auto parentNode = parent->GetParent().lock();
    while (parentNode && !parentNode->IsInstanceOf<RSDisplayRenderNode>()) {
        auto id = childNode->GetId();
        for (auto iter : subSurfaceNodes) {
            parentNode->subSurfaceNodes_[id].erase(
                remove_if(parentNode->subSurfaceNodes_[id].begin(), parentNode->subSurfaceNodes_[id].end(),
                    [iter](WeakPtr it) {
                        return iter.lock() && it.lock() && iter.lock()->GetId() == it.lock()->GetId();
                    }),
                parentNode->subSurfaceNodes_[id].end()
            );
        }
        if (parentNode->subSurfaceNodes_[id].size() == 0) {
            parentNode->subSurfaceNodes_.erase(id);
        }
        if (parentNode->IsInstanceOf<RSSurfaceRenderNode>()) {
            break;
        }
        childNode = parentNode;
        parentNode = parentNode->GetParent().lock();
    }
}

void RSRenderNode::DumpTree(int32_t depth, std::string& out) const
{
    for (int32_t i = 0; i < depth; ++i) {
        out += "  ";
    }
    out += "| ";
    DumpNodeType(out);
    out += "[" + std::to_string(GetId()) + "], instanceRootNodeId" + "[" +
        std::to_string(GetInstanceRootNodeId()) + "]";
    if (IsSuggestedDrawInGroup()) {
        out += ", [node group]";
    }
    DumpSubClassNode(out);
    out += ", Properties: " + GetRenderProperties().Dump();
    out += ", GetBootAnimation: " + std::to_string(GetBootAnimation());
    out += ", isContainBootAnimation_: " + std::to_string(isContainBootAnimation_);
    out += ", isNodeDirty: " + std::to_string(static_cast<int>(dirtyStatus_));
    out += ", isPropertyDirty: " + std::to_string(GetRenderProperties().IsDirty());
    out += ", IsPureContainer: " + std::to_string(IsPureContainer());
    DumpDrawCmdModifiers(out);
    animationManager_.DumpAnimations(out);
    out += "\n";

    for (auto& child : *GetSortedChildren()) {
        child->DumpTree(depth + 1, out);
    }
    for (auto& [child, pos] : disappearingChildren_) {
        child->DumpTree(depth + 1, out);
    }
}

void RSRenderNode::DumpNodeType(std::string& out) const
{
    switch (GetType()) {
        case RSRenderNodeType::DISPLAY_NODE: {
            out += "DISPLAY_NODE";
            break;
        }
        case RSRenderNodeType::RS_NODE: {
            out += "RS_NODE";
            break;
        }
        case RSRenderNodeType::SURFACE_NODE: {
            out += "SURFACE_NODE";
            break;
        }
        case RSRenderNodeType::CANVAS_NODE: {
            out += "CANVAS_NODE";
            break;
        }
        case RSRenderNodeType::ROOT_NODE: {
            out += "ROOT_NODE";
            break;
        }
        case RSRenderNodeType::PROXY_NODE: {
            out += "PROXY_NODE";
            break;
        }
        case RSRenderNodeType::CANVAS_DRAWING_NODE: {
            out += "CANVAS_DRAWING_NODE";
            break;
        }
        default: {
            out += "UNKNOWN_NODE";
            break;
        }
    }
}

void RSRenderNode::DumpSubClassNode(std::string& out) const
{
    if (GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto surfaceNode = (static_cast<const RSSurfaceRenderNode*>(this));
        auto p = parent_.lock();
        out += ", Parent [" + (p != nullptr ? std::to_string(p->GetId()) : "null") + "]";
        out += ", Name [" + surfaceNode->GetName() + "]";
        const RSSurfaceHandler& surfaceHandler = static_cast<const RSSurfaceHandler&>(*surfaceNode);
        out += ", hasConsumer: " + std::to_string(surfaceHandler.HasConsumer());
        std::string contextAlpha = std::to_string(surfaceNode->contextAlpha_);
        std::string propertyAlpha = std::to_string(surfaceNode->GetRenderProperties().GetAlpha());
        out += ", Alpha: " + propertyAlpha + " (include ContextAlpha: " + contextAlpha + ")";
        out += ", Visible: " + std::to_string(surfaceNode->GetRenderProperties().GetVisible());
        out += ", Visible" + surfaceNode->GetVisibleRegion().GetRegionInfo();
        out += ", Opaque" + surfaceNode->GetOpaqueRegion().GetRegionInfo();
        out += ", OcclusionBg: " + std::to_string(surfaceNode->GetAbilityBgAlpha());
        out += ", SecurityLayer: " + std::to_string(surfaceNode->GetSecurityLayer());
        out += ", skipLayer: " + std::to_string(surfaceNode->GetSkipLayer());
    } else if (GetType() == RSRenderNodeType::ROOT_NODE) {
        auto rootNode = static_cast<const RSRootRenderNode*>(this);
        out += ", Visible: " + std::to_string(rootNode->GetRenderProperties().GetVisible());
        out += ", Size: [" + std::to_string(rootNode->GetRenderProperties().GetFrameWidth()) + ", " +
            std::to_string(rootNode->GetRenderProperties().GetFrameHeight()) + "]";
        out += ", EnableRender: " + std::to_string(rootNode->GetEnableRender());
    } else if (GetType() == RSRenderNodeType::DISPLAY_NODE) {
        auto displayNode = static_cast<const RSDisplayRenderNode*>(this);
        out += ", skipLayer: " + std::to_string(displayNode->GetSecurityDisplay());
    }
}

void RSRenderNode::DumpDrawCmdModifiers(std::string& out) const
{
    if (renderContent_->drawCmdModifiers_.empty()) {
        return;
    }
    std::string splitStr = ", ";
    std::string modifierDesc = ", DrawCmdModifiers:[";
    for (auto& [type, modifiers] : renderContent_->drawCmdModifiers_) {
        modifierDesc += "type:" + std::to_string(static_cast<int>(type)) + ", modifiers: ";
        std::string propertyDesc = "Property_";
        for (auto& modifier : modifiers) {
            DumpDrawCmdModifier(propertyDesc, type, modifier);
            propertyDesc += "], ";
        }
        modifierDesc += propertyDesc.substr(0, propertyDesc.length() - splitStr.length());
        modifierDesc += splitStr;
    }
    out += modifierDesc.substr(0, modifierDesc.length() - splitStr.length());
    out += "]";
}

void RSRenderNode::DumpDrawCmdModifier(std::string& propertyDesc, RSModifierType type,
    std::shared_ptr<RSRenderModifier>& modifier) const
{
    if (type < RSModifierType::ENV_FOREGROUND_COLOR) {
        propertyDesc += "drawCmdList:[";
        auto propertyValue = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>
            (modifier->GetProperty())->Get();
        propertyDesc += propertyValue->GetOpsWithDesc();
        std::string::size_type pos = 0;
        while ((pos = propertyDesc.find("\n", pos)) != std::string::npos) {
            propertyDesc.replace(pos, 1, ",");
        }
        propertyDesc.pop_back();
    } else if (type == RSModifierType::ENV_FOREGROUND_COLOR) {
        propertyDesc += "ENV_FOREGROUND_COLOR:[Rgba-";
        auto propertyValue = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>
            (modifier->GetProperty())->Get();
        propertyDesc += std::to_string(propertyValue.AsRgbaInt());
    } else if (type == RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY) {
        propertyDesc += "ENV_FOREGROUND_COLOR_STRATEGY:[";
        auto propertyValue = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>
            (modifier->GetProperty())->Get();
        propertyDesc += std::to_string(static_cast<int>(propertyValue));
    } else if (type == RSModifierType::GEOMETRYTRANS) {
        propertyDesc += "GEOMETRYTRANS:[";
        auto propertyValue = std::static_pointer_cast<RSRenderProperty<SkMatrix>>
            (modifier->GetProperty())->Get();
        propertyValue.dump(propertyDesc, 0);
    }
}

// attention: current all base node's dirty ops causing content dirty
void RSRenderNode::SetContentDirty()
{
    isContentDirty_ = true;
    isOnlyBasicGeoTransform_ = false;
    SetDirty();
}

void RSRenderNode::SetDirty(bool forceAddToActiveList)
{
    bool dirtyEmpty = dirtyTypes_.none();
    // TO avoid redundant add, only add if both: 1. on-tree node 2. newly dirty node (or forceAddToActiveList = true)
    if (dirtyStatus_ == NodeDirty::CLEAN || dirtyEmpty || forceAddToActiveList) {
        if (auto context = GetContext().lock()) {
            context->AddActiveNode(shared_from_this());
        }
    }
    dirtyStatus_ = NodeDirty::DIRTY;
}

void RSRenderNode::CollectSurface(
    const std::shared_ptr<RSRenderNode>& node, std::vector<RSRenderNode::SharedPtr>& vec, bool isUniRender,
    bool onlyFirstLevel)
{
    for (auto& child : *node->GetSortedChildren()) {
        child->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
    }
}

void RSRenderNode::CollectSurfaceForUIFirstSwitch(uint32_t& leashWindowCount, uint32_t minNodeNum)
{
    for (auto& child : *GetSortedChildren()) {
        child->CollectSurfaceForUIFirstSwitch(leashWindowCount, minNodeNum);
        if (leashWindowCount >= minNodeNum) {
            return;
        }
    }
}

void RSRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareChildren(*this);
}

void RSRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->ProcessChildren(*this);
}

void RSRenderNode::SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommandFromRT(command, nodeId);
    }
}

void RSRenderNode::InternalRemoveSelfFromDisappearingChildren()
{
    // internal use only, force remove self from parent's disappearingChildren_
    auto parent = parent_.lock();
    if (parent == nullptr) {
        return;
    }
    auto it = std::find_if(parent->disappearingChildren_.begin(), parent->disappearingChildren_.end(),
        [childPtr = shared_from_this()](const auto& pair) -> bool { return pair.first == childPtr; });
    if (it == parent->disappearingChildren_.end()) {
        return;
    }
    parent->disappearingChildren_.erase(it);
    parent->isFullChildrenListValid_ = false;
    ResetParent();
}

RSRenderNode::~RSRenderNode()
{
    if (appPid_ != 0) {
        RSSingleFrameComposer::AddOrRemoveAppPidToMap(false, appPid_);
    }
    if (fallbackAnimationOnDestroy_) {
        FallbackAnimationsToRoot();
    }
    if (clearCacheSurfaceFunc_ && (cacheSurface_ || cacheCompletedSurface_)) {
        clearCacheSurfaceFunc_(std::move(cacheSurface_), std::move(cacheCompletedSurface_), cacheSurfaceThreadIndex_,
            completedSurfaceThreadIndex_);
    }
    ClearCacheSurface();
}

void RSRenderNode::FallbackAnimationsToRoot()
{
    if (animationManager_.animations_.empty()) {
        return;
    }

    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("Invalid context");
        return;
    }
    auto target = context->GetNodeMap().GetAnimationFallbackNode();
    if (!target) {
        ROSEN_LOGE("Failed to move animation to root, root render node is null!");
        return;
    }
    context->RegisterAnimatingRenderNode(target);

    for (auto& [unused, animation] : animationManager_.animations_) {
        animation->Detach(true);
        // avoid infinite loop for fallback animation
        animation->SetRepeatCount(1);
        target->animationManager_.AddAnimation(std::move(animation));
    }
    animationManager_.animations_.clear();
}

void RSRenderNode::ActivateDisplaySync()
{
    if (!displaySync_) {
        displaySync_ = std::make_shared<RSRenderDisplaySync>(GetId());
    }
}

void RSRenderNode::UpdateDisplaySyncRange()
{
    if (!displaySync_) {
        return;
    }
    auto animationRange = animationManager_.GetFrameRateRange();
    if (animationRange.IsValid()) {
        displaySync_->SetExpectedFrameRateRange(animationRange);
    }
}

std::tuple<bool, bool, bool> RSRenderNode::Animate(int64_t timestamp, int64_t period, bool isDisplaySyncEnabled)
{
    if (displaySync_ && displaySync_->OnFrameSkip(timestamp, period, isDisplaySyncEnabled)) {
        return displaySync_->GetAnimateResult();
    }
    RS_OPTIONAL_TRACE_BEGIN("RSRenderNode:Animate node id: [" + std::to_string(GetId()) + "]");
    auto animateResult = animationManager_.Animate(timestamp, IsOnTheTree());
    if (displaySync_) {
        displaySync_->SetAnimateResult(animateResult);
    }
    RS_OPTIONAL_TRACE_END();
    return animateResult;
}

bool RSRenderNode::IsClipBound() const
{
    return GetRenderProperties().GetClipBounds() || GetRenderProperties().GetClipToFrame();
}

bool RSRenderNode::Update(
    RSDirtyRegionManager& dirtyManager, const std::shared_ptr<RSRenderNode>& parent, bool parentDirty,
    std::optional<RectI> clipRect)
{
    // no need to update invisible nodes
    if (!ShouldPaint() && !isLastVisible_) {
        SetClean();
        GetMutableRenderProperties().ResetDirty();
        return false;
    }
    // [planning] surfaceNode use frame instead
    std::optional<Drawing::Point> offset;
    if (parent != nullptr && !IsInstanceOf<RSSurfaceRenderNode>()) {
        auto& properties = parent->GetRenderProperties();
        offset = Drawing::Point { properties.GetFrameOffsetX(), properties.GetFrameOffsetY() };
    }
    // in some case geodirty_ is not marked in drawCmdModifiers_, we should update node geometry
    // [planing] using drawcmdModifierDirty from dirtyType_
    parentDirty = parentDirty || (dirtyStatus_ != NodeDirty::CLEAN);
    auto parentProperties = parent ? &parent->GetRenderProperties() : nullptr;
    bool dirty = GetMutableRenderProperties().UpdateGeometry(parentProperties, parentDirty, offset,
        GetContextClipRegion());
    if ((IsDirty() || dirty) && renderContent_->drawCmdModifiers_.count(RSModifierType::GEOMETRYTRANS)) {
        RSModifierContext context = { GetMutableRenderProperties() };
        for (auto& modifier : renderContent_->drawCmdModifiers_[RSModifierType::GEOMETRYTRANS]) {
            modifier->Apply(context);
        }
    }
    isDirtyRegionUpdated_ = false;
    isLastVisible_ = ShouldPaint();
    GetMutableRenderProperties().ResetDirty();

    // Note:
    // 1. cache manager will use dirty region to update cache validity, background filter cache manager should use
    // 'dirty region of all the nodes drawn before this node', and foreground filter cache manager should use 'dirty
    // region of all the nodes drawn before this node, this node, and the children of this node'
    // 2. Filter must be valid when filter cache manager is valid, we make sure that in RSRenderNode::ApplyModifiers().
    UpdateFilterCacheWithDirty(dirtyManager, false);
    UpdateDirtyRegion(dirtyManager, dirty, clipRect);
    return dirty;
}

RSProperties& RSRenderNode::GetMutableRenderProperties()
{
    return renderContent_->GetMutableRenderProperties();
}

void RSRenderNode::UpdateDirtyRegion(
    RSDirtyRegionManager& dirtyManager, bool geoDirty, std::optional<RectI> clipRect)
{
    if (!IsDirty() && !geoDirty) {
        return;
    }
    if (RSSystemProperties::GetSkipGeometryNotChangeEnabled()) {
        // while node absRect not change and other content not change, return directly for not generate dirty region
        if (!IsSelfDrawingNode() && !geometryChangeNotPerceived_ && !geoDirty) {
            return;
        }
        geometryChangeNotPerceived_ = false;
    }
    if (!oldDirty_.IsEmpty()) {
        dirtyManager.MergeDirtyRect(oldDirty_);
    }
    // merge old dirty if switch to invisible
    if (!ShouldPaint() && isLastVisible_) {
        ROSEN_LOGD("RSRenderNode:: id %{public}" PRIu64 " UpdateDirtyRegion visible->invisible", GetId());
    } else {
        RectI drawRegion;
        RectI shadowRect;
        auto dirtyRect = GetRenderProperties().GetDirtyRect(drawRegion);
        auto rectFromRenderProperties = dirtyRect;
        if (GetRenderProperties().IsShadowValid()) {
            SetShadowValidLastFrame(true);
            if (IsInstanceOf<RSSurfaceRenderNode>()) {
                const RectF absBounds = {0, 0, GetRenderProperties().GetBoundsWidth(),
                    GetRenderProperties().GetBoundsHeight()};
                RRect absClipRRect = RRect(absBounds, GetRenderProperties().GetCornerRadius());
                RSPropertiesPainter::GetShadowDirtyRect(shadowRect, GetRenderProperties(), &absClipRRect);
            } else {
                RSPropertiesPainter::GetShadowDirtyRect(shadowRect, GetRenderProperties());
            }
            if (!shadowRect.IsEmpty()) {
                dirtyRect = dirtyRect.JoinRect(shadowRect);
            }
        }

        auto& outline = GetRenderProperties().GetOutline();
        RectI outlineRect;
        if (outline && outline->HasBorder()) {
            RSPropertiesPainter::GetOutlineDirtyRect(outlineRect, GetRenderProperties());
            if (!outlineRect.IsEmpty()) {
                dirtyRect = dirtyRect.JoinRect(outlineRect);
            }
        }

        if (GetRenderProperties().pixelStretch_) {
            auto stretchDirtyRect = GetRenderProperties().GetPixelStretchDirtyRect();
            dirtyRect = dirtyRect.JoinRect(stretchDirtyRect);
        }

        if (clipRect.has_value()) {
            dirtyRect = dirtyRect.IntersectRect(*clipRect);
        }
        oldDirty_ = dirtyRect;
        oldDirtyInSurface_ = oldDirty_.IntersectRect(dirtyManager.GetSurfaceRect());
        // filter invalid dirtyrect
        if (!dirtyRect.IsEmpty()) {
            dirtyManager.MergeDirtyRect(dirtyRect);
            isDirtyRegionUpdated_ = true;
            // save types of dirty region of target dirty manager for dfx
            if (dirtyManager.IsTargetForDfx() &&
                (GetType() == RSRenderNodeType::CANVAS_NODE || GetType() == RSRenderNodeType::SURFACE_NODE)) {
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::UPDATE_DIRTY_REGION, oldDirtyInSurface_);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::OVERLAY_RECT, drawRegion);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::SHADOW_RECT, shadowRect);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::PREPARE_CLIP_RECT, clipRect.value_or(RectI()));
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::RENDER_PROPERTIES_RECT, rectFromRenderProperties);
                dirtyManager.UpdateDirtyRegionInfoForDfx(
                    GetId(), GetType(), DirtyRegionType::OUTLINE_RECT, outlineRect);
            }
        }
    }

    SetClean();
}

bool RSRenderNode::IsSelfDrawingNode() const
{
    return GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE || GetType() == RSRenderNodeType::SURFACE_NODE;
}

bool RSRenderNode::IsDirty() const
{
    return dirtyStatus_ != NodeDirty::CLEAN || GetRenderProperties().IsDirty();
}

bool RSRenderNode::IsContentDirty() const
{
    // Considering renderNode, it should consider both basenode's case and its properties
    return isContentDirty_ || GetRenderProperties().IsContentDirty();
}

void RSRenderNode::UpdateRenderStatus(RectI& dirtyRegion, bool isPartialRenderEnabled)
{
    auto dirtyRect = GetRenderProperties().GetDirtyRect();
    // should judge if there's any child out of parent
    if (!isPartialRenderEnabled || HasChildrenOutOfRect()) {
        isRenderUpdateIgnored_ = false;
    } else if (dirtyRegion.IsEmpty() || dirtyRect.IsEmpty()) {
        isRenderUpdateIgnored_ = true;
    } else {
        RectI intersectRect = dirtyRegion.IntersectRect(dirtyRect);
        isRenderUpdateIgnored_ = intersectRect.IsEmpty();
    }
}

void RSRenderNode::UpdateParentChildrenRect(std::shared_ptr<RSRenderNode> parentNode) const
{
    if (!ShouldPaint() || (oldDirty_.IsEmpty() && GetChildrenRect().IsEmpty())) {
        return;
    }
    auto renderParent = (parentNode);
    if (renderParent) {
        // accumulate current node's all children region(including itself)
        // apply oldDirty_ as node's real region(including overlay and shadow)
        RectI accumulatedRect = GetChildrenRect().JoinRect(oldDirty_);
        renderParent->UpdateChildrenRect(accumulatedRect);
        // check each child is inside of parent
        if (!accumulatedRect.IsInsideOf(renderParent->GetOldDirty())) {
            renderParent->UpdateChildrenOutOfRectFlag(true);
        }
    }
}

bool RSRenderNode::IsBackgroundFilterCacheValid() const
{
    auto& manager = GetRenderProperties().GetFilterCacheManager(false);
    if (manager == nullptr) {
        return false;
    }
    bool backgroundFilterCacheValid = manager->IsCacheValid();
    return backgroundFilterCacheValid;
}

void RSRenderNode::UpdateFilterCacheWithDirty(RSDirtyRegionManager& dirtyManager, bool isForeground)
{
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!RSProperties::FilterCacheEnabled) {
        return;
    }
    auto& properties = GetRenderProperties();
    auto& manager = properties.GetFilterCacheManager(isForeground);
    if (manager == nullptr) {
        return;
    }
    if (!manager->IsCacheValid()) {
        return;
    }
    auto& cachedImageRect = manager->GetCachedImageRegion();
    if (manager->UpdateCacheStateWithDirtyRegion(dirtyManager)) {
        dirtyManager.MergeDirtyRect(cachedImageRect);
    }
#endif
}

void RSRenderNode::RenderTraceDebug() const
{
    if (RSSystemProperties::GetRenderNodeTraceEnabled()) {
        RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(GetId(), GetRenderProperties());
        RSPropertyTrace::GetInstance().TracePropertiesByNodeName(GetId(), GetNodeName(), GetRenderProperties());
    }
}

void RSRenderNode::ApplyBoundsGeometry(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::BOUNDS_MATRIX, canvas);
}

void RSRenderNode::ApplyAlpha(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawable(RSPropertyDrawableSlot::ALPHA, canvas);
}

void RSRenderNode::ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawableRange(RSPropertyDrawableSlot::SAVE_ALL, RSPropertyDrawableSlot::MASK, canvas);
}

void RSRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    RSRenderNode::ProcessTransitionBeforeChildren(canvas);
}

void RSRenderNode::ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawable(RSPropertyDrawableSlot::RESTORE_ALL, canvas);
}

void RSRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    DrawPropertyDrawable(RSPropertyDrawableSlot::RESTORE_ALL, canvas);
}

void RSRenderNode::AddModifier(const std::shared_ptr<RSRenderModifier>& modifier, bool isSingleFrameComposer)
{
    if (!modifier) {
        return;
    }
    SetDirty();
    if (RSSystemProperties::GetSingleFrameComposerEnabled() &&
        GetNodeIsSingleFrameComposer() && isSingleFrameComposer) {
        if (singleFrameComposer_ == nullptr) {
            singleFrameComposer_ = std::make_shared<RSSingleFrameComposer>();
        }
        singleFrameComposer_->SingleFrameAddModifier(modifier);
        return;
    }
    if (modifier->GetType() == RSModifierType::BOUNDS || modifier->GetType() == RSModifierType::FRAME) {
        AddGeometryModifier(modifier);
    } else if (modifier->GetType() < RSModifierType::CUSTOM) {
        modifiers_.emplace(modifier->GetPropertyId(), modifier);
    } else {
        modifier->SetSingleFrameModifier(false);
        renderContent_->drawCmdModifiers_[modifier->GetType()].emplace_back(modifier);
    }
    modifier->GetProperty()->Attach(shared_from_this());
}

void RSRenderNode::AddGeometryModifier(const std::shared_ptr<RSRenderModifier>& modifier)
{
    // bounds and frame modifiers must be unique
    if (modifier->GetType() == RSModifierType::BOUNDS) {
        if (boundsModifier_ == nullptr) {
            boundsModifier_ = modifier;
        } else {
            boundsModifier_->Update(modifier->GetProperty(), false);
        }
        modifiers_.emplace(modifier->GetPropertyId(), boundsModifier_);
    }

    if (modifier->GetType() == RSModifierType::FRAME) {
        if (frameModifier_ == nullptr) {
            frameModifier_ = modifier;
        } else {
            frameModifier_->Update(modifier->GetProperty(), false);
        }
        modifiers_.emplace(modifier->GetPropertyId(), frameModifier_);
    }
}

void RSRenderNode::RemoveModifier(const PropertyId& id)
{
    SetDirty();
    auto it = modifiers_.find(id);
    if (it != modifiers_.end()) {
        if (it->second) {
            AddDirtyType(it->second->GetType());
        }
        modifiers_.erase(it);
        return;
    }
    for (auto& [type, modifiers] : renderContent_->drawCmdModifiers_) {
        modifiers.remove_if([id](const auto& modifier) -> bool {
            return modifier ? modifier->GetPropertyId() == id : true;
        });
    }
}

void RSRenderNode::DumpNodeInfo(DfxString& log)
{
// Drawing is not supported
}

bool RSRenderNode::ApplyModifiers()
{
    // quick reject test
    if (!RSRenderNode::IsDirty() || dirtyTypes_.none()) {
        return false;
    }
    const auto prevPositionZ = GetRenderProperties().GetPositionZ();

    // Reset and re-apply all modifiers
    RSModifierContext context = { GetMutableRenderProperties() };

    // Reset before apply modifiers
    GetMutableRenderProperties().ResetProperty(dirtyTypes_);

    // Apply modifiers
    for (auto& [id, modifier] : modifiers_) {
        if (!dirtyTypes_.test(static_cast<size_t>(modifier->GetType()))) {
            continue;
        }
        modifier->Apply(context);
        if (!BASIC_GEOTRANSFORM_ANIMATION_TYPE.count(modifier->GetType())) {
            isOnlyBasicGeoTransform_ = false;
        }
    }
    // execute hooks
    GetMutableRenderProperties().OnApplyModifiers();
    OnApplyModifiers();

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (auto& manager = GetRenderProperties().GetFilterCacheManager(false);
        manager != nullptr &&
        (dirtyTypes_.test(static_cast<size_t>(RSModifierType::BACKGROUND_COLOR)) ||
        dirtyTypes_.test(static_cast<size_t>(RSModifierType::BG_IMAGE)) ||
        dirtyTypes_.test(static_cast<size_t>(RSModifierType::BACKGROUND_SHADER)))) {
        manager->InvalidateCache();
    }
    if (auto& manager = GetRenderProperties().GetFilterCacheManager(true)) {
        manager->InvalidateCache();
    }

    // Generate drawable
    UpdateDrawableVec();
#endif

    // update state
    dirtyTypes_.reset();
    UpdateShouldPaint();

    // update rate decider scale reference size.
    animationManager_.SetRateDeciderScaleSize(GetRenderProperties().GetBoundsWidth(),
        GetRenderProperties().GetBoundsHeight());

    // return true if positionZ changed
    return GetRenderProperties().GetPositionZ() != prevPositionZ;
}

void RSRenderNode::UpdateDrawableVec()
{
    // Collect dirty slots
    auto dirtySlots = RSPropertyDrawable::GenerateDirtySlots(GetRenderProperties(), dirtyTypes_);
    if (!GetIsUsedBySubThread()) {
        UpdateDrawableVecInternal(dirtySlots);
    } else if (auto context = context_.lock()) {
        context->PostTask([weakPtr = weak_from_this(), dirtySlots]() {
            if (auto node = weakPtr.lock()) {
                node->UpdateDrawableVecInternal(dirtySlots);
            }
        });
    } else {
        ROSEN_LOGI("%{public}s GetIsUsedBySubThread[%{public}d].", __func__, GetIsUsedBySubThread());
        UpdateDrawableVecInternal(dirtySlots);
    }
}

void RSRenderNode::UpdateDrawableVecInternal(std::unordered_set<RSPropertyDrawableSlot> dirtySlots)
{
     // initialize necessary save/clip/restore
    if (drawableVecStatus_ == 0) {
        RSPropertyDrawable::InitializeSaveRestore(*renderContent_, renderContent_->propertyDrawablesVec_);
    }
    // Update or regenerate drawable
    bool drawableChanged =
        RSPropertyDrawable::UpdateDrawableVec(*renderContent_, renderContent_->propertyDrawablesVec_, dirtySlots);
    // if 1. first initialized or 2. any drawables changed, update save/clip/restore
    if (drawableChanged || drawableVecStatus_ == 0) {
        RSPropertyDrawable::UpdateSaveRestore(
            *renderContent_, renderContent_->propertyDrawablesVec_, drawableVecStatus_);
    }
}

void RSRenderNode::UpdateEffectRegion(std::optional<Drawing::RectI>& region, bool isForced)
{
    if (!region.has_value()) {
        return;
    }
    const auto& property = GetRenderProperties();
    if (!isForced && !property.GetUseEffect()) {
        return;
    }

    auto absRect = property.GetBoundsGeometry()->GetAbsRect();
    region->Join(Drawing::RectI(absRect.GetLeft(), absRect.GetTop(), absRect.GetRight(), absRect.GetBottom()));
}

std::shared_ptr<RSRenderModifier> RSRenderNode::GetModifier(const PropertyId& id)
{
    if (modifiers_.count(id)) {
        return modifiers_[id];
    }
    for (const auto& [type, modifiers] : renderContent_->drawCmdModifiers_) {
        auto it = std::find_if(modifiers.begin(), modifiers.end(),
            [id](const auto& modifier) -> bool { return modifier->GetPropertyId() == id; });
        if (it != modifiers.end()) {
            return *it;
        }
    }
    return nullptr;
}

void RSRenderNode::FilterModifiersByPid(pid_t pid)
{
    // remove all modifiers added by given pid (by matching higher 32 bits of node id)
    EraseIf(modifiers_, [pid](const auto& pair) -> bool { return ExtractPid(pair.first) == pid; });

    // remove all modifiers added by given pid (by matching higher 32 bits of node id)
    for (auto& [type, modifiers] : renderContent_->drawCmdModifiers_) {
        modifiers.remove_if(
            [pid](const auto& it) -> bool { return ExtractPid(it->GetPropertyId()) == pid; });
    }
}

void RSRenderNode::UpdateShouldPaint()
{
    // node should be painted if either it is visible or it has disappearing transition animation, but only when its
    // alpha is not zero
    shouldPaint_ = (GetRenderProperties().GetAlpha() > 0.0f) &&
        (GetRenderProperties().GetVisible() || HasDisappearingTransition(false));
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!shouldPaint_) {
        // clear filter cache when node is not visible
        GetMutableRenderProperties().ClearFilterCache();
        GetMutableRenderProperties().ReleaseColorPickerTaskShadow();
    }
#endif
}

void RSRenderNode::SetSharedTransitionParam(const std::optional<SharedTransitionParam>&& sharedTransitionParam)
{
    if (!sharedTransitionParam_.has_value() && !sharedTransitionParam.has_value()) {
        // both are empty, do nothing
        return;
    }
    sharedTransitionParam_ = sharedTransitionParam;
    SetDirty();
}

void RSRenderNode::SetGlobalAlpha(float alpha)
{
    if (globalAlpha_ == alpha) {
        return;
    }
    if ((ROSEN_EQ(globalAlpha_, 1.0f) && !ROSEN_EQ(alpha, 1.0f)) ||
        (ROSEN_EQ(alpha, 1.0f) && !ROSEN_EQ(globalAlpha_, 1.0f))) {
        OnAlphaChanged();
    }
    globalAlpha_ = alpha;
}

float RSRenderNode::GetGlobalAlpha() const
{
    return globalAlpha_;
}

void RSRenderNode::SetBootAnimation(bool isBootAnimation)
{
    ROSEN_LOGD("SetBootAnimation:: id:%{public}" PRIu64 "isBootAnimation %{public}d",
        GetId(), isBootAnimation);
    isBootAnimation_ = isBootAnimation;
}

bool RSRenderNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

bool RSRenderNode::NeedInitCacheSurface()
{
    auto cacheType = GetCacheType();
    int width = 0;
    int height = 0;
    if (cacheType == CacheType::ANIMATE_PROPERTY &&
        GetRenderProperties().IsShadowValid() && !GetRenderProperties().IsSpherizeValid()) {
        const RectF boundsRect = GetRenderProperties().GetBoundsRect();
        RRect rrect = RRect(boundsRect, {0, 0, 0, 0});
        RectI shadowRect;
        RSPropertiesPainter::GetShadowDirtyRect(shadowRect, GetRenderProperties(), &rrect, false);
        width = shadowRect.GetWidth();
        height = shadowRect.GetHeight();
    } else {
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (RSSystemProperties::IsOpincRealDrawCacheEnable() && GetAutoCache()->isComputeDrawAreaSucc()) {
            auto& unionRect = GetAutoCache()->GetOpListUnionArea();
            width = unionRect.GetWidth();
            height = unionRect.GetHeight();
        } else {
            Vector2f size = GetOptionalBufferSize();
            width =  size.x_;
            height = size.y_;
        }
#else
        Vector2f size = GetOptionalBufferSize();
        width =  size.x_;
        height = size.y_;
#endif
    }
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheSurface_ == nullptr) {
        return true;
    }
    auto cacheCanvas = cacheSurface_->GetCanvas();
    if (cacheCanvas == nullptr) {
        return true;
    }
    return cacheCanvas->GetWidth() != width || cacheCanvas->GetHeight() != height;
}

bool RSRenderNode::NeedInitCacheCompletedSurface()
{
#ifdef DDGR_ENABLE_FEATURE_OPINC
    int width = 0;
    int height = 0;
    if (RSSystemProperties::IsOpincRealDrawCacheEnable() && GetAutoCache()->isComputeDrawAreaSucc()) {
        auto& unionRect = GetAutoCache()->GetOpListUnionArea();
        width = static_cast<int>(unionRect.GetWidth());
        height = static_cast<int>(unionRect.GetHeight());
    } else {
        Vector2f size = GetOptionalBufferSize();
        width = static_cast<int>(size.x_);
        height = static_cast<int>(size.y_);
    }
#else
    Vector2f size = GetOptionalBufferSize();
    int width = static_cast<int>(size.x_);
    int height = static_cast<int>(size.y_);
#endif
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheCompletedSurface_ == nullptr) {
        return true;
    }
    auto cacheCanvas = cacheCompletedSurface_->GetCanvas();
    if (cacheCanvas == nullptr) {
        return true;
    }
    return cacheCanvas->GetWidth() != width || cacheCanvas->GetHeight() != height;
}

void RSRenderNode::InitCacheSurface(Drawing::GPUContext* gpuContext, ClearCacheSurfaceFunc func, uint32_t threadIndex)
{
    if (func) {
        cacheSurfaceThreadIndex_ = threadIndex;
        if (!clearCacheSurfaceFunc_) {
            clearCacheSurfaceFunc_ = func;
        }
        if (cacheSurface_) {
            func(std::move(cacheSurface_), nullptr,
                cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
            std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
            cacheSurface_ = nullptr;
        }
    } else {
        cacheSurface_ = nullptr;
    }
    auto cacheType = GetCacheType();
    float width = 0.0f, height = 0.0f;
    Vector2f size = GetOptionalBufferSize();
    boundsWidth_ = size.x_;
    boundsHeight_ = size.y_;
    if (cacheType == CacheType::ANIMATE_PROPERTY &&
        GetRenderProperties().IsShadowValid() && !GetRenderProperties().IsSpherizeValid()) {
        const RectF boundsRect = GetRenderProperties().GetBoundsRect();
        RRect rrect = RRect(boundsRect, {0, 0, 0, 0});
        RectI shadowRect;
        RSPropertiesPainter::GetShadowDirtyRect(shadowRect, GetRenderProperties(), &rrect, false);
        width = shadowRect.GetWidth();
        height = shadowRect.GetHeight();
        shadowRectOffsetX_ = -shadowRect.GetLeft();
        shadowRectOffsetY_ = -shadowRect.GetTop();
    } else {
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (RSSystemProperties::IsOpincRealDrawCacheEnable() && GetAutoCache()->isComputeDrawAreaSucc()) {
            auto& unionRect = GetAutoCache()->GetOpListUnionArea();
            width = unionRect.GetWidth();
            height = unionRect.GetHeight();
        } else {
            width = boundsWidth_;
            height = boundsHeight_;
        }
#else
        width = boundsWidth_;
        height = boundsHeight_;
#endif
    }
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (gpuContext == nullptr) {
        if (func) {
            func(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
                cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
            ClearCacheSurface();
        }
        return;
    }
#ifdef RS_ENABLE_GL
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        Drawing::ImageInfo info = Drawing::ImageInfo::MakeN32Premul(width, height);
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        cacheSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext, true, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        cacheBackendTexture_ = MakeBackendTexture(width, height);
        auto vkTextureInfo = cacheBackendTexture_.GetTextureInfo().GetVKTextureInfo();
        if (!cacheBackendTexture_.isValid() || !vkTextureInfo) {
            if (func) {
                func(std::move(cacheSurface_), std::move(cacheCompletedSurface_),
                    cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
                ClearCacheSurface();
            }
            return;
        }
        cacheCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory, vkTextureInfo->vkAlloc.statName);
        cacheSurface_ = Drawing::Surface::MakeFromBackendTexture(
            gpuContext, cacheBackendTexture_.GetTextureInfo(), Drawing::TextureOrigin::BOTTOM_LEFT,
            1, Drawing::ColorType::COLORTYPE_RGBA_8888, nullptr,
            NativeBufferUtils::DeleteVkImage, cacheCleanupHelper_);
    }
#endif
#else
    cacheSurface_ = Drawing::Surface::MakeRasterN32Premul(width, height);
#endif
}

bool RSRenderNode::IsCacheSurfaceValid() const
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    return  (cacheCompletedSurface_ != nullptr);
}

Vector2f RSRenderNode::GetOptionalBufferSize() const
{
    const auto& modifier = boundsModifier_ ? boundsModifier_ : frameModifier_;
    if (!modifier) {
        return {0.0f, 0.0f};
    }
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(modifier->GetProperty());
    auto vector4f = renderProperty->Get();
    // bounds vector4f: x y z w -> left top width height
    return { vector4f.z_, vector4f.w_ };
}

#ifdef DDGR_ENABLE_FEATURE_OPINC
Vector4f RSRenderNode::GetOptionBufferBound() const
{
    const auto& modifier = boundsModifier_ ? boundsModifier_ : frameModifier_;
    if (!modifier) {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(modifier->GetProperty());
    // bounds vector4f: x y z w -> left top width height
    return renderProperty->Get();
}

Vector2f RSRenderNode::GetOpincBufferSize() const
{
    auto vector4f = GetOptionBufferBound();
    return {vector4f.x_ + vector4f.z_, vector4f.y_ + vector4f.w_};
}

Drawing::Rect RSRenderNode::GetOpincBufferBound() const
{
    auto vector4f = GetOptionBufferBound();
    return {vector4f.x_, vector4f.y_, vector4f.x_ + vector4f.z_, vector4f.y_ + vector4f.w_};
}
#endif

void RSRenderNode::DrawCacheSurface(RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst)
{
    if (ROSEN_EQ(boundsWidth_, 0.f) || ROSEN_EQ(boundsHeight_, 0.f)) {
        return;
    }
    auto cacheType = GetCacheType();
    canvas.Save();
    Vector2f size = GetOptionalBufferSize();
#ifdef DDGR_ENABLE_FEATURE_OPINC
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    if (!(RSSystemProperties::IsOpincRealDrawCacheEnable() && GetAutoCache()->isComputeDrawAreaSucc())) {
        scaleX = size.x_ / boundsWidth_;
        scaleY = size.y_ / boundsHeight_;
    }
#else
    float scaleX = size.x_ / boundsWidth_;
    float scaleY = size.y_ / boundsHeight_;
#endif
    canvas.Scale(scaleX, scaleY);
    auto cacheImage = GetCompletedImage(canvas, threadIndex, isUIFirst);
    if (cacheImage == nullptr) {
        canvas.Restore();
        return;
    }
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (cacheImage->IsTextureBacked()) {
            RS_LOGI("RSRenderNode::DrawCacheSurface convert cacheImage from texture to raster image");
            cacheImage = cacheImage->MakeRasterImage();
            if (!cacheImage) {
                RS_LOGE("RSRenderNode::DrawCacheSurface: MakeRasterImage failed");
                canvas.Restore();
                return;
            }
        }
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    if ((cacheType == CacheType::ANIMATE_PROPERTY && GetRenderProperties().IsShadowValid()) || isUIFirst) {
        auto surfaceNode = ReinterpretCastTo<RSSurfaceRenderNode>();
        Vector2f gravityTranslate = surfaceNode ?
            surfaceNode->GetGravityTranslate(cacheImage->GetWidth(), cacheImage->GetHeight()) : Vector2f(0.0f, 0.0f);
        canvas.DrawImage(*cacheImage, -shadowRectOffsetX_ * scaleX + gravityTranslate.x_,
            -shadowRectOffsetY_ * scaleY + gravityTranslate.y_, samplingOptions);
    } else {
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (RSSystemProperties::IsOpincRealDrawCacheEnable() && GetAutoCache()->isComputeDrawAreaSucc() &&
            GetAutoCache()->DrawAutoCache(canvas, *cacheImage, samplingOptions,
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT)) {
            canvas.DetachBrush();
            GetAutoCache()->DrawAutoCacheDfx(canvas);
            canvas.Restore();
            return;
        }
#endif
        if (canvas.GetTotalMatrix().HasPerspective()) {
            // In case of perspective transformation, make dstRect 1px outset to anti-alias
            Drawing::Rect dst(0, 0, cacheImage->GetWidth(), cacheImage->GetHeight());
            dst.MakeOutset(1, 1);
            canvas.DrawImageRect(*cacheImage, dst, samplingOptions);
        } else {
            canvas.DrawImage(*cacheImage, 0.0, 0.0, samplingOptions);
        }
    }
    canvas.DetachBrush();
    canvas.Restore();
}

std::shared_ptr<Drawing::Image> RSRenderNode::GetCompletedImage(
    RSPaintFilterCanvas& canvas, uint32_t threadIndex, bool isUIFirst)
{
    if (isUIFirst) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (!cacheCompletedBackendTexture_.IsValid()) {
            RS_LOGE("invalid grBackendTexture_");
            return nullptr;
        }
#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            if (!cacheCompletedSurface_ || !cacheCompletedCleanupHelper_) {
                return nullptr;
            }
        }
#endif
        auto image = std::make_shared<Drawing::Image>();
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
        Drawing::BitmapFormat info = Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888,
            Drawing::ALPHATYPE_PREMUL };
#ifdef RS_ENABLE_GL
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
            image->BuildFromTexture(*canvas.GetGPUContext(), cacheCompletedBackendTexture_.GetTextureInfo(),
                origin, info, nullptr);
        }
#endif

#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            image->BuildFromTexture(*canvas.GetGPUContext(), cacheCompletedBackendTexture_.GetTextureInfo(),
                origin, info, nullptr,
                NativeBufferUtils::DeleteVkImage, cacheCompletedCleanupHelper_->Ref());
        }
#endif
        return image;
#endif
    }

    if (!cacheCompletedSurface_) {
        RS_LOGE("DrawCacheSurface invalid cacheCompletedSurface");
        return nullptr;
    }
    auto completeImage = cacheCompletedSurface_->GetImageSnapshot();
    if (!completeImage) {
        RS_LOGE("Get complete image failed");
        return nullptr;
    }
    if (threadIndex == completedSurfaceThreadIndex_) {
        return completeImage;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = completeImage->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("get backendTexture failed");
        return nullptr;
    }
    SKResourceManager::Instance().HoldResource(completeImage);
    auto cacheImage = std::make_shared<Drawing::Image>();
    Drawing::BitmapFormat info =
        Drawing::BitmapFormat{ completeImage->GetColorType(), completeImage->GetAlphaType() };
    bool ret = cacheImage->BuildFromTexture(*canvas.GetGPUContext(), backendTexture.GetTextureInfo(),
        origin, info, nullptr);
    if (!ret) {
        RS_LOGE("RSRenderNode::GetCompletedImage image BuildFromTexture failed");
        return nullptr;
    }
    return cacheImage;
#else
    return completeImage;
#endif
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
void RSRenderNode::UpdateBackendTexture()
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    if (cacheSurface_ == nullptr) {
        return;
    }
    cacheBackendTexture_ = cacheSurface_->GetBackendTexture();
}
#endif

std::shared_ptr<Drawing::Surface> RSRenderNode::GetCompletedCacheSurface(uint32_t threadIndex, bool needCheckThread,
    bool releaseAfterGet)
{
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (releaseAfterGet) {
            return std::move(cacheCompletedSurface_);
        }
        if (!needCheckThread || completedSurfaceThreadIndex_ == threadIndex || !cacheCompletedSurface_) {
            return cacheCompletedSurface_;
        }
    }

    // freeze cache scene
    ClearCacheSurfaceInThread();
    return nullptr;
}

void RSRenderNode::ClearCacheSurfaceInThread()
{
    if (clearCacheSurfaceFunc_) {
        clearCacheSurfaceFunc_(std::move(cacheSurface_), std::move(cacheCompletedSurface_), cacheSurfaceThreadIndex_,
            completedSurfaceThreadIndex_);
    }
    ClearCacheSurface();
}

std::shared_ptr<Drawing::Surface> RSRenderNode::GetCacheSurface(uint32_t threadIndex, bool needCheckThread,
    bool releaseAfterGet)
{
    {
        std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
        if (releaseAfterGet) {
            return std::move(cacheSurface_);
        }
        if (!needCheckThread || cacheSurfaceThreadIndex_ == threadIndex || !cacheSurface_) {
            return cacheSurface_;
        }
    }

    // freeze cache scene
    ClearCacheSurfaceInThread();
    return nullptr;
}

void RSRenderNode::CheckGroupableAnimation(const PropertyId& id, bool isAnimAdd)
{
    if (id <= 0 || GetType() != RSRenderNodeType::CANVAS_NODE) {
        return;
    }
    auto context = GetContext().lock();
    if (!RSSystemProperties::GetAnimationCacheEnabled() ||
        !context || !context->GetNodeMap().IsResidentProcessNode(GetId())) {
        return;
    }
    auto target = modifiers_.find(id);
    if (target == modifiers_.end() || !target->second) {
        return;
    }
    if (isAnimAdd) {
        if (GROUPABLE_ANIMATION_TYPE.count(target->second->GetType())) {
            MarkNodeGroup(NodeGroupType::GROUPED_BY_ANIM, true, false);
        } else if (CACHEABLE_ANIMATION_TYPE.count(target->second->GetType())) {
            hasCacheableAnim_ = true;
        }
        return;
    }
    bool hasGroupableAnim = false;
    hasCacheableAnim_ = false;
    for (auto& [_, animation] : animationManager_.animations_) {
        if (!animation || id == animation->GetPropertyId()) {
            continue;
        }
        auto itr = modifiers_.find(animation->GetPropertyId());
        if (itr == modifiers_.end() || !itr->second) {
            continue;
        }
        hasGroupableAnim = (hasGroupableAnim || (GROUPABLE_ANIMATION_TYPE.count(itr->second->GetType()) != 0));
        hasCacheableAnim_ = (hasCacheableAnim_ || (CACHEABLE_ANIMATION_TYPE.count(itr->second->GetType()) != 0));
    }
    MarkNodeGroup(NodeGroupType::GROUPED_BY_ANIM, hasGroupableAnim, false);
}

bool RSRenderNode::IsForcedDrawInGroup() const
{
#ifdef DDGR_ENABLE_FEATURE_OPINC
    return nodeGroupType_ & (NodeGroupType::GROUPED_BY_USER | NodeGroupType::GROUPED_BY_AUTO);
#else
    return nodeGroupType_ & NodeGroupType::GROUPED_BY_USER;
#endif
}

bool RSRenderNode::IsSuggestedDrawInGroup() const
{
    return nodeGroupType_ != NodeGroupType::NONE;
}

void RSRenderNode::MarkNodeGroup(NodeGroupType type, bool isNodeGroup, bool includeProperty)
{
    if (isNodeGroup && type == NodeGroupType::GROUPED_BY_UI) {
        auto context = GetContext().lock();
        if (context && context->GetNodeMap().IsResidentProcessNode(GetId())) {
            nodeGroupType_ |= type;
            SetDirty();
        }
    } else {
        if (isNodeGroup) {
            nodeGroupType_ |= type;
        } else {
            nodeGroupType_ &= ~type;
        }
#ifdef DDGR_ENABLE_FEATURE_OPINC
        if (type != NodeGroupType::GROUPED_BY_AUTO)) {
            SetDirty();
        }
#else
        SetDirty();
#endif
    }
    nodeGroupIncludeProperty_ = includeProperty;
    if (type == NodeGroupType::GROUPED_BY_USER) {
        GetMutableRenderProperties().SetAlphaOffscreen(isNodeGroup);
    }
}

bool RSRenderNode::IsNodeGroupIncludeProperty() const
{
    return nodeGroupIncludeProperty_;
}

void RSRenderNode::MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer, pid_t pid)
{
    isNodeSingleFrameComposer_ = isNodeSingleFrameComposer;
    appPid_ = pid;
}

bool RSRenderNode::GetNodeIsSingleFrameComposer() const
{
    return isNodeSingleFrameComposer_;
}

void RSRenderNode::CheckDrawingCacheType()
{
    if (nodeGroupType_ == NodeGroupType::NONE) {
        SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
#ifdef DDGR_ENABLE_FEATURE_OPINC
    } else if (IsForcedDrawInGroup()) {
#else
    } else if (nodeGroupType_ & NodeGroupType::GROUPED_BY_USER) {
#endif
        SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    } else {
        SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    }
}

#ifdef DDGR_ENABLE_FEATURE_OPINC
const std::shared_ptr<RSRenderNode::RSAutoCache>& RSRenderNode::GetAutoCache()
{
    if (autoCache_ == nullptr) {
        autoCache_ = std::make_shared<RSRenderNode::RSAutoCache>(*this);
    }
    return autoCache_;
}
#endif

#ifdef RS_ENABLE_STACK_CULLING
void RSRenderNode::SetFullSurfaceOpaqueMarks(const std::shared_ptr<RSRenderNode> curSurfaceNodeParam)
{
    if (!isFullSurfaceOpaquCanvasNode_) {
        int32_t tempValue = coldDownCounter_;
        coldDownCounter_ = (coldDownCounter_ + 1) % MAX_COLD_DOWN_NUM;
        if (tempValue != 0) {
            return;
        }
    } else {
        coldDownCounter_ = 0;
    }

    isFullSurfaceOpaquCanvasNode_ = false;
    if (!ROSEN_EQ(GetGlobalAlpha(), 1.0f) || HasFilter()) {
        return;
    }

    if (GetRenderProperties().GetBackgroundColor().GetAlpha() < 255) {
        return;
    }

    if (!curSurfaceNodeParam) {
        return;
    }

    auto curSurfaceNode = (static_cast<const RSSurfaceRenderNode*>(curSurfaceNodeParam.get()));
    auto surfaceNodeAbsRect = curSurfaceNode->GetOldDirty();
    auto absRect = GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
    if (surfaceNodeAbsRect.IsInsideOf(absRect)) {
        isFullSurfaceOpaquCanvasNode_ = true;

        auto rsParent = GetParent().lock();
        while (rsParent) {
            //skip whern another child has set its parent or reach rootnode
            if (rsParent->hasChildFullSurfaceOpaquCanvasNode_) {
                break;
            }

            rsParent->hasChildFullSurfaceOpaquCanvasNode_ = true;
            if (rsParent->IsInstanceOf<RSRootRenderNode>()) {
                break;
            }

            rsParent = rsParent->GetParent().lock();
        }
    }
}

void RSRenderNode::SetSubNodesCovered()
{
    if (hasChildFullSurfaceOpaquCanvasNode_) {
        auto sortedChildren_ = GetSortedChildren();
        if (sortedChildren_->size() <= 1) {
            return;
        }

        bool found = false;
        for (auto child = sortedChildren_->rbegin(); child != sortedChildren_->rend(); child++) {
            if (!found && ((*child)->isFullSurfaceOpaquCanvasNode_ || (*child)->hasChildFullSurfaceOpaquCanvasNode_)) {
                found = true;
                continue;
            }
            if (found) {
                (*child)->isCoveredByOtherNode_ = true;
            }
        }
    }
}
void RSRenderNode::ResetSubNodesCovered()
{
    hasChildFullSurfaceOpaquCanvasNode_ = false;
}
#endif

void RSRenderNode::ResetFilterRectsInCache(const std::unordered_set<NodeId>& curRects)
{
    curCacheFilterRects_ = curRects;
}

void RSRenderNode::GetFilterRectsInCache(std::unordered_map<NodeId, std::unordered_set<NodeId>>& allRects) const
{
    if (!curCacheFilterRects_.empty()) {
        allRects.emplace(GetId(), curCacheFilterRects_);
    }
}

bool RSRenderNode::IsFilterRectsInCache() const
{
    return !curCacheFilterRects_.empty();
}

RectI RSRenderNode::GetFilterRect() const
{
    auto& properties = GetRenderProperties();
    auto geoPtr = (properties.GetBoundsGeometry());
    if (!geoPtr) {
        return {};
    }
    if (properties.GetClipBounds() != nullptr) {
        auto filterRect = properties.GetClipBounds()->GetDrawingPath().GetBounds();
        Drawing::Rect absRect;
        geoPtr->GetAbsMatrix().MapRect(absRect, filterRect);
        return {absRect.GetLeft(), absRect.GetTop(), absRect.GetWidth(), absRect.GetHeight()};
    } else {
        return geoPtr->GetAbsRect();
    }
}

void RSRenderNode::UpdateFullScreenFilterCacheRect(
    RSDirtyRegionManager& dirtyManager, bool isForeground) const
{
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto& renderProperties = GetRenderProperties();
    auto& manager = renderProperties.GetFilterCacheManager(isForeground);
    // Record node's cache area if it has valid filter cache
    // If there are any invalid caches under full screen cache filter, the occlusion should be invalidated
    if (!manager->IsCacheValid() && dirtyManager.IsCacheableFilterRectEmpty()) {
        dirtyManager.InvalidateFilterCacheRect();
    } else if (ROSEN_EQ(GetGlobalAlpha(), 1.0f) && ROSEN_EQ(renderProperties.GetCornerRadius().x_, 0.0f) &&
        manager->GetCachedImageRegion() == dirtyManager.GetSurfaceRect() && !IsInstanceOf<RSEffectRenderNode>()) {
        // Only record full screen filter cache for occlusion calculation
        dirtyManager.UpdateCacheableFilterRect(manager->GetCachedImageRegion());
    }
#endif
}

void RSRenderNode::UpdateFilterCacheManagerWithCacheRegion(
    RSDirtyRegionManager& dirtyManager, const std::optional<RectI>& clipRect)
{
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!RSProperties::FilterCacheEnabled) {
        return;
    }
    auto& renderProperties = GetRenderProperties();
    if (!renderProperties.NeedFilter()) {
        return;
    }
    auto filterRect = GetFilterRect();
    if (clipRect.has_value()) {
        filterRect.IntersectRect(*clipRect);
    }

    // background filter
    if (auto& manager = renderProperties.GetFilterCacheManager(false)) {
        // invalidate cache if filter region is not inside of cached image region
        if (manager->IsCacheValid() && !filterRect.IsInsideOf(manager->GetCachedImageRegion())) {
            manager->UpdateCacheStateWithFilterRegion();
            if (auto context = GetContext().lock()) {
                RS_TRACE_NAME_FMT("background filter of node Id:%" PRIu64 " is invalid", GetId());
                context->MarkNeedPurge(ClearMemoryMoment::FILTER_INVALID, RSContext::PurgeType::STRONGLY);
            }
        }
        UpdateFullScreenFilterCacheRect(dirtyManager, false);
    }
    // foreground filter
    if (auto& manager = renderProperties.GetFilterCacheManager(true)) {
        // invalidate cache if filter region is not inside of cached image region
        if (manager->IsCacheValid() && !filterRect.IsInsideOf(manager->GetCachedImageRegion())) {
            manager->UpdateCacheStateWithFilterRegion();
            if (auto context = GetContext().lock()) {
                RS_TRACE_NAME_FMT("foreground filter of node Id:%" PRIu64 " is invalid", GetId());
                context->MarkNeedPurge(ClearMemoryMoment::FILTER_INVALID, RSContext::PurgeType::STRONGLY);
            }
        }
        UpdateFullScreenFilterCacheRect(dirtyManager, true);
    }
#endif
}

void RSRenderNode::OnTreeStateChanged()
{
    if (isOnTheTree_) {
        // Set dirty and force add to active node list, re-generate children list if needed
        SetDirty(true);
    }
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!isOnTheTree_) {
        // clear filter cache when node is removed from tree
        GetMutableRenderProperties().ClearFilterCache();
        GetMutableRenderProperties().ReleaseColorPickerTaskShadow();
    }
#endif
}

bool RSRenderNode::HasDisappearingTransition(bool recursive) const
{
    if (disappearingTransitionCount_ > 0) {
        return true;
    }
    if (recursive == false) {
        return false;
    }
    auto parent = GetParent().lock();
    if (parent == nullptr) {
        return false;
    }
    return parent->HasDisappearingTransition(true);
}

RSRenderNode::ChildrenListSharedPtr RSRenderNode::GetChildren() const
{
    return std::atomic_load_explicit(&fullChildrenList_, std::memory_order_acquire);
}

RSRenderNode::ChildrenListSharedPtr RSRenderNode::GetSortedChildren() const
{
    return std::atomic_load_explicit(&fullChildrenList_, std::memory_order_acquire);
}

std::shared_ptr<RSRenderNode> RSRenderNode::GetFirstChild() const
{
    return children_.empty() ? nullptr : children_.front().lock();
}

void RSRenderNode::UpdateFullChildrenListIfNeeded()
{
    if (!isFullChildrenListValid_) {
        GenerateFullChildrenList();
    } else if (!isChildrenSorted_) {
        ResortChildren();
    }
}

void RSRenderNode::GenerateFullChildrenList()
{
    // both children_ and disappearingChildren_ are empty, no need to generate fullChildrenList_
    if (children_.empty() && disappearingChildren_.empty()) {
        auto prevFullChildrenList = fullChildrenList_;
        isFullChildrenListValid_ = true;
        isChildrenSorted_ = true;
        std::atomic_store_explicit(&fullChildrenList_, EmptyChildrenList, std::memory_order_release);
        return;
    }

    // Step 0: Initialize
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();

    // Step 1: Copy all children into sortedChildren while checking and removing expired children.
    children_.remove_if([&](const auto& child) -> bool {
        auto existingChild = child.lock();
        if (existingChild == nullptr) {
            ROSEN_LOGI("RSRenderNode::GenerateSortedChildren removing expired child, this is rare but possible.");
            return true;
        }
        if (isContainBootAnimation_ && !existingChild->GetBootAnimation()) {
            ROSEN_LOGD("RSRenderNode::GenerateSortedChildren %{public}" PRIu64 " skip"
            " move not bootAnimation displaynode"
            "child(id %{public}" PRIu64 ")"" into children_", GetId(), existingChild->GetId());
            return false;
        }
        fullChildrenList->emplace_back(std::move(existingChild));
        return false;
    });

    // Step 2: Insert disappearing children into sortedChildren at their original position.
    // Note:
    //     1. We don't need to check if the disappearing transition is finished; it's already handled in
    //     RSRenderTransition::OnDetach.
    //     2. We don't need to check if the disappearing child is expired; it's already been checked when moving from
    //     children_ to disappearingChildren_. We hold ownership of the shared_ptr of the child after that.
    std::for_each(disappearingChildren_.begin(), disappearingChildren_.end(), [&](const auto& pair) -> void {
        auto& disappearingChild = pair.first;
        if (isContainBootAnimation_ && !disappearingChild->GetBootAnimation()) {
            ROSEN_LOGD("RSRenderNode::GenerateSortedChildren %{public}" PRIu64 " skip"
            " move not bootAnimation displaynode"
            "child(id %{public}" PRIu64 ")"" into disappearingChild", GetId(), disappearingChild->GetId());
            return;
        }
        const auto& origPos = pair.second;
        if (origPos < fullChildrenList->size()) {
            fullChildrenList->emplace(std::next(fullChildrenList->begin(), origPos), disappearingChild);
        } else {
            fullChildrenList->emplace_back(disappearingChild);
        }
    });

    // Step 3: Sort all children by z-order
    std::stable_sort(
        fullChildrenList->begin(), fullChildrenList->end(), [](const auto& first, const auto& second) -> bool {
        return first->GetRenderProperties().GetPositionZ() < second->GetRenderProperties().GetPositionZ();
    });

    // Keep a reference to fullChildrenList_ to prevent its deletion when swapping it
    auto prevFullChildrenList = fullChildrenList_;

    // Update the flag to indicate that children are now valid and sorted
    isFullChildrenListValid_ = true;
    isChildrenSorted_ = true;

    // Move the fullChildrenList to fullChildrenList_ atomically
    ChildrenListSharedPtr constFullChildrenList = std::move(fullChildrenList);
    std::atomic_store_explicit(&fullChildrenList_, constFullChildrenList, std::memory_order_release);
}

void RSRenderNode::ResortChildren()
{
    // Make a copy of the fullChildrenList for sorting
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(*fullChildrenList_);

    // Sort the children by their z-order
    std::stable_sort(
        fullChildrenList->begin(), fullChildrenList->end(), [](const auto& first, const auto& second) -> bool {
        return first->GetRenderProperties().GetPositionZ() < second->GetRenderProperties().GetPositionZ();
    });

    // Keep a reference to fullChildrenList_ to prevent its deletion when swapping it
    auto prevFullChildrenList = fullChildrenList_;

    // Update the flag to indicate that children are now sorted
    isChildrenSorted_ = true;

    // Move the fullChildrenList to fullChildrenList_ atomically
    ChildrenListSharedPtr constFullChildrenList = std::move(fullChildrenList);
    std::atomic_store_explicit(&fullChildrenList_, constFullChildrenList, std::memory_order_release);
}

uint32_t RSRenderNode::GetChildrenCount() const
{
    return children_.size();
}

void RSRenderNode::SetTunnelHandleChange(bool change)
{
    isTunnelHandleChange_ = change;
}
bool RSRenderNode::GetTunnelHandleChange() const
{
    return isTunnelHandleChange_;
}
bool RSRenderNode::HasChildrenOutOfRect() const
{
    if (GetRenderProperties().GetClipToBounds() || GetRenderProperties().GetClipToFrame()) {
        return false;
    }
    return hasChildrenOutOfRect_;
}
void RSRenderNode::UpdateChildrenOutOfRectFlag(bool flag)
{
    hasChildrenOutOfRect_ = flag;
}
void RSRenderNode::ResetHasRemovedChild()
{
    hasRemovedChild_ = false;
}
bool RSRenderNode::HasRemovedChild() const
{
    return hasRemovedChild_;
}
RectI RSRenderNode::GetChildrenRect() const
{
    return childrenRect_;
}
bool RSRenderNode::ChildHasFilter() const
{
    return childHasFilter_;
}
void RSRenderNode::SetChildHasFilter(bool childHasFilter)
{
    childHasFilter_ = childHasFilter;
}
NodeId RSRenderNode::GetInstanceRootNodeId() const
{
    return instanceRootNodeId_;
}
const std::shared_ptr<RSRenderNode> RSRenderNode::GetInstanceRootNode() const
{
    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("Invalid context");
        return nullptr;
    }
    return context->GetNodeMap().GetRenderNode(instanceRootNodeId_);
}
NodeId RSRenderNode::GetFirstLevelNodeId() const
{
    return firstLevelNodeId_;
}
const std::shared_ptr<RSRenderNode> RSRenderNode::GetFirstLevelNode() const
{
    auto context = GetContext().lock();
    if (!context) {
        ROSEN_LOGE("Invalid context");
        return nullptr;
    }
    return context->GetNodeMap().GetRenderNode(firstLevelNodeId_);
}
bool RSRenderNode::IsRenderUpdateIgnored() const
{
    return isRenderUpdateIgnored_;
}
RSAnimationManager& RSRenderNode::GetAnimationManager()
{
    return animationManager_;
}
RectI RSRenderNode::GetOldDirty() const
{
    return oldDirty_;
}
RectI RSRenderNode::GetOldDirtyInSurface() const
{
    return oldDirtyInSurface_;
}
bool RSRenderNode::IsDirtyRegionUpdated() const
{
    return isDirtyRegionUpdated_;
}
void RSRenderNode::CleanDirtyRegionUpdated()
{
    isDirtyRegionUpdated_ = false;
}
bool RSRenderNode::IsShadowValidLastFrame() const
{
    return isShadowValidLastFrame_;
}
void RSRenderNode::SetStaticCached(bool isStaticCached)
{
    isStaticCached_ = isStaticCached;
    // ensure defrost subtree would be updated
    if (!isStaticCached_) {
        SetContentDirty();
    }
}
bool RSRenderNode::IsStaticCached() const
{
    return isStaticCached_;
}
void RSRenderNode::SetNodeName(const std::string& nodeName)
{
    nodeName_ = nodeName;
}
const std::string& RSRenderNode::GetNodeName() const
{
    return nodeName_;
}
void RSRenderNode::UpdateCompletedCacheSurface()
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    std::swap(cacheSurface_, cacheCompletedSurface_);
    std::swap(cacheSurfaceThreadIndex_, completedSurfaceThreadIndex_);
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::swap(cacheBackendTexture_, cacheCompletedBackendTexture_);
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        std::swap(cacheCleanupHelper_, cacheCompletedCleanupHelper_);
    }
#endif
    SetTextureValidFlag(true);
#endif
}
void RSRenderNode::SetTextureValidFlag(bool isValid)
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    isTextureValid_ = isValid;
#endif
}
void RSRenderNode::ClearCacheSurface(bool isClearCompletedCacheSurface)
{
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    cacheSurface_ = nullptr;
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        cacheCleanupHelper_ = nullptr;
    }
#endif
    if (isClearCompletedCacheSurface) {
        cacheCompletedSurface_ = nullptr;
#ifdef RS_ENABLE_VK
        if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
            OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
            cacheCompletedCleanupHelper_ = nullptr;
        }
#endif
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
        isTextureValid_ = false;
#endif
    }
}
void RSRenderNode::SetCacheType(CacheType cacheType)
{
    cacheType_ = cacheType;
}
CacheType RSRenderNode::GetCacheType() const
{
    return cacheType_;
}
int RSRenderNode::GetShadowRectOffsetX() const
{
    return shadowRectOffsetX_;
}
int RSRenderNode::GetShadowRectOffsetY() const
{
    return shadowRectOffsetY_;
}
void RSRenderNode::SetDrawingCacheType(RSDrawingCacheType cacheType)
{
    drawingCacheType_ = cacheType;
}
RSDrawingCacheType RSRenderNode::GetDrawingCacheType() const
{
    return drawingCacheType_;
}
void RSRenderNode::SetDrawingCacheChanged(bool cacheChanged)
{
    isDrawingCacheChanged_ = drawingCacheNeedUpdate_ || cacheChanged;
    drawingCacheNeedUpdate_ = isDrawingCacheChanged_;
#ifdef DDGR_ENABLE_FEATURE_OPINC
    if (RSSystemProperties::IsDdgrOpincEnable()) {
        GetAutoCache()->SetOpincCacheChanged(cacheChanged);
    }
#endif
}
bool RSRenderNode::GetDrawingCacheChanged() const
{
    return isDrawingCacheChanged_;
}
void RSRenderNode::ResetDrawingCacheNeedUpdate()
{
    drawingCacheNeedUpdate_ = false;
}
void RSRenderNode::SetCacheGeoPreparationDelay(bool val)
{
    cacheGeoPreparationDelay_ = cacheGeoPreparationDelay_ || val;
}
void RSRenderNode::ResetCacheGeoPreparationDelay()
{
    cacheGeoPreparationDelay_ = false;
}
bool RSRenderNode::GetCacheGeoPreparationDelay() const
{
    return cacheGeoPreparationDelay_;
}

void RSRenderNode::StoreMustRenewedInfo()
{
    mustRenewedInfo_ = hasHardwareNode_ || hasFilter_ || hasEffectNode_;
}

bool RSRenderNode::HasMustRenewedInfo() const
{
    return mustRenewedInfo_;
}

void RSRenderNode::SetUseEffectNodes(bool val)
{
    hasEffectNode_ = val;
}

bool RSRenderNode::HasUseEffectNodes() const
{
    return hasEffectNode_;
}

void RSRenderNode::SetVisitedCacheRootIds(const std::unordered_set<NodeId>& visitedNodes)
{
    visitedCacheRoots_ = visitedNodes;
}
const std::unordered_set<NodeId>& RSRenderNode::GetVisitedCacheRootIds() const
{
    return visitedCacheRoots_;
}
void RSRenderNode::UpdateSubSurfaceCnt(SharedPtr curParent, SharedPtr preParent)
{
    uint32_t subSurfaceCnt = GetType() == RSRenderNodeType::SURFACE_NODE ?
        subSurfaceCnt_ + 1 : subSurfaceCnt_;
    if (subSurfaceCnt == 0) {
        return;
    }
    if (curParent) {
        curParent->subSurfaceCnt_ += subSurfaceCnt;
        UpdateSubSurfaceCnt(curParent->GetParent().lock(), nullptr);
    }
    if (preParent) {
        preParent->subSurfaceCnt_ -= subSurfaceCnt;
        UpdateSubSurfaceCnt(nullptr, preParent->GetParent().lock());
    }
}
bool RSRenderNode::HasSubSurface() const
{
    return subSurfaceCnt_ > 0;
}
void RSRenderNode::SetDrawingCacheRootId(NodeId id)
{
    drawingCacheRootId_ = id;
}
NodeId RSRenderNode::GetDrawingCacheRootId() const
{
    return drawingCacheRootId_;
}
void RSRenderNode::SetIsMarkDriven(bool isMarkDriven)
{
    isMarkDriven_ = isMarkDriven;
}
bool RSRenderNode::IsMarkDriven() const
{
    return isMarkDriven_;
}
void RSRenderNode::SetIsMarkDrivenRender(bool isMarkDrivenRender)
{
    isMarkDrivenRender_ = isMarkDrivenRender;
}
bool RSRenderNode::IsMarkDrivenRender() const
{
    return isMarkDrivenRender_;
}
void RSRenderNode::SetItemIndex(int index)
{
    itemIndex_ = index;
}
int RSRenderNode::GetItemIndex() const
{
    return itemIndex_;
}
void RSRenderNode::SetPaintState(bool paintState)
{
    paintState_ = paintState;
}
bool RSRenderNode::GetPaintState() const
{
    return paintState_;
}
void RSRenderNode::SetIsContentChanged(bool isChanged)
{
    isContentChanged_ = isChanged;
}
bool RSRenderNode::IsContentChanged() const
{
    return isContentChanged_ || HasAnimation();
}
bool RSRenderNode::HasAnimation() const
{
    return !animationManager_.animations_.empty();
}
bool RSRenderNode::HasFilter() const
{
    return hasFilter_;
}
void RSRenderNode::SetHasFilter(bool hasFilter)
{
    hasFilter_ = hasFilter;
}
std::recursive_mutex& RSRenderNode::GetSurfaceMutex() const
{
    return surfaceMutex_;
}
bool RSRenderNode::HasHardwareNode() const
{
    return hasHardwareNode_;
}
void RSRenderNode::SetHasHardwareNode(bool hasHardwareNode)
{
    hasHardwareNode_ = hasHardwareNode;
}
bool RSRenderNode::HasAbilityComponent() const
{
    return hasAbilityComponent_;
}
void RSRenderNode::SetHasAbilityComponent(bool hasAbilityComponent)
{
    hasAbilityComponent_ = hasAbilityComponent;
}
uint32_t RSRenderNode::GetCacheSurfaceThreadIndex() const
{
    return cacheSurfaceThreadIndex_;
}
uint32_t RSRenderNode::GetCompletedSurfaceThreadIndex() const
{
    return completedSurfaceThreadIndex_;
}

bool RSRenderNode::IsMainThreadNode() const
{
    return isMainThreadNode_;
}
void RSRenderNode::SetIsMainThreadNode(bool isMainThreadNode)
{
    isMainThreadNode_ = isMainThreadNode;
}
bool RSRenderNode::IsScale() const
{
    return isScale_;
}
void RSRenderNode::SetIsScale(bool isScale)
{
    isScale_ = isScale;
}
void RSRenderNode::SetPriority(NodePriorityType priority)
{
    priority_ = priority;
}
NodePriorityType RSRenderNode::GetPriority()
{
    return priority_;
}
bool RSRenderNode::IsAncestorDirty() const
{
    return isAncestorDirty_;
}
void RSRenderNode::SetIsAncestorDirty(bool isAncestorDirty)
{
    isAncestorDirty_ = isAncestorDirty;
}
bool RSRenderNode::IsParentLeashWindow() const
{
    return isParentLeashWindow_;
}
void RSRenderNode::SetParentLeashWindow()
{
    isParentLeashWindow_ = true;
}
bool RSRenderNode::IsParentScbScreen() const
{
    return isParentScbScreen_;
}
void RSRenderNode::SetParentScbScreen()
{
    isParentScbScreen_ = true;
}
bool RSRenderNode::HasCachedTexture() const
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::scoped_lock<std::recursive_mutex> lock(surfaceMutex_);
    return isTextureValid_;
#else
    return true;
#endif
}
void RSRenderNode::SetDrawRegion(const std::shared_ptr<RectF>& rect)
{
    drawRegion_ = rect;
    GetMutableRenderProperties().SetDrawRegion(rect);
}
const std::shared_ptr<RectF>& RSRenderNode::GetDrawRegion() const
{
    return drawRegion_;
}
void RSRenderNode::SetOutOfParent(OutOfParentType outOfParent)
{
    outOfParent_ = outOfParent;
}
OutOfParentType RSRenderNode::GetOutOfParent() const
{
    return outOfParent_;
}
RSRenderNode::NodeGroupType RSRenderNode::GetNodeGroupType()
{
    uint8_t type = NodeGroupType::GROUP_TYPE_BUTT;
    while (type != NodeGroupType::NONE) {
        if (nodeGroupType_ & type) {
            return static_cast<NodeGroupType>(type);
        } else {
            type = type >> 1;
        }
    }
    return NodeGroupType::NONE;
}

void RSRenderNode::MarkNonGeometryChanged()
{
    geometryChangeNotPerceived_ = true;
}

bool RSRenderNode::GetIsUsedBySubThread() const
{
    return isUsedBySubThread_.load();
}
void RSRenderNode::SetIsUsedBySubThread(bool isUsedBySubThread)
{
    isUsedBySubThread_.store(isUsedBySubThread);
}

bool RSRenderNode::GetLastIsNeedAssignToSubThread() const
{
    return lastIsNeedAssignToSubThread_;
}
void RSRenderNode::SetLastIsNeedAssignToSubThread(bool lastIsNeedAssignToSubThread)
{
    lastIsNeedAssignToSubThread_ = lastIsNeedAssignToSubThread;
}
} // namespace Rosen
} // namespace OHOS
