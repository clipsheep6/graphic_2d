/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_pre_compose_region_manager.h"
#include "platform/common/rs_log.h"
#include "common/rs_obj_abs_geometry.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSPreComposeRegionManager::RSPreComposeRegionManager()
{
    ROSEN_LOGD("RSPreComposeRegionManager()");
    dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
}

RSPreComposeRegionManager::~RSPreComposeRegionManager()
{
    ROSEN_LOGD("~RSPreComposeRegionManager()");
}

bool RSPreComposeRegionManager::CheckNodeChange()
{
    bool isNodeChange = false;
    std::vector<NodeId> curSurfaceIds;
    curSurfaceIds.reserve(curAllNodes_.size());
    for (auto node = curAllNodes_.begin(); node != curAllNodes_.end(); ++node) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (surface == nullptr) {
            continue;
        }
        curSurfaceIds.emplace_back(surface->GetId());
    }
    isNodeChange = lastSurfaceIds_ != curSurfaceIds;
    isDirty_ |= isNodeChange;
    std::swap(curSurfaceIds, curSurfaceIds_);
    return isNodeChange;
}

void RSPreComposeRegionManager::GetChangedNodeVisRegion()
{
    std::unordered_map<NodeId, Occlusion::Region> curSurfaceVisiableRegion;
    for (auto node = curAllNodes_.begin(); node != curAllNodes_.end(); ++node) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (surface == nullptr) {
            continue;
        }
        curSurfaceVisiableRegion[surface->GetId()] = surface->GetVisibleRegion();
    }
    std::swap(curSurfaceVisiableRegion, curSurfaceVisiableRegion_);
}

bool RSPreComposeRegionManager::CheckSurfaceVisiable(OcclusionRectISet& occlusionSurfaces,
    std::shared_ptr<RSSurfaceRenderNode> curSurface)
{
    size_t beforeSize = occlusionSurfaces.size();
    occlusionSurfaces.insert(curSurface->GetDstRect());
    ROSEN_LOGD("visable %d", occlusionSurfaces.size() > beforeSize);
    return occlusionSurfaces.size() > beforeSize ? true : false;
}

void RSPreComposeRegionManager::CalcOcclusion()
{
    if (!CheckNodeChange()) {
        ROSEN_LOGD("xxb node not change");
        return;
    }
    Occlusion::Region accumulatedRegion;
    VisibleData tempVisVec;
    VisibleData curVisVec;
    OcclusionRectISet occlusionSurfaces;
    std::map<uint32_t, bool> tempVisMap;
    std::map<uint32_t, bool> curVisMap;
    std::vector<NodeInfo> visNodes;
    for (auto node = curAllNodes_.rbegin(); node != curAllNodes_.rend(); ++node) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (curSurface == nullptr || curSurface->GetDstRect().IsEmpty() || curSurface->IsLeashWindow()) {
            continue;
        }
        Occlusion::Rect occlusionRect;
        // In UniRender, CalcOcclusion should consider the shadow area of window
        occlusionRect = Occlusion::Rect {curSurface->GetOldDirtyInSurface()};
        ROSEN_LOGD("name: %{public}s id: %{public}" PRIu64 " rect: %{public}s",
            curSurface->GetName().c_str(), curSurface->GetId(), occlusionRect.GetRectInfo().c_str());
        if (CheckSurfaceVisiable(occlusionSurfaces, curSurface)) {
            Occlusion::Region curRegion { occlusionRect };
            Occlusion::Region subResult = curRegion.Sub(accumulatedRegion);
            NodeInfo info;
            info.id = curSurface->GetId();
            info.type = static_cast<uint8_t>(curSurface->GetSurfaceNodeType());
            info.region = subResult;
            visNodes.push_back(info);
            curSurface->SetVisibleRegionRecursive(subResult, tempVisVec, tempVisMap);
        } else {
            curSurface->SetVisibleRegionRecursive({}, curVisVec, curVisMap);
        }
        accumulatedRegion.OrSelf(curSurface->GetOpaqueRegion());
    }
    std::swap(curVisVec, curVisVec_);
    std::swap(curVisMap, curVisMap_);
    std::swap(visNodes, visNodes_);
    std::swap(accumulatedRegion_, accumulatedRegion);
    GetChangedNodeVisRegion();
}

void RSPreComposeRegionManager::CalcDirtyRegionByNodeChange()
{
    std::vector<Occlusion::Region> regions;
    int32_t curPos = 0;
    int32_t lastPos = 0;
    while (curPos < curSurfaceIds_.size() || lastPos < lastSurfaceIds_.size()) {
        if (curPos == curSurfaceIds_.size()) {
            regions.emplace_back(lastSurfaceVisiableRegion_[lastSurfaceIds_[lastPos++]]);
        } else if (lastPos == lastSurfaceIds_.size()) {
            regions.emplace_back(curSurfaceVisiableRegion_[curSurfaceIds_[curPos++]]);
        } else if (curSurfaceIds_[curPos] != lastSurfaceIds_[lastPos]) {
            regions.emplace_back(lastSurfaceVisiableRegion_[lastSurfaceIds_[lastPos++]]);
        } else {
            lastPos++;
            curPos++;
        }
    }
    for (auto& region : regions) {
        ROSEN_LOGD("CalcGlobalDirtyRegion merge Surface closed %{public}s", region.GetRegionInfo().c_str());
        for (auto changedRect : region.GetRegionRects()) {
            if (!changedRect.IsEmpty()) {
                dirtyManager_->MergeDirtyRect(RectI(changedRect.left_, changedRect.top_,
                    changedRect.right_ - changedRect.left_, changedRect.bottom_ - changedRect.top_));
            }
        }
    }
    ROSEN_LOGD("global dirty region %{public}s", dirtyManager_->GetDirtyRegion().ToString().c_str());
}

void RSPreComposeRegionManager::MergeDirtyHistory()
{
    const int32_t bufferAge = 1;
    const bool useAligned = false;
    for (auto node = curAllNodes_.rbegin(); node != curAllNodes_.rend(); ++node) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (curSurface == nullptr || !curSurface->IsAppWindow()) {
            continue;
        }
        auto surfaceDirtyManager = curSurface->GetDirtyManager();
        if (!surfaceDirtyManager->SetBufferAge(bufferAge)) {
            ROSEN_LOGE("RSUniRenderUtil::MergeVisibleDirtyRegion with invalid buffer age %{public}d", bufferAge);
        }
        surfaceDirtyManager->IntersectDirtyRect(curSurface->GetOldDirtyInSurface());
        surfaceDirtyManager->UpdateDirty(useAligned);
    }
    // update display dirtymanager
    dirtyManager_->SetBufferAge(bufferAge);
    dirtyManager_->UpdateDirty(useAligned);
}

void RSPreComposeRegionManager::MergeVisibleDirtyRegion()
{
    Occlusion::Region allVisibleDirtyRegion;
    for (auto node = curAllNodes_.rbegin(); node != curAllNodes_.rend(); ++node) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (curSurface == nullptr || !curSurface->IsAppWindow() || curSurface->GetDstRect().IsEmpty()) {
            continue;
        }
        auto surfaceDirtyManager = curSurface->GetDirtyManager();
        auto surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        ROSEN_LOGD("name %{public}s, dirtyRect %{public}s", curSurface->GetName().c_str(),
            surfaceDirtyRect.ToString().c_str());
        for (auto& child : curSurface->GetChildHardwareEnabledNodes()) {
            if (auto childNode = child.lock()) {
                const auto& property = childNode->GetRenderProperties();
                auto geoPtr = property.GetBoundsGeometry();
                const RectI& rect = geoPtr->GetAbsRect();
                ROSEN_LOGD("hwc child %{public}s, rect %{public}s", childNode->GetName().c_str(),
                    rect.ToString().c_str());
                if (rect == surfaceDirtyRect) {
                    ROSEN_LOGD("parent dirty rect == hwc child rect");
                    return;
                }
            }
        }
        Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
            surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
        auto visibleRegion = curSurface->GetVisibleRegion();
        Occlusion::Region surfaceDirtyRegion { dirtyRect };
        Occlusion::Region surfaceVisibleDirtyRegion = surfaceDirtyRegion.And(visibleRegion);
        curSurface->SetVisibleDirtyRegion(surfaceVisibleDirtyRegion);
        allVisibleDirtyRegion = allVisibleDirtyRegion.Or(surfaceVisibleDirtyRegion);
    }
    std::swap(allVisibleDirtyRegion_, allVisibleDirtyRegion);
    isDirty_ |= allVisibleDirtyRegion_.GetSize() > 0;
}

void RSPreComposeRegionManager::SetSurfaceGlobalDirtyRegion()
{
    for (auto node = curAllNodes_.rbegin(); node != curAllNodes_.rend(); ++node) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (curSurface == nullptr || !curSurface->IsMainWindowType()) {
            continue;
        }
        curSurface->SetGlobalDirtyRegion(dirtyManager_->GetDirtyRegion());
        curSurface->SetDirtyRegionAlignedEnable(false);
    }
}

void RSPreComposeRegionManager::SetSurfaceBelowDirtyRegion()
{
    Occlusion::Region curVisibleDirtyRegion;
    for (auto node = curAllNodes_.begin(); node != curAllNodes_.end(); ++node) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (curSurface == nullptr || !curSurface->IsMainWindowType()) {
            continue;
        }
        curSurface->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto visibleDirtyRegion = curSurface->GetVisibleDirtyRegion();
        curVisibleDirtyRegion = curVisibleDirtyRegion.Or(visibleDirtyRegion);
    }
}

void RSPreComposeRegionManager::CalcVisiableDirtyRegion()
{
    MergeVisibleDirtyRegion();
    SetSurfaceGlobalDirtyRegion();
    SetSurfaceBelowDirtyRegion();
}

void RSPreComposeRegionManager::CalcDirtyRectsWithDirtyRegion()
{
    std::vector<RectI> dirtyRects;
    for (auto rect : allVisibleDirtyRegion_.GetRegionRects()) {
        // origin transformation
        dirtyRects.emplace_back(RectI(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_));
    }
    RS_LOGD("allVisibleDirtyRegion_ size %{public}d %{public}s",
        allVisibleDirtyRegion_.GetSize(), allVisibleDirtyRegion_.GetRegionInfo().c_str());

    auto rect = dirtyManager_->GetDirtyRegion();
    dirtyRects.emplace_back(rect);
    RS_LOGD("rect %{public}s", rect.ToString().c_str());
    std::swap(dirtyRects, dirtyRects_);
}

void RSPreComposeRegionManager::CalcClipRects()
{
#ifndef USE_ROSEN_DRAWING
        SkRegion region;
#else
        Drawing::Region region;
#endif
    for (auto& r : dirtyRects_) {
        RS_LOGD("clip rect %{public}s", r.ToString().c_str());
#ifndef USE_ROSEN_DRAWING
        region.op(SkIRect::MakeXYWH(r.left_, r.top_, r.width_, r.height_),
            SkRegion::kUnion_Op);
#else
        Drawing::Region tmpRegion;
        tmpRegion.SetRect(Drawing::RectI(r.left_, r.top_,
            r.left_ + r.width_, r.top_ + r.height_));
        region.Op(tmpRegion, Drawing::RegionOp::UNION);
#endif
    }
    std::swap(region_, region);
}

void RSPreComposeRegionManager::UpdateDirtyRegion(std::vector<RSBaseRenderNode::SharedPtr>& curAllNodes)
{
    dirtyManager_->Clear();
    curAllNodes_ = curAllNodes;
    CalcOcclusion();
    CalcDirtyRegionByNodeChange();
    MergeDirtyHistory();
    CalcVisiableDirtyRegion();
    CalcDirtyRectsWithDirtyRegion();
    CalcClipRects();
    std::swap(lastSurfaceIds_, curSurfaceIds_);
    std::swap(lastSurfaceVisiableRegion_, curSurfaceVisiableRegion_);
}

void RSPreComposeRegionManager::GetOcclusion(Occlusion::Region& accumulatedRegion,
    VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap,
    std::vector<NodeInfo> visNodes)
{
    std::swap(accumulatedRegion, accumulatedRegion_);
    std::swap(curVisVec, curVisVec_);
    std::swap(pidVisMap, curVisMap_);
    std::swap(visNodes, visNodes_);
}

bool RSPreComposeRegionManager::IsDirty()
{
    return isDirty_;
}

Occlusion::Region RSPreComposeRegionManager::GetVisibleDirtyRegion()
{
    auto surfaceDirtyRect = dirtyManager_->GetDirtyRegion();
    Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
        surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
    Occlusion::Region surfaceDirtyRegion { dirtyRect };
    return allVisibleDirtyRegion_.Or(surfaceDirtyRegion);
}
} // namespace Rosen
} // namespace OHOS