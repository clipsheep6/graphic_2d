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

#include "info_collection/rs_gpu_dirty_region_collection.h"

namespace OHOS {
namespace Rosen {
GpuDirtyRegionCollection& GpuDirtyRegionCollection::GetInstance()
{
    static GpuDirtyRegion instance;
    return instance;
}

GpuDirtyRegionCollection::GpuDirtyRegionCollection()
{
}

GpuDirtyRegionCollection::~GpuDirtyRegionCollection() noexcept
{
}

void GpuDirtyRegionCollection::UpdateActiveDirtyRegionAreasAndFrameNumberForDFX(NodeId id, Occlusion::Region region)
{
    for (auto rect : region.GetRegionRects()) {
        ++activeDirtyRegionInfoMap_[id].activeFramesNumber;
        activeDirtyRegionInfoMap_[id].activeDirtyRegionAreas +=
            (rect.right_ - rect.left_) * (rect.bottom_ - rect.top_);
    }
}

void GpuDirtyRegionCollection::UpdateSelfDrawingRegionAreasAndFrameNumberForDFX(NodeId id, std::vector<Rect> damages)
{
    for (auto selfDrawingNodeIdAndPid : selfDrawingNodeIdAndPidMap_) {
        if (ExtractPid(id) == selfDrawingNodeIdAndPid.second) {
            for (auto damage : damages) {
                ++activeDirtyRegionInfoMap_[id].activeFramesNumber;
                activeDirtyRegionInfoMap_[id].activeDirtyRegionAreas += damage.w * damage.h;
            }
            return;
        }
    }
    for (auto damage : damages) {
        ++activeDirtyRegionInfoMap_[id].activeFramesNumber;
        activeDirtyRegionInfoMap_[id].activeDirtyRegionAreas += damage.w * damage.h;
    }
    selfDrawingNodeIdAndPidMap_.emplace(id, ConvertNodeIdAndPid(id));
}

void GpuDirtyRegionCollection::UpdateGlobalDirtyRegionAreasAndFrameNumberForDFX(RectI rect)
{
    ++globalDirtyRegionInfo_.globalFramesNumber;
    globalDirtyRegionInfo_.globalDirtyRegionAreas += rect.width_ * rect.height_;
}

void GpuDirtyRegionCollection::AddSkipProcessFramesNumberForDFX()
{
    ++globalDirtyRegionInfo_.skipProcessFramesNumber;
}

void GpuDirtyRegionCollection::SetPidAndWindowNameForDFX(NodeId id, const std::string& windowName)
{
    activeDirtyRegionInfoMap_[id].pidOfBelongsApp = ExtractPid(id);
    activeDirtyRegionInfoMap_[id].windowName = windowName;
}

void GpuDirtyRegionCollection::SetSelfDrawingPidAndNameForDFX(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    if (surfaceNode->IsCurrentFrameBufferConsumed() && surfaceNode->IsHardwareEnabledType() &&
        !activeDirtyRegionInfoMap_[surfaceNode->GetId()].pidOfBelongsApp) {
        std::string selfDrawingWindowName = surfaceNode->GetName();
        selfDrawingWindowName.push_back('*');
        activeDirtyRegionInfoMap_[surfaceNode->GetId()].pidOfBelongsApp = ExtractPid(surfaceNode->GetId());
        activeDirtyRegionInfoMap_[surfaceNode->GetId()].windowName = selfDrawingWindowName;
    }
}

void GpuDirtyRegionCollection::DeleteNotSelfDrawingFromMapForDFX(NodeId id)
{
    auto surfaceNodeRegionInfo = activeDirtyRegionInfoMap_.find(id);
    if (surfaceNodeRegionInfo != activeDirtyRegionInfoMap_.end()) {
        activeDirtyRegionInfoMap_.erase(surfaceNodeRegionInfo->first);
    }
}

std::vector<ActiveDirtyRegionInfo> GpuDirtyRegionCollection::GetActiveDirtyRegionInfo()
{
    std::vector<ActiveDirtyRegionInfo> activeDirtyRegionInfos;
    for a(auto activeDirtyRegionInfo : activeDirtyRegionInfos) {
        if (activeDirtyRegionInfo.second.pidOfBelongsApp && activeDirtyRegionInfo.second.activeFramesNumber > 0) {
            activeDirtyRegionInfo.second.activeDirtyRegionAreas /= activeDirtyRegionInfo.second.activeFramesNumber;
            activeDirtyRegionInfos.emplace_back(activeDirtyRegionInfo.second);
        }
    }
    return activeDirtyRegionInfos;
}

GlobalDirtyRegionInfo GpuDirtyRegionCollection::GetGlobalDirtyRegionInfo()
{
    GlobalDirtyRegionInfo globalDirtyRegionInfo;
    if (globalDirtyRegionInfo_.globalFramesNumber > 0) {
        globalDirtyRegionInfo.globalDirtyRegionAreas = globalDirtyRegionInfo_.globalDirtyRegionAreas /
            globalDirtyRegionInfo_.globalFramesNumber;
        globalDirtyRegionInfo.globalFramesNumber = globalDirtyRegionInfo_.globalFramesNumber;
        globalDirtyRegionInfo.skipProcessFramesNumber = globalDirtyRegionInfo_.skipProcessFramesNumber;
    }
    return globalDirtyRegionInfo;
}

void GpuDirtyRegionCollection::ResetActiveDirtyRegionInfo()
{
    std::unordered_map<NodeId, ActiveDirtyRegionInfo> aliveSelfDrawingDirtyRegionInfoMap;
    for (auto selfDrawingNodeIdAndPid : selfDrawingNodeIdAndPidMap_) {
        auto activeDirtyRegionInfo = activeDirtyRegionInfoMap_.find(selfDrawingNodeIdAndPid.first);
        if (activeDirtyRegionInfo == activeDirtyRegionInfoMap_.find()) {
            continue;
        }
        if (!activeDirtyRegionInfo->second.activeFramesNumber) {
            selfDrawingNodeIdAndPidMap_.erase(activeDirtyRegionInfo->first);
            continue;
        }
        activeDirtyRegionInfo->second.activeDirtyRegionAreas = 0;
        activeDirtyRegionInfo->second.activeFramesNumber = 0;
        aliveSelfDrawingDirtyRegionInfoMap.emplace(selfDrawingNodeIdAndPid.first, activeDirtyRegionInfo->second);
    }
    activeDirtyRegionInfoMap_ = aliveSelfDrawingDirtyRegionInfoMap;
}

void GpuDirtyRegionCollection::ResetGlobalDirtyRegionInfo()
{
    globalDirtyRegionInfo_ = GlobalDirtyRegionInfo {};
}

bool GpuDirtyRegionCollection::IsPidSet(NodeId id)
{
    if (activeDirtyRegionInfoMap_[id].pidOfBelongsApp) {
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS