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

#include "pipeline/rs_render_thread_visitor.h"

#include <cmath>
#ifndef USE_ROSEN_DRAWING
#include <include/core/SkColor.h>
#include <include/core/SkFont.h>
#include <include/core/SkMatrix.h>
#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>
#else
#include "draw/color.h"
#include "drawing/engine_adapter/impl_interface/matrix_impl.h"
#endif

#include "rs_trace.h"

#include "command/rs_base_node_command.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#ifdef NEW_RENDER_CONTEXT
#include "rs_render_surface.h"
#else
#include "platform/drawing/rs_surface.h"
#endif
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

#ifdef ROSEN_OHOS
#include <frame_collector.h>
#include <frame_painter.h>
#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "surface_utils.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif

namespace OHOS {
namespace Rosen {
RSRenderThreadVisitor::RSRenderThreadVisitor()
    : curDirtyManager_(std::make_shared<RSDirtyRegionManager>()), canvas_(nullptr) {}

RSRenderThreadVisitor::~RSRenderThreadVisitor() = default;

bool RSRenderThreadVisitor::IsValidRootRenderNode(RSRootRenderNode& node)
{
    auto ptr = RSNodeMap::Instance().GetNode<RSSurfaceNode>(node.GetRSSurfaceNodeId());
    if (ptr == nullptr) {
        ROSEN_LOGE("No valid RSSurfaceNode id");
        return false;
    }
    if (!node.enableRender_) {
        ROSEN_LOGD("RootNode %{public}s: Invisible", ptr->GetName().c_str());
        return false;
    }
    if (node.GetSuggestedBufferWidth() <= 0 || node.GetSuggestedBufferHeight() <= 0) {
        ROSEN_LOGD("Root %{public}s: Negative width or height [%{public}f %{public}f]", ptr->GetName().c_str(),
            node.GetSuggestedBufferWidth(), node.GetSuggestedBufferHeight());
        return false;
    }
    return true;
}

void RSRenderThreadVisitor::SetPartialRenderStatus(PartialRenderType status, bool isRenderForced)
{
    RS_TRACE_FUNC();
    isRenderForced_ = isRenderForced;
    dfxDirtyType_ = RSSystemProperties::GetDirtyRegionDebugType();
    isEglSetDamageRegion_ = !isRenderForced_ && (status != PartialRenderType::DISABLED);
    isOpDropped_ = (dfxDirtyType_ == DirtyRegionDebugType::DISABLED) && !isRenderForced_ &&
        (status == PartialRenderType::SET_DAMAGE_AND_DROP_OP);
    if (partialRenderStatus_ != status) {
        ROSEN_LOGD("PartialRenderStatus: %{public}d->%{public}d, isRenderForced_=%{public}d, dfxDirtyType_=%{public}d,\
            isEglSetDamageRegion_=%{public}d, isOpDropped_=%{public}d", partialRenderStatus_, status,
            isRenderForced_, dfxDirtyType_, isEglSetDamageRegion_, isOpDropped_);
    }
    partialRenderStatus_ = status;
}

void RSRenderThreadVisitor::PrepareChildren(RSRenderNode& node)
{
    for (auto& child : *node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSRenderThreadVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    if (isIdle_) {
        curDirtyManager_ = node.GetDirtyManager();
        curDirtyManager_->Clear();
        curDirtyManager_->UpdateDebugRegionTypeEnable(dfxDirtyType_);
        // After the node calls ApplyModifiers, the modifiers assign the renderProperties to the node
        // Otherwise node.GetSuggestedBufferHeight always less than 0, causing black screen
        if (!IsValidRootRenderNode(node)) {
            return;
        }
        dirtyFlag_ = false;
        isIdle_ = false;
        PrepareCanvasRenderNode(node);
        isIdle_ = true;
    } else {
        PrepareCanvasRenderNode(node);
    }
}

void RSRenderThreadVisitor::ResetAndPrepareChildrenNode(RSRenderNode& node,
    std::shared_ptr<RSBaseRenderNode> nodeParent)
{
    // merge last childRect as dirty if any child has been removed
    if (curDirtyManager_ && node.HasRemovedChild()) {
        curDirtyManager_->MergeDirtyRect(node.GetChildrenRect());
        node.ResetHasRemovedChild();
    }
    // reset childRect before prepare children
    node.ResetChildrenRect();
    node.UpdateChildrenOutOfRectFlag(false);
    PrepareChildren(node);
    // accumulate direct parent's childrenRect
    node.UpdateParentChildrenRect(nodeParent);
}

void RSRenderThreadVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.ShouldPaint() || curDirtyManager_ == nullptr) {
        return;
    }
    bool dirtyFlag = dirtyFlag_;
    auto nodeParent = node.GetParent().lock();
    dirtyFlag_ = node.Update(*curDirtyManager_, nodeParent, dirtyFlag_);
    if (node.IsDirtyRegionUpdated() && curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        curDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
            DirtyRegionType::UPDATE_DIRTY_REGION, node.GetOldDirty());
    }
    ResetAndPrepareChildrenNode(node, nodeParent);
    node.UpdateEffectRegion(effectRegion_);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (curDirtyManager_ == nullptr) {
        return;
    }
    node.ApplyModifiers();
    bool dirtyFlag = dirtyFlag_;
    auto nodeParent = node.GetParent().lock();
    // If rt buffer switches to be available
    // set its SurfaceRenderNode's render dirty
    if (!node.IsNotifyRTBufferAvailablePre() && node.IsNotifyRTBufferAvailable()) {
        ROSEN_LOGD("NotifyRTBufferAvailable and set it dirty");
        node.SetDirty();
    }
    dirtyFlag_ = node.Update(*curDirtyManager_, nodeParent, dirtyFlag_);
    if (node.IsDirtyRegionUpdated() && curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        curDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::SURFACE_NODE,
            DirtyRegionType::UPDATE_DIRTY_REGION, node.GetOldDirty());
    }
    ResetAndPrepareChildrenNode(node, nodeParent);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::PrepareEffectRenderNode(RSEffectRenderNode& node)
{
#ifndef CROSS_PLATFORM
    if (!node.ShouldPaint() || curDirtyManager_ == nullptr) {
        return;
    }
    auto effectRegion = effectRegion_;

    effectRegion_ = node.InitializeEffectRegion();
    bool dirtyFlag = dirtyFlag_;
    auto nodeParent = node.GetParent().lock();
    dirtyFlag_ = node.Update(*curDirtyManager_, nodeParent, dirtyFlag_);
    ResetAndPrepareChildrenNode(node, nodeParent);
    node.SetEffectRegion(effectRegion_);

    effectRegion_ = effectRegion;
    dirtyFlag_ = dirtyFlag;
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSRenderThreadVisitor::DrawRectOnCanvas(const RectI& dirtyRect, const SkColor color,
    const SkPaint::Style fillType, float alpha, int strokeWidth)
#else
void RSRenderThreadVisitor::DrawRectOnCanvas(const RectI& dirtyRect, const Drawing::ColorQuad color,
    RSPaintStyle fillType, float alpha, int strokeWidth)
#endif // USE_ROSEN_DRAWING
{
    if (dirtyRect.IsEmpty()) {
        ROSEN_LOGD("DrawRectOnCanvas dirty rect is invalid.");
        return;
    }
#ifndef USE_ROSEN_DRAWING
    auto skRect = SkRect::MakeXYWH(dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
    SkPaint rectPaint;
    rectPaint.setColor(color);
    rectPaint.setAntiAlias(true);
    rectPaint.setAlphaf(alpha);
    rectPaint.setStyle(fillType);
    rectPaint.setStrokeWidth(strokeWidth);
    if (fillType == SkPaint::kFill_Style) {
        rectPaint.setStrokeJoin(SkPaint::kRound_Join);
    }
    canvas_->drawRect(skRect, rectPaint);
#else
    auto drawingRect = Drawing::Rect(dirtyRect.left_, dirtyRect.top_,
        dirtyRect.width_ + dirtyRect.left_, dirtyRect.height_ + dirtyRect.top_);
    Drawing::Pen pen;
    Drawing::Brush brush;
    if (fillType == RSPaintStyle::STROKE) {
        pen.SetColor(color);
        pen.SetAntiAlias(true);
        pen.SetAlphaF(alpha);
        pen.SetWidth(strokeWidth);
        pen.SetJoinStyle(Drawing::Pen::JoinStyle::ROUND_JOIN);
        canvas_->AttachPen(pen);
    } else if (fillType == RSPaintStyle::FILL) {
        brush.SetColor(color);
        brush.SetAntiAlias(true);
        brush.SetAlphaF(alpha);
        canvas_->AttachBrush(brush);
    }
    canvas_->DrawRect(drawingRect);
    canvas_->DetachPen();
    canvas_->DetachBrush();
#endif // USE_ROSEN_DRAWING
}

void RSRenderThreadVisitor::DrawDirtyRegion()
{
    auto dirtyRect = RectI();
    const float fillAlpha = 0.2;
    const float edgeAlpha = 0.4;

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::MULTI_HISTORY)) {
        dirtyRect = curDirtyManager_->GetDirtyRegion();
        if (dirtyRect.IsEmpty()) {
            ROSEN_LOGD("DrawDirtyRegion his dirty rect is invalid. dirtyRect = %{public}s",
                dirtyRect.ToString().c_str());
        } else {
            ROSEN_LOGD("DrawDirtyRegion his dirty rect. dirtyRect = %{public}s", dirtyRect.ToString().c_str());
            // green
#ifndef USE_ROSEN_DRAWING
            DrawRectOnCanvas(dirtyRect, 0x442FDD2F, SkPaint::kFill_Style, fillAlpha);
            DrawRectOnCanvas(dirtyRect, 0xFF2FDD2F, SkPaint::kStroke_Style, edgeAlpha);
#else
            DrawRectOnCanvas(dirtyRect, 0x442FDD2F, RSPaintStyle::FILL, fillAlpha);
            DrawRectOnCanvas(dirtyRect, 0xFF2FDD2F, RSPaintStyle::STROKE, edgeAlpha);
#endif // USE_ROSEN_DRAWING
        }
    }

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_WHOLE)) {
        dirtyRect = curDirtyManager_->GetLatestDirtyRegion();
        if (dirtyRect.IsEmpty()) {
            ROSEN_LOGD("DrawDirtyRegion current frame's dirty rect is invalid. dirtyRect = %{public}s",
                dirtyRect.ToString().c_str());
        } else {
            ROSEN_LOGD("DrawDirtyRegion cur dirty rect. dirtyRect = %{public}s", dirtyRect.ToString().c_str());
            // yellow
#ifndef USE_ROSEN_DRAWING
            DrawRectOnCanvas(dirtyRect, 0x88FFFF00, SkPaint::kFill_Style, fillAlpha);
            DrawRectOnCanvas(dirtyRect, 0xFFFFFF00, SkPaint::kStroke_Style, edgeAlpha);
#else
            DrawRectOnCanvas(dirtyRect, 0x88FFFF00, RSPaintStyle::FILL, fillAlpha);
            DrawRectOnCanvas(dirtyRect, 0xFFFFFF00, RSPaintStyle::STROKE, edgeAlpha);
#endif // USE_ROSEN_DRAWING
        }
    }

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        const int strokeWidth = 4;
        std::map<NodeId, RectI> dirtyRegionRects_;
        curDirtyManager_->GetDirtyRegionInfo(dirtyRegionRects_, RSRenderNodeType::CANVAS_NODE,
            DirtyRegionType::UPDATE_DIRTY_REGION);
        ROSEN_LOGD("DrawDirtyRegion canvas dirtyRegionRects_ size %{public}zu", dirtyRegionRects_.size());
        // Draw Canvas Node
        for (const auto& [nid, subRect] : dirtyRegionRects_) {
            ROSEN_LOGD("DrawDirtyRegion canvas node id %{public}" PRIu64 " is dirty. dirtyRect = %{public}s",
                nid, subRect.ToString().c_str());
#ifndef USE_ROSEN_DRAWING
            DrawRectOnCanvas(subRect, 0x88FF0000, SkPaint::kStroke_Style, edgeAlpha, strokeWidth);
#else
            DrawRectOnCanvas(subRect, 0x88FF0000, RSPaintStyle::STROKE, edgeAlpha, strokeWidth);
#endif // USE_ROSEN_DRAWING
        }

        curDirtyManager_->GetDirtyRegionInfo(dirtyRegionRects_, RSRenderNodeType::SURFACE_NODE,
            DirtyRegionType::UPDATE_DIRTY_REGION);
        ROSEN_LOGD("DrawDirtyRegion surface dirtyRegionRects_ size %{public}zu", dirtyRegionRects_.size());
        // Draw Surface Node
        for (const auto& [nid, subRect] : dirtyRegionRects_) {
            ROSEN_LOGD("DrawDirtyRegion surface node id %{public}" PRIu64 " is dirty. dirtyRect = %{public}s",
                nid, subRect.ToString().c_str());
#ifndef USE_ROSEN_DRAWING
            DrawRectOnCanvas(subRect, 0xFFD864D8, SkPaint::kStroke_Style, edgeAlpha, strokeWidth);
#else
            DrawRectOnCanvas(subRect, 0xFFD864D8, RSPaintStyle::STROKE, edgeAlpha, strokeWidth);
#endif // USE_ROSEN_DRAWING
        }
    }
}

#ifdef NEW_RENDER_CONTEXT
void RSRenderThreadVisitor::UpdateDirtyAndSetEGLDamageRegion(std::shared_ptr<RSRenderSurface>& surface)
#else
void RSRenderThreadVisitor::UpdateDirtyAndSetEGLDamageRegion(std::unique_ptr<RSSurfaceFrame>& surfaceFrame)
#endif
{
    RS_TRACE_BEGIN("UpdateDirtyAndSetEGLDamageRegion");
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if (isEglSetDamageRegion_) {
        // get and update valid buffer age(>0) to merge history
#if defined(NEW_RENDER_CONTEXT)
        if (surface == nullptr) {
            ROSEN_LOGE("Failed to UpdateDirtyAndSetEGLDamageRegion, surface is nullptr");
            return;
        }
        int32_t bufferAge = surface->GetBufferAge();
#else
        int32_t bufferAge = surfaceFrame->GetBufferAge();
#endif
        if (!curDirtyManager_->SetBufferAge(bufferAge)) {
            ROSEN_LOGD("ProcessRootRenderNode SetBufferAge with invalid buffer age %{public}d", bufferAge);
            curDirtyManager_->ResetDirtyAsSurfaceSize();
        }
        curDirtyManager_->UpdateDirtyByAligned();
        curDirtyManager_->UpdateDirty();
        curDirtyRegion_ = curDirtyManager_->GetDirtyRegion();
        // only set damage region if dirty region and buffer age is valid(>0)
        if (bufferAge >= 0) {
            // get dirty rect coordinated from upper left to lower left corner in current surface
            RectI dirtyRectFlip = curDirtyManager_->GetRectFlipWithinSurface(curDirtyRegion_);
            // set dirty rect as eglSurfaceFrame's damage region
#if defined(NEW_RENDER_CONTEXT)
            std::vector<RectI> dirtyRects;
            dirtyRects.push_back(dirtyRectFlip);
            surface->SetDamageRegion(dirtyRects);
#else
            surfaceFrame->SetDamageRegion(dirtyRectFlip.left_, dirtyRectFlip.top_, dirtyRectFlip.width_,
                dirtyRectFlip.height_);
#endif
            // flip aligned rect for op drops
            curDirtyRegion_ = curDirtyManager_->GetRectFlipWithinSurface(dirtyRectFlip);
            ROSEN_LOGD("GetPartialRenderEnabled buffer age %{public}d, dirtyRectFlip ="
                "[%{public}d, %{public}d, %{public}d, %{public}d], "
                "dirtyRectAlign = [%{public}d, %{public}d, %{public}d, %{public}d]", bufferAge,
                dirtyRectFlip.left_, dirtyRectFlip.top_, dirtyRectFlip.width_, dirtyRectFlip.height_,
                curDirtyRegion_.left_, curDirtyRegion_.top_, curDirtyRegion_.width_, curDirtyRegion_.height_);
        }
    } else {
        curDirtyManager_->UpdateDirty();
        curDirtyRegion_ = curDirtyManager_->GetDirtyRegion();
    }
#else
    curDirtyManager_->UpdateDirty();
    curDirtyRegion_ = curDirtyManager_->GetDirtyRegion();
#endif
    ROSEN_LOGD("UpdateDirtyAndSetEGLDamageRegion dirtyRect = [%{public}d, %{public}d, %{public}d, %{public}d]",
        curDirtyRegion_.left_, curDirtyRegion_.top_, curDirtyRegion_.width_, curDirtyRegion_.height_);
    RS_TRACE_END();
}

void RSRenderThreadVisitor::ProcessShadowFirst(RSRenderNode& node)
{
    if (RSSystemProperties::GetUseShadowBatchingEnabled()
        && (node.GetRenderProperties().GetUseShadowBatching())) {
        auto children = node.GetSortedChildren();
        for (auto& child : *children) {
            if (auto node = child->ReinterpretCastTo<RSCanvasRenderNode>()) {
                node->ProcessShadowBatching(*canvas_);
            }
        }
    }
}

void RSRenderThreadVisitor::ProcessChildren(RSRenderNode& node)
{
    ProcessShadowFirst(node);
    for (auto& child : *node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSRenderThreadVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!isIdle_) {
        ProcessCanvasRenderNode(node);
        return;
    }
    auto ptr = RSNodeMap::Instance().GetNode<RSSurfaceNode>(node.GetRSSurfaceNodeId());
    if (!IsValidRootRenderNode(node)) {
        return;
    }
    drawCmdListVector_.clear();
    curDirtyManager_ = node.GetDirtyManager();

    auto surfaceNodeColorSpace = ptr->GetColorSpace();
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(ptr);
#else
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(ptr);
#endif
    if (rsSurface == nullptr) {
        ROSEN_LOGE("ProcessRoot %{public}s: No RSSurface found", ptr->GetName().c_str());
        return;
    }
    // Update queue size for each process loop in case it dynamically changes
    queueSize_ = rsSurface->GetQueueSize();

    auto rsSurfaceColorSpace = rsSurface->GetColorSpace();
    if (surfaceNodeColorSpace != rsSurfaceColorSpace) {
        ROSEN_LOGD("Set new colorspace %{public}d to rsSurface", surfaceNodeColorSpace);
        rsSurface->SetColorSpace(surfaceNodeColorSpace);
    }

#if defined(ACE_ENABLE_GL) || defined (ACE_ENABLE_VK)
#if defined(NEW_RENDER_CONTEXT)
    std::shared_ptr<RenderContextBase> rc = RSRenderThread::Instance().GetRenderContext();
    std::shared_ptr<DrawingContext> dc = RSRenderThread::Instance().GetDrawingContext();
    rsSurface->SetDrawingContext(dc);
#else
    RenderContext* rc = RSRenderThread::Instance().GetRenderContext();
#endif
    rsSurface->SetRenderContext(rc);
#endif

#ifdef ACE_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        auto skContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
        if (skContext == nullptr) {
            ROSEN_LOGE("RSRenderThreadVisitor::ProcessRootRenderNode CreateDrawingContext is null");
            return;
        }
        std::static_pointer_cast<RSSurfaceOhosVulkan>(rsSurface)->SetSkContext(skContext);
    }
#endif
    uiTimestamp_ = RSRenderThread::Instance().GetUITimestamp();
    RS_TRACE_BEGIN(ptr->GetName() + " rsSurface->RequestFrame");
#ifdef ROSEN_OHOS
    FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::ReleaseStart);
#endif

    const auto& property = node.GetRenderProperties();
    const float bufferWidth = node.GetSuggestedBufferWidth() * property.GetScaleX();
    const float bufferHeight = node.GetSuggestedBufferHeight() * property.GetScaleY();
#ifdef ROSEN_OHOS
    auto surfaceFrame = rsSurface->RequestFrame(bufferWidth, bufferHeight, uiTimestamp_);
#else
    auto surfaceFrame = rsSurface->RequestFrame(std::round(bufferWidth), std::round(bufferHeight), uiTimestamp_);
#endif
    RS_TRACE_END();
    if (surfaceFrame == nullptr) {
        ROSEN_LOGI("ProcessRoot %{public}s: Request Frame Failed", ptr->GetName().c_str());
#ifdef ROSEN_OHOS
        FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::ReleaseEnd);
#endif
        return;
    }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_RENDER_CONTEXT
    auto skSurface = rsSurface->GetSurface();
#else
    auto skSurface = surfaceFrame->GetSurface();
#endif
    if (skSurface == nullptr) {
        ROSEN_LOGE("skSurface null.");
        return;
    }
    if (skSurface->getCanvas() == nullptr) {
        ROSEN_LOGE("skSurface.getCanvas is null.");
        return;
    }
#else
    auto surface = surfaceFrame->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("surface null.");
        return;
    }
    if (surface->GetCanvas() == nullptr) {
        ROSEN_LOGE("surface.GetCanvas is null.");
        return;
    }
#endif

#ifdef ROSEN_OHOS
    // if listenedCanvas is nullptr, that means disabled or listen failed
    std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
    std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;

    if (RSOverdrawController::GetInstance().IsEnabled()) {
        auto &oc = RSOverdrawController::GetInstance();
#ifndef USE_ROSEN_DRAWING
        listenedCanvas = std::make_shared<RSListenedCanvas>(skSurface.get());
#else
        listenedCanvas = std::make_shared<RSListenedCanvas>(*surface);
#endif
        overdrawListener = oc.CreateListener<RSCPUOverdrawCanvasListener>(listenedCanvas.get());
        if (overdrawListener == nullptr) {
            overdrawListener = oc.CreateListener<RSGPUOverdrawCanvasListener>(listenedCanvas.get());
        }

        if (overdrawListener != nullptr) {
            listenedCanvas->SetListener(overdrawListener);
        } else {
            // create listener failed
            listenedCanvas = nullptr;
        }
    }

    if (listenedCanvas != nullptr) {
        canvas_ = listenedCanvas;
    } else {
#ifndef USE_ROSEN_DRAWING
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
#else
        canvas_ = std::make_shared<RSPaintFilterCanvas>(surface.get());
#endif // USE_ROSEN_DRAWING
    }
#else
#ifndef USE_ROSEN_DRAWING
    canvas_ = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
#else
    canvas_ = std::make_shared<RSPaintFilterCanvas>(surface.get());
#endif // USE_ROSEN_DRAWING
#endif

    canvas_->SetHighContrast(RSRenderThread::Instance().isHighContrastEnabled());

    // node's surface size already check, so here we do not need to check return
    // attention: currently surfaceW/H are float values transformed into int implicitly
    (void)curDirtyManager_->SetSurfaceSize(bufferWidth, bufferHeight);
    // keep non-negative rect region within surface
    curDirtyManager_->ClipDirtyRectWithinSurface();
    // reset matrix
    const float rootWidth = property.GetFrameWidth() * property.GetScaleX();
    const float rootHeight = property.GetFrameHeight() * property.GetScaleY();
#ifndef USE_ROSEN_DRAWING
    SkMatrix gravityMatrix;
#else
    Drawing::Matrix gravityMatrix;
#endif // USE_ROSEN_DRAWING
    (void)RSPropertiesPainter::GetGravityMatrix(
        Gravity::RESIZE, RectF { 0.0f, 0.0f, bufferWidth, bufferHeight }, rootWidth, rootHeight, gravityMatrix);

    if (isRenderForced_ ||
        curDirtyManager_->GetCurrentFrameDirtyRegion().GetWidth() == 0 ||
        curDirtyManager_->GetCurrentFrameDirtyRegion().GetHeight() == 0 ||
#ifndef USE_ROSEN_DRAWING
        !gravityMatrix.isIdentity()) {
#else
        !(gravityMatrix == Drawing::Matrix())) {
#endif // USE_ROSEN_DRAWING
        curDirtyManager_->ResetDirtyAsSurfaceSize();
    }
#ifdef NEW_RENDER_CONTEXT
    UpdateDirtyAndSetEGLDamageRegion(rsSurface);
#else
    UpdateDirtyAndSetEGLDamageRegion(surfaceFrame);
#endif

#ifndef USE_ROSEN_DRAWING
    canvas_->clipRect(SkRect::MakeWH(bufferWidth, bufferHeight));
    canvas_->clear(SK_ColorTRANSPARENT);
#else
    canvas_->ClipRect(Drawing::Rect(0, 0, bufferWidth, bufferHeight), Drawing::ClipOp::INTERSECT, false);
    canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
#endif // USE_ROSEN_DRAWING
    isIdle_ = false;

    // clear current children before traversal, we will re-add them again during traversal
    childSurfaceNodeIds_.clear();

#ifndef USE_ROSEN_DRAWING
    canvas_->concat(gravityMatrix);
#else
    canvas_->ConcatMatrix(gravityMatrix);
#endif // USE_ROSEN_DRAWING
    parentSurfaceNodeMatrix_ = gravityMatrix;

    RS_TRACE_BEGIN("ProcessRenderNodes");
    ProcessCanvasRenderNode(node);

    if (childSurfaceNodeIds_ != node.childSurfaceNodeIds_) {
        auto thisSurfaceNodeId = node.GetRSSurfaceNodeId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(thisSurfaceNodeId);
        SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_TO_SELF);
        for (const auto& childSurfaceNodeId : childSurfaceNodeIds_) {
            command = std::make_unique<RSBaseNodeAddChild>(thisSurfaceNodeId, childSurfaceNodeId, -1);
            SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_TO_SELF);
        }
        node.childSurfaceNodeIds_ = std::move(childSurfaceNodeIds_);
    }
    RS_TRACE_END();

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        ROSEN_LOGD("RSRenderThreadVisitor FlushImplicitTransactionFromRT uiTimestamp = %{public}" PRIu64,
            uiTimestamp_);
        transactionProxy->FlushImplicitTransactionFromRT(uiTimestamp_);
    }

    if ((dfxDirtyType_ != DirtyRegionDebugType::DISABLED) && curDirtyManager_->IsDirty()) {
        ROSEN_LOGD("ProcessRootRenderNode %{public}s [%{public}" PRIu64 "] draw dirtyRect",
            ptr->GetName().c_str(), node.GetId());
        DrawDirtyRegion();
    }

#ifdef ROSEN_OHOS
    if (overdrawListener != nullptr) {
        overdrawListener->Draw();
    }

    FramePainter fpainter(FrameCollector::GetInstance());
    fpainter.Draw(*canvas_);
    FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::ReleaseEnd);
    FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::FlushStart);
#endif

    RS_TRACE_BEGIN(ptr->GetName() + " rsSurface->FlushFrame");
    ROSEN_LOGD("RSRenderThreadVisitor FlushFrame surfaceNodeId = %{public}" PRIu64 ", uiTimestamp = %{public}" PRIu64,
        node.GetRSSurfaceNodeId(), uiTimestamp_);
#ifdef NEW_RENDER_CONTEXT
    rsSurface->FlushFrame(uiTimestamp_);
#else
    rsSurface->FlushFrame(surfaceFrame, uiTimestamp_);
#endif
#ifdef ROSEN_OHOS
    FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::FlushEnd);
#endif
    RS_TRACE_END();

    canvas_ = nullptr;
    isIdle_ = true;
}

void RSRenderThreadVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.ShouldPaint()) {
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    node.UpdateRenderStatus(curDirtyRegion_, isOpDropped_);
    if (node.IsRenderUpdateIgnored()) {
        return;
    }
#endif
    const auto& property = node.GetRenderProperties();
    if (property.IsSpherizeValid()) {
        if (node.GetCacheType() != CacheType::ANIMATE_PROPERTY) {
            node.SetCacheType(CacheType::ANIMATE_PROPERTY);
            node.ClearCacheSurface();
        }
        if (!node.GetCompletedCacheSurface() && UpdateAnimatePropertyCacheSurface(node)) {
            node.UpdateCompletedCacheSurface();
        }
        node.ProcessTransitionBeforeChildren(*canvas_);
        RSPropertiesPainter::DrawSpherize(
            property, *canvas_, node.GetCompletedCacheSurface());
        node.ProcessTransitionAfterChildren(*canvas_);
        return;
    }

    if (node.GetCompletedCacheSurface()) {
        node.SetCacheType(CacheType::NONE);
        node.ClearCacheSurface();
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    node.ProcessRenderContents(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

bool RSRenderThreadVisitor::UpdateAnimatePropertyCacheSurface(RSRenderNode& node)
{
    if (!node.GetCacheSurface()) {
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
        node.InitCacheSurface(canvas_ ? canvas_->recordingContext() : nullptr);
#else
        node.InitCacheSurface(canvas_ ? canvas_->getGrContext() : nullptr);
#endif
#else
        node.InitCacheSurface(canvas_ ? canvas_->GetGPUContext().get() : nullptr);
#endif
    }
    if (!node.GetCacheSurface()) {
        return false;
    }
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(node.GetCacheSurface().get());
    if (!cacheCanvas) {
        return false;
    }

    // copy current canvas properties into cacheCanvas
    cacheCanvas->CopyConfiguration(*canvas_);

    // When drawing CacheSurface, all child node should be drawn.
    // So set isOpDropped_ = false here.
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;

    swap(cacheCanvas, canvas_);
    node.ProcessAnimatePropertyBeforeChildren(*canvas_);
    node.ProcessRenderContents(*canvas_);
    ProcessChildren(node);
    node.ProcessAnimatePropertyAfterChildren(*canvas_);
    swap(cacheCanvas, canvas_);

    isOpDropped_ = isOpDropped;
    return true;
}

void RSRenderThreadVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGD("RSRenderThreadVisitor::ProcessEffectRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSRenderThreadVisitor::ProcessEffectRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

int RSRenderThreadVisitor::CacRotationFromTransformType(GraphicTransformType transform)
{
    GraphicTransformType rotation;
    switch (transform) {
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT90:
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT90:
            rotation = GraphicTransformType::GRAPHIC_ROTATE_90;
            break;
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT180:
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT180:
            rotation = GraphicTransformType::GRAPHIC_ROTATE_180;
            break;
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT270:
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT270:
            rotation = GraphicTransformType::GRAPHIC_ROTATE_270;
            break;
        default:
            rotation = transform;
            break;
    }
    static const std::map<GraphicTransformType, int> transformTypeEnumToIntMap = {
        {GraphicTransformType::GRAPHIC_ROTATE_NONE, 0}, {GraphicTransformType::GRAPHIC_ROTATE_90, 270},
        {GraphicTransformType::GRAPHIC_ROTATE_180, 180}, {GraphicTransformType::GRAPHIC_ROTATE_270, 90}};
    auto iter = transformTypeEnumToIntMap.find(rotation);
    return iter != transformTypeEnumToIntMap.end() ? iter->second : 0;
}

void RSRenderThreadVisitor::ProcessSurfaceViewInRT(RSSurfaceRenderNode& node)
{
#ifdef ROSEN_OHOS
    const auto& property = node.GetRenderProperties();
    sptr<Surface> surface = SurfaceUtils::GetInstance()->GetSurface(node.GetSurfaceId());
    if (surface == nullptr) {
        RS_LOGE("RSRenderThreadVisitor::ProcessSurfaceViewInRT nodeId is %llu cannot find surface by surfaceId %llu",
            node.GetId(), node.GetSurfaceId());
        return;
    }
    sptr<SurfaceBuffer> surfaceBuffer;
    // a 4 * 4 idetity matrix
    float matrix[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    sptr<SyncFence> fence;
    int ret = surface->GetLastFlushedBuffer(surfaceBuffer, fence, matrix);
    if (ret != OHOS::GSERROR_OK || surfaceBuffer == nullptr) {
        RS_LOGE("RSRenderThreadVisitor::ProcessSurfaceViewInRT: GetLastFlushedBuffer failed, err: %{public}d", ret);
        return;
    }
    if (fence != nullptr) {
        fence->Wait(3000); // wait at most 3000ms
    }
    Drawing::Matrix transfromMatrix;
    auto transform = surface->GetTransform();
    int rotation = CacRotationFromTransformType(transform);
    transfromMatrix.PreRotate(rotation, property.GetBoundsWidth() * 0.5f, property.GetBoundsHeight() * 0.5f);
    canvas_->ConcatMatrix(transfromMatrix);
#ifndef USE_ROSEN_DRAWING
    auto recordingCanvas = std::make_shared<RSRecordingCanvas>(property.GetBoundsWidth(), property.GetBoundsHeight());
    RSSurfaceBufferInfo rsSurfaceBufferInfo(surfaceBuffer, property.GetBoundsPositionX(), property.GetBoundsPositionY(),
        property.GetBoundsWidth(), property.GetBoundsHeight());
#else
    auto recordingCanvas = std::make_shared<ExtendRecordingCanvas>(property.GetBoundsWidth(),
        property.GetBoundsHeight());
    DrawingSurfaceBufferInfo rsSurfaceBufferInfo(surfaceBuffer, property.GetBoundsPositionX(),
        property.GetBoundsPositionY(), property.GetBoundsWidth(), property.GetBoundsHeight());
#endif //USE_ROSEN_DRAWING
    recordingCanvas->DrawSurfaceBuffer(rsSurfaceBufferInfo);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    drawCmdList->Playback(*canvas_);
    drawCmdListVector_.emplace_back(drawCmdList);
#endif
}

void RSRenderThreadVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    if (!node.ShouldPaint()) {
        ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode node : %{public}" PRIu64 " is invisible",
            node.GetId());
        node.SetContextAlpha(0.0f);
        return;
    }
    // RSSurfaceRenderNode in RSRenderThreadVisitor do not have information of property.
    // We only get parent's matrix and send it to RenderService
#ifndef USE_ROSEN_DRAWING
    SkMatrix invertMatrix;
    SkMatrix contextMatrix = canvas_->getTotalMatrix();

    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(invertMatrix);
#else
    Drawing::Matrix invertMatrix;
    Drawing::Matrix contextMatrix = canvas_->GetTotalMatrix();

    if (parentSurfaceNodeMatrix_.Invert(invertMatrix)) {
        contextMatrix.PreConcat(invertMatrix);
#endif // USE_ROSEN_DRAWING
    } else {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessSurfaceRenderNode, invertMatrix failed");
    }
    node.SetContextMatrix(contextMatrix);
    node.SetContextAlpha(canvas_->GetAlpha());

    // PLANNING: This is a temporary modification. Animation for surfaceView should not be triggered in RenderService.
    // We plan to refactor code here.
    node.SetContextBounds(node.GetRenderProperties().GetBounds());
#ifdef USE_SURFACE_TEXTURE
    if (node.GetIsTextureExportNode()) {
        ProcessSurfaceViewInRT(node);
    } else if (node.GetSurfaceNodeType() == RSSurfaceNodeType::SURFACE_TEXTURE_NODE) {
        ProcessTextureSurfaceRenderNode(node);
    } else {
        ProcessOtherSurfaceRenderNode(node);
    }
#else
    if (node.GetIsTextureExportNode()) {
        ProcessSurfaceViewInRT(node);
    } else {
        ProcessOtherSurfaceRenderNode(node);
    }
#endif

    // 1. add this node to parent's children list
    childSurfaceNodeIds_.emplace_back(node.GetId());

    // 2. backup and reset environment variables before traversal children
    std::vector<NodeId> siblingSurfaceNodeIds(std::move(childSurfaceNodeIds_));
    childSurfaceNodeIds_.clear();
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
#ifndef USE_ROSEN_DRAWING
    parentSurfaceNodeMatrix_ = canvas_->getTotalMatrix();
#else
    parentSurfaceNodeMatrix_ = canvas_->GetTotalMatrix();
#endif // USE_ROSEN_DRAWING

    // 3. traversal children, child surface node will be added to childSurfaceNodeIds_
    // note: apply current node properties onto canvas if there is any child node
    ProcessChildren(node);

    // 4. if children changed, sync children to RenderService
    if (childSurfaceNodeIds_ != node.childSurfaceNodeIds_) {
        auto thisSurfaceNodeId = node.GetId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(thisSurfaceNodeId);
        SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_TO_SELF);
        for (const auto& childSurfaceNodeId : childSurfaceNodeIds_) {
            command = std::make_unique<RSBaseNodeAddChild>(thisSurfaceNodeId, childSurfaceNodeId, -1);
            SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_TO_SELF);
        }
        node.childSurfaceNodeIds_ = std::move(childSurfaceNodeIds_);
    }

    // 5. restore environments variables before continue traversal siblings
    childSurfaceNodeIds_ = std::move(siblingSurfaceNodeIds);
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;

    // 6.draw border
#ifndef USE_ROSEN_DRAWING
    canvas_->save();
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    canvas_->concat(geoPtr->GetMatrix());
    RSPropertiesPainter::DrawOutline(node.GetRenderProperties(), *canvas_);
    RSPropertiesPainter::DrawBorder(node.GetRenderProperties(), *canvas_);
    canvas_->restore();
#else
    canvas_->Save();
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    canvas_->ConcatMatrix(geoPtr->GetMatrix());
    RSPropertiesPainter::DrawOutline(node.GetRenderProperties(), *canvas_);
    RSPropertiesPainter::DrawBorder(node.GetRenderProperties(), *canvas_);
    canvas_->Restore();
#endif
}

void RSRenderThreadVisitor::ProcessProxyRenderNode(RSProxyRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessProxyRenderNode, canvas is nullptr");
        return;
    }
    // RSProxyRenderNode in RSRenderThreadVisitor do not have information of property.
    // We only get parent's matrix and send it to RenderService
#ifdef ROSEN_OHOS
#ifndef USE_ROSEN_DRAWING
    SkMatrix invertMatrix;
    SkMatrix contextMatrix = canvas_->getTotalMatrix();
    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessProxyRenderNode, invertMatrix failed");
    }
#else
    Drawing::Matrix invertMatrix;
    Drawing::Matrix contextMatrix = canvas_->GetTotalMatrix();

    if (parentSurfaceNodeMatrix_.Invert(invertMatrix)) {
        contextMatrix.PreConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessProxyRenderNode, invertMatrix failed");
    }
#endif // USE_ROSEN_DRAWING
    node.SetContextMatrix(contextMatrix);
    node.SetContextAlpha(canvas_->GetAlpha());

    // context clipRect should be based on parent node's coordinate system, in dividend render mode, it is the
    // same as canvas coordinate system.
    auto clipRect = RSPaintFilterCanvas::GetLocalClipBounds(*canvas_);
    node.SetContextClipRegion(clipRect);
#endif
}

void RSRenderThreadVisitor::AddSurfaceChangedCallBack(uint64_t id,
    const std::function<void(float, float, float, float)>& callback)
{
    surfaceCallbacks_.emplace(id, callback);
}

void RSRenderThreadVisitor::RemoveSurfaceChangedCallBack(uint64_t id)
{
    surfaceCallbacks_.erase(id);
}

void RSRenderThreadVisitor::ClipHoleForSurfaceNode(RSSurfaceRenderNode& node)
{
    // Calculation position in RenderService may appear floating point number, and it will be removed.
    // It caused missed line problem on surfaceview hap, so we subtract one pixel when cliphole to avoid this problem
    static int pixel = 1;
    auto x = std::ceil(node.GetRenderProperties().GetBoundsPositionX() + pixel); // x increase 1 pixel
    auto y = std::ceil(node.GetRenderProperties().GetBoundsPositionY() + pixel); // y increase 1 pixel
    auto width = std::floor(node.GetRenderProperties().GetBoundsWidth() - (2 * pixel)); // width decrease 2 pixels
    auto height = std::floor(node.GetRenderProperties().GetBoundsHeight() - (2 * pixel)); // height decrease 2 pixels
#ifndef USE_ROSEN_DRAWING
    canvas_->save();
    SkRect originRect = SkRect::MakeXYWH(x, y, width, height);
    canvas_->clipRect(originRect);

    auto iter = surfaceCallbacks_.find(node.GetId());
    if (iter != surfaceCallbacks_.end()) {
        (iter->second)(canvas_->getTotalMatrix().getTranslateX(), canvas_->getTotalMatrix().getTranslateY(), width, height);
    }

    if (node.IsNotifyRTBufferAvailable() && !node.GetIsForeground()) {
        ROSEN_LOGD("RSRenderThreadVisitor::ClipHoleForSurfaceNode node : %{public}" PRIu64 ","
            " clip [%{public}f, %{public}f, %{public}f, %{public}f]", node.GetId(), x, y, width, height);
        canvas_->clear(SK_ColorTRANSPARENT);
    } else {
        ROSEN_LOGD("RSRenderThreadVisitor::ClipHoleForSurfaceNode node : %{public}" PRIu64 ","
            " not clip [%{public}f, %{public}f, %{public}f, %{public}f]",
            node.GetId(), x, y, width, height);
        auto backgroundColor = node.GetRenderProperties().GetBackgroundColor();
        if (backgroundColor != RgbPalette::Transparent()) {
            canvas_->clear(backgroundColor.AsArgbInt());
        }
    }
    canvas_->restore();
#else
    canvas_->Save();
    Drawing::Rect originRect = Drawing::Rect(x, y, width + x, height + y);
    canvas_->ClipRect(originRect, Drawing::ClipOp::INTERSECT, false);
    auto iter = surfaceCallbacks_.find(node.GetId());
    if (iter != surfaceCallbacks_.end()) {
        (iter->second)(canvas_->GetTotalMatrix().Get(Drawing::Matrix::TRANS_X),
            canvas_->GetTotalMatrix().Get(Drawing::Matrix::TRANS_Y), width, height);
    }
    if (node.IsNotifyRTBufferAvailable() == true) {
        ROSEN_LOGI("RSRenderThreadVisitor::ClipHoleForSurfaceNode node : %{public}" PRIu64 ","
            "clip [%{public}f, %{public}f, %{public}f, %{public}f]", node.GetId(), x, y, width, height);
        canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    } else {
        ROSEN_LOGI("RSRenderThreadVisitor::ClipHoleForSurfaceNode node : %{public}" PRIu64 ","
            "not clip [%{public}f, %{public}f, %{public}f, %{public}f]", node.GetId(), x, y, width, height);
        auto backgroundColor = node.GetRenderProperties().GetBackgroundColor();
        if (backgroundColor != RgbPalette::Transparent()) {
            canvas_->Clear(backgroundColor.AsArgbInt());
        }
    }
    canvas_->Restore();
#endif // USE_ROSEN_DRAWING
}

void RSRenderThreadVisitor::SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommandFromRT(command, nodeId, followType);
    }
}

#ifdef USE_SURFACE_TEXTURE
void RSRenderThreadVisitor::ProcessTextureSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    auto texture = node.GetSurfaceTexture();
    if (texture == nullptr) {
        ROSEN_LOGE("ProcessTextureSurfaceRenderNode %{public}" PRIu64, node.GetId());
        return;
    }
    auto clipRect = RSPaintFilterCanvas::GetLocalClipBounds(*canvas_);
#ifndef USE_ROSEN_DRAWING
    if (!clipRect.has_value() ||
        clipRect->width() < std::numeric_limits<float>::epsilon() ||
        clipRect->height() < std::numeric_limits<float>::epsilon()) {
#else
    if (!clipRect.has_value() ||
        clipRect->GetWidth() < std::numeric_limits<float>::epsilon() ||
        clipRect->GetHeight() < std::numeric_limits<float>::epsilon()) {
#endif
        // if clipRect is empty, this node will be removed from parent's children list.
        return;
    }

    // Calculation position in RenderService may appear floating point number, and it will be removed.
    // It caused missed line problem on surfaceview hap, so we subtract one pixel when cliphole to avoid this problem
    static int pixel = 1;
    auto x = std::ceil(node.GetRenderProperties().GetBoundsPositionX() + pixel); // x increase 1 pixel
    auto y = std::ceil(node.GetRenderProperties().GetBoundsPositionY() + pixel); // y increase 1 pixel
    auto width = std::floor(node.GetRenderProperties().GetBoundsWidth() - (2 * pixel)); // width decrease 2 pixels
    auto height = std::floor(node.GetRenderProperties().GetBoundsHeight() - (2 * pixel)); // height decrease 2 pixels

#ifndef USE_ROSEN_DRAWING
    canvas_->save();
    SkRect originRect = SkRect::MakeXYWH(x, y, width, height);
#else
    canvas_->Save();
    Drawing::Rect originRect = Drawing::Rect(x, y, width + x, height + y);
#endif

    if (node.IsNotifyRTBufferAvailable()) {
        texture->DrawTextureImage(*canvas_, false, originRect);
    } else {
        auto backgroundColor = node.GetRenderProperties().GetBackgroundColor();
        if (backgroundColor != RgbPalette::Transparent()) {
#ifndef USE_ROSEN_DRAWING
            canvas_->clear(backgroundColor.AsArgbInt());
#else
            canvas_->Clear(backgroundColor.AsArgbInt());
#endif
        }
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->restore();
#else
    canvas_->Restore();
#endif
}
#endif

void RSRenderThreadVisitor::ProcessOtherSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    auto clipRect = RSPaintFilterCanvas::GetLocalClipBounds(*canvas_);
#ifndef USE_ROSEN_DRAWING
    if (!clipRect.has_value() ||
        clipRect->width() < std::numeric_limits<float>::epsilon() ||
        clipRect->height() < std::numeric_limits<float>::epsilon()) {
#else
    if (!clipRect.has_value() ||
        clipRect->GetWidth() < std::numeric_limits<float>::epsilon() ||
        clipRect->GetHeight() < std::numeric_limits<float>::epsilon()) {
#endif
        // if clipRect is empty, this node will be removed from parent's children list.
        node.SetContextClipRegion(std::nullopt);

        static int pixel = 1;
        auto width = std::floor(node.GetRenderProperties().GetBoundsWidth() - (2 * pixel)); // width decrease 2 pixels
        auto height = std::floor(node.GetRenderProperties().GetBoundsHeight() - (2 * pixel)); // height decrease 2 pixels
        auto iter = surfaceCallbacks_.find(node.GetId());
        if (iter != surfaceCallbacks_.end()) {
#ifndef USE_ROSEN_DRAWING
            (iter->second)(canvas_->getTotalMatrix().getTranslateX(), canvas_->getTotalMatrix().getTranslateY(), width, height);
#else
            (iter->second)(canvas_->GetTotalMatrix().Get(Drawing::Matrix::TRANS_X),
                canvas_->GetTotalMatrix().Get(Drawing::Matrix::TRANS_Y), width, height);
#endif
        }
        return;
    }
    node.SetContextClipRegion(clipRect);
    // temporary workaround since ContextAlpha/ContextClipRegion happens after ApplyModifiers
    node.ApplyModifiers();

    // clip hole
    ClipHoleForSurfaceNode(node);
}
} // namespace Rosen
} // namespace OHOS
