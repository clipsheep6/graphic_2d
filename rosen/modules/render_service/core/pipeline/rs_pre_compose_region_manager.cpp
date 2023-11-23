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
    isNodeChange_ = false;
    std::vector<NodeId> curSurfaceIds;
    curSurfaceIds.reserve(curAllNodes_.size());
    std::unordered_map<NodeId, RectI> curSurfaceVisiableRegion;
    for (auto node = curAllNodes_.begin(); node != curAllNodes_.end(); ++node) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (surface == nullptr) {
            continue;
        }
        ROSEN_LOGD("name %{public}s id %{public}" PRIu64
            " olddirty %{public}s dstRegion %{public}s oldSurfacedirty %{public}s"
            "hasContainer %{public}d containerRegion %{public}s surfaceDirtyRect %{public}s"
            "transparentRegion %{public}s",
            surface->GetName().c_str(), surface->GetId(), surface->GetOldDirty().ToString().c_str(),
            surface->GetDstRect().ToString().c_str(), surface->GetOldDirtyInSurface().ToString().c_str(),
            surface->HasContainerWindow(), surface->GetContainerRegion().GetRegionInfo().c_str(),
            surface->GetDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str(),
            surface->GetTransparentRegion().GetRegionInfo().c_str());
        curSurfaceIds.emplace_back(surface->GetId());
        curSurfaceVisiableRegion[surface->GetId()] = surface->GetOldDirtyInSurface();
    }
    isNodeChange_ = lastSurfaceIds_ != curSurfaceIds;
    isDirty_ |= isNodeChange_;
    std::swap(curSurfaceIds, curSurfaceIds_);
    std::swap(curSurfaceVisiableRegion, curSurfaceVisiableRegion_);
    return isNodeChange_;
}

bool RSPreComposeRegionManager::CheckSurfaceVisiable(OcclusionRectISet& occlusionSurfaces,
    std::shared_ptr<RSSurfaceRenderNode> curSurface)
{
    size_t beforeSize = occlusionSurfaces.size();
    occlusionSurfaces.insert(curSurface->GetDstRect());
    ROSEN_LOGD("RSPreComposeRegionManager visable %d", occlusionSurfaces.size() > beforeSize);
    return occlusionSurfaces.size() > beforeSize ? true : false;
}

void RSPreComposeRegionManager::Clear()
{
    std::shared_ptr<RSDirtyRegionManager> dirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::vector<NodeId> lastSurfaceIds;
    VisibleData curVisVec;
    std::map<uint32_t, bool> curVisMap;
    Occlusion::Region accumulatedRegion;
    std::vector<NodeInfo> visNodes;
    std::unordered_map<NodeId, RectI> lastSurfaceVisiableRegion;
    std::swap(dirtyManager, dirtyManager_);
    std::swap(lastSurfaceIds, lastSurfaceIds_);
    std::swap(curVisVec, curVisVec_);
    std::swap(curVisMap, curVisMap_);
    std::swap(visNodes, visNodes_);
    std::swap(accumulatedRegion, accumulatedRegion_);
    std::swap(lastSurfaceVisiableRegion, lastSurfaceVisiableRegion_);
}

void RSPreComposeRegionManager::CalcOcclusion()
{
    if (!CheckNodeChange()) {
        ROSEN_LOGD("RSPreComposeRegionManager xxb node not change");
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
}

void RSPreComposeRegionManager::CalcGlobalDirtyRegionByAnimate()
{
    bool isAnimate = false;
    for (auto node = curAllNodes_.begin(); node != curAllNodes_.end(); ++node) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (surface == nullptr) {
            continue;
        }
        ROSEN_LOGD("Name %{public}s IsScale %{public}d",
            surface->GetName().c_str(), surface->IsScale());
        isAnimate = isAnimate || (surface->IsLeashWindow() && surface->IsScale());
    }
    if (isAnimate) {
        dirtyManager_->MergeDirtyRect(
            RectI{ 0, 0, screenInfo_.width, screenInfo_.height });
    }
}

void RSPreComposeRegionManager::CalcGlobalDirtyRegion()
{
    CalcGlobalDirtyRegionByAnimate();
    CalcGlobalDirtyRegionByNodeChange();
    CalcGlobalDirtyRegionByContainer();
}

void RSPreComposeRegionManager::CalcGlobalDirtyRegionByNodeChange()
{
    std::vector<RectI> rects;
    int32_t curPos = 0;
    int32_t lastPos = 0;
    while (curPos < curSurfaceIds_.size() || lastPos < lastSurfaceIds_.size()) {
        if (curPos == curSurfaceIds_.size()) {
            rects.emplace_back(lastSurfaceVisiableRegion_[lastSurfaceIds_[lastPos++]]);
        } else if (lastPos == lastSurfaceIds_.size()) {
            rects.emplace_back(curSurfaceVisiableRegion_[curSurfaceIds_[curPos++]]);
        } else if (curSurfaceIds_[curPos] != lastSurfaceIds_[lastPos]) {
            rects.emplace_back(lastSurfaceVisiableRegion_[lastSurfaceIds_[lastPos++]]);
        } else {
            lastPos++;
            curPos++;
        }
    }
    for (auto& rect : rects) {
        ROSEN_LOGD("CalcGlobalDirtyRegion merge Surface closed %{public}s", rect.ToString().c_str());
        if (!rect.IsEmpty()) {
            dirtyManager_->MergeDirtyRect(rect);
        }
    }
    ROSEN_LOGD("RSPreComposeRegionManager global dirty region %{public}s",
        dirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str());
}

void RSPreComposeRegionManager::CalcGlobalDirtyRegionByContainer()
{
    for (auto node = curAllNodes_.rbegin(); node != curAllNodes_.rend(); ++node) {
        auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*node);
        if (curSurface == nullptr) {
            continue;
        }
        auto surfaceDirtyManager = curSurface->GetDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        RectI surfaceDirtyRect = surfaceDirtyManager->GetCurrentFrameDirtyRegion();

        if (curSurface->HasContainerWindow()) {
            auto containerRegion = curSurface->GetContainerRegion();
            auto surfaceDirtyRegion = Occlusion::Rect{surfaceDirtyRect};
            ROSEN_LOGD("containerRegion is intersect %d", containerRegion.IsIntersectWith(surfaceDirtyRegion));
            if (containerRegion.IsIntersectWith(surfaceDirtyRegion)) {
                const auto& rect = containerRegion.GetBoundRef();
                dirtyManager_->MergeDirtyRect(
                    RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        } else {
            auto transparentRegion = curSurface->GetTransparentRegion();
            auto surfaceDirtyRegion = Occlusion::Rect{surfaceDirtyRect};
            ROSEN_LOGD("transparentRegion is intersect %d", transparentRegion.IsIntersectWith(surfaceDirtyRegion));
            if (transparentRegion.IsIntersectWith(surfaceDirtyRegion)) {
                const auto& rect = transparentRegion.GetBoundRef();
                dirtyManager_->MergeDirtyRect(
                    RectI{ rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_ });
            }
        }
    }
    ROSEN_LOGD("global rect %{public}s", dirtyManager_->GetCurrentFrameDirtyRegion().ToString().c_str());
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
            ROSEN_LOGE("RSPreComposeRegionManager with invalid buffer age %{public}d", bufferAge);
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
        ROSEN_LOGD("name %{public}s, dirtyRect %{public}s isDirty %d", curSurface->GetName().c_str(),
            surfaceDirtyRect.ToString().c_str(), surfaceDirtyManager->IsDirty());
        for (auto& child : curSurface->GetChildHardwareEnabledNodes()) {
            if (auto childNode = child.lock()) {
                const RectI& rect = childNode->GetDstRect();
                ROSEN_LOGD("hwc child %{public}s, rect %{public}s", childNode->GetName().c_str(),
                    rect.ToString().c_str());
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
        RS_LOGD("RSPreComposeRegionManager clip rect %{public}s", r.ToString().c_str());
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
    isDirty_ = false;
    curAllNodes_ = curAllNodes;
    CalcOcclusion();
    CalcGlobalDirtyRegion();
    MergeDirtyHistory();
    CalcVisiableDirtyRegion();
    CalcDirtyRectsWithDirtyRegion();
    CalcClipRects();
    std::swap(lastSurfaceIds_, curSurfaceIds_);
    std::swap(lastSurfaceVisiableRegion_, curSurfaceVisiableRegion_);
}

void RSPreComposeRegionManager::GetOcclusion(Occlusion::Region& accumulatedRegion,
    VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap,
    std::vector<NodeInfo>& visNodes)
{
    accumulatedRegion = accumulatedRegion_;
    curVisVec = curVisVec_;
    pidVisMap = curVisMap_;
    visNodes = visNodes_;
}

bool RSPreComposeRegionManager::IsDirty()
{
    return isDirty_;
}

RectI RSPreComposeRegionManager::GetNodeChangeDirtyRect()
{
    return dirtyManager_->GetDirtyRegion();
}

Occlusion::Region RSPreComposeRegionManager::GetVisibleDirtyRegion()
{
    return allVisibleDirtyRegion_;
}
} // namespace Rosen
} // namespace OHOS