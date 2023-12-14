/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_decide_hardware_disable.h"
#include <algorithm>
#include <cfloat>
#include <math.h>
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {
const uint32_t VSYNC_CNT_QUEUE_MAX_SIZE = 10;
const uint32_t SUPPORT_HWC_MAX_SIZE = 8;
const uint32_t LONGTIME_NO_UPDATE_VSYNC_CNT = 100;
const std::string HWC_WHITE_LIST[] = {
    "pointer window",
};
RSDecideHardwareDisable* RSDecideHardwareDisable::Instance()
{
    static RSDecideHardwareDisable instance;
    return &instance;
}

bool RSDecideHardwareDisable::SortVectorCmp(const std::pair<NodeId, double>& p1,
    const std::pair<NodeId, double>& p2)
{
    return p1.second < p2.second;
}

void RSDecideHardwareDisable::UpdateSurfaceBufferSortVector()
{
    std::vector <std::pair<NodeId, double>>().swap(surfaceBufferSortVec_);
    curVSyncHwcWhiteCnt_ = 0;
    for (auto iter = surfaceBufferUpdateMap_.begin(); iter != surfaceBufferUpdateMap_.end(); ++iter) {
        auto surfaceNode = iter->second.second.surfaceNode.lock();
        if (surfaceNode && surfaceNode->GetHardwareForcedDisabled()) {
            continue;
        }
        if (IsInHwcWhiteList(surfaceNode->GetName())) {
            curVSyncHwcWhiteCnt_++;
        }
        uint32_t queueSize = iter->second.first.size();
        uint32_t queueTail = iter->second.first.back();
        uint64_t recent = iter->second.second.recent + (vsyncCnt_ - queueTail) * queueSize;
        uint64_t frequecy = iter->second.second.frequecy;
        double result = (static_cast<double>(frequecy) / queueSize) * (static_cast<double>(recent) / queueSize);
        if (fabs(result - 0.0) < 1e-8) {
            result = DBL_MAX;
        }
        surfaceBufferSortVec_.emplace_back(std::make_pair(iter->first, result));
    }
    std::sort(surfaceBufferSortVec_.begin(), surfaceBufferSortVec_.end(), SortVectorCmp);
}

bool RSDecideHardwareDisable::IsInHwcWhiteList(const std::string& nodeName)
{
    for (std::string i : HWC_WHITE_LIST) {
        if (nodeName.compare(i) == 0) {
            return true;
        }
    }
    return false;
}

bool RSDecideHardwareDisable::IsHardwareDisabledBySort(const RSSurfaceRenderNode& node)
{
    if (IsInHwcWhiteList(node.GetName())) {
        return false;
    }
    NodeId nodeId = node.GetId();
    if (surfaceBufferUpdateMap_.find(nodeId) == surfaceBufferUpdateMap_.end()) {
        return true;
    }
    uint32_t queueTail = surfaceBufferUpdateMap_[nodeId].first.back();
    if (vsyncCnt_ >= queueTail) {
        if ((vsyncCnt_ - queueTail) > LONGTIME_NO_UPDATE_VSYNC_CNT) {
            return true;
        }
    } else {
        if (((UINT32_MAX - queueTail) + vsyncCnt_) > LONGTIME_NO_UPDATE_VSYNC_CNT) {
            return true;
        }
    }

    uint32_t sequence = 0;
    bool flag = false;
    for (auto iter = surfaceBufferSortVec_.begin(); iter != surfaceBufferSortVec_.end(); ++iter) {
        if ((*iter).first == nodeId) {
            flag = true;
            break;
        }
        sequence++;
    }

    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareDisable::IsHardwareDisabledBySort nodeId:" + std::to_string(nodeId) +
        " flag:" + std::to_string(flag) + " vecSize:" + std::to_string(static_cast<uint32_t>(surfaceBufferSortVec_.size())) +
        " sequence:" + std::to_string(sequence) + " curVSyncHwcWhiteCnt_:" + std::to_string(curVSyncHwcWhiteCnt_));
    if (curVSyncHwcWhiteCnt_ < SUPPORT_HWC_MAX_SIZE) {
        if (flag && (sequence < (SUPPORT_HWC_MAX_SIZE - curVSyncHwcWhiteCnt_))) {
            return false;
        }
    }
    return true;
}

void RSDecideHardwareDisable::UpdateSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    if (surfaceNode == nullptr) {
        return;
    }
    NodeId nodeId = surfaceNode->GetId();
    if (surfaceBufferUpdateMap_.find(nodeId) == surfaceBufferUpdateMap_.end()) {
        std::queue<uint32_t> queue;
        queue.push(vsyncCnt_);
        SortElements sortElements = {0, 0, surfaceNode};
        surfaceBufferUpdateMap_[nodeId] = std::make_pair(queue, sortElements);
        return;
    }

    auto queue = &(surfaceBufferUpdateMap_[nodeId].first);
    auto sortElements = &(surfaceBufferUpdateMap_[nodeId].second);
    uint32_t queueHead = queue->front();
    uint32_t queueTail = queue->back();
    uint32_t queueSize = queue->size();
    if (queueSize > VSYNC_CNT_QUEUE_MAX_SIZE) {
        queue->pop();
        uint32_t queueNewHead = queue->front();
        if (LIKELY(queueNewHead >= queueHead)) {
            sortElements->frequecy -= (queueNewHead - queueHead);
        } else {
            sortElements->frequecy -= ((UINT32_MAX - queueHead) + queueNewHead);
        }
        if (LIKELY(vsyncCnt_ >= queueHead)) {
            sortElements->recent -= (vsyncCnt_ - queueHead);
        } else {
            sortElements->recent -= ((UINT32_MAX - queueHead) + vsyncCnt_);
        }
    }

    if (LIKELY(vsyncCnt_ >= queueTail)) {
        sortElements->frequecy += (vsyncCnt_ - queueTail);
        sortElements->recent += ((vsyncCnt_ - queueTail) * queueSize);
    } else {
        sortElements->frequecy += ((UINT32_MAX - queueTail) + vsyncCnt_);
        sortElements->recent += (((UINT32_MAX - queueTail) + vsyncCnt_) * queueSize);
    }

    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareDisable::UpdateSurfaceNode nodeId:" +
        std::to_string(nodeId) + " vsyncCnt_:" + std::to_string(vsyncCnt_) + " frequecy:" +
        std::to_string(sortElements->frequecy) + " recent:" + std::to_string(sortElements->recent));
    surfaceBufferUpdateMap_[nodeId].first.push(vsyncCnt_);
}

void RSDecideHardwareDisable::DeleteSurfaceNode(NodeId nodeId)
{
    if (surfaceBufferUpdateMap_.find(nodeId) == surfaceBufferUpdateMap_.end()) {
        return;
    }
    
    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareDisable::DeleteSurfaceNode nodeId:" + std::to_string(nodeId));
    surfaceBufferUpdateMap_.erase(nodeId);
}

void RSDecideHardwareDisable::UpdateVSyncCnt()
{
    vsyncCnt_++;
}
}
}