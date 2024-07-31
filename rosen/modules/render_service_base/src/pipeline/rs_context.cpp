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

#include "pipeline/rs_context.h"

#include "rs_trace.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

int g_Material_Interval =  0;
int g_Arbar_Interval    =  0;
int pre_frameMaterial   = -1;

namespace OHOS::Rosen {
void RSContext::RegisterAnimatingRenderNode(const std::shared_ptr<RSRenderNode>& nodePtr)
{
    NodeId id = nodePtr->GetId();
    animatingNodeList_.emplace(id, nodePtr);
    nodePtr->ActivateDisplaySync();
    ROSEN_LOGD("RSContext::RegisterAnimatingRenderNode, register node id: %{public}" PRIu64, id);
}

bool RSContext::IsHasFilter(int focus_pid)
{
    for (auto[filter, pid] : cacheManagerFilterList_) {
        if (pid == focus_pid) {
            return true;
        }
    }
    return false;
}

void RSContext::AddFilter(int focus_pid, std::shared_ptr<RSFilter> filter)
{
    if (cacheManagerFilterList_.find(filter) == cacheManagerFilterList_.end()) {
        cacheManagerFilterList_[filter] = focus_pid;
    }
}

int RSContext::GetMaterialLongOrShortFrame()
{
    if (cacheManagerFilterList_.empty()) {
        pre_frameMaterial = INVALID_FRAME;
        return INVALID_FRAME;
    }
    if (uiLong_) {
        pre_frameMaterial = MATERIAL_UILONG_FRAME;
        return MATERIAL_UILONG_FRAME;
    }
    for (auto[filter, pid] : cacheManagerFilterList_) {
        if (filter->GetFilterType() == RSFilter::MATERIAL && g_Material_Interval == 0) {
            pre_frameMaterial = MATERIAL_LONG_FRAME;
            return MATERIAL_LONG_FRAME;
        }
    }
    // The secondary frame is forcibly determined as a long frame
    if (pre_frameMaterial == INVALID_FRAME) {
        pre_frameMaterial = MATERIAL_LONG_FRAME;
        return MATERIAL_LONG_FRAME;
    }
    pre_frameMaterial = MATERIAL_SHORT_FRAME;
    return MATERIAL_SHORT_FRAME;
}

int RSContext::GetArbarLongOrShortFrame()
{
    if (cacheManagerFilterList_.empty()) {
        return INVALID_FRAME;
    }
    for (auto[filter, pid] : cacheManagerFilterList_) {
        if (filter->GetFilterType() == RSFilter::AIBAR && g_Arbar_Interval == 0) {
            return AIBAR_LONG_FRAME;
        }
    }
    return AIBAR_SHORT_FRAME;
}

void RSContext::ClearFilte() {
    uiLong_ = false;
    g_Material_Interval = 0;
    g_Arbar_Interval = 0;
    cacheManagerFilterList_.clear();
}

void RSContext::UnregisterAnimatingRenderNode(NodeId id)
{
    animatingNodeList_.erase(id);
    ROSEN_LOGD("RSContext::UnregisterAnimatingRenderNode, unregister node id: %{public}" PRIu64, id);
}

void RSContext::AddActiveNode(const std::shared_ptr<RSRenderNode>& node)
{
    if (node == nullptr || node->GetId() == INVALID_NODEID) {
        return;
    }
    auto rootNodeId = node->GetInstanceRootNodeId();
    std::lock_guard<std::mutex> lock(activeNodesInRootMutex_);
    activeNodesInRoot_[rootNodeId].emplace(node->GetId(), node);
}

bool RSContext::HasActiveNode(const std::shared_ptr<RSRenderNode>& node)
{
    if (node == nullptr || node->GetId() == INVALID_NODEID) {
        return false;
    }
    auto rootNodeId = node->GetInstanceRootNodeId();
    std::lock_guard<std::mutex> lock(activeNodesInRootMutex_);
    return activeNodesInRoot_[rootNodeId].count(node->GetId()) > 0;
}

void RSContext::AddPendingSyncNode(const std::shared_ptr<RSRenderNode> node)
{
    if (node == nullptr || node->GetId() == INVALID_NODEID) {
        return;
    }
    pendingSyncNodes_.emplace(node->GetId(), node);
}

void RSContext::MarkNeedPurge(ClearMemoryMoment moment, PurgeType purgeType)
{
    clearMoment_ = moment;
    purgeType_ = purgeType;
}

void RSContext::SetClearMoment(ClearMemoryMoment moment)
{
    clearMoment_ = moment;
}

ClearMemoryMoment RSContext::GetClearMoment() const
{
    return clearMoment_;
}

void RSContext::Initialize()
{
    nodeMap.Initialize(weak_from_this());
    globalRootRenderNode_->OnRegister(weak_from_this());
}
} // namespace OHOS::Rosen
