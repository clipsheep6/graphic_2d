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

#include "pipeline/rs_pre_compose_element.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "rs_main_thread.h"
#include "rs_uni_render_util.h"

namespace {
    const unsigned PRECOMPOSE_THREAD_INDEX = (1 << 8);
}

namespace OHOS {
namespace Rosen {
RSPreComposeElement::RSPreComposeElement(ScreenInfo& info, int32_t id,
    std::shared_ptr<RSPreComposeRegionManager> regionManager) :
    screenInfo_(info), id_(id), regionManager_(regionManager)
{
    ROSEN_LOGD("RSPreComposeElement() id %{public}d width %{public}d height %{public}d", id_, width_, height_);
    width_ = info.width;
    height_ = info.height;
}

RSPreComposeElement::~RSPreComposeElement()
{
    ROSEN_LOGD("~RSPreComposeElement()");
}

void RSPreComposeElement::SetNewNodeList(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList)
{
    surfaceNodeList_ = surfaceNodeList;
    std::unordered_set<NodeId> surfaceIds;
    std::vector<RSBaseRenderNode::SharedPtr> allSurfaceNodes;
    for (auto surfaceNode : surfaceNodeList_) {
        surfaceNode->CollectSurface(surfaceNode, allSurfaceNodes, true, false);
        surfaceIds.insert(surfaceNode->GetId());
    }
    std::swap(allSurfaceNodes_, allSurfaceNodes);
    std::swap(surfaceIds, surfaceIds_);

    std::unordered_set<NodeId> nodeIds;
    for (auto node : allSurfaceNodes_) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
        if (surface == nullptr) {
            continue;
        }
        ROSEN_LOGD("RSPreComposeElement %d xxb node id %{public}" PRIu64 ", name %{public}s isDirty %{public}d",
            id_, node->GetId(), surface->GetName().c_str(), node->IsDirty());
        nodeIds.insert(node->GetId());
    }
    std::swap(nodeIds_, nodeIds);
}

void RSPreComposeElement::StartCalculateAndDrawImage()
{
    ROSEN_LOGD("RSPreComposeElement %d", id_);
    needDraw_ = true;
    lastOcclusionId_ = 0;
    getHwcNodesDone_ = false;
    std::vector<std::pair<std::shared_ptr<RSSurfaceRenderNode>, Occlusion::Region>> gpuNodes;
    std::vector<std::pair<std::shared_ptr<RSSurfaceRenderNode>, RectI>> hwcNodes;
    std::swap(gpuNodes_, gpuNodes);
    std::swap(hwcNodes_, hwcNodes);
}

void RSPreComposeElement::SetParams(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
        std::shared_ptr<RSUniRenderVisitor> visitor, uint64_t focusNodeId, uint64_t leashFocusId)
{
    ROSEN_LOGD("RSPreComposeElement %d", id_);
    displayVisitor_ = visitor;
    SetNewNodeList(surfaceNodeList);
    focusNodeId_ = focusNodeId;
    leashFocusId_ = leashFocusId;
    state_ = ElementState::ELEMENT_STATE_DOING;
}

void RSPreComposeElement::ClipRect()
{
    if (canvas_ == nullptr) {
        ROSEN_LOGE("canvas is nullptr");
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkRegion& region = regionManager_->GetClipRegion();
    if (region.isEmpty()) {
        canvas_->clipRect(SkRect::MakeEmpty());
    } else if (region.isRect()) {
        canvas_->clipRegion(region);
    } else {
        SkPath dirtyPath;
        region.getBoundaryPath(&dirtyPath);
        canvas_->clipPath(dirtyPath, true);
    }
#else
    Drawing::Region& region = regionManager_->GetClipRegion();
#endif
}

void RSPreComposeElement::CalVisDirtyRegion()
{
    if (!isVisDirtyRegionUpdate_) {
        visDirtyRegion_ = dirtyRegion_.Sub(aboveRegion_);
        isVisDirtyRegionUpdate_ = true;
    }
}

void RSPreComposeElement::UpdateDirtyRegion()
{
    regionManager_->UpdateDirtyRegion(allSurfaceNodes_);
    isDirty_ = regionManager_->IsDirty();
    ClipRect();
    dirtyRegion_ = regionManager_->GetVisibleDirtyRegion();
    dirtyRect_ = regionManager_->GetNodeChangeDirtyRect();
    regionManager_->GetOcclusion(accumulatedRegion_, curVisVec_, curVisMap_, visNodes_);
    isVisDirtyRegionUpdate_ = false;
    ROSEN_LOGD("isDirty %{public}d dirtyRect %{public}s dirtyRegion %{public}s",
        isDirty_, dirtyRect_.ToString().c_str(),dirtyRegion_.GetRegionInfo().c_str());
}

ElementState RSPreComposeElement::GetState()
{
    return state_;
}

void RSPreComposeElement::UpdateImage()
{
    ROSEN_LOGD("RSPreComposeElement Update start id %d", id_);
    if (cacheSurface_ == nullptr) {
        ROSEN_LOGE("cacheSurface_ is nullptr");
        return;
    }
    visitor_ = std::make_shared<RSUniRenderVisitor>();
    visitor_->SetInfosForPreCompose(displayVisitor_, canvas_, PRECOMPOSE_THREAD_INDEX);
    canvas_->clear(SK_ColorTRANSPARENT);
    for (auto node : surfaceNodeList_) {
        if (node == nullptr) {
            ROSEN_LOGE("surfaceNode is nullptr");
            continue;
        }
        node->Process(visitor_);
    }
    ROSEN_LOGD("RSPreComposeElement flush end id %d", id_);
    cacheSurface_->flushAndSubmit(true);
    cacheTexture_ = cacheSurface_->getBackendTexture(SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess);
    UpdateHwcNodes();
    (void)RSBaseRenderUtil::WritePreComposeToPng(id_, cacheSurface_);
    state_ = ElementState::ELEMENT_STATE_DONE;
    ROSEN_LOGD("RSPreComposeElement Update end id %d", id_);
}

void RSPreComposeElement::UpdateAppWindowNodes(
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& appWindowNodes)
{
   appWindowNodes.insert(appWindowNodes.end(), appWindowNodes_.begin(), appWindowNodes_.end());
}

void RSPreComposeElement::UpdateNodes(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    std::vector<RSBaseRenderNode::SharedPtr> newAllSurfaces;
    for (auto node : curAllSurfaces) {
        if (nodeIds_.count(node->GetId()) == 0) {
            newAllSurfaces.emplace_back(node);
        }
    }
    std::swap(newAllSurfaces, curAllSurfaces);
}

void RSPreComposeElement::UpdateGlobalDirty(std::shared_ptr<RSDirtyRegionManager>& dirtyManager)
{
    dirtyManager->MergeDirtyRect(dirtyRect_);
}

#if !(defined USE_ROSEN_DRAWING) && (defined NEW_SKIA)
sk_sp<SkSurface> RSPreComposeElement::CreateSkSurface(GrDirectContext *grContext)
{
#if ((defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)) || (defined RS_ENABLE_VK)
    SkImageInfo info = SkImageInfo::MakeN32Premul(width_, height_);
    cacheSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kYes, info);
#else
    cacheSurface_ = SkSurface::MakeRasterN32Premul(width_, height_);
#endif
    return cacheSurface_;
}

void RSPreComposeElement::Init(GrDirectContext *grContext)
{
    if (grContext == nullptr) {
        ROSEN_LOGE("grContext is nullptr");
        return;
    }
    if (cacheSurface_ == nullptr) {
        cacheSurface_ = CreateSkSurface(grContext);
    }
    if (cacheSurface_ == nullptr) {
        ROSEN_LOGE("cacheSurface_ is nullptr");
        return;
    }
    if (canvas_ == nullptr) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(cacheSurface_.get());
    }
    if (canvas_ == nullptr) {
        ROSEN_LOGE("make canvas failed");
    }
}
#endif

