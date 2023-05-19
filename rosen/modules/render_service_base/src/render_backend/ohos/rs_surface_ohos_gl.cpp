/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos/rs_surface_ohos_gl.h"

#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "window.h"
#include <hilog/log.h>
#include "include/core/SkSurface.h"

#if !defined(NEW_SKIA)
#include "memory/rs_tag_tracker.h"
#endif

#include "ohos/render_context_ohos_gl.h"

namespace OHOS {
namespace Rosen {
RSSurfaceOhosGl::~RSSurfaceOhosGl()
{
    DestoryNativeWindow(nativeWindow_);
    if (renderContext_ != nullptr) {
        renderContext_->DestroySurface(eglSurface_);
    }
    nativeWindow_ = nullptr;
    eglSurface_ = EGL_NO_SURFACE;
    frame_ = nullptr;
}

std::shared_ptr<RSSurfaceFrame> RSSurfaceOhosGl::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC)
{
    if (renderContext_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosGl::RequestFrame, renderContext_ is nullptr");
        return nullptr;
    }
    if (nativeWindow_ == nullptr) {
        nativeWindow_ = CreateNativeWindowFromSurface(&producer_);
        eglSurface_ = static_cast<EGLSurface>(renderContext_->CreateSurface(nativeWindow_));
        ROSEN_LOGD("RSSurfaceOhosGl:: create and Init EglSurface");
    }

    if (eglSurface_ == EGL_NO_SURFACE) {
        ROSEN_LOGE("RSSurfaceOhosGl:: Invalid eglSurface, return");
        return nullptr;
    }
    NativeWindowHandleOpt(nativeWindow_, SET_FORMAT, frame_->pixelFormat_);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        NativeWindowHandleOpt(nativeWindow_, GET_FORMAT, &format);
        if (format == PIXEL_FMT_RGBA_8888 && useAFBC) {
            bufferUsage_ =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif
    NativeWindowHandleOpt(nativeWindow_, SET_USAGE, bufferUsage_);
    NativeWindowHandleOpt(nativeWindow_, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(nativeWindow_, SET_COLOR_GAMUT, frame_->colorSpace_);
    NativeWindowHandleOpt(nativeWindow_, SET_UI_TIMESTAMP, uiTimestamp);

    renderContext_->MakeCurrent(eglSurface_);

    frame_->width_ = width;
    frame_->height_ = height;
    ROSEN_LOGD("RSSurfaceOhosGl::RequestFrame, width is %d, height is %d",
        width, height);

    return frame_;
}

bool RSSurfaceOhosGl::FlushFrame(uint64_t uiTimestamp)
{
    if (renderContext_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosGl::FlushFrame, GetRenderContext failed!");
        return false;
    }

    // gpu render flush
    RenderFrame();
    renderContext_->SwapBuffers();
    ROSEN_LOGD("RSSurfaceOhosGl:: FlushFrame, SwapBuffers eglsurface");
    return true;
}

void RSSurfaceOhosGl::SetUiTimeStamp(uint64_t uiTimestamp)
{
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    NativeWindowHandleOpt(nativeWindow_, SET_UI_TIMESTAMP, duration);
}

void RSSurfaceOhosGl::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    renderContext_->DamageFrame(left, top, width, height);
}

void RSSurfaceOhosGl::SetDamageRegion(const std::vector<RectI> &rects)
{
    renderContext_->DamageFrame(rects);
}

int32_t RSSurfaceOhosGl::GetBufferAge()
{
    return renderContext_->GetBufferAge();
}

void RSSurfaceOhosGl::ClearBuffer()
{
    if (renderContext_ != nullptr && eglSurface_ != EGL_NO_SURFACE && producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosGl:: Clear surface buffer!");
        DestoryNativeWindow(nativeWindow_);
        renderContext_->MakeCurrent(EGL_NO_SURFACE);
        renderContext_->DestroySurface(eglSurface_);
        eglSurface_ = EGL_NO_SURFACE;
        nativeWindow_ = nullptr;
        producer_->GoBackground();
    }
}

sk_sp<SkSurface> RSSurfaceOhosGl::AcquireSurface()
{
    if (!renderContext_->SetUpGrContext()) {
        ROSEN_LOGE("RSSurfaceOhosGl::AcquireSurface GrContext is not ready!!!");
        return nullptr;
    }
#if defined(NEW_SKIA)
    GrDirectContext* grContext = renderContext_->GetGrContext();
#else
    GrContext* grContext = renderContext_->GetGrContext();
#endif
    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;

    GrBackendRenderTarget backendRenderTarget(frame_->width_, frame_->height_, 0, 8, framebufferInfo);
#if defined(NEW_SKIA)
    SkSurfaceProps surfaceProps(0, kRGB_H_SkPixelGeometry);
#else
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
#endif

    sk_sp<SkColorSpace> skColorSpace = nullptr;
    
    ColorGamut colorSpace = frame_->colorSpace_;
    switch (colorSpace) {
        // [planning] in order to stay consistant with the colorspace used before, we disabled
        // COLOR_GAMUT_SRGB to let the branch to default, then skColorSpace is set to nullptr
        case COLOR_GAMUT_DISPLAY_P3:
#if defined(NEW_SKIA)
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDisplayP3);
#else
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
#endif
            break;
        case COLOR_GAMUT_ADOBE_RGB:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kAdobeRGB);
            break;
        case COLOR_GAMUT_BT2020:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kRec2020);
            break;
        default:
            break;
    }
#if !defined(NEW_SKIA)
    RSTagTracker tagTracker(grContext, RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);
#endif
    sk_sp<SkSurface> skSurface = SkSurface::MakeFromBackendRenderTarget(
        grContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType,
        skColorSpace, &surfaceProps);
    if (skSurface == nullptr) {
        ROSEN_LOGE("skSurface is nullptr");
        return nullptr;
    }

    ROSEN_LOGD("RSSurfaceOhosGl::AcquireSurface successfully");
    return skSurface;
}

void RSSurfaceOhosGl::RenderFrame()
{
    RS_TRACE_FUNC();
    // flush commands
    if (frame_->skSurface_->getCanvas() != nullptr) {
        ROSEN_LOGD("RenderFrame: Canvas");
        frame_->skSurface_->getCanvas()->flush();
    } else {
        ROSEN_LOGE("canvas is nullptr!!!");
    }
}

}
}