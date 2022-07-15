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

#include "rs_composer_adapter.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "rs_base_render_util.h"
#include "rs_divided_render_util.h"
#include "rs_trace.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
bool RSComposerAdapter::Init(ScreenId screenId,  int32_t offsetX, int32_t offsetY, float mirrorAdaptiveCoefficient,
    const FallbackCallback& cb)
{
    hdiBackend_ = HdiBackend::GetInstance();
    if (hdiBackend_ == nullptr) {
        RS_LOGE("RSComposerAdapter::Init: hdiBackend is nullptr");
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSComposerAdapter::Init: ScreenManager is nullptr");
        return false;
    }
    output_ = screenManager->GetOutput(screenId);
    if (output_ == nullptr) {
        RS_LOGE("RSComposerAdapter::Init: output_ is nullptr");
        return false;
    }

    fallbackCb_ = cb;
    auto onPrepareCompleteFunc = [this](auto& surface, const auto& param, void* data) {
        OnPrepareComplete(surface, param, data);
    };
    hdiBackend_->RegPrepareComplete(onPrepareCompleteFunc, this);

    offsetX_ = offsetX;
    offsetY_ = offsetY;
    mirrorAdaptiveCoefficient_ = mirrorAdaptiveCoefficient;
    screenInfo_ = screenManager->QueryScreenInfo(screenId);
    IRect damageRect {0, 0, static_cast<int32_t>(screenInfo_.width), static_cast<int32_t>(screenInfo_.height)};
    output_->SetOutputDamage(1, damageRect);

#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    // enable direct GPU composition.
    output_->SetLayerCompCapacity(LAYER_COMPOSITION_CAPACITY);
#else // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    output_->SetLayerCompCapacity(LAYER_COMPOSITION_CAPACITY_INVALID);
#endif // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)

    return true;
}

void RSComposerAdapter::CommitLayers(const std::vector<LayerInfoPtr>& layers)
{
    if (hdiBackend_ == nullptr) {
        RS_LOGE("RSComposerAdapter::CommitLayers: backend is nullptr");
        return;
    }

    if (output_ == nullptr) {
        RS_LOGE("RSComposerAdapter::CommitLayers: output is nullptr");
        return;
    }

    // do composition.
    output_->SetLayerInfo(layers);
    std::vector<std::shared_ptr<HdiOutput>> outputs {output_};
    hdiBackend_->Repaint(outputs);

    // set all layers' releaseFence.
    const auto layersReleaseFence = hdiBackend_->GetLayersReleaseFence(output_);
    for (const auto& [layer, fence] : layersReleaseFence) {
        if (layer == nullptr) {
            continue;
        }

        auto nodePtr = static_cast<RSBaseRenderNode*>(layer->GetLayerAdditionalInfo());
        if (nodePtr == nullptr) {
            RS_LOGW("RSComposerAdapter::PostProcess: layer's node is nullptr.");
            continue;
        }

        RSSurfaceHandler* surfaceHandler = nullptr;
        if (nodePtr->IsInstanceOf<RSSurfaceRenderNode>()) {
            auto surfaceNode = static_cast<RSSurfaceRenderNode*>(nodePtr);
            surfaceHandler = static_cast<RSSurfaceHandler*>(surfaceNode);
        } else if (nodePtr->IsInstanceOf<RSDisplayRenderNode>()) {
            auto displayNode = static_cast<RSDisplayRenderNode*>(nodePtr);
            surfaceHandler = static_cast<RSSurfaceHandler*>(displayNode);
        }
        if (surfaceHandler == nullptr) {
            continue;
        }
        surfaceHandler->SetReleaseFence(fence);
    }
}

// private func
bool RSComposerAdapter::IsOutOfScreenRegion(const ComposeInfo& info) const
{
    uint32_t boundWidth = screenInfo_.width;
    uint32_t boundHeight = screenInfo_.height;
    ScreenRotation rotation = screenInfo_.rotation;
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(boundWidth, boundHeight);
    }

    const auto& dstRect = info.dstRect;
    if (dstRect.x >= boundWidth || dstRect.y >= boundHeight) {
        return true;
    }

    return false;
}

void RSComposerAdapter::DealWithNodeGravity(RSSurfaceRenderNode& node, ComposeInfo& info) const
{
    const auto& buffer = node.GetBuffer(); // private func, guarantee node's buffer is valid.
    const auto& property = node.GetRenderProperties();
    const float frameWidth = buffer->GetSurfaceBufferWidth();
    const float frameHeight = buffer->GetSurfaceBufferHeight();
    const float boundsWidth = property.GetBoundsWidth();
    const float boundsHeight = property.GetBoundsHeight();
    const Gravity frameGravity = property.GetFrameGravity();
    // we do not need to do additional works for Gravity::RESIZE and if frameSize == boundsSize.
    if (frameGravity == Gravity::RESIZE || (frameWidth == boundsWidth && frameHeight == boundsHeight)) {
        return;
    }

    auto traceInfo = node.GetName() + " DealWithNodeGravity " + std::to_string(static_cast<int>(frameGravity));
    RS_TRACE_NAME(traceInfo.c_str());

    // get current node's translate matrix and calculate gravity matrix.
    auto translateMatrix = SkMatrix::MakeTrans(
        std::ceil(node.GetTotalMatrix().getTranslateX()), std::ceil(node.GetTotalMatrix().getTranslateY()));
    SkMatrix gravityMatrix;
    (void)RSPropertiesPainter::GetGravityMatrix(frameGravity,
        RectF {0.0f, 0.0f, boundsWidth, boundsHeight}, frameWidth, frameHeight, gravityMatrix);

    // create a canvas to calculate new dstRect and new srcRect
    int32_t screenWidth = screenInfo_.width;
    int32_t screenHeight = screenInfo_.height;
    const auto screenRotation = screenInfo_.rotation;
    if (screenRotation == ScreenRotation::ROTATION_90 || screenRotation == ScreenRotation::ROTATION_270) {
        std::swap(screenWidth, screenHeight);
    }
    auto canvas = std::make_unique<SkCanvas>(screenWidth, screenHeight);
    canvas->concat(translateMatrix);
    canvas->concat(gravityMatrix);
    SkRect clipRect;
    gravityMatrix.mapRect(&clipRect, SkRect::MakeWH(frameWidth, frameHeight));
    canvas->clipRect(SkRect::MakeWH(clipRect.width(), clipRect.height()));
    SkIRect newDstRect = canvas->getDeviceClipBounds();
    // we make the newDstRect as the intersection of new and old dstRect,
    // to deal with the situation that frameSize > boundsSize.
    newDstRect.intersect(SkIRect::MakeXYWH(info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h));
    auto localRect = canvas->getLocalClipBounds();
    IRect newSrcRect = {
        std::clamp<int>(localRect.left(), 0, frameWidth),
        std::clamp<int>(localRect.top(), 0, frameHeight),
        std::clamp<int>(localRect.width(), 0, frameWidth - localRect.left()),
        std::clamp<int>(localRect.height(), 0, frameHeight - localRect.top())};

    // log and apply new dstRect and srcRect
    RS_LOGD("RsDebug DealWithNodeGravity: name[%s], gravity[%d], oldDstRect[%d %d %d %d], newDstRect[%d %d %d %d],"\
        " oldSrcRect[%d %d %d %d], newSrcRect[%d %d %d %d].",
        node.GetName().c_str(), static_cast<int>(frameGravity),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h,
        newDstRect.left(), newDstRect.top(), newDstRect.width(), newDstRect.height(),
        info.srcRect.x, info.srcRect.y, info.srcRect.w, info.srcRect.h,
        newSrcRect.x, newSrcRect.y, newSrcRect.w, newSrcRect.h);
    info.dstRect = {newDstRect.left(), newDstRect.top(), newDstRect.width(), newDstRect.height()};
    info.srcRect = newSrcRect;
}

// private func, for RSSurfaceRenderNode.
ComposeInfo RSComposerAdapter::BuildComposeInfo(RSSurfaceRenderNode& node) const
{
    const auto& dstRect = node.GetDstRect();
    const auto& srcRect = node.GetSrcRect();
    const auto& buffer = node.GetBuffer(); // we guarantee the buffer is valid.

    ComposeInfo info {};
    info.srcRect = IRect {srcRect.left_, srcRect.top_, srcRect.width_, srcRect.height_};
    info.dstRect = IRect {
        static_cast<int32_t>(static_cast<float>(dstRect.left_) * mirrorAdaptiveCoefficient_),
        static_cast<int32_t>(static_cast<float>(dstRect.top_) * mirrorAdaptiveCoefficient_),
        static_cast<int32_t>(static_cast<float>(dstRect.width_) * mirrorAdaptiveCoefficient_),
        static_cast<int32_t>(static_cast<float>(dstRect.height_) * mirrorAdaptiveCoefficient_)
    };
    info.zOrder = static_cast<int32_t>(node.GetGlobalZOrder());
    info.alpha.enGlobalAlpha = true;
    info.alpha.gAlpha = node.GetGlobalAlpha() * 255; // map gAlpha from float(0, 1) to uint8_t(0, 255).
    info.buffer = buffer;
    info.fence = node.GetAcquireFence();
    info.blendType = node.GetBlendType();

    info.dstRect.x -= static_cast<int32_t>(static_cast<float>(offsetX_) * mirrorAdaptiveCoefficient_);
    info.dstRect.y -= static_cast<int32_t>(static_cast<float>(offsetY_) * mirrorAdaptiveCoefficient_);
    info.visibleRect = info.dstRect;

    const auto& property = node.GetRenderProperties();
    const auto bufferWidth = buffer->GetSurfaceBufferWidth();
    const auto bufferHeight = buffer->GetSurfaceBufferHeight();
    const auto boundsWidth = property.GetBoundsWidth();
    const auto boundsHeight = property.GetBoundsHeight();
    if (bufferWidth != boundsWidth || bufferHeight != boundsHeight) {
        float xScale = (ROSEN_EQ(boundsWidth, 0.0f) ? 1.0f : bufferWidth / boundsWidth);
        float yScale = (ROSEN_EQ(boundsHeight, 0.0f) ? 1.0f : bufferHeight / boundsHeight);
        info.srcRect.x = info.srcRect.x * xScale;
        info.srcRect.y = info.srcRect.y * yScale;
        info.srcRect.w = info.srcRect.w * xScale;
        info.srcRect.h = info.srcRect.h * yScale;
    }

    bool needClient = RSDividedRenderUtil::IsNeedClient(node, info);
    if (buffer->GetSurfaceBufferColorGamut() != static_cast<ColorGamut>(screenInfo_.colorGamut)) {
        needClient = true;
    }
    info.needClient = needClient;

    DealWithNodeGravity(node, info);

    return info;
}

// private func, for RSDisplayRenderNode
ComposeInfo RSComposerAdapter::BuildComposeInfo(RSDisplayRenderNode& node) const
{
    const auto& buffer = node.GetBuffer(); // we guarantee the buffer is valid.
    ComposeInfo info {};
    info.srcRect = IRect {0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight()};
    info.dstRect = IRect {
        0,
        0,
        static_cast<int32_t>(static_cast<float>(screenInfo_.width) * mirrorAdaptiveCoefficient_),
        static_cast<int32_t>(static_cast<float>(screenInfo_.height) * mirrorAdaptiveCoefficient_)
    };
    info.visibleRect = IRect {info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h};
    info.zOrder = static_cast<int32_t>(node.GetGlobalZOrder());
    info.alpha.enGlobalAlpha = false;
    info.buffer = buffer;
    info.fence = node.GetAcquireFence();
    info.blendType = BLEND_NONE;
    info.needClient = false;
    return info;
}

void RSComposerAdapter::SetComposeInfoToLayer(
    const LayerInfoPtr& layer,
    const ComposeInfo& info,
    const sptr<Surface>& surface,
    RSBaseRenderNode* node) const
{
    if (layer == nullptr) {
        return;
    }

    layer->SetSurface(surface);
    layer->SetBuffer(info.buffer, info.fence);
    layer->SetZorder(info.zOrder);
    layer->SetAlpha(info.alpha);
    layer->SetLayerSize(info.dstRect);
    layer->SetLayerAdditionalInfo(node);
    layer->SetCompositionType(info.needClient ?
        CompositionType::COMPOSITION_CLIENT : CompositionType::COMPOSITION_DEVICE);
    layer->SetVisibleRegion(1, info.visibleRect);
    layer->SetDirtyRegion(info.srcRect);
    layer->SetBlendType(info.blendType);
    layer->SetCropRect(info.srcRect);
}

LayerInfoPtr RSComposerAdapter::CreateLayer(RSSurfaceRenderNode& node)
{
    if (output_ == nullptr) {
        RS_LOGE("RSComposerAdapter::CreateLayer: output is nullptr");
        return nullptr;
    }

    if (node.GetBuffer() == nullptr) {
        RS_LOGE("RsDebug RSComposerAdapter::CreateLayer: node(%llu) has no available buffer.", node.GetId());
        return nullptr;
    }

    const auto& dstRect = node.GetDstRect();
    const auto& srcRect = node.GetSrcRect();
    // check if the node's srcRect and dstRect are valid.
    if (srcRect.width_ <= 0 || srcRect.height_ <= 0 || dstRect.width_ <= 0 || dstRect.height_ <= 0) {
        return nullptr;
    }

    RS_LOGD("RsDebug RSComposerAdapter::CreateLayer start(node(%llu) name:[%s] dst:[%d %d %d %d]).",
        node.GetId(), node.GetName().c_str(), dstRect.left_, dstRect.top_, dstRect.width_, dstRect.height_);

    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RsDebug RSComposerAdapter::CreateLayer: node(%llu)'s geoPtr is nullptr!", node.GetId());
        return nullptr;
    }

    if (!node.IsNotifyRTBufferAvailable()) {
        // Only ipc for one time.
        RS_LOGD("RsDebug RSPhysicalScreenProcessor::ProcessSurface id = %llu "\
                "Notify RT buffer available", node.GetId());
        node.NotifyRTBufferAvailable();
    }

    ComposeInfo info = BuildComposeInfo(node);
    if (IsOutOfScreenRegion(info)) {
        RS_LOGE("RsDebug RSComposerAdapter::CreateLayer: node(%llu) name:[%s] out of screen region, no need to composite.",
            node.GetId(), node.GetName().c_str());
        return nullptr;
    }
    std::string traceInfo;
    AppendFormat(traceInfo, "ProcessSurfaceNode:%s XYWH[%d %d %d %d]", node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h);
    RS_TRACE_NAME(traceInfo.c_str());
    RS_LOGD("RsDebug RSComposerAdapter::CreateLayer surfaceNode id:%llu name:[%s] dst [%d %d %d %d]"\
        "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d] buffaddr:%p, z:%f, globalZOrder:%d, blendType = %d",
        node.GetId(), node.GetName().c_str(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        info.buffer->GetWidth(), info.buffer->GetHeight(),
        info.buffer->GetSurfaceBufferWidth(), info.buffer->GetSurfaceBufferHeight(),
        info.buffer.GetRefPtr(), node.GetGlobalZOrder(), info.zOrder, info.blendType);

    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    SetComposeInfoToLayer(layer, info, node.GetConsumer(), &node);
    LayerRotate(layer);
    LayerCrop(layer);
    LayerScaleDown(layer);
    return layer;
}

LayerInfoPtr RSComposerAdapter::CreateLayer(RSDisplayRenderNode& node)
{
    if (output_ == nullptr) {
        RS_LOGE("RSComposerAdapter::CreateLayer: output is nullptr");
        return nullptr;
    }

    RS_LOGD("RSComposerAdapter::CreateLayer displayNode id:%llu available buffer:%d", node.GetId(),
        node.GetAvailableBufferCount());
    if (!RSBaseRenderUtil::ConsumeAndUpdateBuffer(node)) {
        RS_LOGE("RSComposerAdapter::CreateLayer consume buffer failed.");
        return nullptr;
    }

    ComposeInfo info = BuildComposeInfo(node);
    RS_LOGI("RSComposerAdapter::ProcessSurface displayNode id:%llu dst [%d %d %d %d]"\
        "SrcRect [%d %d] rawbuffer [%d %d] surfaceBuffer [%d %d] buffaddr:%p, globalZOrder:%d, blendType = %d",
        node.GetId(),
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.srcRect.w, info.srcRect.h,
        info.buffer->GetWidth(), info.buffer->GetHeight(),
        info.buffer->GetSurfaceBufferWidth(), info.buffer->GetSurfaceBufferHeight(),
        info.buffer.GetRefPtr(), info.zOrder, info.blendType);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    SetComposeInfoToLayer(layer, info, node.GetConsumer(), &node);
    LayerRotate(layer);
    // do not crop or scale down for displayNode's layer.
    return layer;
}

// private func, guarantee the layer and the surface are valid
// screenRotation: anti-clockwise, surfaceTransform: anti-clockwise, layerTransform: clockwise
static void RotateLayerWhenScreenRotation90(const LayerInfoPtr& layer, const sptr<Surface>& surface)
{
    // screenRotation is already 90 degrees anti-clockwise.
    switch (surface->GetTransform()) {
        case TransformType::ROTATE_90: {
            layer->SetTransform(TransformType::ROTATE_180);
            break;
        }
        case TransformType::ROTATE_180: {
            layer->SetTransform(TransformType::ROTATE_90);
            break;
        }
        case TransformType::ROTATE_270: {
            layer->SetTransform(TransformType::ROTATE_NONE);
            break;
        }
        default: {
            layer->SetTransform(TransformType::ROTATE_270);
            break;
        }
    }
}

// private func, guarantee the layer and the surface are valid
// screenRotation: anti-clockwise, surfaceTransform: anti-clockwise, layerTransform: clockwise
static void RotateLayerWhenScreenRotation180(const LayerInfoPtr& layer, const sptr<Surface>& surface)
{
    // screenRotation is already 180 degrees anti-clockwise.
    switch (surface->GetTransform()) {
        case TransformType::ROTATE_90: {
            layer->SetTransform(TransformType::ROTATE_90);
            break;
        }
        case TransformType::ROTATE_180: {
            layer->SetTransform(TransformType::ROTATE_NONE);
            break;
        }
        case TransformType::ROTATE_270: {
            layer->SetTransform(TransformType::ROTATE_270);
            break;
        }
        default: {
            layer->SetTransform(TransformType::ROTATE_180);
            break;
        }
    }
}

// private func, guarantee the layer and the surface are valid
// screenRotation: anti-clockwise, surfaceTransform: anti-clockwise, layerTransform: clockwise
static void RotateLayerWhenScreenRotation270(const LayerInfoPtr& layer, const sptr<Surface>& surface)
{
    // screenRotation is already 270 degrees anti-clockwise.
    switch (surface->GetTransform()) {
        case TransformType::ROTATE_90: {
            layer->SetTransform(TransformType::ROTATE_NONE);
            break;
        }
        case TransformType::ROTATE_180: {
            layer->SetTransform(TransformType::ROTATE_270);
            break;
        }
        case TransformType::ROTATE_270: {
            layer->SetTransform(TransformType::ROTATE_180);
            break;
        }
        default: {
            layer->SetTransform(TransformType::ROTATE_90);
            break;
        }
    }
}

// private func, guarantee the layer and the surface are valid
// screenRotation: anti-clockwise, surfaceTransform: anti-clockwise, layerTransform: clockwise
static void RotateLayerWhenScreenRotationNone(const LayerInfoPtr& layer, const sptr<Surface>& surface)
{
    // screenRotation is 0.
    switch (surface->GetTransform()) {
        case TransformType::ROTATE_90: {
            layer->SetTransform(TransformType::ROTATE_270);
            break;
        }
        case TransformType::ROTATE_180: {
            layer->SetTransform(TransformType::ROTATE_180);
            break;
        }
        case TransformType::ROTATE_270: {
            layer->SetTransform(TransformType::ROTATE_90);
            break;
        }
        default: {
            layer->SetTransform(TransformType::ROTATE_NONE);
            break;
        }
    }
}

// private func, guarantee the layer is valid
void RSComposerAdapter::LayerRotate(const LayerInfoPtr& layer) const
{
    auto surface = layer->GetSurface();
    if (surface == nullptr) {
        return;
    }

    const auto screenWidth = static_cast<int32_t>(screenInfo_.width);
    const auto screenHeight = static_cast<int32_t>(screenInfo_.height);
    const auto screenRotation = screenInfo_.rotation;
    const auto rect = layer->GetLayerSize();
    // screenRotation: anti-clockwise, surfaceTransform: anti-clockwise, layerTransform: clockwise
    switch (screenRotation) {
        case ScreenRotation::ROTATION_90: {
            RS_LOGD("RsDebug ScreenRotation 90, Before Rotate layer size [%d %d %d %d]",
                rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(IRect {rect.y, screenHeight - rect.x - rect.w, rect.h, rect.w});
            RS_LOGD("RsDebug ScreenRotation 90, After Rotate layer size [%d %d %d %d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            RotateLayerWhenScreenRotation90(layer, surface);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            RS_LOGD("RsDebug ScreenRotation 180, Before Rotate layer size [%d %d %d %d]",
                rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(
                IRect {screenWidth - rect.x - rect.w, screenHeight - rect.y - rect.h, rect.w, rect.h});
            RS_LOGD("RsDebug ScreenRotation 180, After Rotate layer size [%d %d %d %d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            RotateLayerWhenScreenRotation180(layer, surface);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            RS_LOGD("RsDebug ScreenRotation 270, Before Rotate layer size [%d %d %d %d]",
                rect.x, rect.y, rect.w, rect.h);
            layer->SetLayerSize(IRect {screenWidth - rect.y - rect.h, rect.x, rect.h, rect.w});
            RS_LOGD("RsDebug ScreenRotation 270, After Rotate layer size [%d %d %d %d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            RotateLayerWhenScreenRotation270(layer, surface);
            break;
        }
        default:  {
            RotateLayerWhenScreenRotationNone(layer, surface);
            break;
        }
    }
}

// private func, guarantee the layer is valid
void RSComposerAdapter::LayerCrop(const LayerInfoPtr& layer) const
{
    IRect dstRect = layer->GetLayerSize();
    IRect srcRect = layer->GetCropRect();
    IRect originSrcRect = srcRect;

    RectI dstRectI(dstRect.x, dstRect.y, dstRect.w, dstRect.h);
    int32_t screenWidth = static_cast<int32_t>(screenInfo_.width);
    int32_t screenHeight = static_cast<int32_t>(screenInfo_.height);
    RectI screenRectI(0, 0, screenWidth, screenHeight);
    RectI resDstRect = dstRectI.IntersectRect(screenRectI);
    if (resDstRect == dstRectI) {
        return;
    }
    dstRect = {resDstRect.left_, resDstRect.top_, resDstRect.width_, resDstRect.height_};
    srcRect.x = resDstRect.IsEmpty() ? 0 : std::ceil((resDstRect.left_ - dstRectI.left_) *
        originSrcRect.w / dstRectI.width_);
    srcRect.y = resDstRect.IsEmpty() ? 0 : std::ceil((resDstRect.top_ - dstRectI.top_) *
        originSrcRect.h / dstRectI.height_);
    srcRect.w = dstRectI.IsEmpty() ? 0 : originSrcRect.w * resDstRect.width_ / dstRectI.width_;
    srcRect.h = dstRectI.IsEmpty() ? 0 : originSrcRect.h * resDstRect.height_ / dstRectI.height_;
    layer->SetLayerSize(dstRect);
    layer->SetDirtyRegion(srcRect);
    layer->SetCropRect(srcRect);
    RS_LOGD("RsDebug RSComposerAdapter::LayerCrop layer has been cropped dst[%d %d %d %d] src[%d %d %d %d]",
        dstRect.x, dstRect.y, dstRect.w, dstRect.h, srcRect.x, srcRect.y, srcRect.w, srcRect.h);
}

// private func, guarantee the layer is valid
void RSComposerAdapter::LayerScaleDown(const LayerInfoPtr& layer) const
{
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    const auto& buffer = layer->GetBuffer();
    const auto& surface = layer->GetSurface();
    if (buffer == nullptr || surface == nullptr) {
        return;
    }

    if (surface->GetScalingMode(buffer->GetSeqNum(), scalingMode) == GSERROR_OK &&
        scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        IRect dstRect = layer->GetLayerSize();
        IRect srcRect = layer->GetCropRect();

        uint32_t newWidth = static_cast<uint32_t>(srcRect.w);
        uint32_t newHeight = static_cast<uint32_t>(srcRect.h);

        if (newWidth * dstRect.h > newHeight * dstRect.w) {
            // too wide
            newWidth = dstRect.w * newHeight / dstRect.h;
        } else if (newWidth * dstRect.h < newHeight * dstRect.w) {
            // too tall
            newHeight = dstRect.h * newWidth / dstRect.w;
        } else {
            return;
        }

        uint32_t currentWidth = static_cast<uint32_t>(srcRect.w);
        uint32_t currentHeight = static_cast<uint32_t>(srcRect.h);

        if (newWidth < currentWidth) {
            // the crop is too wide
            uint32_t dw = currentWidth - newWidth;
            auto halfdw = dw / 2;
            srcRect.x += halfdw;
            srcRect.w = newWidth;
        } else {
            // thr crop is too tall
            uint32_t dh = currentHeight - newHeight;
            auto halfdh = dh / 2;
            srcRect.y += halfdh;
            srcRect.h = newHeight;
        }
        layer->SetDirtyRegion(srcRect);
        layer->SetCropRect(srcRect);
        RS_LOGD("RsDebug RSComposerAdapter::LayerScaleDown layer has been scaledown dst[%d %d %d %d]"\
            "src[%d %d %d %d]", dstRect.x, dstRect.y, dstRect.w, dstRect.h,
            srcRect.x, srcRect.y, srcRect.w, srcRect.h);
    }
}

void RSComposerAdapter::OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data)
{
    // unused data.
    (void)(data);

    if (!param.needFlushFramebuffer) {
        RS_LOGD("RsDebug RSComposerAdapter::OnPrepareComplete: no need to flush frame buffer");
        return;
    }

    if (fallbackCb_ != nullptr) {
        fallbackCb_(surface, param.layers);
    }
}
} // namespace Rosen
} // namespace OHOS
