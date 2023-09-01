/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_surface_capture_task.h"

#include <memory>
#include <sys/mman.h>

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#else
#include "draw/surface.h"
#include "draw/color.h"
#endif
#include "rs_trace.h"

#include "common/rs_background_thread.h"
#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_cold_start_thread.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "render/rs_skia_filter.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"

namespace OHOS {
namespace Rosen {
bool RSSurfaceCaptureTask::Run(sptr<RSISurfaceCaptureCallback> callback)
{
    if (ROSEN_EQ(scaleX_, 0.f) || ROSEN_EQ(scaleY_, 0.f) || scaleX_ < 0.f || scaleY_ < 0.f) {
        RS_LOGE("RSSurfaceCaptureTask::Run: SurfaceCapture scale is invalid.");
        return false;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: node is nullptr");
        return false;
    }
    std::unique_ptr<Media::PixelMap> pixelmap;
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(scaleX_, scaleY_, RSUniRenderJudgement::IsUniRender());
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        RS_LOGD("RSSurfaceCaptureTask::Run: Into SURFACE_NODE SurfaceRenderNodeId:[%{public}" PRIu64 "]",
            node->GetId());
        pixelmap = CreatePixelMapBySurfaceNode(surfaceNode, visitor_->IsUniRender());
        visitor_->IsDisplayNode(false);
    } else if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        RS_LOGD("RSSurfaceCaptureTask::Run: Into DISPLAY_NODE DisplayRenderNodeId:[%{public}" PRIu64 "]",
            node->GetId());
        visitor_->SetHasingSecurityLayer(FindSecurityLayer());
        pixelmap = CreatePixelMapByDisplayNode(displayNode, visitor_->IsUniRender(),
            visitor_->GetHasingSecurityLayer());
        visitor_->IsDisplayNode(true);
    } else {
        RS_LOGE("RSSurfaceCaptureTask::Run: Invalid RSRenderNodeType!");
        return false;
    }
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: pixelmap == nullptr!");
        return false;
    }
#ifndef USE_ROSEN_DRAWING
#if defined(RS_ENABLE_GL)
#if defined(NEW_RENDER_CONTEXT)
    auto drawingContext = RSMainThread::Instance()->GetRenderEngine()->GetDrawingContext();
#ifdef NEW_SKIA
    GrDirectContext* grContext = drawingContext != nullptr ? drawingContext->GetDrawingContext() : nullptr;
#else
    GrContext* grContext = drawingContext != nullptr ? drawingContext->GetDrawingContext() : nullptr;
#endif
#else
    auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
#ifdef NEW_SKIA
    GrDirectContext* grContext = renderContext != nullptr ? renderContext->GetGrContext() : nullptr;
#else
    GrContext* grContext = renderContext != nullptr ? renderContext->GetGrContext() : nullptr;
#endif
#endif
    RSTagTracker tagTracker(grContext, node->GetId(), RSTagTracker::TAGTYPE::TAG_CAPTURE);
#endif
#endif
#ifndef USE_ROSEN_DRAWING
    auto skSurface = CreateSurface(pixelmap);
    if (skSurface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: surface is nullptr!");
        return false;
    }
    visitor_->SetSurface(skSurface.get());
#else
    auto surface = CreateSurface(pixelmap);
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: surface is nullptr!");
        return false;
    }
    visitor_->SetSurface(surface.get());
#endif
    node->Process(visitor_);
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_DRIVEN_RENDER
    if (RSSystemProperties::GetSnapshotWithDMAEnabled()) {
        skSurface->flushAndSubmit(true);
        GrBackendTexture grBackendTexture
            = skSurface->getBackendTexture(SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess);
        if (!grBackendTexture.isValid()) {
            RS_LOGE("SkiaSurface bind Image failed: BackendTexture is invalid");
            return false;
        }
        auto wrapper = std::make_shared<std::tuple<std::unique_ptr<Media::PixelMap>>>();
        std::get<0>(*wrapper) = std::move(pixelmap);
        auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>();
        auto rotation = (displayNode != nullptr) ? displayNode->GetRotation()
            : ScreenRotation::INVALID_SCREEN_ROTATION;
        bool ableRotation = ((displayNode != nullptr) && visitor_->IsUniRender());
        auto id = nodeId_;

        std::function<void()> copytask = [wrapper, grBackendTexture = std::move(grBackendTexture), callback,
                                             ableRotation, rotation, id]() -> void {
            RS_TRACE_NAME("copy and send capture");
            if (!grBackendTexture.isValid()) {
                RS_LOGE("COPYTASK: SkiaSurface bind Image failed: BackendTexture is invalid");
                callback->OnSurfaceCapture(id, nullptr);
                return;
            }
            auto pixelmap = std::move(std::get<0>(*wrapper));
            if (pixelmap == nullptr) {
                RS_LOGE("COPYTASK: pixelmap == nullptr");
                callback->OnSurfaceCapture(id, nullptr);
                return;
            }

            DmaMem dmaMem;
            SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
                kRGBA_8888_SkColorType, kPremul_SkAlphaType);
            sptr<SurfaceBuffer> surfaceBuffer = dmaMem.DmaMemAlloc(info, pixelmap);
            auto skSurface = dmaMem.GetSkSurfaceFromSurfaceBuffer(surfaceBuffer);
            if (skSurface == nullptr) {
                RS_LOGE("GetSkSurfaceFromSurfaceBuffer fail,surface is nullptr!");
                dmaMem.ReleaseGLMemory();
                callback->OnSurfaceCapture(id, nullptr);
                return;
            }
            auto canvas = std::make_shared<RSPaintFilterCanvas>(skSurface.get());

            auto tmpImg = SkImage::MakeFromTexture(canvas->recordingContext(), grBackendTexture,
                kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
            canvas->drawImage(tmpImg, 0, 0);
            skSurface->flushAndSubmit(true);

            if (ableRotation) {
                if (rotation == ScreenRotation::ROTATION_90) {
                    pixelmap->rotate(static_cast<int32_t>(90)); // 90 degrees
                }
                if (rotation == ScreenRotation::ROTATION_270) {
                    pixelmap->rotate(static_cast<int32_t>(270)); // 270 degrees
                }
                RS_LOGD("COPYTASK: PixelmapRotation: %d", static_cast<int32_t>(rotation));
            }
            callback->OnSurfaceCapture(id, pixelmap.get());
            dmaMem.ReleaseGLMemory();
            RSBackgroundThread::Instance().CleanGrResource();
        };
        RSBackgroundThread::Instance().PostTask(copytask);
        return true;
    } else {
        sk_sp<SkImage> img(skSurface.get()->makeImageSnapshot());
        if (!img) {
            RS_LOGE("RSSurfaceCaptureTask::Run: img is nullptr");
            return false;
        }
        if (!CopyDataToPixelMap(img, pixelmap)) {
            RS_LOGE("RSSurfaceCaptureTask::Run: CopyDataToPixelMap failed");
            return false;
        }
    }
#else
    sk_sp<SkImage> img(skSurface.get()->makeImageSnapshot());
    if (!img) {
        RS_LOGE("RSSurfaceCaptureTask::Run: img is nullptr");
        return false;
    }
    if (!CopyDataToPixelMap(img, pixelmap)) {
        RS_LOGE("RSSurfaceCaptureTask::Run: CopyDataToPixelMap failed");
        return false;
    }
#endif // RS_ENABLE_DRIVEN_RENDER
#else
    std::shared_ptr<Drawing::Image> img(surface.get()->GetImageSnapshot());
    if (!img) {
        RS_LOGE("RSSurfaceCaptureTask::Run: img is nullptr");
        return false;
    }

    auto size = pixelmap->GetRowBytes() * pixelmap->GetHeight();
    if (size < 0) {
        RS_LOGE("RSSurfaceCaptureTask::Run: pxielmap size is invalid");
        return false;
    }

    auto data = (uint8_t *)malloc(size);
    if (data == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: data is nullptr");
        return false;
    }
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    bitmap.SetPixels(data);
    if (!img->ReadPixels(bitmap, 0, 0)) {
        RS_LOGE("RSSurfaceCaptureTask::Run: readPixels failed");
        free(data);
        data = nullptr;
        return false;
    }
    pixelmap->SetPixelsAddr(data, nullptr, pixelmap->GetRowBytes() * pixelmap->GetHeight(),
        Media::AllocatorType::HEAP_ALLOC, nullptr);
