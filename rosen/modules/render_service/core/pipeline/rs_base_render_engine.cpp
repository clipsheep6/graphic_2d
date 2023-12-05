/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_base_render_engine.h"
#include <memory>

#ifdef RS_ENABLE_EGLIMAGE
#include "src/gpu/gl/GrGLDefines.h"
#endif

#include "rs_divided_render_util.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#if defined(NEW_RENDER_CONTEXT)
#include "render_context_factory.h"
#include "rs_surface_factory.h"
#include "ohos/rs_render_surface_ohos.h"
#else
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif
#endif
#include "render/rs_skia_filter.h"
#include "metadata_helper.h"

namespace OHOS {
namespace Rosen {
RSBaseRenderEngine::RSBaseRenderEngine()
{
}

RSBaseRenderEngine::~RSBaseRenderEngine() noexcept
{
}

void RSBaseRenderEngine::Init(bool independentContext)
{
    (void)independentContext;
#if defined(NEW_RENDER_CONTEXT)
    RenderType renderType = RenderType::RASTER;
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    renderType = RSSystemProperties::GetRsVulkanEnabled() ? RenderType::VULKAN : RenderType::GLES
#endif
    renderContext_ = RenderContextBaseFactory::CreateRenderContext(renderType);
    renderContext_->Init();
    drawingContext_ = std::make_shared<Rosen::DrawingContext>(renderContext_->GetRenderType());
    drawingContext_->SetUpDrawingContext();
#else
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    renderContext_ = std::make_shared<RenderContext>();
#ifdef RS_ENABLE_GL
    renderContext_->InitializeEglContext();
#endif
    if (RSUniRenderJudgement::IsUniRender()) {
        RS_LOGI("RSRenderEngine::RSRenderEngine set new cacheDir");
        renderContext_->SetUniRenderMode(true);
    }
#ifndef USE_ROSEN_DRAWING
#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        skContext_ = RsVulkanContext::GetSingleton().CreateSkContext(independentContext);
        vkImageManager_ = std::make_shared<RSVkImageManager>();
        renderContext_->SetUpGrContext(skContext_);
    } else {
        renderContext_->SetUpGrContext(nullptr);
    }
#else
    renderContext_->SetUpGrContext(nullptr);
#endif // RS_ENABLE_VK
#else
#if defined(RS_ENABLE_VK)
    skContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext(independentContext);
    vkImageManager_ = std::make_shared<RSVkImageManager>();
    renderContext_->SetUpGpuContext(skContext_);
#else
    renderContext_->SetUpGpuContext();
#endif
#endif // USE_ROSEN_DRAWING
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#endif
#if defined(RS_ENABLE_EGLIMAGE)
#if defined(NEW_RENDER_CONTEXT)
    std::shared_ptr<RSRenderSurfaceFrame> frame = renderContext_->GetRSRenderSurfaceFrame();
    eglImageManager_ = std::make_shared<RSEglImageManager>(frame->eglState->eglDisplay);
#else
    eglImageManager_ = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
#endif
#endif // RS_ENABLE_EGLIMAGE
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetRsVulkanEnabled()) {
#ifndef USE_ROSEN_DRAWING
        skContext_ = RsVulkanContext::GetSingleton().CreateSkContext();
#else
        skContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext();
#endif
        vkImageManager_ = std::make_shared<RSVkImageManager>();
    }
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
    colorSpaceConverterDisplay_ = Media::VideoProcessingEngine::ColorSpaceConverterDisplay::Create();
#endif
}

void RSBaseRenderEngine::ResetCurrentContext()
{
#if (defined RS_ENABLE_GL)
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        return;
    }
    if (renderContext_ == nullptr) {
        RS_LOGE("This render context is nullptr.");
        return;
    }
    renderContext_->ShareMakeCurrentNoSurface(EGL_NO_CONTEXT);
#endif // RS_ENABLE_GL
}

bool RSBaseRenderEngine::NeedForceCPU(const std::vector<LayerInfoPtr>& layers)
{
    bool forceCPU = false;
    for (const auto& layer: layers) {
        if (layer == nullptr) {
            continue;
        }

        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            continue;
        }

#ifndef RS_ENABLE_EGLIMAGE
        const auto bufferFormat = buffer->GetFormat();
        if (bufferFormat == GRAPHIC_PIXEL_FMT_YCRCB_420_SP || bufferFormat == GRAPHIC_PIXEL_FMT_YCBCR_420_SP) {
            forceCPU = true;
            break;
        }
#endif

#ifndef USE_VIDEO_PROCESSING_ENGINE
        GraphicColorGamut srcGamut = static_cast<GraphicColorGamut>(buffer->GetSurfaceBufferColorGamut());
        GraphicColorGamut dstGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
        if (srcGamut != dstGamut) {
            forceCPU = true;
            break;
        }
#endif
    }

    return forceCPU;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> RSBaseRenderEngine::CreateEglImageFromBuffer(RSPaintFilterCanvas& canvas,
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, const uint32_t threadIndex,
    GraphicColorGamut colorGamut)
#else
std::shared_ptr<Drawing::Image> RSBaseRenderEngine::CreateEglImageFromBuffer(RSPaintFilterCanvas& canvas,
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, const uint32_t threadIndex,
    GraphicColorGamut colorGamut)
#endif
{
#ifdef RS_ENABLE_EGLIMAGE
    if (!RSBaseRenderUtil::IsBufferValid(buffer)) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer invalid param!");
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
#if defined(RS_ENABLE_GL)
    if (!RSSystemProperties::GetRsVulkanEnabled() && canvas.recordingContext() == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer recordingContext is null!");
        return nullptr;
    }
#endif
#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetRsVulkanEnabled() && renderContext_->GetGrContext() == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
#endif
#else
    if (canvas.getGrContext() == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
#endif
#else
#if defined(RS_ENABLE_GL)
    if (canvas.GetGPUContext() == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
#elif defined(RS_ENABLE_VK)
    if (renderContext_->GetDrGPUContext() == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
#endif
#endif
    auto eglTextureId = eglImageManager_->MapEglImageFromSurfaceBuffer(buffer, acquireFence, threadIndex);
    if (eglTextureId == 0) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer MapEglImageFromSurfaceBuffer return invalid texture ID");
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkColorSpace> skColorSpace = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    skColorSpace = ConvertColorGamutToSkColorSpace(colorGamut);
#else
    (void)colorGamut;
#endif

    SkColorType colorType = kRGBA_8888_SkColorType;
    auto pixelFmt = buffer->GetFormat();
    if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
        colorType = kBGRA_8888_SkColorType;
    } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        colorType = kRGBA_1010102_SkColorType;
    }
    auto glType = GR_GL_RGBA8;
    if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
        glType = GR_GL_BGRA8;
    } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        glType = GR_GL_RGB10_A2;
    }

    GrGLTextureInfo grExternalTextureInfo = { GL_TEXTURE_EXTERNAL_OES, eglTextureId, static_cast<GrGLenum>(glType) };
    GrBackendTexture backendTexture(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(),
        GrMipMapped::kNo, grExternalTextureInfo);
#ifndef ROSEN_EMULATOR
    auto surfaceOrigin = kTopLeft_GrSurfaceOrigin;
#else
    auto surfaceOrigin = kBottomLeft_GrSurfaceOrigin;
#endif
#ifdef NEW_SKIA
#if defined(RS_ENABLE_GL)
    if (!RSSystemProperties::GetRsVulkanEnabled()) {
        return SkImage::MakeFromTexture(canvas.recordingContext(), backendTexture,
            surfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
    }
#endif
#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        return SkImage::MakeFromTexture(renderContext_->GetGrContext(), backendTexture,
            surfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
    }
#endif
    return nullptr;
#else
    return SkImage::MakeFromTexture(canvas.getGrContext(), backendTexture,
        surfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
#endif
#else
    Drawing::ColorType colorType = (buffer->GetFormat() == GRAPHIC_PIXEL_FMT_BGRA_8888) ?
        Drawing::ColorType::COLORTYPE_BGRA_8888 : Drawing::ColorType::COLORTYPE_RGBA_8888;
    Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };

    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(buffer->GetSurfaceBufferWidth());
    externalTextureInfo.SetHeight(buffer->GetSurfaceBufferHeight());
    externalTextureInfo.SetIsMipMapped(false);
    externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
    externalTextureInfo.SetID(eglTextureId);
    externalTextureInfo.SetFormat(GL_RGBA8);

    auto image = std::make_shared<Drawing::Image>();
#if defined(RS_ENABLE_GL)
    if (!RSSystemProperties::GetRsVulkanEnabled() && !image->BuildFromTexture(*canvas.GetGPUContext(), externalTextureInfo,
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer image BuildFromTexture failed");
        return nullptr;
    }
    return image;
#endif

#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetRsVulkanEnabled() && !image->BuildFromTexture(*renderContext_->GetDrGPUContext(), externalTextureInfo,
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer image BuildFromTexture failed");
        return nullptr;
    }
    return image;
#endif
    return nullptr;
#endif
#else
    return nullptr;
#endif // RS_ENABLE_EGLIMAGE
}

#ifdef NEW_RENDER_CONTEXT
std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(
    const std::shared_ptr<RSRenderSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC)
#else
std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(const std::shared_ptr<RSSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC)
#endif
{
    if (rsSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        return nullptr;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::RequestFrame(RSSurface)");
    rsSurface->SetColorSpace(config.colorGamut);
    rsSurface->SetSurfacePixelFormat(config.format);

    auto bufferUsage = config.usage;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (forceCPU) {
        bufferUsage |= BUFFER_USAGE_CPU_WRITE;
    }
#else
    bufferUsage |= BUFFER_USAGE_CPU_WRITE;
#endif
    rsSurface->SetSurfaceBufferUsage(bufferUsage);

    // check if we can use GPU context
#if defined(NEW_RENDER_CONTEXT)
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (forceCPU) {
        std::shared_ptr<RenderContextBase> renderContextRaster = RenderContextBaseFactory::CreateRenderContext(
            RenderType::RASTER);
        renderContextRaster->Init();
        std::shared_ptr<DrawingContext> drawingContextRaster = std::make_shared<DrawingContext>(RenderType::RASTER);
        rsSurface->SetRenderContext(renderContextRaster);
        rsSurface->SetDrawingContext(drawingContextRaster);
        RS_LOGD("RSBaseRenderEngine::RequestFrame force CPU");
    } else {
        if (renderContext_ != nullptr) {
            rsSurface->SetRenderContext(renderContext_);
        }
        if (drawingContext_ != nullptr) {
            rsSurface->SetDrawingContext(drawingContext_);
        }
    }
#endif
#else
#ifdef RS_ENABLE_GL
    if (!RSSystemProperties::GetRsVulkanEnabled() && renderContext_ != nullptr) {
        rsSurface->SetRenderContext(renderContext_.get());
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetRsVulkanEnabled() && skContext_ != nullptr) {
        std::static_pointer_cast<RSSurfaceOhosVulkan>(rsSurface)->SetSkContext(skContext_);
    }
#endif
#endif
    auto surfaceFrame = rsSurface->RequestFrame(config.width, config.height, 0, useAFBC);
    RS_OPTIONAL_TRACE_END();
    if (surfaceFrame == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: request SurfaceFrame failed!");
        return nullptr;
    }
#ifdef NEW_RENDER_CONTEXT
    return std::make_unique<RSRenderFrame>(rsSurface);
#else
    return std::make_unique<RSRenderFrame>(rsSurface, std::move(surfaceFrame));
#endif
}

std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(const sptr<Surface>& targetSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::RequestFrame(targetSurface)");
    if (targetSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        RS_OPTIONAL_TRACE_END();
        return nullptr;
    }

#if defined(NEW_RENDER_CONTEXT)
    std::shared_ptr<RSRenderSurfaceOhos> rsSurface = nullptr;
    std::shared_ptr<RSRenderSurface> renderSurface = RSSurfaceFactory::CreateRSSurface(PlatformName::OHOS,
        targetSurface);
    rsSurface = std::static_pointer_cast<RSRenderSurfaceOhos>(renderSurface);
#else
    std::shared_ptr<RSSurfaceOhos> rsSurface = nullptr;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (!RSSystemProperties::GetRsVulkanEnabled()) {
        if (forceCPU) {
            rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
        } else {
            rsSurface = std::make_shared<RSSurfaceOhosGl>(targetSurface);
        }
    }
#endif
#if (defined RS_ENABLE_VK)
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        rsSurface = std::make_shared<RSSurfaceOhosVulkan>(targetSurface);
    }
#endif
    if (rsSurface == nullptr) {
        rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
    }
#endif
    RS_OPTIONAL_TRACE_END();
    return RequestFrame(rsSurface, config, forceCPU, useAFBC);
}

#ifdef NEW_RENDER_CONTEXT
void RSBaseRenderEngine::SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame,
    std::shared_ptr<RSRenderSurfaceOhos> surfaceOhos)
#else
void RSBaseRenderEngine::SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame,
    std::shared_ptr<RSSurfaceOhos> surfaceOhos)
#endif
{
    if (surfaceOhos == nullptr) {
        RS_LOGE("RSBaseRenderEngine::SetUiTimeStamp: surfaceOhos is null!");
        return;
    }

    if (renderFrame == nullptr) {
        RS_LOGE("RSBaseRenderEngine::SetUiTimeStamp: renderFrame is null!.");
        return;
    }

#ifdef NEW_RENDER_CONTEXT
    surfaceOhos->SetUiTimeStamp();
#else
    auto& frame = renderFrame->GetFrame();
    surfaceOhos->SetUiTimeStamp(frame);
#endif
}

void RSBaseRenderEngine::DrawDisplayNodeWithParams(RSPaintFilterCanvas& canvas, RSDisplayRenderNode& node,
    BufferDrawParam& params)
{
    if (params.useCPU) {
        DrawBuffer(canvas, params);
    } else {
        RegisterDeleteBufferListener(node.GetConsumer());
        DrawImage(canvas, params);
    }
}

void RSBaseRenderEngine::SetColorFilterMode(ColorFilterMode mode)
{
    uint32_t uMode = static_cast<uint32_t>(mode);
    uint32_t uInvertMode = static_cast<uint32_t>(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    uint32_t ucolorFilterMode = static_cast<uint32_t>(colorFilterMode_);
    bool isInvertModeEnabled = ((ucolorFilterMode & uInvertMode) == uInvertMode);

    switch (mode) {
        case ColorFilterMode::INVERT_COLOR_DISABLE_MODE: {
            if (colorFilterMode_ != ColorFilterMode::COLOR_FILTER_END) {
                colorFilterMode_ = static_cast<ColorFilterMode>(ucolorFilterMode & ~uInvertMode);
            }
            break;
        }
        case ColorFilterMode::INVERT_COLOR_ENABLE_MODE: {
            if (colorFilterMode_ != ColorFilterMode::COLOR_FILTER_END) {
                colorFilterMode_ = static_cast<ColorFilterMode>(ucolorFilterMode | uInvertMode);
            } else {
                colorFilterMode_ = mode;
            }
            break;
        }
        case ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE:
        case ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE:
        case ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE: {
            if (isInvertModeEnabled) {
                colorFilterMode_ = static_cast<ColorFilterMode>(uMode | uInvertMode);
            } else {
                colorFilterMode_ = mode;
            }
            break;
        }
        case ColorFilterMode::DALTONIZATION_NORMAL_MODE: {
            if (isInvertModeEnabled) {
                colorFilterMode_ = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
            } else {
                colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;
            }
            break;
        }

        // INVERT_DALTONIZATION_PROTANOMALY_MODE, INVERT_DALTONIZATION_DEUTERANOMALY_MODE
        // INVERT_DALTONIZATION_TRITANOMALY_MODE and COLOR_FILTER_END can not be set directly
        case ColorFilterMode::INVERT_DALTONIZATION_PROTANOMALY_MODE: // fall-through
        case ColorFilterMode::INVERT_DALTONIZATION_DEUTERANOMALY_MODE: // fall-through
        case ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE: // fall-through
        case ColorFilterMode::COLOR_FILTER_END: // fall-through
        default: {
            colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;
            break;
        }
    }
}

ColorFilterMode RSBaseRenderEngine::GetColorFilterMode()
{
    return colorFilterMode_;
}

void RSBaseRenderEngine::DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::DrawBuffer(CPU)");
#ifndef USE_ROSEN_DRAWING
    SkBitmap bitmap;
#else
    Drawing::Bitmap bitmap;
#endif
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(params.buffer, newBuffer, params.targetColorGamut, bitmap,
        params.metaDatas)) {
        RS_LOGE("RSDividedRenderUtil::DrawBuffer: create bitmap failed.");
        RS_OPTIONAL_TRACE_END();
        return;
    }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    canvas.drawImageRect(bitmap.asImage(), params.srcRect, params.dstRect,
        SkSamplingOptions(), &(params.paint), SkCanvas::kStrict_SrcRectConstraint);
#else
    canvas.drawBitmapRect(bitmap, params.srcRect, params.dstRect, &(params.paint));
#endif
#else
    Drawing::Image drImage;
    drImage.BuildFromBitmap(bitmap);
    canvas.DrawImageRect(drImage, params.srcRect, params.dstRect, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
#endif
    RS_OPTIONAL_TRACE_END();
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
bool RSBaseRenderEngine::ConvertColorGamutToSpaceInfo(const GraphicColorGamut& colorGamut,
    HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    static const std::map<GraphicColorGamut, CM_ColorSpaceType> RS_TO_COMMON_COLOR_SPACE_TYPE_MAP {
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT601, CM_BT601_EBU_FULL},
        {GRAPHIC_COLOR_GAMUT_STANDARD_BT709, CM_BT709_FULL},
        {GRAPHIC_COLOR_GAMUT_SRGB, CM_SRGB_FULL},
        {GRAPHIC_COLOR_GAMUT_ADOBE_RGB, CM_ADOBERGB_FULL},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_P3, CM_P3_FULL},
        {GRAPHIC_COLOR_GAMUT_BT2020, CM_DISPLAY_BT2020_SRGB},
        {GRAPHIC_COLOR_GAMUT_BT2100_PQ, CM_BT2020_PQ_FULL},
        {GRAPHIC_COLOR_GAMUT_BT2100_HLG, CM_BT2020_HLG_FULL},
        {GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, CM_DISPLAY_BT2020_SRGB},
    };

    CM_ColorSpaceType colorSpaceType = CM_COLORSPACE_NONE;
    if (RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.find(colorGamut) != RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.end()) {
        colorSpaceType = RS_TO_COMMON_COLOR_SPACE_TYPE_MAP.at(colorGamut);
    }

    GSError ret = MetadataHelper::ConvertColorSpaceTypeToInfo(colorSpaceType, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSBaseRenderEngine::ConvertColorGamutToSpaceInfo ConvertColorSpaceTypeToInfo failed with \
            %{public}u.", ret);
        return false;
    }

    return true;
}

bool RSBaseRenderEngine::SetColorSpaceConverterDisplayParameter(
    const BufferDrawParam& params, Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter& parameter)
{
    using namespace HDI::Display::Graphic::Common::V1_0;

    GSError ret = MetadataHelper::GetColorSpaceInfo(params.buffer, parameter.inputColorSpace.colorSpaceInfo);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor GetColorSpaceInfo failed with %{public}u.", ret);
        return false;
    }

    if (!ConvertColorGamutToSpaceInfo(params.targetColorGamut, parameter.outputColorSpace.colorSpaceInfo)) {
        return false;
    }

    CM_HDR_Metadata_Type hdrMetadataType = CM_METADATA_NONE;
    ret = MetadataHelper::GetHDRMetadataType(params.buffer, hdrMetadataType);
    if (ret != GSERROR_OK) {
        RS_LOGW("RSBaseRenderEngine::ColorSpaceConvertor GetHDRMetadataType failed with %{public}u.", ret);
    }

    parameter.inputColorSpace.metadataType = hdrMetadataType;
    parameter.outputColorSpace.metadataType = hdrMetadataType;

    ret = MetadataHelper::GetHDRStaticMetadata(params.buffer, parameter.staticMetadata);
    if (ret != GSERROR_OK) {
        RS_LOGW("RSBaseRenderEngine::ColorSpaceConvertor GetHDRStaticMetadata failed with %{public}u.", ret);
    }
    ret = MetadataHelper::GetHDRDynamicMetadata(params.buffer, parameter.dynamicMetadata);
    if (ret != GSERROR_OK) {
        RS_LOGW("RSBaseRenderEngine::ColorSpaceConvertor GetHDRDynamicMetadata failed with %{public}u.", ret);
    }

    // Set brightness to screen brightness when HDR Vivid, otherwise 500 nits
    if (hdrMetadataType == CM_VIDEO_HDR_VIVID) {
        parameter.tmoNits = params.screenBrightnessNits;
        parameter.currentDisplayNits = params.screenBrightnessNits;
    } else {
        constexpr float SDR_SCREEN_LIGHT = 500.0f;
        parameter.tmoNits = SDR_SCREEN_LIGHT;
        parameter.currentDisplayNits = SDR_SCREEN_LIGHT;
    }

    RS_LOGD("RSBaseRenderEngine::ColorSpaceConvertor parameter inputColorSpace.colorSpaceInfo.primaries = %{public}u, \
            inputColorSpace.metadataType = %{public}u, outputColorSpace.colorSpaceInfo.primaries = %{public}u, \
            outputColorSpace.metadataType = %{public}u, tmoNits = %{public}f, currentDisplayNits = %{public}f",
            parameter.inputColorSpace.colorSpaceInfo.primaries, parameter.inputColorSpace.metadataType,
            parameter.outputColorSpace.colorSpaceInfo.primaries, parameter.outputColorSpace.metadataType,
            parameter.tmoNits, parameter.currentDisplayNits);

    return true;
}

sk_sp<SkColorSpace> RSBaseRenderEngine::ConvertColorGamutToSkColorSpace(GraphicColorGamut colorGamut)
{
    sk_sp<SkColorSpace> skColorSpace = nullptr;
    switch (colorGamut) {
        case GRAPHIC_COLOR_GAMUT_DISPLAY_P3:
#if defined(NEW_SKIA)
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDisplayP3);
#else
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
#endif
            break;
        case GRAPHIC_COLOR_GAMUT_ADOBE_RGB:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kAdobeRGB);
            break;
        case GRAPHIC_COLOR_GAMUT_BT2020:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kRec2020);
            break;
        default:
            break;
    }

    return skColorSpace;
}

void RSBaseRenderEngine::ColorSpaceConvertor(sk_sp<SkShader> &inputShader, BufferDrawParam& params)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::ColorSpaceConvertor");
    Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter parameter;
    if (!SetColorSpaceConverterDisplayParameter(params, parameter)) {
        RS_OPTIONAL_TRACE_END();
        return;
    }

    sk_sp<SkShader> outputShader;
    auto convRet = colorSpaceConverterDisplay_->Process(inputShader, outputShader, parameter);
    if (convRet != Media::VideoProcessingEngine::VPE_ALGO_ERR_OK) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor colorSpaceConverterDisplay failed with %{public}u.", convRet);
        RS_OPTIONAL_TRACE_END();
        return;
    }
    if (outputShader == nullptr) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor outputShader is nullptr.");
        RS_OPTIONAL_TRACE_END();
        return;
    }
    params.paint.setShader(outputShader);
    RS_OPTIONAL_TRACE_END();
}
#endif

void RSBaseRenderEngine::DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::DrawImage(GPU)");
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(params.buffer,
            params.acquireFence, params.threadIndex);
        auto& backendTexture = imageCache->GetBackendTexture();
#ifndef USE_ROSEN_DRAWING
        if (!backendTexture.isValid()) {
            ROSEN_LOGE("RSBaseRenderEngine::DrawImage: backendTexture is not valid!!!");
            RS_OPTIONAL_TRACE_END();
            return;
        }

        SkColorType colorType = (params.buffer->GetFormat() == GRAPHIC_PIXEL_FMT_BGRA_8888) ?
            kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
#ifndef ROSEN_EMULATOR
        auto surfaceOrigin = kTopLeft_GrSurfaceOrigin;
#else
        auto surfaceOrigin = kBottomLeft_GrSurfaceOrigin;
#endif
        auto image = SkImage::MakeFromTexture(
            canvas.recordingContext(), backendTexture, surfaceOrigin, colorType, kPremul_SkAlphaType,
            SkColorSpace::MakeSRGB(), NativeBufferUtils::DeleteVkImage, imageCache->RefCleanupHelper());

        canvas.drawImageRect(image, params.srcRect, params.dstRect,
            SkSamplingOptions(), &(params.paint), SkCanvas::kStrict_SrcRectConstraint);
#else
        Drawing::ColorType colorType = (params.buffer->GetFormat() == GRAPHIC_PIXEL_FMT_BGRA_8888) ?
            Drawing::ColorType::COLORTYPE_BGRA_8888 : Drawing::ColorType::COLORTYPE_RGBA_8888;
        Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
        auto image = std::make_shared<Drawing::Image>();
#ifndef ROSEN_EMULATOR
        auto surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
#else
        auto surfaceOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
#endif
        if (!image->BuildFromTexture(*canvas.GetGPUContext(), backendTexture,
            surfaceOrigin, bitmapFormat, nullptr,
            NativeBufferUtils::DeleteVkImage, imageCache->RefCleanupHelper())) {
                ROSEN_LOGE("RSBaseRenderEngine::DrawImage: backendTexture is not valid!!!");
                RS_OPTIONAL_TRACE_END();
                return;
            }

            canvas.AttachBrush(params.paint);
            canvas.DrawImageRect(*image, params.srcRect, params.dstRect,
                Drawing::SamplingOptions(), Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
            canvas.DetachBrush();
#endif
    RS_OPTIONAL_TRACE_END();
    }
#endif

#ifdef RS_ENABLE_GL // RS_ENABLE_GL
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        return;
    }
    auto image = CreateEglImageFromBuffer(canvas, params.buffer, params.acquireFence, params.threadIndex,
        params.targetColorGamut);
    if (image == nullptr) {
        RS_LOGE("RSBaseRenderEngine::DrawImage: image is nullptr!");
        RS_OPTIONAL_TRACE_END();
        return;
    }

#ifdef USE_VIDEO_PROCESSING_ENGINE
    SkMatrix matrix;
    auto sx = params.dstRect.width() / params.srcRect.width();
    auto sy = params.dstRect.height() / params.srcRect.height();
    matrix.setScaleTranslate(sx, sy, params.dstRect.x(), params.dstRect.y());
    auto samplingOptions = RSSystemProperties::IsPhoneType()
                            ? SkSamplingOptions()
                            : SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear);
    sk_sp<SkShader> imageShader = image->makeShader(samplingOptions, matrix);
    if (imageShader == nullptr) {
        RS_LOGE("RSBaseRenderEngine::DrawImage imageShader is nullptr.");
    } else {
        params.paint.setShader(imageShader);
        ColorSpaceConvertor(imageShader, params);
    }
#endif

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
#ifndef USE_VIDEO_PROCESSING_ENGINE
    auto samplingOptions = RSSystemProperties::IsPhoneType()
                               ? SkSamplingOptions()
                               : SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear);
    canvas.drawImageRect(
        image, params.srcRect, params.dstRect, samplingOptions, &(params.paint), SkCanvas::kStrict_SrcRectConstraint);
#else
    canvas.drawRect(params.dstRect, (params.paint));
#endif // USE_VIDEO_PROCESSING_ENGINE
#else
#ifndef USE_VIDEO_PROCESSING_ENGINE
    canvas.drawImageRect(image, params.srcRect, params.dstRect, &(params.paint));
#else
    canvas.drawRect(params.dstRect, &(params.paint));
#endif // USE_VIDEO_PROCESSING_ENGINE
#endif
#else
    canvas.AttachBrush(params.paint);
    auto samplingOptions = RSSystemProperties::IsPhoneType()
                               ? Drawing::SamplingOptions()
                               : Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    canvas.DrawImageRect(*image.get(), params.srcRect, params.dstRect, samplingOptions,
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
#endif
    RS_OPTIONAL_TRACE_END();
#endif // RS_ENABLE_GL
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(const sptr<IConsumerSurface>& consumer, bool isForUniRedraw)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        auto regUnMapVkImageFunc = [this, isForUniRedraw](int32_t bufferId) {
            if (isForUniRedraw) {
                vkImageManager_->UnMapVkImageFromSurfaceBufferForUniRedraw(bufferId);
            } else {
                vkImageManager_->UnMapVkImageFromSurfaceBuffer(bufferId);
            }
        };
        if (consumer == nullptr ||
            (consumer->RegisterDeleteBufferListener(regUnMapVkImageFunc, isForUniRedraw) != GSERROR_OK)) {
            RS_LOGE("RSBaseRenderEngine::RegisterDeleteBufferListener: failed to register UnMapVkImage callback.");
        }
        return;
    }
#endif // #ifdef RS_ENABLE_VK

#ifdef RS_ENABLE_EGLIMAGE
    auto regUnMapEglImageFunc = [this, isForUniRedraw](int32_t bufferId) {
        if (isForUniRedraw) {
            eglImageManager_->UnMapEglImageFromSurfaceBufferForUniRedraw(bufferId);
        } else {
            eglImageManager_->UnMapEglImageFromSurfaceBuffer(bufferId);
        }
    };
    if (consumer == nullptr ||
        (consumer->RegisterDeleteBufferListener(regUnMapEglImageFunc, isForUniRedraw) != GSERROR_OK)) {
        RS_LOGE("RSBaseRenderEngine::RegisterDeleteBufferListener: failed to register UnMapEglImage callback.");
    }
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(RSSurfaceHandler& handler)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        auto regUnMapVkImageFunc = [this](int32_t bufferId) {
            vkImageManager_->UnMapVkImageFromSurfaceBuffer(bufferId);
        };
        handler.RegisterDeleteBufferListener(regUnMapVkImageFunc);
        return;
    }
#endif // #ifdef RS_ENABLE_VK

#ifdef RS_ENABLE_EGLIMAGE
    auto regUnMapEglImageFunc = [this](int32_t bufferId) {
        eglImageManager_->UnMapEglImageFromSurfaceBuffer(bufferId);
    };
    handler.RegisterDeleteBufferListener(regUnMapEglImageFunc);
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::ShrinkCachesIfNeeded(bool isForUniRedraw)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetRsVulkanEnabled()) {
        if (vkImageManager_ != nullptr) {
            vkImageManager_->ShrinkCachesIfNeeded(isForUniRedraw);
        }
        return;
    }
#endif // RS_ENABLE_VK

#ifdef RS_ENABLE_EGLIMAGE
    if (eglImageManager_ != nullptr) {
        eglImageManager_->ShrinkCachesIfNeeded(isForUniRedraw);
    }
#endif // RS_ENABLE_EGLIMAGE
}
} // namespace Rosen
} // namespace OHOS
