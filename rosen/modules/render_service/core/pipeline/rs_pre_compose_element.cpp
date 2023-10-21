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
    std::vector<RSBaseRenderNode::SharedPtr> allSurfaceNodes;
    for (auto surfaceNode : surfaceNodeList_) {
        surfaceNode->CollectSurface(surfaceNode, allSurfaceNodes, true, false);
    }
    std::swap(allSurfaceNodes_, allSurfaceNodes);

    std::unordered_set<NodeId> nodeIds;
    for (auto node : allSurfaceNodes) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
        if (surface == nullptr) {
            continue;
        }
        ROSEN_LOGD("xxb node id %{public}" PRIu64 ", name %{public}s",
            node->GetId(), surface->GetName().c_str());
        nodeIds.insert(node->GetId());
    }
    std::swap(nodeIds_, nodeIds);
}

void RSPreComposeElement::StartCalculateAndDrawImage()
{
    needDraw_ = true;
    lastOcclusionId_ = 0;
    getHwcNodesDone_ = false;
}

void RSPreComposeElement::SetParams(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
        std::shared_ptr<RSUniRenderVisitor> visitor, uint64_t focusNodeId, uint64_t leashFocusId)
{
    displayVisitor_ = visitor;
    SetNewNodeList(surfaceNodeList);
    focusNodeId_ = focusNodeId;
    leashFocusId_ = leashFocusId;
}

void RSPreComposeElement::ClipRect()
{
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

void RSPreComposeElement::UpdateDirtyRegion()
{
    regionManager_->UpdateDirtyRegion(allSurfaceNodes_);
    isDirty_ = regionManager_->IsDirty();
    ClipRect();
    dirtyRegion_ = regionManager_->GetVisibleDirtyRegion();
    regionManager_->GetOcclusion(accumulatedRegion_, curVisVec_, curVisMap_, visNodes_);
}

void RSPreComposeElement::UpdateImage()
{
    if (cacheSurface_ == nullptr) {
        ROSEN_LOGE("cacheSurface_ is nullptr");
        return;
    }
    visitor_ = std::make_shared<RSUniRenderVisitor>();
    visitor_->SetInfosForPreCompose(displayVisitor_, canvas_);
    for (auto node : surfaceNodeList_) {
        if (node == nullptr) {
            ROSEN_LOGE("surfaceNode is nullptr");
            continue;
        }
        node->Process(visitor_);
    }
    cacheSurface_->flushAndSubmit(true);
    cacheTexture_ = cacheSurface_->getBackendTexture(SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess);
    UpdateHwcNodes();
    (void)RSBaseRenderUtil::WritePreComposeToPng(cacheSurface_);
    isDone_ = true;
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

void RSPreComposeElement::CreateShareEglContext()
{
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
    if (renderContext_ == nullptr) {
        auto renderEngine = RSMainThread::Instance()->GetRenderEngine();
        renderContext_ = renderEngine->GetRenderContext().get();
    }
    eglShareContext_ = renderContext_->CreateShareContext();
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        ROSEN_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
        ROSEN_LOGE("eglMakeCurrent failed");
    }
#endif
#endif
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSPreComposeElement::CreateShareGrContext()
#endif
{
    ROSEN_LOGD("CreateShareGrContext start");
    CreateShareEglContext();
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        ROSEN_LOGD("CreateShareGrContext failed");
        return nullptr;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeGL(std::move(glInterface), options);
#endif
    if (grContext == nullptr) {
        ROSEN_LOGD("grContext is nullptr");
        return nullptr;
    }
    return grContext;
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkSurface> RSPreComposeElement::CreateSkSurface()
{
#if ((defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)) || (defined RS_ENABLE_VK)
    SkImageInfo info = SkImageInfo::MakeN32Premul(width_, height_);
    cacheSurface_ = SkSurface::MakeRenderTarget(grContext_.get(), SkBudgeted::kYes, info);
#else
    cacheSurface_ = SkSurface::MakeRasterN32Premul(width_, height_);
#endif
    return cacheSurface_;
}
#endif

void RSPreComposeElement::Init()
{
#ifndef USE_ROSEN_DRAWING
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
        if (grContext_ == nullptr) {
            ROSEN_LOGE("grContext is nullptr");
            return;
        }
    }
    if (cacheSurface_ == nullptr) {
        cacheSurface_ = CreateSkSurface();
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
#endif
}

void RSPreComposeElement::Deinit()
{
    cacheSurface_ = nullptr;
    grContext_ = nullptr;
}

void RSPreComposeElement::Reset()
{
    isDone_ = false;
    lastOcclusionId_ = 0;
}

void RSPreComposeElement::UpdateOcclusion(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    Occlusion::Region& accumulatedRegion, VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap)
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
        visDirtyRegion_ = dirtyRegion_.Sub(accumulatedRegion);
        visRegion_ = accumulatedRegion_.Sub(accumulatedRegion);
        aboveRegion_ = accumulatedRegion;
        accumulatedRegion.OrSelf(accumulatedRegion_);
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
    return visDirtyRegion_;
}

Occlusion::Region RSPreComposeElement::GetVisibleDirtyRegionWithGpuNodes()
{
    for (auto iter : hardwareNodes_) {
        if (iter.first->IsHardwareForcedDisabled() == true || iter.first->IsHardwareForcedDisabledByFilter() == true) {
            auto parent = iter.second;
            std::shared_ptr<RSSurfaceRenderNode> node = iter.first;
            node->SetHardwareForcedDisabledState(true);
            const auto& property = node->GetRenderProperties();
            auto geoPtr = property.GetBoundsGeometry();
            RectI hwcRect = geoPtr->GetAbsRect();
            Occlusion::Rect dirtyRect { hwcRect.left_, hwcRect.top_,
                hwcRect.GetRight(), hwcRect.GetBottom() };
            Occlusion::Region surfaceDirtyRegion { dirtyRect };
            auto visRegion = parent->GetVisibleRegion();
            surfaceDirtyRegion.AndSelf(visRegion);
            surfaceDirtyRegion.SubSelf(aboveRegion_);
            visDirtyRegion_.Or(surfaceDirtyRegion);
            gpuNodes_.push_back({ node, surfaceDirtyRegion });
        }
    }
    std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair> temp;
    std::swap(temp, hardwareNodes_);
    return visDirtyRegion_;
}

void RSPreComposeElement::UpdateCanvasMatrix(std::shared_ptr<RSPaintFilterCanvas>& canvas,
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
        RS_LOGD("clip rect %{public}s", r.ToString().c_str());
#ifndef USE_ROSEN_DRAWING
        region.op(SkIRect::MakeXYWH(r.left_, r.top_, r.width_, r.height_),
            SkRegion::kUnion_Op);
#endif
    }
#ifndef USE_ROSEN_DRAWING
    if (region.isEmpty()) {
        canvas_->clipRect(SkRect::MakeEmpty());
    } else if (region.isRect()) {
        canvas_->clipRegion(region);
    } else {
        SkPath dirtyPath;
        region.getBoundaryPath(&dirtyPath);
        canvas_->clipPath(dirtyPath, true);
    }
#endif
    const auto& property = node->GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    canvas->concat(geoPtr->GetAbsMatrix());
}

void RSPreComposeElement::DrawGpuNodes(std::shared_ptr<RSPaintFilterCanvas>& canvas, uint32_t threadIndex)
{
    auto renderEngine = RSMainThread::Instance()->GetRenderEngine();
    for (auto iter : gpuNodes_) {
        canvas->save();
        UpdateCanvasMatrix(canvas, iter.first, iter.second);
        auto node = iter.first;
        node->SetGlobalAlpha(1.0f);
        auto params = RSUniRenderUtil::CreateBufferDrawParam(*node, false, threadIndex);
        renderEngine->DrawSurfaceNodeWithParams(*canvas, *node, params);
        canvas->restore();
    }
}

bool RSPreComposeElement::ProcessNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas,
    uint32_t threadIndex)
{
    if (nodeIds_.count(node.GetId()) != 0) {
        if (needDraw_) {
            needDraw_ = false;
            if (visRegion_.IsEmpty()) {
                return true;
            }
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
            if (!cacheTexture_.isValid()) {
                ROSEN_LOGE("invalid grBackendTexture_");
                return true;
            }
            DrawGpuNodes(canvas, threadIndex);
            auto image = SkImage::MakeFromTexture(canvas->recordingContext(), cacheTexture_,
                kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
            if (image == nullptr) {
                return true;
            }
            auto samplingOptions = SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kNone);
            canvas->drawImage(image, 0.f, 0.f, samplingOptions);
#endif
#endif
        }
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS