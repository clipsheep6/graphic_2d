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

#include "drawing_context.h"

#include "GLES3/gl32.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/gl/GrGLInterface.h"

#include "render_context/shader_cache.h"
#include "utils/log.h"

#include "render_context_base.h"

namespace OHOS {
namespace Rosen {
sk_sp<SkSurface> DrawingContext::AcquireSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame,
    const RenderType& renderType)
{
    if (renderType == RenderType::GLES) {
        return AcquireSurfaceInGLES(frame);
    } else if (renderType == RenderType::RASTER){
        return AcquireSurfaceInRaster(frame);
    } else {
        return AcquireSurfaceInVulkan(frame);
    }
}

bool DrawingContext::SetUpGrContext()
{
#if defined(RS_ENABLE_GL)
    if (grContext_ != nullptr) {
        LOGD("grContext has already initialized");
        return true;
    }

    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (glInterface.get() == nullptr) {
        LOGE("SetUpGrContext failed to make native interface");
        return false;
    }

    GrContextOptions options;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
    options.fAllowPathMaskCaching = true;
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    bool isUni = false;
    auto &cache = ShaderCache::Instance();
#if defined(RS_ENABLE_UNI_RENDER)
    isUni = true;
#endif
    cache.InitShaderCache(glesVersion, size, isUni);
    options.fPersistentCache = &cache;

#if defined(NEW_SKIA)
    sk_sp<GrDirectContext> grContext(GrDirectContext::MakeGL(std::move(glInterface), options));
#else
    sk_sp<GrContext> grContext(GrContext::MakeGL(std::move(glInterface), options));
#endif
    if (grContext == nullptr) {
        LOGE("Failed to create grContext, grContext is nullptr");
        return false;
    }
    grContext_ = std::move(grContext);
    return true;
#else
    return false;
#endif
}

#if defined(NEW_SKIA)
GrDirectContext* DrawingContext::GetGrContext() const
{
    return grContext_.get();
}
#else
GrContext* DrawingContext::GetGrContext() const
{
    return grContext_.get();
}
#endif

sk_sp<SkSurface> DrawingContext::AcquireSurfaceInGLES(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (!SetUpGrContext()) {
        LOGE("Failed to acquire skia surface in gles, grcontext is not ready");
        return nullptr;
    }
#if defined(NEW_SKIA)
    GrDirectContext* grContext = GetGrContext();
#else
    GrContext* grContext = GetGrContext();
#endif
    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;

    GrBackendRenderTarget backendRenderTarget(frame->width_, frame->height_, 0, 8, framebufferInfo);
#if defined(NEW_SKIA)
    SkSurfaceProps surfaceProps(0, kRGB_H_SkPixelGeometry);
#else
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
#endif

    sk_sp<SkColorSpace> skColorSpace = nullptr;
    
    ColorGamut colorSpace = frame->colorSpace_;
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
        LOGE("Skia surface is nullptr");
        return nullptr;
    }

    LOGD("Acquire skia surface in gles successfully");
    return skSurface;
}

sk_sp<SkSurface> DrawingContext::AcquireSurfaceInRaster(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    sptr<SurfaceBuffer> buffer = frame->buffer_;
    if ((buffer == nullptr) || (buffer->GetWidth() <= 0) || (buffer->GetHeight() <= 0)) {
        LOGE("Buffer is invalide");
        return nullptr;
    }

    auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
    if (addr == nullptr) {
        LOGE("buffer addr is invalid");
        return nullptr;
    }
    SkImageInfo info =
        SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    sk_sp<SkSurface> skSurface = SkSurface::MakeRasterDirect(info, addr, buffer->GetStride());

    LOGE("Acquire skia surface in raster successfully");
    
    return skSurface;
}

sk_sp<SkSurface> DrawingContext::AcquireSurfaceInVulkan(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
#ifdef RS_ENABLE_VK
    if (frame->vulkanWindow_ == nullptr) {
        LOGE("Failed to acquire skia surface in vulkan, vulkanWindow_ is nullptr");
        return nullptr;
    }
    return frame_->vulkanWindow_->AcquireSurface();
#else
    return nullptr;
#endif
}
}
}