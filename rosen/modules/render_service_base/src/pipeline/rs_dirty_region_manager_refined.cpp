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

#include "pipeline/rs_dirty_region_manager_refined.h"
#include <climits>
#include <memory>

namespace OHOS {
namespace Rosen {
namespace DirtyRegionManager {
DirtyRegion::DirtyRegion() {}
DirtyRegion::DirtyRegion(RectI rect)
{
    dirtyRects_.emplace_back(rect);
}

int DirtyRegion::GetRoughRectDistance(const RectI& a, const RectI& b)
{
    return abs(a.left_ + a.width_ / 2 - b.left_ - b.width_ / 2) + 
        abs(a.top_ + a.height_ / 2 - b.top_ - b.height_ / 2);
}

void DirtyRegion::MergeDirtyRect(const RectI& rect)
{
    if (IsEmpty()) {
        dirtyRects_.emplace_back(rect);
        return;
    }
    // record the closest rect in dirtyRects_ if all rects in dirtyRects_ are not intersect with current rect
    int minDisIndex = 0;
    int minDis = INT_MAX;
    for (unsigned int i = 0; i < dirtyRects_.size(); i++) {
        auto joinedRect = dirtyRects_[i].IntersectRect(rect);
        if (!joinedRect.IsEmpty()) {
            dirtyRects_[i] = dirtyRects_[i].JoinRect(rect);
            return;
        } else { // Get a roughly rect distance if not intersect;
            int roughDis = GetRoughRectDistance(dirtyRects_[i], rect);
            if (roughDis < minDis) {
                minDis = roughDis;
                minDisIndex = i;
            }
        }
    }
    if (dirtyRects_.size() < _s_maxSeperateDirtyRegion_num_) {
        dirtyRects_.emplace_back(rect);
    } else {
        // merge with closest rect in dirtyRects_
        dirtyRects_[minDisIndex] = dirtyRects_[minDisIndex].JoinRect(rect);
    }
}

void DirtyRegion::IntersectWithRect(const RectI& rect)
{
    for(auto& r : dirtyRects_) {
        r = r.IntersectRect(rect);
    }
    std::__libcpp_erase_if_container(dirtyRects_, [](RectI r) { return r.IsEmpty(); } );
}

const std::vector<RectI>& DirtyRegion::GetDirtyRects() const
{
    return dirtyRects_;
}

void DirtyRegion::MergeDirtyRegion(const DirtyRegion& history)
{
    for(auto &hRect : history.GetDirtyRects()) {
        MergeDirtyRect(hRect);
    }
}


RSDirtyRegionManagerRefined::RSDirtyRegionManagerRefined()
{
    dirtyHistory_.resize(HISTORY_QUEUE_MAX_SIZE);
    debugRegionEnabled_.resize(DebugRegionType::TYPE_MAX);
}

void RSDirtyRegionManagerRefined::Clear()
{
    current_.Clear();
    dirtyCanvasNodes_.clear();
    dirtySurfaceNodes_.clear();
    UpdateDebugRegionTypeEnable();
}

void RSDirtyRegionManagerRefined::MergeDirtyRect(const RectI& rect)
{
    current_.MergeDirtyRect(rect);
}

const DirtyRegion& RSDirtyRegionManagerRefined::GetDirtyRegion() const
{
    return current_;
}

std::vector<RectI> RSDirtyRegionManagerRefined::GetDirtyRegionFlipWithinSurface() const
{
    std::vector<RectI> res;
    for(auto& r : current_.GetDirtyRects()) {
        RectI glRect = r;
        // left-top to left-bottom corner(in current surface)
        glRect.top_ = surfaceRect_.height_ - r.top_ - r.height_;
        res.emplace_back(glRect);
    }
    return res;
}

bool RSDirtyRegionManagerRefined::SetBufferAge(const int age)
{
    if (age < 0) {
        bufferAge_ = 0; // reset invalid age
        return false;
    }
    bufferAge_ = static_cast<unsigned int>(age);
    return true;
}

void RSDirtyRegionManagerRefined::UpdateDirtyCanvasNodes(NodeId id, const RectI& rect)
{
    dirtyCanvasNodes_[id] = rect;
}

void RSDirtyRegionManagerRefined::UpdateDirtySurfaceNodes(NodeId id, const RectI& rect)
{
    dirtySurfaceNodes_[id] = rect;
}

void RSDirtyRegionManagerRefined::GetDirtyCanvasNodes(std::map<NodeId, RectI>& target) const
{
    target = dirtyCanvasNodes_;
}

void RSDirtyRegionManagerRefined::GetDirtySurfaceNodes(std::map<NodeId, RectI>& target) const
{
    target = dirtySurfaceNodes_;
}

void RSDirtyRegionManagerRefined::UpdateDebugRegionTypeEnable()
{
    DirtyRegionDebugType dirtyDebugType = RSSystemProperties::GetDirtyRegionDebugType();
    debugRegionEnabled_.assign(DebugRegionType::TYPE_MAX, false);
    switch (dirtyDebugType) {
        case DirtyRegionDebugType::CURRENT_SUB:
            debugRegionEnabled_[DebugRegionType::CURRENT_SUB] = true;
            break;
        case DirtyRegionDebugType::CURRENT_WHOLE:
            debugRegionEnabled_[DebugRegionType::CURRENT_WHOLE] = true;
            break;
        case DirtyRegionDebugType::MULTI_HISTORY:
            debugRegionEnabled_[DebugRegionType::MULTI_HISTORY] = true;
            break;
        case DirtyRegionDebugType::CURRENT_SUB_AND_WHOLE:
            debugRegionEnabled_[DebugRegionType::CURRENT_SUB] = true;
            debugRegionEnabled_[DebugRegionType::CURRENT_WHOLE] = true;
            break;
        case DirtyRegionDebugType::CURRENT_WHOLE_AND_MULTI_HISTORY:
            debugRegionEnabled_[DebugRegionType::CURRENT_WHOLE] = true;
            debugRegionEnabled_[DebugRegionType::MULTI_HISTORY] = true;
            break;
        case DirtyRegionDebugType::DISABLED:
        default:
            break;
    }
}

void RSDirtyRegionManagerRefined::UpdateDirty()
{
    PushHistory(current_);
    current_ = MergeHistory(bufferAge_, current_);
}

void RSDirtyRegionManagerRefined::UpdateDirty(const int bufferage)
{
    SetBufferAge(bufferage);
    PushHistory(current_);
    current_ = MergeHistory(bufferAge_, current_);
}

bool RSDirtyRegionManagerRefined::SetSurfaceSize(const int width, const int height)
{
    if (width < 0 || height < 0) {
        return false;
    }
    surfaceRect_ = RectI(0, 0, width, height);
    return true;
}

void RSDirtyRegionManagerRefined::IntersectDirtyRectWithSurfaceRect()
{
    current_.IntersectWithRect(surfaceRect_);
}

DirtyRegion RSDirtyRegionManagerRefined::MergeHistory(unsigned int age, DirtyRegion dirtyRegion) const
{
    if (age == 0 || age > historySize_) {
        return surfaceRect_;
    }
    // GetHistory(historySize_) = dirtyHistory_[historyHead_]
    // therefore, this loop merges rect with (age-1) frames' dirtyRect
    for (unsigned int i = historySize_ - 1; i > historySize_ - age; --i) {
        auto subRegion = GetHistory(i);
        if (subRegion.IsEmpty()) {
            continue;
        }
        if (dirtyRegion.IsEmpty()) {
            dirtyRegion = subRegion;
            continue;
        }
        // only join valid history dirty region
        dirtyRegion.MergeDirtyRegion(subRegion);
    }
    return dirtyRegion;
}

void RSDirtyRegionManagerRefined::PushHistory(DirtyRegion dirtyRegion)
{
    int next = (historyHead_ + 1) % HISTORY_QUEUE_MAX_SIZE;
    dirtyHistory_[next] = dirtyRegion;
    if (historySize_ < HISTORY_QUEUE_MAX_SIZE) {
        ++historySize_;
    }
    historyHead_ = next;
}

const DirtyRegion& RSDirtyRegionManagerRefined::GetHistory(unsigned int i) const
{
    if (i >= HISTORY_QUEUE_MAX_SIZE) {
        i %= HISTORY_QUEUE_MAX_SIZE;
    }
    if (historySize_ == HISTORY_QUEUE_MAX_SIZE) {
        i = (i + historyHead_) % HISTORY_QUEUE_MAX_SIZE;
    }
    return dirtyHistory_[i];
}

}
}
}