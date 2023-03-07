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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_REFINED_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_REFINED_H

#include <map>
#include <vector>

#include "common/rs_rect.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
namespace DirtyRegionManager {
enum DebugRegionType {
    CURRENT_SUB,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    TYPE_MAX
};


class DirtyRegion final {
public:
    DirtyRegion();
    DirtyRegion(RectI rect);
    ~DirtyRegion() = default;

    // when a new dirty rect is added, try to join with existing rect if intersect
    // when rect is not intersect with all existing rects, join with the closest rect
    void MergeDirtyRect(const RectI& rect);

    // each rect in dirtyRects_ turns to intersection with input rect
    void IntersectWithRect(const RectI& rect);

    // Merge history DirtyRegion
    void MergeDirtyRegion(const DirtyRegion& history);
    static int GetRoughRectDistance(const RectI& a, const RectI& b);
    inline bool IsDirty() const
    {
        return IsEmpty();
    }
    inline bool IsEmpty() const
    {
        return dirtyRects_.size() > 0;
    }
    inline void Clear()
    {
        dirtyRects_.clear();
    }
    const std::vector<RectI>& GetDirtyRects() const;

    // squeeze dirty region at last before merge history
    void Squeeze();

private:
    inline static unsigned int _s_maxSeperateDirtyRegion_num_ = 3;
    std::vector<RectI> dirtyRects_;

};

class RSDirtyRegionManagerRefined final {
public:
    RSDirtyRegionManagerRefined();
    ~RSDirtyRegionManagerRefined() = default;
    void Clear();
    inline bool IsDirty() const
    {
        return current_.IsDirty();
    }

    void MergeDirtyRect(const RectI& rect);
    
    // return merged historical region
    const DirtyRegion& GetDirtyRegion() const;
    // return merged historical region upsize down in surface
    std::vector<RectI> GetDirtyRegionFlipWithinSurface() const;
    
    // history related functions
    bool SetBufferAge(const int age);
    void UpdateDirty();
    void UpdateDirty(const int age);
    
    // set current framebuffer size, clip to framebuffer bound
    bool SetSurfaceSize(const int width, const int height);
    void IntersectDirtyRectWithSurfaceRect();

    // record dirty canvas and surface node id & their dirty rect for debug
    void UpdateDirtyCanvasNodes(NodeId id, const RectI& rect);
    void UpdateDirtySurfaceNodes(NodeId id, const RectI& rect);
    void GetDirtyCanvasNodes(std::map<NodeId, RectI>& target) const;
    void GetDirtySurfaceNodes(std::map<NodeId, RectI>& target) const;
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
    DirtyRegion MergeHistory(unsigned int age, DirtyRegion rect) const;
    void PushHistory(DirtyRegion dirtyRegion);
    const DirtyRegion& GetHistory(unsigned int i) const;

    // surface (framebuffer) size
    RectI surfaceRect_;

    // debug related
    std::map<NodeId, RectI> dirtyCanvasNodes_;
    std::map<NodeId, RectI> dirtySurfaceNodes_;
    std::vector<bool> debugRegionEnabled_;

    // main dirty Region variable
    // The reason we do not use Occlusion::Region to represent this variable is that a single
    // window can have a lot of dirty rects, and using Occlusion::Region::Or cound be much slower than Rect joining
    DirtyRegion current_;

    // history related
    std::vector<DirtyRegion> dirtyHistory_;
    int historyHead_ = -1;
    unsigned int historySize_ = 0;
    const unsigned HISTORY_QUEUE_MAX_SIZE = 4;
    // may add new set function for bufferAge
    unsigned int bufferAge_ = HISTORY_QUEUE_MAX_SIZE;
};
}
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_REFINED_H