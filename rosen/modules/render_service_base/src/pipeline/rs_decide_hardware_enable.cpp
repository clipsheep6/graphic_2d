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

#include "pipeline/rs_decide_hardware_enable.h"
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
RSDecideHardwareEnable* RSDecideHardwareEnable::Instance()
{
    static RSDecideHardwareEnable instance;
    return &instance;
}

bool RSDecideHardwareEnable::SortVectorCmp(const std::pair<NodeId, double>& p1,
    const std::pair<NodeId, double>& p2)
{
    return p1.second < p2.second;
}

void RSDecideHardwareEnable::UpdateSurfaceBufferSortVector()
{
    std::vector <std::pair<NodeId, double>>().swap(surfaceBufferSortVec_);
    for (auto iter = surfaceBufferUpdateMap_.begin(); iter != surfaceBufferUpdateMap_.end(); ++iter) {
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

bool RSDecideHardwareEnable::IsShouldHardwareEnable(NodeId nodeId) const
{
    if (surfaceBufferUpdateMap_.find(nodeId) == surfaceBufferUpdateMap_.end()) {
        return false;
    }
    uint32_t queueTail = surfaceBufferUpdateMap_[nodeId].first.back();
    if (vsyncCnt_ >= queueTail) {
        if (vsyncCnt - queueTail > LONGTIME_NO_UPDATE_VSYNC_CNT) {
            return false;
        }
    } else {
        if ((UINT32_MAX - queueTail) + vsyncCnt_ > LONGTIME_NO_UPDATE_VSYNC_CNT) {
            return false;
        }
    }

    uint32_t sequence = 0;
    for (auto iter = surfaceBufferSortVec_.begin(); iter != surfaceBufferSortVec_.end(); ++iter) {
        if ((*iter).first == nodeId) {
            break;
        }
        sequence++;
    }

    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareEnable::IsShouldHardwareEnable nodeId:" + std::to_string(nodeId) +
        " sequence:" + std::to_string(sequence));
    if (sequence < SUPPORT_HWC_MAX_SIZE) {
        return true;
    }
    return false;
}

void RSDecideHardwareEnable::UpdateSurfaceNode(NodeId nodeId)
{
    if (nodeId == 0) {
        return;
    }
    if (surfaceBufferUpdateMap_.find(nodeId) == surfaceBufferUpdateMap_.end()) {
        std::queue<uint32_t> queue;
        queue.push(vsyncCnt_)
        SortElements sortElements = {0, 0};
        surfaceBufferUpdateMap_[nodeId] = std::make_pair(queue, sortElements);
        return;
    }

    auto& queue = &(surfaceBufferUpdateMap_[nodeId].first);
    auto& sortElements = &(surfaceBufferUpdateMap_[nodeId].second);
    uint32_t queueHead = queue.front();
    uint32_t queueTail = queue.back();
    uint32_t queueSize = queue.size();
    if (queue.size() > VSYNC_CNT_QUEUE_MAX_SIZE) {
        queue.pop();
        uint32_t queueNewHead = queue.front();
        if (LIKELY(queueNewHead >= queueHead)) {
            sortElements.frequecy -= (queueNewHead - queueHead);
        } else {
            sortElements.frequecy -= ((UINT32_MAX - queueHead) + queueNewHead);
        }
        if (LIKELY(vsyncCnt_ >= queueHead)) {
            sortElements.recent -= (vsyncCnt_ - queueHead);
        } else {
            sortElements.recent -= ((UINT32_MAX - queueHead) + vsyncCnt_);
        }
    }

    if (LIKELY(vsyncCnt_ >= queueTail)) {
        sortElements.frequecy += (vsyncCnt_ - queueTail);
        sortElements.recent += ((vsyncCnt_ - queueTail) * queueSize);
    } else {
        sortElements.frequecy += ((UINT32_MAX - queueTail) + vsyncCnt_);
        sortElements.recent += (((UINT32_MAX - queueTail) + vsyncCnt_) * queueSize);
    }

    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareEnable::UpdateSurfaceNode nodeId:" +
        std::to_string(nodeId) + " vsyncCnt_:" + std::to_string(vsyncCnt_) + " frequecy:" +
        std::to_string(sortElements.frequecy) + " recent:" + std::to_string(sortElements.recent));
    surfaceBufferUpdateMap_[nodeId].first.push(vsyncCnt_);
}

void RSDecideHardwareEnable::DeleteSurfaceNode(NodeId nodeId)
{
    if (surfaceBufferUpdateMap_.find(nodeId) == surfaceBufferUpdateMap_.end()) {
        return;
    }
    
    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareEnable::DeleteSurfaceNode nodeId:" + std::to_string(nodeId));
    surfaceBufferUpdateMap_.erase(nodeId);
}

void RSDecideHardwareEnable::UpdateVSyncCnt()
{
    vsyncCnt_++;
}
}
}