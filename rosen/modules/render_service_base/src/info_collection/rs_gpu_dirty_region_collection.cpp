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
    static GpuDirtyRegionCollection instance;
    return instance;
}

GpuDirtyRegionCollection::GpuDirtyRegionCollection()
{
}

GpuDirtyRegionCollection::~GpuDirtyRegionCollection() noexcept
{
}

void GpuDirtyRegionCollection::UpdateActiveDirtyRegionAreasAndFrameNumberForDFX(NodeId id, std::vector<RectI> rects)
{
    ++gpuDirtyRegionInfoMap_[id].activeFramesNumber;
    for (auto rect : rects) {
        gpuDirtyRegionInfoMap_[id].activeGpuDirtyRegionAreas += rect.width_ * rect.height_;
    }
}

void GpuDirtyRegionCollection::UpdateGlobalDirtyRegionAreasAndFrameNumberForDFX(NodeId id, RectI rect)
{
    ++gpuDirtyRegionInfoMap_[id].globalFramesNumber;
    gpuDirtyRegionInfoMap_[id].globalGpuDirtyRegionAreas += rect.width_ * rect.height_;
}

void GpuDirtyRegionCollection::AddSkipProcessFramesNumberForDFX(NodeId id)
{
    ++gpuDirtyRegionInfoMap_[id].skipProcessFramesNumber;
}

void GpuDirtyRegionCollection::SetWindowNameForDFX(NodeId id, std::string& windowName)
{
    gpuDirtyRegionInfoMap_[id].windowName = windowName;
}

GpuDirtyRegionInfo GpuDirtyRegionCollection::GetGpuDirtyRegionInfo(NodeId id)
{
    if (gpuDirtyRegionInfoMap_.find(id) == gpuDirtyRegionInfoMap_.end()) {
        return GpuDirtyRegionInfo {};
    } else {
        gpuDirtyRegionInfoMap_[id].activeGpuDirtyRegionAreas /= gpuDirtyRegionInfoMap_[id].activeFramesNumber;
        gpuDirtyRegionInfoMap_[id].globalGpuDirtyRegionAreas /= gpuDirtyRegionInfoMap_[id].globalFramesNumber;
        return gpuDirtyRegionInfoMap_[id];
    }
}

void GpuDirtyRegionCollection::ResetDirtyRegionInfo()
{
    gpuDirtyRegionInfoMap_.clear();
}
} // namespace Rosen
} // namespace OHOS