#endif // USE_ROSEN_DRAWING
#endif
    if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        if (visitor_->IsUniRender()) {
            auto rotation = displayNode->GetRotation();
            if (rotation == ScreenRotation::ROTATION_90) {
                pixelmap->rotate(static_cast<int32_t>(90)); // 90 degrees
            }
            if (rotation == ScreenRotation::ROTATION_270) {
                pixelmap->rotate(static_cast<int32_t>(270)); // 270 degrees
            }
            RS_LOGD("RSSurfaceCaptureTask::Run: PixelmapRotation: %{public}d", static_cast<int32_t>(rotation));
        }
    }
    callback->OnSurfaceCapture(nodeId_, pixelmap.get());
    return true;
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode(
    std::shared_ptr<RSSurfaceRenderNode> node, bool isUniRender)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: node == nullptr");
        return nullptr;
    }
    if (!isUniRender && node->GetBuffer() == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: node GetBuffer == nullptr");
        return nullptr;
    }
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGD("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: origin pixelmap width is [%{public}u],"
        " height is [%{public}u], created pixelmap width is [%{public}u], height is [%{public}u], the scale"
        " is scaleY:[%{public}f], scaleY:[%{public}f]",
        pixmapWidth, pixmapHeight, opts.size.width, opts.size.height, scaleX_, scaleY_);
    return Media::PixelMap::Create(opts);
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapByDisplayNode(
    std::shared_ptr<RSDisplayRenderNode> node, bool isUniRender, bool hasSecurityLayer)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: node is nullptr");
        return nullptr;
    }
    uint64_t screenId = node->GetScreenId();
    RSScreenModeInfo screenModeInfo;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: screenManager is nullptr!");
        return nullptr;
    }
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    uint32_t pixmapWidth = screenInfo.width;
    uint32_t pixmapHeight = screenInfo.height;
    if (!isUniRender) {
        auto rotation = node->GetRotation();
        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(pixmapWidth, pixmapHeight);
        }
    }
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGD("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: origin pixelmap width is [%{public}u],"
        " height is [%{public}u], created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleY:[%{public}f], scaleY:[%{public}f]",
        pixmapWidth, pixmapHeight, opts.size.width, opts.size.height, scaleX_, scaleY_);
    return Media::PixelMap::Create(opts);
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkSurface> RSSurfaceCaptureTask::CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: address == nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
#if defined(NEW_RENDER_CONTEXT)
    auto drawingContext = RSMainThread::Instance()->GetRenderEngine()->GetDrawingContext();
    if (drawingContext == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    drawingContext->SetUpDrawingContext();
    return SkSurface::MakeRenderTarget(drawingContext->GetDrawingContext(), SkBudgeted::kNo, info);
#else
    auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
    if (renderContext == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    renderContext->SetUpGrContext();
    return SkSurface::MakeRenderTarget(renderContext->GetGrContext(), SkBudgeted::kNo, info);
#endif
#endif
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
void DmaMem::ReleaseGLMemory()
{
    if (texId_ != 0U) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &texId_);
        texId_ = 0U;
    }
    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
    if (eglImage_ != EGL_NO_IMAGE_KHR) {
        auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglDestroyImageKHR(disp, eglImage_);
        eglImage_ = EGL_NO_IMAGE_KHR;
    }
}
#endif
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
sptr<SurfaceBuffer> DmaMem::DmaMemAlloc(SkImageInfo &dstInfo, const std::unique_ptr<Media::PixelMap>& pixelmap)
{
#if defined(_WIN32) || defined(_APPLE) || defined(A_PLATFORM) || defined(IOS_PLATFORM)
    RS_LOGE("Unsupport dma mem alloc");
    return nullptr;
#else
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = dstInfo.width(),
        .height = dstInfo.height(),
        .strideAlignment = 0x8, // set 0x8 as default value to alloc SurfaceBufferImpl
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888, // PixelFormat
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = surfaceBuffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        RS_LOGE("SurfaceBuffer Alloc failed, %{public}s", GSErrorStr(ret).c_str());
        return nullptr;
    }
    void* nativeBuffer = surfaceBuffer.GetRefPtr();
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(nativeBuffer);
    ref->IncStrongRef(ref);
    int32_t bufferSize = pixelmap->GetByteCount();
    pixelmap->SetPixelsAddr(surfaceBuffer->GetVirAddr(), nativeBuffer, bufferSize, Media::AllocatorType::DMA_ALLOC, nullptr);
    return surfaceBuffer;
#endif
}
#endif
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
sk_sp<SkSurface> DmaMem::GetSkSurfaceFromSurfaceBuffer(sptr<SurfaceBuffer> surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        RS_LOGE("GetSkImageFromSurfaceBuffer surfaceBuffer is nullptr");
        return nullptr;
    }
    if (nativeWindowBuffer_ == nullptr) {
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer);
        if (!nativeWindowBuffer_) {
            RS_LOGE("GetSkImageFromSurfaceBuffer create native window buffer fail");
            return nullptr;
        }
    }
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED,
        EGL_TRUE,
        EGL_NONE,
    };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
        if (eglImage_ == EGL_NO_IMAGE_KHR) {
            RS_LOGE("%s create egl image fail %d", __func__, eglGetError());
            return nullptr;
        }
    }

    // Create texture object
    if (texId_ == 0U) {
        glGenTextures(1, &texId_);
        glBindTexture(GL_TEXTURE_2D, texId_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, static_cast<GLeglImageOES>(eglImage_));
    }

    GrGLTextureInfo textureInfo = { GL_TEXTURE_2D, texId_, GL_RGBA8_OES };

    GrBackendTexture backendTexture(
        surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), GrMipMapped::kNo, textureInfo);
#ifdef RS_ENABLE_DRIVEN_RENDER
    auto grContext = RSBackgroundThread::Instance().GetShareGrContext().get();
#else
    auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
    if (renderContext == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    auto grContext = renderContext->GetGrContext();
#endif
    auto skSurface = SkSurface::MakeFromBackendTexture(grContext, backendTexture,
        kTopLeft_GrSurfaceOrigin, 0, kRGBA_8888_SkColorType, SkColorSpace::MakeSRGB(), nullptr);
    return skSurface;
}
#endif
#endif

bool CopyDataToPixelMap(sk_sp<SkImage> img, const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    auto size = pixelmap->GetRowBytes() * pixelmap->GetHeight();
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
#ifdef ROSEN_OHOS
    int fd = AshmemCreate("RSSurfaceCapture Data", size);
    if (fd < 0) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap AshmemCreate fd < 0");
        return false;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap AshmemSetProt error");
        ::close(fd);
        return false;
    }
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    auto data = static_cast<uint8_t*>(ptr);
    if (ptr == MAP_FAILED || ptr == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap data is nullptr");
        ::close(fd);
        return false;
    }

    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap readPixels failed");
        ::close(fd);
        return false;
    }
    void* fdPtr = new int32_t();
    *static_cast<int32_t*>(fdPtr) = fd;
    pixelmap->SetPixelsAddr(data, fdPtr, size, Media::AllocatorType::SHARE_MEM_ALLOC, nullptr);
#else
    auto data = (uint8_t *)malloc(size);
    if (data == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap data is nullptr");
        return false;
    }
    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSSurfaceCaptureTask::CopyDataToPixelMap readPixels failed");
        free(data);
        data = nullptr;
        return false;
    }
    pixelmap->SetPixelsAddr(data, nullptr, size, Media::AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return true;
}
#else
std::shared_ptr<Drawing::Surface> RSSurfaceCaptureTask::CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: address == nullptr");
        return nullptr;
    }
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto bitmap = std::make_shared<Drawing::Bitmap>();
    bitmap->Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    bitmap->SetPixels(address);
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    auto renderEngine = RSMainThread::Instance()->GetRenderEngine();
    if (renderEngine == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderEngine is nullptr");
        return nullptr;
    }
    auto renderContext = renderEngine->GetRenderContext();
    if (renderContext == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    renderContext->SetUpGpuContext();
    auto gpuContext = renderContext->GetDrGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: Drawing::GPUContext is nullptr");
        return nullptr;
    }
    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromBitmap(*gpuContext, *bitmap)) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: Drawing::Image is nullptr");
        return nullptr;
    }
    auto surface = std::make_shared<Drawing::Surface>();
    if (!surface->Bind(*image)) {
        if (!surface->Bind(*bitmap)) {
            RS_LOGE("RSSurfaceCaptureTask::CreateSurface: Drawing::Surface is nullptr");
            return nullptr;
        }
    }
    return surface;
#else
    auto surface = std::make_shared<Drawing::Surface>();
    if (!surface->Bind(*bitmap)) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: Drawing::Surface is nullptr");
        return nullptr;
    }
    return surface;
#endif
}
#endif

bool RSSurfaceCaptureTask::FindSecurityLayer()
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    bool hasSecurityLayer = false;
    nodeMap.TraverseSurfaceNodes([this, &hasSecurityLayer](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
        mutable {
        if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
            return;
        }
        if (surfaceNode->GetSecurityLayer()) {
            hasSecurityLayer = true;
            return;
        }
    });
    return hasSecurityLayer;
}

RSSurfaceCaptureVisitor::RSSurfaceCaptureVisitor(float scaleX, float scaleY, bool isUniRender)
    : scaleX_(scaleX), scaleY_(scaleY), isUniRender_(isUniRender)
{
    renderEngine_ = RSMainThread::Instance()->GetRenderEngine();
    isUIFirst_ = RSMainThread::Instance()->IsUIFirstOn();
}

#ifndef USE_ROSEN_DRAWING
void RSSurfaceCaptureVisitor::SetSurface(SkSurface* surface)
{
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::SetSurface: surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->scale(scaleX_, scaleY_);
}
#else
void RSSurfaceCaptureVisitor::SetSurface(Drawing::Surface* surface)
{
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::SetSurface: surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->Scale(scaleX_, scaleY_);
}
#endif

void RSSurfaceCaptureVisitor::ProcessChildren(RSRenderNode &node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSSurfaceCaptureVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode:" +
        std::to_string(node.GetId()));
    RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode child size:[%{public}d] total",
        node.GetChildrenCount());

    // Mirror Display is unable to snapshot.
    if (node.IsMirrorDisplay()) {
        RS_LOGW("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
            Mirror Display(id:[%{public}" PRIu64 "])", node.GetId());
        return;
    }

    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: Canvas is null!");
        return;
    }

    if (IsUniRender()) {
        FindHardwareEnabledNodes();
        if (hasSecurityLayer_) {
            RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
                process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.", node.GetId());
            ProcessChildren(node);
            DrawWatermarkIfNeed(node);
        } else {
            if (node.GetBuffer() == nullptr) {
                RS_LOGE("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: buffer is null!");
                return;
            }

            RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
                process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) using UniRender buffer.", node.GetId());

            if (hardwareEnabledNodes_.size() != 0) {
                AdjustZOrderAndDrawSurfaceNode();
            }

            auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);

            // Screen capture considering color inversion
            ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
            if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
                colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
                RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
                    SetColorFilterModeToPaint mode:%{public}d.", static_cast<int32_t>(colorFilterMode));
                RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, params.paint);
            }

            renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);
        }
    } else {
        ProcessChildren(node);
    }
}

void RSSurfaceCaptureVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    if (!IsUniRender()) {
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessEffectRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessEffectRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSSurfaceCaptureVisitor::FindHardwareEnabledNodes()
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([this](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
            return;
        }
        if (surfaceNode->IsLastFrameHardwareEnabled() && surfaceNode->GetBuffer() != nullptr) {
            hardwareEnabledNodes_.emplace_back(surfaceNode);
        }
    });
}

void RSSurfaceCaptureVisitor::AdjustZOrderAndDrawSurfaceNode()
{
    if (!RSSystemProperties::GetHardwareComposerEnabled()) {
        RS_LOGW("RSSurfaceCaptureVisitor::AdjustZOrderAndDrawSurfaceNode: \
            HardwareComposer is not enabled.");
        return;
    }

    // sort the surfaceNodes by ZOrder
    std::stable_sort(
        hardwareEnabledNodes_.begin(), hardwareEnabledNodes_.end(), [](const auto& first, const auto& second) -> bool {
            return first->GetGlobalZOrder() < second->GetGlobalZOrder();
        });

    // draw hardwareEnabledNodes
    for (auto& surfaceNode : hardwareEnabledNodes_) {
        if (surfaceNode->IsLastFrameHardwareEnabled() && surfaceNode->GetBuffer() != nullptr) {
#ifndef USE_ROSEN_DRAWING
            RSAutoCanvasRestore acr(canvas_);
#else
            Drawing::AutoCanvasRestore acr(*canvas_, true);
#endif
            CaptureSurfaceInDisplayWithUni(*surfaceNode);
        }
    }
}

#ifndef USE_ROSEN_DRAWING
void RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni(RSSurfaceRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni node:%{public}" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    if (node.IsAppWindow()) {
        // When CaptureSingleSurfaceNodeWithUni, we should consider scale factor of canvas_ and
        // child nodes (self-drawing surfaceNode) of AppWindow should use relative coordinates
        // which is the node relative to the upper-left corner of the window.
        // So we have to get the invert matrix of AppWindow here and apply it to canvas_
        // when we calculate the position of self-drawing surfaceNode.
        captureMatrix_.setScaleX(scaleX_);
        captureMatrix_.setScaleY(scaleY_);
        SkMatrix invertMatrix;
        if (geoPtr->GetAbsMatrix().invert(&invertMatrix)) {
            captureMatrix_.preConcat(invertMatrix);
        }
    } else if (!node.IsStartingWindow()) {
        canvas_->setMatrix(captureMatrix_);
        canvas_->concat(geoPtr->GetAbsMatrix());
    }

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
    }
    canvas_->save();
    if (isSelfDrawingSurface && !property.GetCornerRadius().IsZero()) {
        canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas_->clipRect(SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight()));
    }
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        canvas_->clear(SK_ColorWHITE);
        canvas_->restore(); // restore clipRect
        canvas_->restore(); // restore translate and concat
        return;
    }

    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawBackground(property, *canvas_);
        RSPropertiesPainter::DrawMask(property, *canvas_);
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::BACKGROUND_FILTER);
    } else {
        auto backgroundColor = static_cast<SkColor>(property.GetBackgroundColor().AsArgbInt());
        if (SkColorGetA(backgroundColor) != SK_AlphaTRANSPARENT) {
            canvas_->drawColor(backgroundColor);
        }
    }
    canvas_->restore();
    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::FOREGROUND_FILTER);
        RSPropertiesPainter::DrawLinearGradientBlurFilter(property, *canvas_);
    }

    if (isSelfDrawingSurface) {
        canvas_->restore();
    }

    if (isUIFirst_ && RSUniRenderUtil::HandleCaptureNode(node, *canvas_)) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: \
            process RSSurfaceRenderNode [%{public}s, %{public}" PRIu64 "] use cache texture.",
            node.GetName().c_str(), node.GetId());
        return;
    }

    if (node.IsAppWindow() && RSColdStartManager::Instance().IsColdStartThreadRunning(node.GetId()) &&
        node.GetCachedImage() != nullptr) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: DrawCachedImage.");
        RSUniRenderUtil::DrawCachedImage(node, *canvas_, node.GetCachedImage());
    } else {
        ProcessChildren(node);
    }
}
#else
void RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni(RSSurfaceRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni node:%" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }
    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: canvas_ is nullptr.");
        return;
    }

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isSelfDrawingSurface) {
        canvas_->Save();
    }

    if (node.IsAppWindow()) {
        // When CaptureSingleSurfaceNodeWithUni, we should consider scale factor of canvas_ and
        // child nodes (self-drawing surfaceNode) of AppWindow should use relative coordinates
        // which is the node relative to the upper-left corner of the window.
        // So we have to get the invert matrix of AppWindow here and apply it to canvas_
        // when we calculate the position of self-drawing surfaceNode.
        captureMatrix_.Set(Drawing::Matrix::Index::SCALE_X, scaleX_);
        captureMatrix_.Set(Drawing::Matrix::Index::SCALE_Y, scaleY_);
        Drawing::Matrix invertMatrix;
        if (geoPtr->GetAbsMatrix().Invert(invertMatrix)) {
            captureMatrix_.PreConcat(invertMatrix);
        }
    } else if (!node.IsStartingWindow()) {
        canvas_->SetMatrix(captureMatrix_);
        canvas_->ConcatMatrix(geoPtr->GetAbsMatrix());
    }

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
    }
    canvas_->Save();
    if (isSelfDrawingSurface && !property.GetCornerRadius().IsZero()) {
        canvas_->ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(absClipRRect), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas_->ClipRect(Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()),
            Drawing::ClipOp::INTERSECT, true);
    }
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        canvas_->Clear(Drawing::Color::COLOR_WHITE);
        canvas_->Restore(); // restore clipRect
        canvas_->Restore(); // restore translate and concat
        return;
    }

    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawBackground(property, *canvas_);
        RSPropertiesPainter::DrawMask(property, *canvas_);
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::BACKGROUND_FILTER);
    } else {
        auto backgroundColor = static_cast<Drawing::ColorQuad>(property.GetBackgroundColor().AsArgbInt());
        if (Drawing::Color::ColorQuadGetA(backgroundColor) != Drawing::Color::COLOR_TRANSPARENT) {
            canvas_->Clear(backgroundColor);
        }
    }
    canvas_->Restore();
    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::FOREGROUND_FILTER);
        RSPropertiesPainter::DrawLinearGradientBlurFilter(property, *canvas_);
    }

    if (isSelfDrawingSurface) {
        canvas_->Restore();
    }

    if (isUIFirst_ && RSUniRenderUtil::HandleCaptureNode(node, *canvas_)) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: \
            process RSSurfaceRenderNode [%{public}s, %{public}" PRIu64 "] use cache texture.",
            node.GetName().c_str(), node.GetId());
        return;
    }

    if (node.IsAppWindow() && RSColdStartManager::Instance().IsColdStartThreadRunning(node.GetId()) &&
        node.GetCachedImage() != nullptr) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: DrawCachedImage.");
        RSUniRenderUtil::DrawCachedImage(node, *canvas_, node.GetCachedImage());
    } else {
        ProcessChildren(node);
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }

    if (isUIFirst_ && RSUniRenderUtil::HandleSubThreadNode(node, *canvas_)) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
            process RSSurfaceRenderNode [%{public}s, %{public}" PRIu64 "] use cache texture.",
            node.GetName().c_str(), node.GetId());
        return;
    }
    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;

    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (geoPtr) {
        canvas_->setMatrix(geoPtr->GetAbsMatrix());
    }

    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect, node.IsLeashWindow());
    if (!property.GetCornerRadius().IsZero()) {
        canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas_->clipRect(SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight()));
    }
    auto backgroundColor = static_cast<SkColor>(property.GetBackgroundColor().AsArgbInt());
    if (SkColorGetA(backgroundColor) != SK_AlphaTRANSPARENT) {
        canvas_->drawColor(backgroundColor);
    }
    RSPropertiesPainter::DrawMask(property, *canvas_);

    RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::BACKGROUND_FILTER);
    if (isSelfDrawingSurface) {
        canvas_->restore();
    }

    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }

    ProcessChildren(node);
    RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::FOREGROUND_FILTER);
    RSPropertiesPainter::DrawLinearGradientBlurFilter(property, *canvas_);
}
#else
void RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }

    if (isUIFirst_ && RSUniRenderUtil::HandleSubThreadNode(node, *canvas_)) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
            process RSSurfaceRenderNode [%{public}s, %{public}" PRIu64 "] use cache texture.",
            node.GetName().c_str(), node.GetId());
        return;
    }
    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;

    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (geoPtr) {
        canvas_->SetMatrix(geoPtr->GetAbsMatrix());
    }

    if (isSelfDrawingSurface) {
        canvas_->Save();
    }

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect, node.IsLeashWindow());
    if (!property.GetCornerRadius().IsZero()) {
        canvas_->ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(absClipRRect), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas_->ClipRect(Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()),
            Drawing::ClipOp::INTERSECT, false);
    }
    auto backgroundColor = static_cast<Drawing::ColorQuad>(property.GetBackgroundColor().AsArgbInt());
    if (Drawing::Color::ColorQuadGetA(backgroundColor) != Drawing::Color::COLOR_TRANSPARENT) {
        canvas_->Clear(backgroundColor);
    }
    RSPropertiesPainter::DrawMask(property, *canvas_);

    RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::BACKGROUND_FILTER);
    if (isSelfDrawingSurface) {
        canvas_->Restore();
    }

    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        canvas_->Save();
        canvas_->SetMatrix(node.GetTotalMatrix());
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        canvas_->Restore();
    }

    ProcessChildren(node);
    RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::FOREGROUND_FILTER);
    RSPropertiesPainter::DrawLinearGradientBlurFilter(property, *canvas_);
    DrawWatermarkIfNeed(property.GetBoundsWidth(), property.GetBoundsHeight());
}
#endif

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode &node)
{
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGW("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode node:%{public}" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }

#ifndef USE_ROSEN_DRAWING
    RSAutoCanvasRestore acr(canvas_);
#else
    Drawing::AutoCanvasRestore acr(*canvas_, true);
#endif
    canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha());
    if (isDisplayNode_) {
        CaptureSurfaceInDisplayWithUni(node);
    } else {
        CaptureSingleSurfaceNodeWithUni(node);
    }
}

void RSSurfaceCaptureVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }

#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(canvas_.get(), true);
#else
    Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
#endif
    ProcessCanvasRenderNode(node);
}

void RSSurfaceCaptureVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!IsUniRender()) {
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }

    node.ProcessRenderBeforeChildren(*canvas_);
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasDrawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>();
#ifndef USE_ROSEN_DRAWING
        SkBitmap bitmap = canvasDrawingNode->GetBitmap();
#ifndef NEW_SKIA
        canvas_->drawBitmap(bitmap, 0, 0);
#else
        canvas_->drawImage(bitmap.asImage(), 0, 0);
#endif
#else
        Drawing::Bitmap bitmap = canvasDrawingNode->GetBitmap();
        canvas_->DrawBitmap(bitmap, 0, 0);
#endif
    } else {
        node.ProcessRenderContents(*canvas_);
    }
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

#ifndef USE_ROSEN_DRAWING
void RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni(RSSurfaceRenderNode& node)
{
    SkMatrix translateMatrix;
    auto parentPtr = node.GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        // calculate the offset from this node's parent, and perform translate.
        auto parentNode = std::static_pointer_cast<RSSurfaceRenderNode>(parentPtr);
        const float parentNodeTranslateX = parentNode->GetTotalMatrix().getTranslateX();
        const float parentNodeTranslateY = parentNode->GetTotalMatrix().getTranslateY();
        const float thisNodetranslateX = node.GetTotalMatrix().getTranslateX();
        const float thisNodetranslateY = node.GetTotalMatrix().getTranslateY();
        translateMatrix.preTranslate(
            thisNodetranslateX - parentNodeTranslateX, thisNodetranslateY - parentNodeTranslateY);
    }
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        SkAutoCanvasRestore acr(canvas_.get(), true);
        canvas_->concat(translateMatrix);
        canvas_->clear(SK_ColorWHITE);
        return;
    }

    if (node.GetChildrenCount() > 0) {
        canvas_->concat(translateMatrix);
        const auto saveCnt = canvas_->save();
        ProcessChildren(node);
        canvas_->restoreToCount(saveCnt);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    } else {
        SkAutoCanvasRestore acr(canvas_.get(), true);
        canvas_->concat(translateMatrix);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    }
}
#else
void RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni(RSSurfaceRenderNode& node)
{
    Drawing::Matrix translateMatrix;
    auto parentPtr = node.GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        // calculate the offset from this node's parent, and perform translate.
        auto parentNode = std::static_pointer_cast<RSSurfaceRenderNode>(parentPtr);
        const float parentNodeTranslateX = parentNode->GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X);
        const float parentNodeTranslateY = parentNode->GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y);
        const float thisNodetranslateX = node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X);
        const float thisNodetranslateY = node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y);
        translateMatrix.PreTranslate(
            thisNodetranslateX - parentNodeTranslateX, thisNodetranslateY - parentNodeTranslateY);
    }
    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni: canvas_ is nullptr.");
        return;
    }
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
        canvas_->ConcatMatrix(translateMatrix);
        canvas_->Clear(Drawing::Color::COLOR_WHITE);
        return;
    }

    if (node.GetChildrenCount() > 0) {
        canvas_->ConcatMatrix(translateMatrix);
        const auto saveCnt = canvas_->GetSaveCount();
        canvas_->Save();
        ProcessChildren(node);
        canvas_->RestoreToCount(saveCnt);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    } else {
        Drawing::AutoCanvasRestore acr(*canvas_.get(), true);
        canvas_->ConcatMatrix(translateMatrix);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
        canvas_->Restore();
    }
}
#endif

void RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }
    ProcessChildren(node);
    if (node.GetBuffer() != nullptr) {
        // in display's coordinate.
        auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, false, false, false, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
}

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithoutUni(RSSurfaceRenderNode& node)
{
    if (isDisplayNode_) {
        CaptureSurfaceInDisplayWithoutUni(node);
    } else {
        CaptureSingleSurfaceNodeWithoutUni(node);
    }
}

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::Process:" + node.GetName());

    if (canvas_ == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode node: %{public}" PRIu64 " invisible", node.GetId());
        return;
    }

    // execute security layer in each case, ignore display snapshot and set it white for surface snapshot
    if (IsUniRender()) {
        ProcessSurfaceRenderNodeWithUni(node);
    } else {
        ProcessSurfaceRenderNodeWithoutUni(node);
    }
}

void RSSurfaceCaptureVisitor::DrawWatermarkIfNeed(RSDisplayRenderNode& node)
{
    if (RSMainThread::Instance()->GetWatermarkFlag()) {
        sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
        auto screenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
#ifndef USE_ROSEN_DRAWING
        sk_sp<SkImage> skImage = RSMainThread::Instance()->GetWatermarkImg();
        SkPaint rectPaint;
        auto skSrcRect = SkRect::MakeWH(skImage->width(), skImage->height());
        auto skDstRect = SkRect::MakeWH(screenInfo.width, screenInfo.height);
#ifdef NEW_SKIA
        canvas_->drawImageRect(
            skImage, skSrcRect, skDstRect, SkSamplingOptions(),
            &rectPaint, SkCanvas::kStrict_SrcRectConstraint);
#else
        canvas_->drawImageRect(skImage, skSrcRect, skDstRect, &rectPaint);
#endif
#else
        auto image = RSMainThread::Instance()->GetWatermarkImg();
        if (image == nullptr) {
            return;
        }

        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, screenInfo.width, screenInfo.height);
        Drawing::Brush rectBrush;
        canvas_->AttachBrush(rectBrush);
        canvas_->DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas_->DetachBrush();
#endif
    }
}
} // namespace Rosen
} // namespace OHOS