void RSPreComposeElement::Deinit()
{
    cacheSurface_ = nullptr;
}

void RSPreComposeElement::Reset()
{
    state_ = ElementState::ELEMENT_STATE_IDLE;
    lastOcclusionId_ = 0;
}

void RSPreComposeElement::UpdateOcclusion(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    Occlusion::Region& accumulatedRegion, VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap,
    std::shared_ptr<RSPreComposeElement> next)
{
    if (lastOcclusionId_ == focusNodeId_) {
        curVisVec.insert(curVisVec.end(), curVisVec_.begin(), curVisVec_.end());
        pidVisMap.insert(curVisMap_.begin(), curVisMap_.end());
        for (auto& it : visNodes_) {
            RSSurfaceNodeType type = static_cast<RSSurfaceNodeType>(it.type);
            if (type == RSSurfaceNodeType::SELF_DRAWING_NODE || type == RSSurfaceNodeType::ABILITY_COMPONENT_NODE) {
                curVisVec.emplace_back(it.id);
                continue;
            }
            auto region = it.region;
            region.SubSelf(accumulatedRegion);
            bool vis = region.GetSize() > 0;
            if (vis) {
                curVisVec.emplace_back(it.id);
            }
            uint32_t tmpPid = ExtractPid(it.id);
            if (pidVisMap.find(tmpPid) != pidVisMap.end()) {
                pidVisMap[tmpPid] |= vis;
            } else {
                pidVisMap[tmpPid] = vis;
            }
        }
        visRegion_ = accumulatedRegion_.Sub(accumulatedRegion);
        aboveRegion_ = accumulatedRegion;
        accumulatedRegion.OrSelf(accumulatedRegion_);
        next->aboveRegion_ = aboveRegion_;
        next->visRegion_ = visRegion_;
        ROSEN_LOGD("RSPreComposeElement vis Region %{public}s", visRegion_.GetRegionInfo().c_str());
        isVisDirtyRegionUpdate_ = false;
    }
    lastOcclusionId_ = surfaceNode->GetId();
}

bool RSPreComposeElement::IsDirty()
{
    return isDirty_;
}

void RSPreComposeElement::UpdateHwcNodes()
{
    if (visitor_ == nullptr) {
        return;
    }
    std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair> hardwareNodes;
    appWindowNodes_ = visitor_->GetAppWindowNodes();
    for (auto node : appWindowNodes_) {
        for (auto& child : node->GetChildHardwareEnabledNodes()) {
            if (auto childNode = child.lock()) {
                hardwareNodes.push_back({ childNode, node });
                ROSEN_LOGD("RSPreComposeElement Name %s Parent %s",
                    childNode->GetName().c_str(), node->GetName().c_str());
            }
        }
    }
    std::swap(hardwareNodes, hardwareNodes_);
}

void RSPreComposeElement::GetHwcNodes(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair>& prevHwcEnabledNodes)
{
    if (getHwcNodesDone_) {
        return;
    }
    if (surfaceNode->GetId() == focusNodeId_ || surfaceNode->GetId() == leashFocusId_) {
        prevHwcEnabledNodes.insert(prevHwcEnabledNodes.end(), hardwareNodes_.begin(), hardwareNodes_.end());
        getHwcNodesDone_ = true;
        return;
    }
}

Occlusion::Region RSPreComposeElement::GetDirtyVisibleRegion()
{
    CalVisDirtyRegion();
    return visDirtyRegion_;
}

Occlusion::Region RSPreComposeElement::GetVisibleDirtyRegionWithGpuNodes()
{
    CalVisDirtyRegion();
    for (auto iter : hardwareNodes_) {
        auto parent = iter.second;
        auto child = iter.first;
        if (child->IsHardwareForcedDisabledByFilter() == true) {
            child->SetHardwareForcedDisabledState(true);
            RectI hwcRect = child->GetDstRect();
            Occlusion::Rect dirtyRect { hwcRect.left_, hwcRect.top_,
                hwcRect.GetRight(), hwcRect.GetBottom() };
            Occlusion::Region surfaceDirtyRegion { dirtyRect };
            auto visRegion = parent->GetVisibleRegion();
            surfaceDirtyRegion.AndSelf(visRegion);
            surfaceDirtyRegion.SubSelf(aboveRegion_);
            visDirtyRegion_.OrSelf(surfaceDirtyRegion);
            gpuNodes_.push_back({ child, surfaceDirtyRegion });
        } else {
            child->SetHardwareForcedDisabledState(false);
            hwcNodes_.push_back({ child, child->GetDstRect() });
        }
    }
    ROSEN_LOGD("RSPreComposeElement dirtyRegion after gpu node %{public}s", visDirtyRegion_.GetRegionInfo().c_str());
    return visDirtyRegion_;
}

void RSPreComposeElement::UpdateGpuNodesCanvasMatrix(std::shared_ptr<RSPaintFilterCanvas>& canvas,
    std::shared_ptr<RSSurfaceRenderNode> node, Occlusion::Region& surfaceDirtyRegion)
{
#ifndef USE_ROSEN_DRAWING
    SkRegion region;
#endif
    std::vector<RectI> dirtyRects;
    for (auto rect : surfaceDirtyRegion.GetRegionRects()) {
        // origin transformation
        dirtyRects.emplace_back(RectI(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_));
    }
    for (auto& r : dirtyRects) {
        ROSEN_LOGD("RSPreComposeElement clip rect %{public}s", r.ToString().c_str());
#ifndef USE_ROSEN_DRAWING
        region.op(SkIRect::MakeXYWH(r.left_, r.top_, r.width_, r.height_),
            SkRegion::kUnion_Op);
#endif
    }
#ifndef USE_ROSEN_DRAWING
    if (region.isEmpty()) {
        canvas->clipRect(SkRect::MakeEmpty());
    } else if (region.isRect()) {
        canvas->clipRegion(region);
    } else {
        ROSEN_LOGD("isPath");
        SkPath dirtyPath;
        region.getBoundaryPath(&dirtyPath);
        canvas->clipPath(dirtyPath, true);
    }
#endif
    const auto& property = node->GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    canvas->concat(geoPtr->GetAbsMatrix());
}

