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

#include "rs_uni_render_processor.h"

#include "rs_trace.h"
#include "string_utils.h"

#include "platform/common/rs_log.h"

#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"
#if defined(RS_ENABLE_DRIVEN_RENDER)
#include "pipeline/driven_render/rs_driven_surface_render_node.h"
#endif

#include "pipeline/rs_uni_render_util.h"
#include "src/gpu/gl/GrGLDefines.h"

namespace OHOS {
namespace Rosen {
RSUniRenderProcessor::RSUniRenderProcessor()
    : uniComposerAdapter_(std::make_unique<RSUniRenderComposerAdapter>())
{
}

RSUniRenderProcessor::~RSUniRenderProcessor() noexcept
{
}

bool RSUniRenderProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
                                std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine)) {
        return false;
    }
    // In uni render mode, we can handle screen rotation in the rendering process,
    // so we do not need to handle rotation in composer adapter any more,
    // just pass the buffer to composer straightly.
    screenInfo_.rotation = ScreenRotation::ROTATION_0;
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    return uniComposerAdapter_->Init(screenInfo_, offsetX, offsetY, mirrorAdaptiveCoefficient_);
}

void RSUniRenderProcessor::PostProcess(RSDisplayRenderNode* node)
{
    if (node != nullptr) {
        auto acquireFence = node->GetAcquireFence();
        auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
        for (auto surfaceNode : selfDrawingNodes) {
            if (!surfaceNode->IsCurrentFrameHardwareEnabled()) {
                // current frame : gpu
                // use display node's acquire fence as release fence to ensure not release buffer until gpu finish
                surfaceNode->SetCurrentReleaseFence(acquireFence);
                if (surfaceNode->IsLastFrameHardwareEnabled()) {
                    // last frame : hwc
                    // use display node's acquire fence as release fence to ensure not release buffer until its real
                    // release fence signals
                    surfaceNode->SetReleaseFence(acquireFence);
                }
            }
        }
    }
    uniComposerAdapter_->CommitLayers(layers_);
    if (!isPhone_) {
        MultiLayersPerf(layerNum);
    }
    RS_LOGD("RSUniRenderProcessor::PostProcess layers_:%{public}zu", layers_.size());
}

void RSUniRenderProcessor::GetPointerIntersectImageByLayer(LayerInfoPtr& layer, const RectI& pointerRect,
    RectI& maxDstRect, std::shared_ptr<Drawing::Image>& image, std::shared_ptr<RSPaintFilterCanvas>& canvas,
    std::shared_ptr<RSBaseRenderEngine>& renderEngine)
{
    GraphicIRect layerSize = layer->GetLayerSize();
    RectI curRect = RectI(layerSize.x, layerSize.y, layerSize.w, layerSize.h);
    RectI dstRect = curRect.IntersectRect(pointerRect);
    if (dstRect.width_ * dstRect.height_ >= maxDstRect.width_ * maxDstRect.height_) {
        bool forceCPU = RSBaseRenderEngine::NeedForceCPU(layers_);
        auto params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
        Drawing::Bitmap bitmap;
        std::shared_ptr<Drawing::Image> layerImage = std::make_shared<Drawing::Image>();
        sptr<SurfaceBuffer> buffer = layer->GetBuffer();

#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            auto imageCache = NativeVkImageRes::Create(buffer);
            auto& backendTexture = imageCache->GetBackendTexture();

            Drawing::ColorType colorType = Drawing::ColorType::COLORTYPE_RGBA_8888;
            auto pixelFmt = params.buffer->GetFormat();
            if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
                colorType = Drawing::ColorType::COLORTYPE_BGRA_8888;
            } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
                colorType = Drawing::ColorType::COLORTYPE_RGBA_1010102;
            } else if (pixelFmt == GRAPHIC_PIXEL_FMT_RGB_565) {
                colorType = Drawing::ColorType::COLORTYPE_RGB_565;
            }
            Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
            auto surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;

            std::shared_ptr<Drawing::GPUContext> contextTest = std::make_shared<Drawing::GPUContext>();

            auto context = canvas->GetGPUContext();
            if (context == nullptr) {
                RS_LOGE("RSUniRenderProcessor::GetPointerIntersectImageByLayer contextDrawingVk is nullptr.");
                return;
            }
            if (!layerImage->BuildFromTexture(*context, backendTexture.GetTextureInfo(),
                surfaceOrigin, bitmapFormat, nullptr,
                NativeBufferUtils::DeleteVkImage, imageCache->RefCleanupHelper())) {
                RS_LOGE("RSUniRenderProcessor::GetPointerIntersectImageByLayer backendTexture is not valid!!!");
                return;
            }

            dstRect = dstRect.Offset(-curRect.GetLeft(), -curRect.GetTop());
            Drawing::RectI imgCutRect = Drawing::RectI(0, 0, dstRect.GetWidth(), dstRect.GetHeight());
            std::shared_ptr<Drawing::Image> cutDownImage = std::make_shared<Drawing::Image>();
            cutDownImage->BuildSubset(layerImage, imgCutRect, *context);
            image = cutDownImage;
            maxDstRect = dstRect;
        }
#endif
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_EGLIMAGE)
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
            auto eglImageCache = renderEngine->GetEglImageManager()->CreateImageCacheFromBuffer(params.buffer,
                params.acquireFence);
            if (eglImageCache == nullptr) {
                RS_LOGE("RSUniRenderProcessor::GetPointerIntersectImageByLayer eglImageCache nullptr");
                return;
            }
            uint32_t eglTextureId = eglImageCache->TextureId();
            if (eglTextureId == 0) {
                RS_LOGE("RSUniRenderProcessor::GetPointerIntersectImageByLayer invalid texture ID");
                return;
            }

            Drawing::ColorType colorType = Drawing::ColorType::COLORTYPE_RGBA_8888;
            auto pixelFmt = params.buffer->GetFormat();
            if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
                colorType = Drawing::ColorType::COLORTYPE_BGRA_8888;
            } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
                colorType = Drawing::ColorType::COLORTYPE_RGBA_1010102;
            }

            Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };

            Drawing::TextureInfo externalTextureInfo;
            externalTextureInfo.SetWidth(params.buffer->GetSurfaceBufferWidth());
            externalTextureInfo.SetHeight(params.buffer->GetSurfaceBufferHeight());
            externalTextureInfo.SetIsMipMapped(false);
            externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
            externalTextureInfo.SetID(eglTextureId);
            auto glType = GR_GL_RGBA8;
            if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
                glType = GR_GL_BGRA8;
            } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
                glType = GL_RGB10_A2;
            }
            externalTextureInfo.SetFormat(glType);

            std::shared_ptr<Drawing::Image> layerImage = std::make_shared<Drawing::Image>();
            image = std::make_shared<Drawing::Image>();
            if (!layerImage->BuildFromTexture(*canvas->GetGPUContext(), externalTextureInfo,
                Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
                RS_LOGE("RSUniRenderProcessor::GetPointerIntersectImageByLayer image BuildFromTexture failed");
                return;
            }

            dstRect = dstRect.Offset(-curRect.GetLeft(), -curRect.GetTop());
            Drawing::RectI imgCutRect = Drawing::RectI(0, 0, dstRect.GetWidth(), dstRect.GetHeight());
            std::shared_ptr<Drawing::Image> cutDownImage = std::make_shared<Drawing::Image>();
            cutDownImage->BuildSubset(layerImage, imgCutRect, *canvas->GetGPUContext());
            image = cutDownImage;
            maxDstRect = dstRect;
        }
#endif
    }
}

void RSUniRenderProcessor::GetPointerIntersectImage(std::shared_ptr<Drawing::Image>& image,
    std::shared_ptr<RSPaintFilterCanvas>& canvas, std::shared_ptr<RSBaseRenderEngine>& renderEngine)
{
    bool hasPointer = false;
    RectI pointerRect;
    size_t displayNodeNum = SIZE_MAX;

    for (size_t i = 0; i < layers_.size(); ++i) {
        if (layers_[i]->GetSurface()->GetName().find("DisplayNode") != std::string::npos) {
            displayNodeNum = i;
            continue;
        }
        if (layers_[i]->GetSurface()->GetName().find("pointer") != std::string::npos) {
            GraphicIRect pointerGraphicRect = layers_[i]->GetLayerSize();
            pointerRect.SetAll(pointerGraphicRect.x, pointerGraphicRect.y,
                pointerGraphicRect.w, pointerGraphicRect.h);
            hasPointer = true;
            continue;
        }
    }

    if (!hasPointer || displayNodeNum == SIZE_MAX) {
        return;
    }

    RectI maxDstRect;
    for (size_t i = 0; i >= 0 && i < displayNodeNum; --i) {
        GetPointerIntersectImageByLayer(layers_[i], pointerRect, maxDstRect, image, canvas, renderEngine);
    }

    if (maxDstRect.width_ * maxDstRect.height_ == 0 && displayNodeNum != SIZE_MAX) {
        GetPointerIntersectImageByLayer(layers_[displayNodeNum], pointerRect, maxDstRect, image, canvas, renderEngine);
    }
}

void RSUniRenderProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    node.MarkCurrentFrameHardwareEnabled();
    layers_.emplace_back(layer);
}

void RSUniRenderProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessDisplaySurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    if (node.GetFingerprint()) {
        layer->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_HBM_SYNC);
        RS_LOGD("RSUniRenderProcessor::ProcessDisplaySurface, set layer mask hbm sync");
    } else {
        layer->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_NORMAL);
    }
    layers_.emplace_back(layer);
    for (auto surface : node.GetCurAllSurfaces()) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surface);
        if (!surfaceNode || !surfaceNode->GetOcclusionVisible() || surfaceNode->IsLeashWindow()) {
            continue;
        }
        layerNum++;
    }
}

void RSUniRenderProcessor::ProcessDrivenSurface(RSDrivenSurfaceRenderNode& node)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessDrivenSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    layers_.emplace_back(layer);
#endif
}

void RSUniRenderProcessor::ProcessRcdSurface(RSRcdSurfaceRenderNode& node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessRcdSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    layers_.emplace_back(layer);
}

} // namespace Rosen
} // namespace OHOS
