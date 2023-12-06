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

#include "pipeline/rs_dicide_hardware_enable.h"
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

bool RSDecideHardwareEnable::SortVectorCmp(const std::pair<uint64_t, double>& p1,
    const std::pair<uint64_t, double>& p2)
{
    return p1.second < p2.second;
}

bool RSDecideHardwareEnable::IsShouldHardwareEnable(uint64_t surfaceNodePtr) const
{
    if (surfaceBufferUpdateMap_.find(surfaceNodePtr) == surfaceBufferUpdateMap_.end()) {
        return false;
    }
    uint32_t queueTail = surfaceBufferUpdateMap_[surfaceNodePtr].first.back();
    if (vsyncCnt_ >= queueTail) {
        if (vsyncCnt - queueTail > LONGTIME_NO_UPDATE_VSYNC_CNT) {
            return false;
        }
    } else {
        if ((UINT32_MAX - queueTail) + vsyncCnt_ > LONGTIME_NO_UPDATE_VSYNC_CNT) {
            return false;
        }
    }

    std::vector<std::pair<uint64_t, double>> sortVec;
    for (auto iter = surfaceBufferUpdateMap_.begin(); iter != surfaceBufferUpdateMap_.end(); ++iter) {
        uint32_t queueSize = iter->second.first.size();
        uint32_t queueTail = iter->second.first.back();
        uint64_t recent = iter->second.second.recent + (vsyncCnt_ - queueTail) * queueSize;
        uint64_t frequecy = iter->second.second.frequecy;
        double result = (static_cast<double>(frequecy) / queueSize) * (static_cast<double>(recent) / queueSize);
        if (fabs(result - 0.0) < 1e-8) {
            result = DBL_MAX;
        }
        sortVec.push_back(std::make_pair(iter->first, result));
    }
    std::sort(sortVec.begin(), sortVec.end(), SortVectorCmp);

    uint32_t sequence = 0;
    for (auto iter = sortVec.begin(); iter != sortVec.end(); ++iter) {
        if ((*iter).first == surfaceNodePtr) {
            break;
        }
        sequence++;
    }

    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareEnable::IsShouldHardwareEnable surfaceNodePtr:" + std::to_string(surfaceNodePtr) +
        " sequence:" + std::to_string(sequence));
    if (sequence < SUPPORT_HWC_MAX_SIZE) {
        return true;
    }
    return false;
}

void RSDecideHardwareEnable::UpdateSurfaceNode(uint64_t surfaceNodePtr)
{
    if (surfaceBufferUpdateMap_.find(surfaceNodePtr) == surfaceBufferUpdateMap_.end()) {
        std::queue<uint32_t> queue;
        queue.push(vsyncCnt_)
        SortElements sortElements = {0, 0};
        surfaceBufferUpdateMap_[surfaceNodePtr] = std::make_pair(queue, sortElements);
        return;
    }

    uint32_t queueHead = surfaceBufferUpdateMap_[surfaceNodePtr].first.front();
    uint32_t queueTail = surfaceBufferUpdateMap_[surfaceNodePtr].first.back();
    uint32_t queueSize = surfaceBufferUpdateMap_[surfaceNodePtr].first.size();
    if (surfaceBufferUpdateMap_[surfaceNodePtr].first.size() > VSYNC_CNT_QUEUE_MAX_SIZE) {
        surfaceBufferUpdateMap_[surfaceNodePtr].first.pop();
        uint32_t queueNewHead = surfaceBufferUpdateMap_[surfaceNodePtr].first.front();
        if (LIKELY(queueNewHead >= queueHead)) {
            surfaceBufferUpdateMap_[surfaceNodePtr].second.frequecy -= (queueNewHead - queueHead);
        } else {
            surfaceBufferUpdateMap_[surfaceNodePtr].second.frequecy -= ((UINT32_MAX - queueHead) + queueNewHead);
        }
        if (LIKELY(vsyncCnt_ >= queueHead)) {
            surfaceBufferUpdateMap_[surfaceNodePtr].second.recent -= (vsyncCnt_ - queueHead);
        } else {
            surfaceBufferUpdateMap_[surfaceNodePtr].second.recent -= ((UINT32_MAX - queueHead) + vsyncCnt_);
        }
    }

    if (LIKELY(vsyncCnt_ >= queueTail)) {
        surfaceBufferUpdateMap_[surfaceNodePtr].second.frequecy += (vsyncCnt_ - queueTail);
        surfaceBufferUpdateMap_[surfaceNodePtr].second.recent += ((vsyncCnt_ - queueTail) * queueSize);
    } else {
        surfaceBufferUpdateMap_[surfaceNodePtr].second.frequecy += ((UINT32_MAX - queueTail) + vsyncCnt_);
        surfaceBufferUpdateMap_[surfaceNodePtr].second.recent += (((UINT32_MAX - queueTail) + vsyncCnt_) * queueSize);
    }

    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareEnable::UpdateSurfaceNode surfaceNodePtr:" +
        std::to_string(surfaceNodePtr) + " vsyncCnt_:" + std::to_string(vsyncCnt_) + " frequecy:" +
        std::to_string(surfaceBufferUpdateMap_[surfaceNodePtr].second.frequecy) + " recent:" +
        std::to_string(surfaceBufferUpdateMap_[surfaceNodePtr].second.recent));
    surfaceBufferUpdateMap_[surfaceNodePtr].first.push(vsyncCnt_);
}

void RSDecideHardwareEnable::DeleteSurfaceNode(uint64_t surfaceNodePtr)
{
    if (surfaceBufferUpdateMap_.find(surfaceNodePtr) == surfaceBufferUpdateMap_.end()) {
        return;
    }
    
    RS_OPTIONAL_TRACE_NAME("RSDecideHardwareEnable::DeleteSurfaceNode surfaceNodePtr:" + std::to_string(surfaceNodePtr));
    surfaceBufferUpdateMap_.erase(surfaceNodePtr);
}

void RSDecideHardwareEnable::UpdateVSyncCnt()
{
    vsyncCnt++;
}
}
}