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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H

#include <map>
#include <vector>

#include "common/rs_rect.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
enum DebugRegionType {
    CURRENT_SUB,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    TYPE_MAX
};

class RSDirtyRegionManager final {
public:
    RSDirtyRegionManager();
    ~RSDirtyRegionManager() = default;
    void MergeDirtyRect(const RectI& rect);
    void IntersectDirtyRect(const RectI& rect);
    void Clear();
    const RectI& GetDirtyRegion() const;
    bool IsDirty() const;
    void UpdateDirty();
    void UpdateDirtyCanvasNodes(NodeId id, const RectI& rect);
    void UpdateDirtySurfaceNodes(NodeId id, const RectI& rect);
    void GetDirtyCanvasNodes(std::map<NodeId, RectI>& target) const;
    void GetDirtySurfaceNodes(std::map<NodeId, RectI>& target) const;
    bool SetBufferAge(const int age);
    RectI GetAllHistoryMerge();
    std::vector<RectI> GetDirtyHistory() const
    {
        return dirtyHistory_;
    }

    void UpdateDebugRegionTypeEnable();
    
    inline bool IsDebugRegionTypeEnable(DebugRegionType var) const
    {
        if (var < DebugRegionType::TYPE_MAX) {
            return debugRegionEnabled_[var];
        }
        return false;
    }
    
    inline bool IsDebugEnabled() const
    {
        return RSSystemProperties::GetDirtyRegionDebugType() != DirtyRegionDebugType::DISABLED;
    }

private:
    RectI MergeHistory(unsigned int age, RectI rect) const;
    void PushHistory(RectI rect);
    RectI GetHistory(unsigned int i) const;

    RectI dirtyRegion_;
    std::map<NodeId, RectI> dirtyCanvasNodes_;
    std::map<NodeId, RectI> dirtySurfaceNodes_;
    std::vector<bool> debugRegionEnabled_;
    std::vector<RectI> dirtyHistory_;
    int historyHead_ = -1;
    unsigned int historySize_ = 0;
    const unsigned HISTORY_QUEUE_MAX_SIZE = 4;
    // may add new set function for bufferAge
    unsigned int bufferAge_ = HISTORY_QUEUE_MAX_SIZE;

    int surfaceWidth_ = 0;
    int surfaceHeight_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H