void RSPreComposeElement::UpdateHwcNodesCanvasRect(std::shared_ptr<RSPaintFilterCanvas>& canvas)
{
    if (hwcNodes_.empty()) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkRegion region;
#endif
    for (auto iter : hwcNodes_) {
        auto rect = iter.second;
        ROSEN_LOGD("rect %s", rect.ToString().c_str());
#ifndef USE_ROSEN_DRAWING
        region.op(SkIRect::MakeXYWH(rect.left_, rect.top_, rect.width_, rect.height_),
            SkRegion::kUnion_Op);
#endif
    }
    canvas->save();
#ifndef USE_ROSEN_DRAWING
    if (region.isEmpty()) {
        canvas->clipRect(SkRect::MakeEmpty());
    } else if (region.isRect()) {
        canvas->clipRegion(region);
    } else {
        ROSEN_LOGD("isPath");
        SkPath dirtyPath;
        region.getBoundaryPath(&dirtyPath);
        canvas->clipPath(dirtyPath, true);
    }
#endif
    canvas->clear(SK_ColorTRANSPARENT);
    canvas->restore();
}

void RSPreComposeElement::DrawGpuNodes(std::shared_ptr<RSPaintFilterCanvas>& canvas, uint32_t threadIndex)
{
    auto renderEngine = RSMainThread::Instance()->GetRenderEngine();
    for (auto iter : gpuNodes_) {
        canvas->save();
        UpdateGpuNodesCanvasMatrix(canvas, iter.first, iter.second);
        auto node = iter.first;
        node->SetGlobalAlpha(1.0f);
        auto params = RSUniRenderUtil::CreateBufferDrawParam(*node, false, threadIndex);
        renderEngine->DrawSurfaceNodeWithParams(*canvas, *node, params);
        canvas->restore();
    }
}

void RSPreComposeElement::DrawHardwareNodes(std::shared_ptr<RSPaintFilterCanvas>& canvas, uint32_t threadIndex)
{
    UpdateHwcNodesCanvasRect(canvas);
    DrawGpuNodes(canvas, threadIndex);
}

bool RSPreComposeElement::IsSkip()
{
    ROSEN_LOGD("RSPreComposeElement visRegion %s", visRegion_.GetRegionInfo().c_str());
    if (visRegion_.IsEmpty()) {
        ROSEN_LOGD("RSPreComposeElement visRegion is Empty");
        return true;
    }
    bool isSkipPreCompose = RSSystemProperties::GetDebugSkipPreComposeEnabled();
    if (isSkipPreCompose) {
        return true;
    }
    int skipId = RSSystemProperties::GetDebugSkipPreComposeId();
    if (id_ == skipId) {
        ROSEN_LOGD("RSPreComposeElement skip with id %d", skipId);
        return true;
    }
    return false;
}

bool RSPreComposeElement::ProcessNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas,
    uint32_t threadIndex)
{
    if (surfaceIds_.count(node.GetId()) != 0) {
        ROSEN_LOGD("RSPreComposeElement node skip %{public}" PRIu64 " ", node.GetId());
        if (needDraw_) {
            needDraw_ = false;
            if (IsSkip()) {
                ROSEN_LOGD("RSPreComposeElement skip %d", id_);
                return true;
            }
#if !(defined USE_ROSEN_DRAWING) && (defined RS_ENABLE_GL)
            if (!cacheTexture_.isValid()) {
                ROSEN_LOGE("invalid grBackendTexture_");
                return true;
            }
            DrawHardwareNodes(canvas, threadIndex);
            auto image = SkImage::MakeFromTexture(canvas->recordingContext(), cacheTexture_,
                kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
            if (image == nullptr) {
                return true;
            }
            auto samplingOptions = SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kNone);
            canvas->drawImage(image, 0.f, 0.f, samplingOptions);
#endif
        }
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS