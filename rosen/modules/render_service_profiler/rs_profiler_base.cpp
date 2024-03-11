/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "message_parcel.h"
#include "rs_profiler.h"
#include "rs_profiler_utils.h"

#include "animation/rs_animation_manager.h"
#include "command/rs_base_node_command.h"
#include "command/rs_canvas_drawing_node_command.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_effect_node_command.h"
#include "command/rs_proxy_node_command.h"
#include "command/rs_root_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {

static Mode g_mode;
static std::vector<pid_t> g_pids;
static pid_t g_pid = 0;
static NodeId g_parentNode = 0;
static std::map<uint64_t, ImageCacheRecord> g_imageCache;
static std::atomic<uint32_t> g_commandCount = 0;
static std::atomic<uint32_t> g_lastImageCount = 0;

static uint64_t g_pauseAfterTime = 0;
static uint64_t g_pauseCumulativeTime = 0;
static int64_t g_transactionTimeCorrection = 0;

ImageCache& RSProfiler::GetImageCache()
{
    return g_imageCache;
}

void RSProfiler::ClearImageCache()
{
    g_imageCache.clear();
}

uint32_t RSProfiler::GetImageCount()
{
    const uint32_t count = g_imageCache.size() - g_lastImageCount;
    g_lastImageCount = g_imageCache.size();
    return count;
}

uint32_t RSProfiler::GetCommandCount()
{
    const uint32_t count = g_commandCount;
    g_commandCount = 0;
    return count;
}

bool RSProfiler::IsParcelMock(const Parcel& parcel)
{
    // gcc C++ optimization error (?): this is not working without volatile
    const volatile auto address = reinterpret_cast<uint64_t>(&parcel);
    return ((address & 1u) != 0);
}

std::shared_ptr<MessageParcel> RSProfiler::CopyParcel(const MessageParcel& parcel)
{
    if (IsParcelMock(parcel)) {
        auto* buffer = new uint8_t[sizeof(MessageParcel) + 1];
        auto* mpPtr = new (buffer + 1) MessageParcel;
        return std::shared_ptr<MessageParcel>(mpPtr, [](MessageParcel* ptr) {
            ptr->~MessageParcel();
            auto* allocPtr = reinterpret_cast<uint8_t*>(ptr);
            allocPtr--;
            delete allocPtr;
        });
    }

    return std::make_shared<MessageParcel>();
}

NodeId RSProfiler::ReadNodeId(Parcel& parcel)
{
    return PatchPlainNodeId(parcel, static_cast<NodeId>(parcel.ReadUint64()));
}

pid_t RSProfiler::ReadPid(Parcel& parcel)
{
    return PatchPlainPid(parcel, static_cast<pid_t>(parcel.ReadUint32()));
}

void RSProfiler::PatchCommand(const Parcel& parcel, RSCommand* command)
{
    g_commandCount++;

    if (command && IsParcelMock(parcel)) {
        command->Patch();
    }
}

NodeId RSProfiler::PatchPlainNodeId(const Parcel& parcel, NodeId id)
{
    if ((g_mode != Mode::READ && g_mode != Mode::READ_EMUL) || !IsParcelMock(parcel)) {
        return id;
    }

    return Utils::PatchNodeId(id);
}

pid_t RSProfiler::PatchPlainPid(const Parcel& parcel, pid_t pid)
{
    if ((g_mode != Mode::READ && g_mode != Mode::READ_EMUL) || !IsParcelMock(parcel)) {
        return pid;
    }

    return Utils::GetMockPid(pid);
}

void RSProfiler::SetMode(Mode mode)
{
    g_mode = mode;
    if (g_mode == Mode::NONE) {
        g_pauseAfterTime = 0;
        g_pauseCumulativeTime = 0;
    }
}

Mode RSProfiler::GetMode()
{
    return g_mode;
}

void RSProfiler::SetSubstitutingPid(const std::vector<pid_t>& pids, pid_t pid, NodeId parent)
{
    g_pids = pids;
    g_pid = pid;
    g_parentNode = parent;
}

NodeId RSProfiler::GetParentNode()
{
    return g_parentNode;
}

const std::vector<pid_t>& RSProfiler::GetPids()
{
    return g_pids;
}

pid_t RSProfiler::GetSubstitutingPid()
{
    return g_pid;
}

void RSProfiler::AddImage(uint64_t uniqueId, void* image, uint32_t size, uint32_t skipBytes)
{
    if ((g_mode != Mode::WRITE && g_mode != Mode::WRITE_EMUL) || (g_imageCache.count(uniqueId) > 0)) {
        return;
    }

    if (image && (size > 0)) {
        auto imageData = std::make_unique<uint8_t[]>(size);
        if (memmove_s(imageData.get(), size, image, size) == 0) {
            ImageCacheRecord record;
            record.image = std::move(imageData);
            record.imageSize = size;
            record.skipBytes = skipBytes;
            g_imageCache.insert({ uniqueId, record });
        }
    }
}

ImageCacheRecord* RSProfiler::GetImage(uint64_t uniqueId)
{
    if (g_mode != Mode::READ && g_mode != Mode::READ_EMUL) {
        return nullptr;
    }

    if (g_imageCache.count(uniqueId) == 0) {
        return nullptr;
    }

    return &g_imageCache[uniqueId];
}

std::string RSProfiler::DumpImageCache()
{
    if (g_mode != Mode::READ && g_mode != Mode::READ_EMUL) {
        return "";
    }

    std::string out;
    for (const auto& it : g_imageCache) {
        out += std::to_string(Utils::ExtractPid(it.first)) + ":" + std::to_string(Utils::ExtractNodeId(it.first)) + " ";
    }

    return out;
}

uint64_t RSProfiler::PatchTime(uint64_t time)
{
    if (g_mode != Mode::READ && g_mode != Mode::READ_EMUL) {
        return time;
    }
    if (time == 0.0) {
        return 0.0;
    }
    if (time >= g_pauseAfterTime && g_pauseAfterTime > 0) {
        return g_pauseAfterTime - g_pauseCumulativeTime;
    }
    return time - g_pauseCumulativeTime;
}

uint64_t RSProfiler::PatchTransactionTime(const Parcel& parcel, uint64_t time)
{
    if (g_mode != Mode::READ) {
        return time;
    }
    if (time == 0.0) {
        return 0.0;
    }
    if (!IsParcelMock(parcel)) {
        return time;
    }

    return PatchTime(time + g_transactionTimeCorrection);
}

void RSProfiler::TimePauseAt(uint64_t curTime, uint64_t newPauseAfterTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += curTime - g_pauseAfterTime;
        }
    }
    g_pauseAfterTime = newPauseAfterTime;
}

void RSProfiler::TimePauseResume(uint64_t curTime)
{
    if (g_pauseAfterTime > 0) {
        if (curTime > g_pauseAfterTime) {
            g_pauseCumulativeTime += curTime - g_pauseAfterTime;
        }
    }
    g_pauseAfterTime = 0;
}

void RSProfiler::TimePauseClear()
{
    g_pauseCumulativeTime = 0;
    g_pauseAfterTime = 0;
}

uint32_t RSProfiler::GenerateImageId()
{
    static uint32_t uniqueImageId = 0u;
    return uniqueImageId++;
}

std::shared_ptr<RSDisplayRenderNode> RSProfiler::GetDisplayNode(RSContext& context)
{
    const std::shared_ptr<RSBaseRenderNode>& root = context.GetGlobalRootRenderNode();
    // without these checks device might get stuck on startup
    if (!root || (root->GetChildrenCount() != 1)) {
        return nullptr;
    }

    return RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(root->GetSortedChildren().front());
}

Vector4f RSProfiler::GetScreenRect(RSContext& context)
{
    std::shared_ptr<RSDisplayRenderNode> node = GetDisplayNode(context);
    if (!node) {
        return {};
    }

    const RectI rect = node->GetDirtyManager()->GetSurfaceRect();
    return { rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() };
}

void RSProfiler::FilterForPlayback(RSRenderNodeMap& map, pid_t pid)
{
    auto canBeRemoved = [](NodeId node, pid_t pid) -> bool {
        return (ExtractPid(node) == pid) && (Utils::ExtractNodeId(node) != 1);
    };

    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    EraseIf(map.renderNodeMap_, [pid, canBeRemoved](const auto& pair) -> bool {
        if (canBeRemoved(pair.first, pid) == false) {
            return false;
        }
        // update node flag to avoid animation fallback
        pair.second->fallbackAnimationOnDestroy_ = false;
        // remove node from tree
        pair.second->RemoveFromTree(false);
        return true;
    });

    EraseIf(
        map.surfaceNodeMap_, [pid, canBeRemoved](const auto& pair) -> bool { return canBeRemoved(pair.first, pid); });

    EraseIf(map.drivenRenderNodeMap_,
        [pid, canBeRemoved](const auto& pair) -> bool { return canBeRemoved(pair.first, pid); });

    EraseIf(map.residentSurfaceNodeMap_,
        [pid, canBeRemoved](const auto& pair) -> bool { return canBeRemoved(pair.first, pid); });

    EraseIf(
        map.displayNodeMap_, [pid, canBeRemoved](const auto& pair) -> bool { return canBeRemoved(pair.first, pid); });

    if (auto fallbackNode = map.GetAnimationFallbackNode()) {
        fallbackNode->GetAnimationManager().FilterAnimationByPid(pid);
    }
}

void RSProfiler::FilterMockNode(RSRenderNodeMap& map)
{
    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    EraseIf(map.renderNodeMap_, [](const auto& pair) -> bool {
        if (!Utils::IsNodeIdPatched(pair.first)) {
            return false;
        }
        // update node flag to avoid animation fallback
        pair.second->fallbackAnimationOnDestroy_ = false;
        // remove node from tree
        pair.second->RemoveFromTree(false);
        return true;
    });

    EraseIf(map.surfaceNodeMap_, [](const auto& pair) -> bool { return Utils::IsNodeIdPatched(pair.first); });
    EraseIf(map.drivenRenderNodeMap_, [](const auto& pair) -> bool { return Utils::IsNodeIdPatched(pair.first); });
    EraseIf(map.residentSurfaceNodeMap_, [](const auto& pair) -> bool { return Utils::IsNodeIdPatched(pair.first); });
    EraseIf(map.displayNodeMap_, [](const auto& pair) -> bool { return Utils::IsNodeIdPatched(pair.first); });

    if (auto fallbackNode = map.GetAnimationFallbackNode()) {
        // remove all fallback animations belong to given pid
        FilterAnimationForPlayback(fallbackNode->GetAnimationManager());
    }
}

void RSProfiler::GetSurfacesTrees(
    const RSRenderNodeMap& map, std::map<std::string, std::tuple<NodeId, std::string>>& list)
{
    constexpr uint32_t treeDumpDepth = 2;

    list.clear();
    for (const auto& item : map.renderNodeMap_) {
        if (item.second->GetType() == RSRenderNodeType::SURFACE_NODE) {
            const auto surfaceNode = item.second->ReinterpretCastTo<RSSurfaceRenderNode>();

            std::string tree;
            item.second->DumpTree(treeDumpDepth, tree);
            list.insert({ surfaceNode->GetName(), { surfaceNode->GetId(), tree } });
        }
    }
}

void RSProfiler::GetSurfacesTreesByPid(const RSRenderNodeMap& map, pid_t pid, std::map<NodeId, std::string>& list)
{
    constexpr uint32_t treeDumpDepth = 2;

    list.clear();
    for (const auto& item : map.renderNodeMap_) {
        if (item.second->GetId() == Utils::GetRootNodeId(pid)) {
            std::string tree;
            item.second->DumpTree(treeDumpDepth, tree);
            list.insert({ item.second->GetId(), tree });
        }
    }
}

size_t RSProfiler::GetRenderNodeCount(const RSRenderNodeMap& map)
{
    return map.renderNodeMap_.size();
}

NodeId RSProfiler::GetRandomSurfaceNode(const RSRenderNodeMap& map)
{
    for (const auto& item : map.surfaceNodeMap_) {
        return item.first;
    }
    return 0;
}

void RSProfiler::MarshallingNodes(const RSRenderNodeMap& map, std::stringstream& data, RSContext& context)
{
    uint32_t count = map.renderNodeMap_.size();
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));

    std::vector<std::shared_ptr<RSRenderNode>> rootNodeList;
    rootNodeList.emplace_back(context.GetGlobalRootRenderNode());

    for (const auto& item : map.renderNodeMap_) {
        MarshallingNode(map, data, item.second.get());

        std::shared_ptr<RSRenderNode> parent = item.second->GetParent().lock();
        if (parent == nullptr) {
            if (item.second != context.GetGlobalRootRenderNode()) {
                rootNodeList.emplace_back(item.second);
            }
        }
    }

    uint32_t rootNodeCount = rootNodeList.size();
    data.write(reinterpret_cast<const char*>(&rootNodeCount), sizeof(rootNodeCount));
    for (uint32_t i = 0; i < rootNodeCount; i++) {
        MarshallingNodeTree(map, data, rootNodeList[i].get());
    }
}

void RSProfiler::UnmarshallingNodes(RSRenderNodeMap& map, std::stringstream& data, RSContext& context)
{
    uint32_t count;
    data.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (uint32_t i = 0; i < count; i++) {
        UnmarshallingNode(map, data, context);
    }

    uint32_t rootNodeCount;
    data.read(reinterpret_cast<char*>(&rootNodeCount), sizeof(rootNodeCount));
    for (uint32_t i = 0; i < rootNodeCount; i++) {
        UnmarshallingNodeTree(map, data, context);
    }
}

void RSProfiler::MarshallingNode(const RSRenderNodeMap& map, std::stringstream& data, RSRenderNode* node)
{
    RSRenderNodeType nodeType = node->GetType();
    data.write(reinterpret_cast<const char*>(&nodeType), sizeof(nodeType));
    NodeId nodeId = node->GetId();
    data.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));

    bool isTextureExportNode = node->GetIsTextureExportNode();
    data.write(reinterpret_cast<const char*>(&isTextureExportNode), sizeof(isTextureExportNode));

    if (node->GetType() == RSRenderNodeType::SURFACE_NODE) {
        std::string surfName = reinterpret_cast<RSSurfaceRenderNode*>(node)->GetName();
        uint32_t size = surfName.size();
        data.write(reinterpret_cast<const char*>(&size), sizeof(size));
        data.write(reinterpret_cast<const char*>(surfName.c_str()), size);

        std::string surfBundleName = reinterpret_cast<RSSurfaceRenderNode*>(node)->GetBundleName();
        size = surfBundleName.size();
        data.write(reinterpret_cast<const char*>(&size), sizeof(size));
        data.write(reinterpret_cast<const char*>(surfBundleName.c_str()), size);

        RSSurfaceNodeType surfNodeType = reinterpret_cast<RSSurfaceRenderNode*>(node)->GetSurfaceNodeType();
        data.write(reinterpret_cast<const char*>(&surfNodeType), sizeof(surfNodeType));

        uint8_t bgAlpha = reinterpret_cast<RSSurfaceRenderNode*>(node)->GetAbilityBgAlpha();
        data.write(reinterpret_cast<const char*>(&bgAlpha), sizeof(bgAlpha));

        uint8_t globalAlpha = reinterpret_cast<RSSurfaceRenderNode*>(node)->GetGlobalAlpha();
        data.write(reinterpret_cast<const char*>(&globalAlpha), sizeof(globalAlpha));
    }

    float posZ = node->GetRenderProperties().GetPositionZ();
    data.write(reinterpret_cast<const char*>(&posZ), sizeof(posZ));

    float pivotZ = node->GetRenderProperties().GetPivotZ();
    data.write(reinterpret_cast<const char*>(&pivotZ), sizeof(pivotZ));

    NodePriorityType npt = node->GetPriority();
    data.write(reinterpret_cast<const char*>(&npt), sizeof(npt));

    bool isOnTree = node->IsOnTheTree();
    data.write(reinterpret_cast<const char*>(&isOnTree), sizeof(isOnTree));

    MarshallingNode(*node, data);
}

std::shared_ptr<RSRenderNode> RSProfiler::UnmarshallingNode(
    RSRenderNodeMap& map, std::stringstream& data, RSContext& context)
{
    RSRenderNodeType nodeType = RSRenderNodeType::UNKNOW;
    data.read(reinterpret_cast<char*>(&nodeType), sizeof(nodeType));

    NodeId nodeId = 0;
    data.read(reinterpret_cast<char*>(&nodeId), sizeof(nodeId));

    bool isTextureExportNode = false;
    data.read(reinterpret_cast<char*>(&isTextureExportNode), sizeof(isTextureExportNode));

    nodeId = Utils::PatchNodeId(nodeId);

    std::shared_ptr<RSRenderNode> renderNode = nullptr;
    if (nodeType == RSRenderNodeType::RS_NODE) {
        RootNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::DISPLAY_NODE) {
        RootNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::SURFACE_NODE) {
        uint32_t size = 0u;
        data.read(reinterpret_cast<char*>(&size), sizeof(size));

        std::string name;
        name.resize(size, ' ');
        data.read(reinterpret_cast<char*>(name.data()), size);

        data.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string bundleName;
        bundleName.resize(size, ' ');
        data.read(reinterpret_cast<char*>(bundleName.data()), size);

        RSSurfaceNodeType nodeType = RSSurfaceNodeType::DEFAULT;
        data.read(reinterpret_cast<char*>(&nodeType), sizeof(nodeType));

        RSSurfaceRenderNodeConfig config;
        config.isTextureExportNode = isTextureExportNode;
        config.nodeType = nodeType;
        config.id = nodeId;
        config.name = name + "_";
        config.bundleName = bundleName;

        std::shared_ptr<RSSurfaceRenderNode> surfRenderNode =
            std::make_shared<RSSurfaceRenderNode>(config, context.weak_from_this());
        renderNode = surfRenderNode;
        map.RegisterRenderNode(renderNode);

        uint8_t backgroundAlpha = 0u;
        data.read(reinterpret_cast<char*>(&backgroundAlpha), sizeof(backgroundAlpha));
        reinterpret_cast<RSSurfaceRenderNode*>(renderNode.get())->SetAbilityBGAlpha(backgroundAlpha);

        uint8_t globalAlpha = 0u;
        data.read(reinterpret_cast<char*>(&globalAlpha), sizeof(globalAlpha));
        reinterpret_cast<RSSurfaceRenderNode*>(renderNode.get())->SetGlobalAlpha(globalAlpha);
    } else if (nodeType == RSRenderNodeType::PROXY_NODE) {
        ProxyNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::CANVAS_NODE) {
        RSCanvasNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::EFFECT_NODE) {
        EffectNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::ROOT_NODE) {
        RootNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else if (nodeType == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        RSCanvasDrawingNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    } else {
        RootNodeCommandHelper::Create(context, nodeId, isTextureExportNode);
    }

    float positionZ = 0.0f;
    data.read(reinterpret_cast<char*>(&positionZ), sizeof(positionZ));

    float pivotZ = 0.0f;
    data.read(reinterpret_cast<char*>(&pivotZ), sizeof(pivotZ));

    NodePriorityType nodePriorityType = NodePriorityType::MAIN_PRIORITY;
    data.read(reinterpret_cast<char*>(&nodePriorityType), sizeof(nodePriorityType));

    bool isOnTree = false;
    data.read(reinterpret_cast<char*>(&isOnTree), sizeof(isOnTree));

    renderNode = map.GetRenderNode(nodeId);
    if (renderNode) {
        renderNode->GetMutableRenderProperties().SetPositionZ(positionZ);
        renderNode->GetMutableRenderProperties().SetPivotZ(pivotZ);
        renderNode->SetPriority(nodePriorityType);
        renderNode->SetIsOnTheTree(isOnTree);
        UnmarshallingNode(*renderNode, data);
    }

    return renderNode;
}

void RSProfiler::MarshallingNodeTree(const RSRenderNodeMap& map, std::stringstream& data, RSRenderNode* node)
{
    const auto& childrenList = node->children_;

    NodeId curNodeId = node->GetId();
    data.write(reinterpret_cast<const char*>(&curNodeId), sizeof(curNodeId));

    uint32_t count = childrenList.size();
    data.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& child : childrenList) {
        RSRenderNode* childNode = child.lock().get();
        NodeId childNodeId = childNode->GetId();
        data.write(reinterpret_cast<const char*>(&childNodeId), sizeof(childNodeId));
        MarshallingNodeTree(map, data, child.lock().get());
    }
}

void RSProfiler::UnmarshallingNodeTree(RSRenderNodeMap& map, std::stringstream& data, RSContext& context)
{
    NodeId curNodeId;
    data.read(reinterpret_cast<char*>(&curNodeId), sizeof(curNodeId));

    curNodeId = Utils::PatchNodeId(curNodeId);

    uint32_t count;
    data.read(reinterpret_cast<char*>(&count), sizeof(count));

    auto renderNode = map.GetRenderNode(curNodeId);

    for (uint32_t childIndex = 0; childIndex < count; childIndex++) {
        NodeId childNodeId;
        data.read(reinterpret_cast<char*>(&childNodeId), sizeof(childNodeId));

        childNodeId = Utils::PatchNodeId(childNodeId);

        auto childRenderNode = map.GetRenderNode(childNodeId);
        renderNode->AddChild(childRenderNode, childIndex);

        UnmarshallingNodeTree(map, data, context);
    }
}

void RSProfiler::MarshallingNode(const RSRenderNode& node, std::stringstream& data)
{
    data.write(reinterpret_cast<const char*>(&node.instanceRootNodeId_), sizeof(node.instanceRootNodeId_));
    data.write(reinterpret_cast<const char*>(&node.firstLevelNodeId_), sizeof(node.firstLevelNodeId_));

    const int recordingParcelMaxCapcity = 234 * 1024 * 1024;

    const uint32_t modCnt = node.modifiers_.size();
    data.write(reinterpret_cast<const char*>(&modCnt), sizeof(modCnt));
    for (auto& [id, modifier] : node.modifiers_) {
        data.write(reinterpret_cast<const char*>(&id), sizeof(id));

        Parcel parcel;
        parcel.SetMaxCapacity(recordingParcelMaxCapcity);
        modifier->Marshalling(parcel);
        const int32_t dataSize = parcel.GetDataSize();
        data.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        data.write(reinterpret_cast<const char*>(parcel.GetData()), dataSize);

        const int32_t objCount = parcel.GetOffsetsSize();
        data.write(reinterpret_cast<const char*>(&objCount), sizeof(objCount));
        data.write(reinterpret_cast<char*>(parcel.GetObjectOffsets()), objCount * sizeof(binder_size_t));
    }

    const uint32_t drawmodCnt = node.renderContent_->drawCmdModifiers_.size();
    data.write(reinterpret_cast<const char*>(&drawmodCnt), sizeof(drawmodCnt));

    for (auto& [type, modifiers] : node.renderContent_->drawCmdModifiers_) {
        data.write(reinterpret_cast<const char*>(&type), sizeof(type));
        uint32_t modCnt = modifiers.size();
        data.write(reinterpret_cast<const char*>(&modCnt), sizeof(modCnt));
        for (const auto& drawModifier : modifiers) {
            Parcel parcel;
            parcel.SetMaxCapacity(recordingParcelMaxCapcity);

            if (auto drawCmdList = reinterpret_cast<Drawing::DrawCmdList*>(drawModifier->GetDrawCmdListId())) {
                drawCmdList->MarshallingDrawOps();
            }

            drawModifier->Marshalling(parcel);
            const int32_t dataSize = parcel.GetDataSize();
            data.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
            data.write(reinterpret_cast<const char*>(parcel.GetData()), dataSize);

            const int32_t objCount = parcel.GetOffsetsSize();
            data.write(reinterpret_cast<const char*>(&objCount), sizeof(objCount));
            data.write(reinterpret_cast<char*>(parcel.GetObjectOffsets()), objCount * sizeof(binder_size_t));
        }
    }
}

