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

namespace OHOS {
namespace Rosen {
RSPreComposeElement::RSPreComposeElement(ScreenInfo& info, RenderContext* renderContext,
    std::shared_ptr<RSPaintFilterCanvas> canvas, int32_t id)
    : screenInfo_(info), renderContext_(renderContext), mainCanvas_(canvas), id_(id)
{
    ROSEN_LOGD("RSPreComposeElement()");
    regionManager_ = std::make_shared<RSPreComposeRegionManager>();
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
    swap(allSurfaceNodes_, allSurfaceNodes);

    std::unordered_set<NodeId> nodeIds;
    for (auto node : allSurfaceNodes) {
        auto surface = RSBaseRenderNode:ReinterpretCast<RSSurfaceRenderNode>(node);
        if (surface == nullptr) {
            continue;
        }
        ROSEN_LOGD("xxb node id %{public}" PRIu64 ", name %{public}s", node->GetId(), surface->GetName().c_str())
        nodeIds.insert(node->GetId());
    }
    swap(nodeIds_, nodeIds);
}

void RSPreComposeElement::SetParams(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
        std::shared_ptr<RSUniRenderVisitor> visitor);
{
    displayVisitor_ = visitor;
    SetNewNodeList(surfaceNodeList);
}

void RSPreComposeElement::UpdateDirtyRegion()
{
    regionManager_->UpdateDirtyRegion(allSurfaceNodes_);
}

void RSPreComposeElement::UpdateImage()
{
    bool isFirstFrame = isFirstFrame_;
    if (cacheSurface_ == nullptr) {
        ROSEN_LOGE("cacheSurface_ is nullptr");
        return;
    }
    visitor_ = make_shared<RSUniRenderVisitor>();
    visitor_->SetInfosForPreCompose(displayVisitor_, canvas_);
    for (auto node : surfaceNodeList_) {
        if (node == nullptr) {
            ROSEN_LOGE("surfaceNode is nullptr");
            continue;
        }
        node->Process(visitor_);
    }
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
    swap(newAllSurfaces, curAllSurfaces);
}

void RSPreComposeElement::CreateShareEglContext()
{
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
    if (renderContext_ == nullptr) {
        ROSEN_LOGE("renderContext_ is nullptr");
        return;
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
    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeGL(move(glInterface), options);
#endif
    if (grContext == nullptr) {
        ROSEN_LOGD("grContext is nullptr");
        return nullptr;
    }
    return grContext;
}
#endif

void RSPreComposeElement::CreateSkSurface()
{
#if ((defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)) || (defined RS_ENABLE_VK)
    SkImageInfo info = SkImageInfo::MakeN32Premul(width_, height_);
    cacheSurface_ = SkSurface::MakeRenderTarget(grContext_.get(), SkBudgeted::kYes, info);
#else
    cacheSurface_ = SkSurface::MakeRasterN32Premul(width_, height_);
#endif
    return cacheSurface_;
}

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

bool RSPreComposeElement::Reset()
{
    isDone_ = false;
}

void RSPreComposeElement::UpdateOcclusion(Occlusion::Region& accumulatedRegion,
    VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap)
{
    // TODO
}

bool RSPreComposeElement::IsDirty()
{
    // TODO
}

void RSPreComposeElement::GetHwcNodes(std::vector<SurfaceDirtyMgrPair>& prevHwcEnabledNodes)
{
    // TODO
}

Occlusion::Region RSPreComposeElement::GetDirtyRegion()
{
    // TODO
}

Occlusion::Region RSPreComposeElement::GetVisibleDirtyRegion()
{
    // TODO
}

void RSPreComposeElement::SetGlobalDirtyRegion(Occlusion::Region& globalRegion)
{
    // TODO
}

bool RSPreComposeElement::ProcessNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas)
{
    if (nodeIds_.count(node->GetId()) != 0) {
        if (needDraw_) {
            needDraw_ = false;
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
            if (!cacheTexture_.isValid()) {
                ROSEN_LOGE("invalid grBackendTexture_");
                return;
            }
            auto image = SkImage::MakeFromTexture(canvas->recordingContext(), cacheTexture_,
                kBottomLeft_GrSurfaceOrigin, kRGRA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
            if (image == nullptr) {
                return;
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