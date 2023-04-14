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

#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_cold_start_thread.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_divided_render_util.h"
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
std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::Run()
{
    if (ROSEN_EQ(scaleX_, 0.f) || ROSEN_EQ(scaleY_, 0.f) || scaleX_ < 0.f || scaleY_ < 0.f) {
        RS_LOGE("RSSurfaceCaptureTask::Run: SurfaceCapture scale is invalid.");
        return nullptr;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: node is nullptr");
        return nullptr;
    }
    std::unique_ptr<Media::PixelMap> pixelmap;
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(scaleX_, scaleY_, RSUniRenderJudgement::IsUniRender());
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        RS_LOGD("RSSurfaceCaptureTask::Run: Into SURFACE_NODE SurfaceRenderNodeId:[%" PRIu64 "]", node->GetId());
        pixelmap = CreatePixelMapBySurfaceNode(surfaceNode, visitor_->IsUniRender());
        visitor_->IsDisplayNode(false);
    } else if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        RS_LOGD("RSSurfaceCaptureTask::Run: Into DISPLAY_NODE DisplayRenderNodeId:[%" PRIu64 "]", node->GetId());
        visitor_->SetHasingSecurityLayer(FindSecurityLayer());
        pixelmap = CreatePixelMapByDisplayNode(displayNode, visitor_->IsUniRender(),
            visitor_->GetHasingSecurityLayer());
        visitor_->IsDisplayNode(true);
    } else {
        RS_LOGE("RSSurfaceCaptureTask::Run: Invalid RSRenderNodeType!");
        return nullptr;
    }
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: pixelmap == nullptr!");
        return nullptr;
    }
#ifdef RS_ENABLE_GL
#ifndef NEW_SKIA
    auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
    GrContext* grContext = renderContext != nullptr ? renderContext->GetGrContext() : nullptr;
    RSTagTracker tagTracker(grContext, node->GetId(), RSTagTracker::TAGTYPE::TAG_CAPTURE);
#endif
#endif
    auto skSurface = CreateSurface(pixelmap);
    if (skSurface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: surface is nullptr!");
        return nullptr;
    }
    visitor_->SetSurface(skSurface.get());
    node->Process(visitor_);
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    sk_sp<SkImage> img(skSurface.get()->makeImageSnapshot());
    if (!img) {
        RS_LOGE("RSSurfaceCaptureTask::Run: img is nullptr");
        return nullptr;
    }

    auto data = (uint8_t *)malloc(pixelmap->GetRowBytes() * pixelmap->GetHeight());
    if (data == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: data is nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSSurfaceCaptureTask::Run: readPixels failed");
        free(data);
        data = nullptr;
        return nullptr;
    }
    pixelmap->SetPixelsAddr(data, nullptr, pixelmap->GetRowBytes() * pixelmap->GetHeight(),
        Media::AllocatorType::HEAP_ALLOC, nullptr);
#endif
    if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        if (visitor_->IsUniRender() && !visitor_->GetHasingSecurityLayer()) {
            auto rotation = displayNode->GetRotation();
            if (rotation == ScreenRotation::ROTATION_90) {
                pixelmap->rotate(static_cast<int32_t>(90)); // 90 degrees
            }

            if (rotation == ScreenRotation::ROTATION_270) {
                pixelmap->rotate(static_cast<int32_t>(270)); // 270 degrees
            }
            RS_LOGD("RSSurfaceCaptureTask::Run: PixelmapRotation: %d", static_cast<int32_t>(rotation));
        }
    }
    return pixelmap;
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
    RS_LOGD("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: origin pixelmap width is [%u], height is [%u], "\
        "created pixelmap width is [%u], height is [%u], the scale is scaleY:[%f], scaleY:[%f]",
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
    if (!isUniRender || hasSecurityLayer) {
        auto rotation = node->GetRotation();
        if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
            std::swap(pixmapWidth, pixmapHeight);
        }
    }
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGD("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: origin pixelmap width is [%u], height is [%u], "\
        "created pixelmap width is [%u], height is [%u], the scale is scaleY:[%f], scaleY:[%f]",
        pixmapWidth, pixmapHeight, opts.size.width, opts.size.height, scaleX_, scaleY_);
    return Media::PixelMap::Create(opts);
}

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
    auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
    if (renderContext == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    renderContext->SetUpGrContext();
    return SkSurface::MakeRenderTarget(renderContext->GetGrContext(), SkBudgeted::kNo, info);
#endif
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

bool RSSurfaceCaptureTask::FindSecurityLayer()
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    bool hasSecurityLayer = false;
    nodeMap.TraverseSurfaceNodes([this, &hasSecurityLayer](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
        mutable {
        if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
            RS_LOGW("RSSurfaceCaptureTask::FindSecurityLayer: \
                process RSSurfaceRenderNode(id:[%" PRIu64 "]) paused since it is null or not on the tree.",
                surfaceNode->GetId());
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
}

void RSSurfaceCaptureVisitor::SetSurface(SkSurface* surface)
{
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureVisitor::SetSurface: surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->scale(scaleX_, scaleY_);
}

void RSSurfaceCaptureVisitor::ProcessBaseRenderNode(RSBaseRenderNode &node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSSurfaceCaptureVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode:" +
        std::to_string(node.GetId()));
    RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode child size:[%d] total size:[%d]",
        node.GetChildrenCount(), node.GetSortedChildren().size());

    // Mirror Display is unable to snapshot.
    if (node.IsMirrorDisplay()) {
        RS_LOGW("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
            Mirror Display(id:[%" PRIu64 "])", node.GetId());
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
                process RSDisplayRenderNode(id:[%" PRIu64 "]) Not using UniRender buffer.", node.GetId());
            ProcessBaseRenderNode(node);
        } else {
            if (node.GetBuffer() == nullptr) {
                RS_LOGE("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: buffer is null!");
                return;
            }

            RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
                process RSDisplayRenderNode(id:[%" PRIu64 "]) using UniRender buffer.", node.GetId());

            if (hardwareEnabledNodes_.size() != 0) {
                AdjustZOrderAndDrawSurfaceNode();
            }

            auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);

            // Screen capture considering color inversion
            ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
            if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
                colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
                RS_LOGD("RSSurfaceCaptureVisitor::ProcessDisplayRenderNode: \
                    SetColorFilterModeToPaint mode:%d.", static_cast<int32_t>(colorFilterMode));
                RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, params.paint);
            }

            renderEngine_->DrawDisplayNodeWithParams(*canvas_, node, params);
        }
    } else {
        ProcessBaseRenderNode(node);
    }
}

void RSSurfaceCaptureVisitor::FindHardwareEnabledNodes()
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([this](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
        if (surfaceNode == nullptr || !surfaceNode->IsOnTheTree()) {
            RS_LOGW("RSSurfaceCaptureVisitor::FindHardwareEnabledNodes: \
                process RSSurfaceRenderNode(id:[%" PRIu64 "]) paused since it is null or not on the tree.",
                surfaceNode->GetId());
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
            CaptureSingleSurfaceNodeWithUni(*surfaceNode);
        }
    }
}

void RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni(RSSurfaceRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni node:%" PRIu64 ", get geoPtr failed",
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
    } else if (!node.IsStartingWindow() && !isSelfDrawingSurface) {
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
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        canvas_->clear(SK_ColorWHITE);
        canvas_->restore(); // restore clipRect
        canvas_->restore(); // restore translate and concat
        return;
    }
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawBackground(property, *canvas_);
        RSPropertiesPainter::DrawMask(property, *canvas_);
        auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
        if (filter != nullptr) {
            auto skRectPtr = std::make_unique<SkRect>();
            skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
            RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
        }
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
        auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
        if (filter != nullptr) {
            auto skRectPtr = std::make_unique<SkRect>();
            skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
            RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
        }
    }

    if (isSelfDrawingSurface) {
        canvas_->restore();
    }
    if (node.IsAppWindow() && RSColdStartManager::Instance().IsColdStartThreadRunning(node.GetId()) &&
        node.GetCachedImage() != nullptr) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: DrawCachedImage.");
        RSUniRenderUtil::DrawCachedImage(node, *canvas_, node.GetCachedImage());
    } else {
        ProcessBaseRenderNode(node);
    }
}

void RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }
    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;

    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    const auto& property = node.GetRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (geoPtr) {
        canvas_->concat(geoPtr->GetMatrix());
    }

    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
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

    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }
    if (isSelfDrawingSurface) {
        canvas_->restore();
    }

    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }

    if (isSelfDrawingSurface) {
        canvas_->restore();
    }

    ProcessBaseRenderNode(node);
    filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }
    DrawWatermarkIfNeed(property.GetBoundsWidth(), property.GetBoundsHeight());
}

void RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode &node)
{
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGW("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode node:%" PRIu64 ", get geoPtr failed", node.GetId());
        return;
    }

    RSAutoCanvasRestore acr(canvas_);
    canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha() * node.GetContextAlpha());
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

    SkAutoCanvasRestore acr(canvas_.get(), true);
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
    node.GetMutableRenderProperties().CheckEmptyBounds();
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

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
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        SkAutoCanvasRestore acr(canvas_.get(), true);
        canvas_->concat(translateMatrix);
        canvas_->clear(SK_ColorWHITE);
        return;
    }

    if (node.GetChildrenCount() > 0) {
        canvas_->concat(translateMatrix);
        const auto saveCnt = canvas_->save();
        ProcessBaseRenderNode(node);
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

void RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni: \
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }
    ProcessBaseRenderNode(node);
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
        RS_LOGD("RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode node: %" PRIu64 " invisible", node.GetId());
        return;
    }

    // execute security layer in each case, ignore display snapshot and set it white for surface snapshot
    if (IsUniRender()) {
        ProcessSurfaceRenderNodeWithUni(node);
    } else {
        ProcessSurfaceRenderNodeWithoutUni(node);
    }
}

void RSSurfaceCaptureVisitor::DrawWatermarkIfNeed(float screenWidth, float screenHeight)
{
    if (RSMainThread::Instance()->GetWatermarkFlag()) {
        sk_sp<SkImage> skImage = RSMainThread::Instance()->GetWatermarkImg();
#ifdef NEW_SKIA
        sk_sp<SkShader> shader = skImage->makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat, SkSamplingOptions());
#else
        sk_sp<SkShader> shader = skImage->makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat);
#endif
        SkPaint rectPaint;
        rectPaint.setShader(shader);
        canvas_->drawRect(SkRect::MakeWH(screenWidth, screenHeight), rectPaint);
    }
}
} // namespace Rosen
} // namespace OHOS