void RSProfiler::UnmarshallingNode(RSRenderNode& node, std::stringstream& data)
{
    data.read(reinterpret_cast<char*>(&node.instanceRootNodeId_), sizeof(node.instanceRootNodeId_));
    node.instanceRootNodeId_ = Utils::PatchNodeId(node.instanceRootNodeId_);

    data.read(reinterpret_cast<char*>(&node.firstLevelNodeId_), sizeof(node.firstLevelNodeId_));
    node.firstLevelNodeId_ = Utils::PatchNodeId(node.firstLevelNodeId_);

    const int recordingParcelMaxCapcity = 234 * 1024 * 1024;

    int32_t modCnt = 0;
    data.read(reinterpret_cast<char*>(&modCnt), sizeof(modCnt));
    for (int32_t i = 0; i < modCnt; i++) {
        PropertyId propertyId;
        data.read(reinterpret_cast<char*>(&propertyId), sizeof(propertyId));
        propertyId = Utils::PatchNodeId(propertyId);

        int32_t bufferCnt = 0;
        data.read(reinterpret_cast<char*>(&bufferCnt), sizeof(bufferCnt));

        auto* buffer = new uint8_t[bufferCnt];
        data.read(reinterpret_cast<char*>(buffer), bufferCnt);

        uint8_t mpBuffer[sizeof(Parcel) + 1];
        auto* mpPtr = new (mpBuffer + 1) Parcel;
        mpPtr->SetMaxCapacity(recordingParcelMaxCapcity);
        mpPtr->WriteBuffer(buffer, bufferCnt);

        uint32_t objCount = 0;
        data.read(reinterpret_cast<char*>(&objCount), sizeof(objCount));
        binder_size_t objOffsets[objCount];
        data.read(reinterpret_cast<char*>(objOffsets), sizeof(objOffsets));
        mpPtr->InjectOffsets((binder_size_t)objOffsets, objCount);

        std::shared_ptr<RSRenderModifier> ptr =
            std::shared_ptr<RSRenderModifier>(RSRenderModifier::Unmarshalling(*mpPtr));
        node.AddModifier(ptr);

        delete[] buffer;
    }

    uint32_t drawmodCnt = 0;
    data.read(reinterpret_cast<char*>(&drawmodCnt), sizeof(drawmodCnt));

    for (uint32_t i = 0; i < drawmodCnt; i++) {
        RSModifierType modType;
        data.read(reinterpret_cast<char*>(&modType), sizeof(modType));

        uint32_t modCnt;
        data.read(reinterpret_cast<char*>(&modCnt), sizeof(modCnt));
        for (uint32_t j = 0; j < modCnt; j++) {
            uint32_t bufferCnt = 0;
            data.read(reinterpret_cast<char*>(&bufferCnt), sizeof(bufferCnt));

            auto* buffer = new uint8_t[bufferCnt];
            data.read(reinterpret_cast<char*>(buffer), bufferCnt);

            uint8_t mpBuffer[sizeof(Parcel) + 1];
            auto* mpPtr = new (mpBuffer + 1) Parcel;
            mpPtr->SetMaxCapacity(recordingParcelMaxCapcity);
            mpPtr->WriteBuffer(buffer, bufferCnt);

            uint32_t objCount = 0;
            data.read(reinterpret_cast<char*>(&objCount), sizeof(objCount));
            binder_size_t objOffsets[objCount];
            data.read(reinterpret_cast<char*>(objOffsets), sizeof(objOffsets));
            mpPtr->InjectOffsets((binder_size_t)objOffsets, objCount);

            std::shared_ptr<RSRenderModifier> ptr =
                std::shared_ptr<RSRenderModifier>(RSRenderModifier::Unmarshalling(*mpPtr));
            node.AddModifier(ptr);

            delete[] buffer;
        }
    }

    node.ApplyModifiers();
}

std::string RSProfiler::DumpRenderProperties(const RSRenderNode& node)
{
    if (node.renderContent_) {
        return node.renderContent_->renderProperties_.Dump();
    }
    return "";
}

std::string RSProfiler::DumpModifiers(const RSRenderNode& node)
{
    if (!node.renderContent_) {
        return "";
    }

    std::string out;
    out += "<";

    for (auto& [type, modifiers] : node.renderContent_->drawCmdModifiers_) {
        out += "(";
        out += std::to_string(static_cast<int32_t>(type));
        out += ", ";
        for (auto& item : modifiers) {
            out += "[";
            const auto propertyId = item->GetPropertyId();
            out += std::to_string(Utils::ExtractPid(propertyId));
            out += "|";
            out += std::to_string(Utils::ExtractNodeId(propertyId));
            out += " type=";
            out += std::to_string(static_cast<int32_t>(item->GetType()));
            out += " [modifier dump is not implemented yet]";
            out += "]";
        }
        out += ")";
    }

    out += ">";
    return out;
}

std::string RSProfiler::DumpSurfaceNode(const RSRenderNode& node)
{
    if (node.GetType() != RSRenderNodeType::SURFACE_NODE) {
        return "";
    }

    std::string out;
    const auto& surfaceNode = (static_cast<const RSSurfaceRenderNode&>(node));
    const auto parent = node.parent_.lock();
    out += ", Parent [" + (parent ? std::to_string(parent->GetId()) : "null") + "]";
    out += ", Name [" + surfaceNode.GetName() + "]";
    out += ", hasConsumer: " + std::to_string(static_cast<const RSSurfaceHandler&>(surfaceNode).HasConsumer());
    std::string contextAlpha = std::to_string(surfaceNode.contextAlpha_);
    std::string propertyAlpha = std::to_string(surfaceNode.GetRenderProperties().GetAlpha());
    out += ", Alpha: " + propertyAlpha + " (include ContextAlpha: " + contextAlpha + ")";
    out += ", Visible: " + std::to_string(surfaceNode.GetRenderProperties().GetVisible());
    out += ", " + surfaceNode.GetVisibleRegion().GetRegionInfo();
    out += ", OcclusionBg: " + std::to_string(surfaceNode.GetAbilityBgAlpha());
    out += ", Properties: " + node.GetRenderProperties().Dump();
    return out;
}

// RSAnimationManager
void RSProfiler::FilterAnimationForPlayback(RSAnimationManager& manager)
{
    EraseIf(manager.animations_, [](const auto& pair) -> bool {
        if (!Utils::IsNodeIdPatched(pair.first)) {
            return false;
        }
        pair.second->Finish();
        pair.second->Detach();
        return true;
    });
}

void RSProfiler::SetReplayTimes(double replayStartTime, double recordStartTime)
{
    g_transactionTimeCorrection = static_cast<int64_t>((replayStartTime - recordStartTime) * NS_TO_S);
}

} // namespace OHOS::Rosen