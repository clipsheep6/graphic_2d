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

#include "pipeline/rs_surface_render_node.h"

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "rs_trace.h"
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#endif

#include "command/rs_surface_node_command.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSSurfaceRenderNode::RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context)
    : RSRenderNode(config.id, context),
      RSSurfaceHandler(config.id),
      name_(config.name),
      nodeType_(config.nodeType),
      dirtyManager_(std::make_shared<RSDirtyRegionManager>()),
      cacheSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    MemoryInfo info = {sizeof(*this), ExtractPid(config.id), config.id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(config.id, info);
}

RSSurfaceRenderNode::RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSSurfaceRenderNode(RSSurfaceRenderNodeConfig{id, "SurfaceNode"}, context)
{}

RSSurfaceRenderNode::~RSSurfaceRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::SetConsumer(const sptr<IConsumerSurface>& consumer)
{
    consumer_ = consumer;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSSurfaceRenderNode::UpdateSrcRect(const RSPaintFilterCanvas& canvas, const SkIRect& dstRect)
{
    auto localClipRect = RSPaintFilterCanvas::GetLocalClipBounds(canvas, &dstRect).value_or(SkRect::MakeEmpty());
    const RSProperties& properties = GetRenderProperties();
    int left = std::clamp<int>(localClipRect.left(), 0, properties.GetBoundsWidth());
    int top = std::clamp<int>(localClipRect.top(), 0, properties.GetBoundsHeight());
    int width = std::clamp<int>(localClipRect.width(), 0, properties.GetBoundsWidth() - left);
    int height = std::clamp<int>(localClipRect.height(), 0, properties.GetBoundsHeight() - top);
    RectI srcRect = {left, top, width, height};
    SetSrcRect(srcRect);
}
#else
void RSSurfaceRenderNode::UpdateSrcRect(const RSPaintFilterCanvas& canvas, const Drawing::RectI& dstRect)
{
    auto localClipRect = RSPaintFilterCanvas::GetLocalClipBounds(canvas, &dstRect).value_or(Drawing::Rect());
    const RSProperties& properties = GetRenderProperties();
    int left = std::clamp<int>(localClipRect.GetLeft(), 0, properties.GetBoundsWidth());
    int top = std::clamp<int>(localClipRect.GetTop(), 0, properties.GetBoundsHeight());
    int width = std::clamp<int>(localClipRect.GetWidth(), 0, properties.GetBoundsWidth() - left);
    int height = std::clamp<int>(localClipRect.GetHeight(), 0, properties.GetBoundsHeight() - top);
    RectI srcRect = {left, top, width, height};
    SetSrcRect(srcRect);
}
#endif

bool RSSurfaceRenderNode::ShouldPrepareSubnodes()
{
    // if a appwindow or abilitycomponent has a empty dstrect, its subnodes' prepare stage can be skipped
    // most common scenario: HiBoard (SwipeLeft screen on home screen)
    if (GetDstRect().IsEmpty() && (IsAppWindow() || IsAbilityComponent())) {
        return false;
    }
    return true;
}

std::string RSSurfaceRenderNode::DirtyRegionDump() const
{
    std::string dump = GetName() +
        " SurfaceNodeType [" + std::to_string(static_cast<unsigned int>(GetSurfaceNodeType())) + "]" +
        " Transparent [" + std::to_string(IsTransparent()) +"]" +
        " DstRect: " + GetDstRect().ToString() +
        " VisibleRegion: " + GetVisibleRegion().GetRegionInfo() +
        " VisibleDirtyRegion: " + GetVisibleDirtyRegion().GetRegionInfo() +
        " GlobalDirtyRegion: " + GetGlobalDirtyRegion().GetRegionInfo();
    if (GetDirtyManager()) {
        dump += " DirtyRegion: " + GetDirtyManager()->GetDirtyRegion().ToString();
    }
    return dump;
}

void RSSurfaceRenderNode::PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    // Save the current state of the canvas before modifying it.
#ifndef USE_ROSEN_DRAWING
    renderNodeSaveCount_ = canvas.Save();
#else
    renderNodeSaveCount_ = canvas.SaveAllStatus();
#endif

    // Apply alpha to canvas
    const RSProperties& properties = GetRenderProperties();
    canvas.MultiplyAlpha(properties.GetAlpha());

    // Apply matrix to canvas
    auto currentGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(properties.GetBoundsGeometry());
    if (currentGeoPtr != nullptr) {
        currentGeoPtr->UpdateByMatrixFromSelf();
        auto matrix = currentGeoPtr->GetMatrix();
#ifndef USE_ROSEN_DRAWING
        matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
        matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
        canvas.concat(matrix);
#else
        matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
        matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
        canvas.ConcatMatrix(matrix);
#endif
    }

    // Clip by bounds
#ifndef USE_ROSEN_DRAWING
    canvas.clipRect(SkRect::MakeWH(std::floor(properties.GetBoundsWidth()), std::floor(properties.GetBoundsHeight())));

    // Extract srcDest and dstRect from SkCanvas, localCLipBounds as SrcRect, deviceClipBounds as DstRect
    auto deviceClipRect = canvas.getDeviceClipBounds();
    UpdateSrcRect(canvas, deviceClipRect);
    RectI dstRect = { deviceClipRect.left(), deviceClipRect.top(), deviceClipRect.width(), deviceClipRect.height() };
    SetDstRect(dstRect);

    // Save TotalMatrix and GlobalAlpha for compositor
    SetTotalMatrix(canvas.getTotalMatrix());
#else
    canvas.ClipRect(Drawing::Rect(0, 0, std::floor(properties.GetBoundsWidth()),
        std::floor(properties.GetBoundsHeight())), Drawing::ClipOp::INTERSECT, false);

    // Extract srcDest and dstRect from Drawing::Canvas, localCLipBounds as SrcRect, deviceClipBounds as DstRect
    auto deviceClipRect = canvas.GetDeviceClipBounds();
    UpdateSrcRect(canvas, deviceClipRect);
    RectI dstRect = {
        deviceClipRect.GetLeft(), deviceClipRect.GetTop(), deviceClipRect.GetWidth(), deviceClipRect.GetHeight() };
    SetDstRect(dstRect);

    // Save TotalMatrix and GlobalAlpha for compositor
    SetTotalMatrix(canvas.GetTotalMatrix());
#endif
    SetGlobalAlpha(canvas.GetAlpha());
}

void RSSurfaceRenderNode::PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    canvas.RestoreStatus(renderNodeSaveCount_);
}

void RSSurfaceRenderNode::CollectSurface(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender)
{
    if (IsStartingWindow()) {
        if (isUniRender) {
            vec.emplace_back(shared_from_this());
        }
        return;
    }
    if (IsLeashWindow()) {
        if (isUniRender) {
            vec.emplace_back(shared_from_this());
        }
        for (auto& child : node->GetSortedChildren()) {
            child->CollectSurface(child, vec, isUniRender);
        }
        return;
    }

#ifndef ROSEN_CROSS_PLATFORM
    auto& consumer = GetConsumer();
    if (consumer != nullptr && consumer->GetTunnelHandle() != nullptr) {
        return;
    }
#endif
    auto num = find(vec.begin(), vec.end(), shared_from_this());
    if (num != vec.end()) {
        return;
    }
    if (isUniRender && ShouldPaint()) {
        vec.emplace_back(shared_from_this());
    } else {
#ifndef ROSEN_CROSS_PLATFORM
        if (GetBuffer() != nullptr && ShouldPaint()) {
            vec.emplace_back(shared_from_this());
        }
#endif
    }
}

void RSSurfaceRenderNode::ClearChildrenCache(const std::shared_ptr<RSBaseRenderNode>& node)
{
    for (auto& child : node->GetSortedChildren()) {
        auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode == nullptr) {
            continue;
        }
#ifndef ROSEN_CROSS_PLATFORM
        auto& consumer = surfaceNode->GetConsumer();
        if (consumer != nullptr) {
            consumer->GoBackground();
        }
#endif
    }
}

void RSSurfaceRenderNode::OnTreeStateChanged()
{
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
#ifdef RS_ENABLE_GL
    if (grContext_ && !IsOnTheTree() && IsLeashWindow()) {
        RS_TRACE_NAME_FMT("purgeUnlockedResources this SurfaceNode isn't onthe tree Id:%" PRIu64 " Name:%s",
            GetId(), GetName().c_str());
        grContext_->purgeUnlockedResources(true);
    }
#endif
}

void RSSurfaceRenderNode::ResetParent()
{
    RSBaseRenderNode::ResetParent();

    if (nodeType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        ClearChildrenCache(shared_from_this());
    } else {
#ifndef ROSEN_CROSS_PLATFORM
        auto& consumer = GetConsumer();
        if (consumer != nullptr && !IsSelfDrawingType() && !IsAbilityComponent()) {
            consumer->GoBackground();
        }
#endif
    }
}

void RSSurfaceRenderNode::SetIsNotifyUIBufferAvailable(bool available)
{
    isNotifyUIBufferAvailable_.store(available);
}

void RSSurfaceRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    visitor->PrepareSurfaceRenderNode(*this);
}

void RSSurfaceRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessSurfaceRenderNode(*this);
}

void RSSurfaceRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    needDrawAnimateProperty_ = true;
    ProcessAnimatePropertyBeforeChildren(canvas);
    needDrawAnimateProperty_ = false;
}

void RSSurfaceRenderNode::ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas)
{
    if (GetCacheType() != CacheType::ANIMATE_PROPERTY && !needDrawAnimateProperty_) {
        return;
    }
    const auto& property = GetRenderProperties();
    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, canvas, &absClipRRect);

#ifndef USE_ROSEN_DRAWING
    if (!property.GetCornerRadius().IsZero()) {
        canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas.clipRect(SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight()));
    }
#else
    if (!property.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(absClipRRect), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()),
            Drawing::ClipOp::INTERSECT, false);
    }
#endif

    RSPropertiesPainter::DrawBackground(property, canvas);
    RSPropertiesPainter::DrawMask(property, canvas);
#ifndef USE_ROSEN_DRAWING
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, skRectPtr, canvas.GetSurface());
    }
    SetTotalMatrix(canvas.getTotalMatrix());
#else
    auto filter = std::static_pointer_cast<RSDrawingFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto rectPtr =
            std::make_unique<Drawing::Rect>(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, rectPtr, canvas.GetSurface());
    }
    SetTotalMatrix(canvas.GetTotalMatrix());
#endif
}

void RSSurfaceRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    needDrawAnimateProperty_ = true;
    ProcessAnimatePropertyAfterChildren(canvas);
    needDrawAnimateProperty_ = false;
}

void RSSurfaceRenderNode::ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas)
{
    if (GetCacheType() != CacheType::ANIMATE_PROPERTY && !needDrawAnimateProperty_) {
        return;
    }
    const auto& property = GetRenderProperties();
#ifndef USE_ROSEN_DRAWING
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, skRectPtr, canvas.GetSurface());
    }
    auto para = property.GetLinearGradientBlurPara();
    if (para != nullptr && para->blurRadius_ > 0) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawLinearGradientBlurFilter(property, canvas, skRectPtr);
    }
    canvas.save();
    if (GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE) {
        auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
        canvas.concat(geoPtr->GetMatrix());
    }
    RSPropertiesPainter::DrawBorder(property, canvas);
    canvas.restore();
#else
    auto filter = std::static_pointer_cast<RSDrawingFilter>(property.GetFilter());
    if (filter != nullptr) {
        auto rectPtr = std::make_unique<Drawing::Rect>(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, rectPtr, canvas.GetSurface());
    }
    canvas.Save();
    if (GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE) {
        auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
        canvas.ConcatMatrix(geoPtr->GetMatrix());
    }
    RSPropertiesPainter::DrawBorder(property, canvas);
    canvas.Restore();
#endif
}

void RSSurfaceRenderNode::SetContextBounds(const Vector4f bounds)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetBounds>(GetId(), bounds);
    SendCommandFromRT(command, GetId());
}

std::shared_ptr<RSDirtyRegionManager> RSSurfaceRenderNode::GetDirtyManager() const
{
    return dirtyManager_;
}

std::shared_ptr<RSDirtyRegionManager> RSSurfaceRenderNode::GetCacheSurfaceDirtyManager() const
{
    return cacheSurfaceDirtyManager_;
}

void RSSurfaceRenderNode::MarkUIHidden(bool isHidden)
{
    isUIHidden_ = isHidden;
}

bool RSSurfaceRenderNode::IsUIHidden() const
{
    return isUIHidden_;
}

#ifndef USE_ROSEN_DRAWING
void RSSurfaceRenderNode::SetContextMatrix(const std::optional<SkMatrix>& matrix, bool sendMsg)
#else
void RSSurfaceRenderNode::SetContextMatrix(const std::optional<Drawing::Matrix>& matrix, bool sendMsg)
#endif
{
    if (contextMatrix_ == matrix) {
        return;
    }
    contextMatrix_ = matrix;
    SetContentDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextMatrix>(GetId(), matrix);
    SendCommandFromRT(command, GetId());
}

void RSSurfaceRenderNode::SetContextAlpha(float alpha, bool sendMsg)
{
    if (contextAlpha_ == alpha) {
        return;
    }
    contextAlpha_ = alpha;
    SetContentDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextAlpha>(GetId(), alpha);
    SendCommandFromRT(command, GetId());
}

#ifndef USE_ROSEN_DRAWING
void RSSurfaceRenderNode::SetContextClipRegion(const std::optional<SkRect>& clipRegion, bool sendMsg)
#else
void RSSurfaceRenderNode::SetContextClipRegion(const std::optional<Drawing::Rect>& clipRegion, bool sendMsg)
#endif
{
    if (contextClipRect_ == clipRegion) {
        return;
    }
    contextClipRect_ = clipRegion;
    SetContentDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextClipRegion>(GetId(), clipRegion);
    SendCommandFromRT(command, GetId());
}

void RSSurfaceRenderNode::SetSecurityLayer(bool isSecurityLayer)
{
    isSecurityLayer_ = isSecurityLayer;
}

bool RSSurfaceRenderNode::GetSecurityLayer() const
{
    return isSecurityLayer_;
}

void RSSurfaceRenderNode::SetFingerprint(bool hasFingerprint)
{
    hasFingerprint_ = hasFingerprint;
}

bool RSSurfaceRenderNode::GetFingerprint() const
{
    return hasFingerprint_;
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNode::SetColorSpace(GraphicColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

GraphicColorGamut RSSurfaceRenderNode::GetColorSpace() const
{
    return colorSpace_;
}
#endif

void RSSurfaceRenderNode::UpdateSurfaceDefaultSize(float width, float height)
{
#ifndef ROSEN_CROSS_PLATFORM
    if (consumer_ != nullptr) {
        consumer_->SetDefaultWidthAndHeight(width, height);
    }
#endif
}

#ifndef ROSEN_CROSS_PLATFORM
GraphicBlendType RSSurfaceRenderNode::GetBlendType()
{
    return blendType_;
}

void RSSurfaceRenderNode::SetBlendType(GraphicBlendType blendType)
{
    blendType_ = blendType;
}
#endif

void RSSurfaceRenderNode::RegisterBufferAvailableListener(
    sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (isFromRenderThread) {
        std::lock_guard<std::mutex> lock(mutexRT_);
        callbackFromRT_ = callback;
    } else {
        {
            std::lock_guard<std::mutex> lock(mutexUI_);
            callbackFromUI_ = callback;
        }
        isNotifyUIBufferAvailable_ = false;
    }
}

void RSSurfaceRenderNode::ConnectToNodeInRenderService()
{
    ROSEN_LOGI("RSSurfaceRenderNode::ConnectToNodeInRenderService nodeId = %" PRIu64, GetId());
    auto renderServiceClient =
        std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    if (renderServiceClient != nullptr) {
        renderServiceClient->RegisterBufferAvailableListener(
            GetId(), [weakThis = weak_from_this()]() {
                auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(weakThis.lock());
                if (node == nullptr) {
                    return;
                }
                node->NotifyRTBufferAvailable();
            }, true);
    }
}

void RSSurfaceRenderNode::NotifyRTBufferAvailable()
{
    // In RS, "isNotifyRTBufferAvailable_ = true" means buffer is ready and need to trigger ipc callback.
    // In RT, "isNotifyRTBufferAvailable_ = true" means RT know that RS have had available buffer
    // and ready to trigger "callbackForRenderThreadRefresh_" to "clip" on parent surface.
    isNotifyRTBufferAvailablePre_ = isNotifyRTBufferAvailable_;
    if (isNotifyRTBufferAvailable_) {
        return;
    }
    isNotifyRTBufferAvailable_ = true;

    if (callbackForRenderThreadRefresh_) {
        ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %" PRIu64 " RenderThread", GetId());
        callbackForRenderThreadRefresh_();
    }

    {
        std::lock_guard<std::mutex> lock(mutexRT_);
        if (callbackFromRT_) {
            ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %" PRIu64 " RenderService", GetId());
            callbackFromRT_->OnBufferAvailable();
        }
        if (!callbackForRenderThreadRefresh_ && !callbackFromRT_) {
            isNotifyRTBufferAvailable_ = false;
        }
    }
}

void RSSurfaceRenderNode::NotifyUIBufferAvailable()
{
    if (isNotifyUIBufferAvailable_) {
        return;
    }
    isNotifyUIBufferAvailable_ = true;
    {
        std::lock_guard<std::mutex> lock(mutexUI_);
        if (callbackFromUI_) {
            ROSEN_LOGD("RSSurfaceRenderNode::NotifyUIBufferAvailable nodeId = %" PRIu64, GetId());
            callbackFromUI_->OnBufferAvailable();
        }
    }
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailable() const
{
#if defined(ROSEN_ANDROID) || defined(ROSEN_IOS)
    return true;
#else
    return isNotifyRTBufferAvailable_;
#endif
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailablePre() const
{
#if defined(ROSEN_ANDROID) || defined(ROSEN_IOS)
    return true;
#else
    return isNotifyRTBufferAvailablePre_;
#endif
}

bool RSSurfaceRenderNode::IsNotifyUIBufferAvailable() const
{
    return isNotifyUIBufferAvailable_;
}

void RSSurfaceRenderNode::SetCallbackForRenderThreadRefresh(std::function<void(void)> callback)
{
    callbackForRenderThreadRefresh_ = callback;
}

bool RSSurfaceRenderNode::NeedSetCallbackForRenderThreadRefresh()
{
    return (callbackForRenderThreadRefresh_ == nullptr);
}

bool RSSurfaceRenderNode::IsStartAnimationFinished() const
{
    return startAnimationFinished_;
}

void RSSurfaceRenderNode::SetStartAnimationFinished()
{
    RS_LOGD("RSSurfaceRenderNode::SetStartAnimationFinished");
    startAnimationFinished_ = true;
}

bool RSSurfaceRenderNode::UpdateDirtyIfFrameBufferConsumed()
{
    if (isCurrentFrameBufferConsumed_) {
        SetContentDirty();
        return true;
    }
    return false;
}

void RSSurfaceRenderNode::SetVisibleRegionRecursive(const Occlusion::Region& region,
                                                    VisibleData& visibleVec,
                                                    std::map<uint32_t, bool>& pidVisMap)
{
    if (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE || IsAbilityComponent()) {
        SetOcclusionVisible(true);
        return;
    }
    visibleRegion_ = region;
    bool vis = region.GetSize() > 0;
    if (vis) {
        visibleVec.emplace_back(GetId());
    }

    // collect visible changed pid
    if (qosPidCal_ && GetType() == RSRenderNodeType::SURFACE_NODE) {
        uint32_t tmpPid = ExtractPid(GetId());
        if (pidVisMap.find(tmpPid) != pidVisMap.end()) {
            pidVisMap[tmpPid] |= vis;
        } else {
            pidVisMap[tmpPid] = vis;
        }
    }

    SetOcclusionVisible(vis);
    for (auto& child : GetSortedChildren()) {
        if (auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child)) {
            surface->SetVisibleRegionRecursive(region, visibleVec, pidVisMap);
        }
    }
}

bool RSSurfaceRenderNode::SubNodeIntersectWithExtraDirtyRegion(const RectI& r) const
{
    if (!isDirtyRegionAlignedEnable_) {
        return false;
    }
    if (!extraDirtyRegionAfterAlignmentIsEmpty_) {
        return extraDirtyRegionAfterAlignment_.IsIntersectWith(r);
    }
    return false;
}

bool RSSurfaceRenderNode::SubNodeIntersectWithDirty(const RectI& r) const
{
    Occlusion::Rect nodeRect { r.left_, r.top_, r.GetRight(), r.GetBottom() };
    // if current node rect r is in global dirtyregion, it CANNOT be skipped
    if (!globalDirtyRegionIsEmpty_) {
        auto globalRect = globalDirtyRegion_.IsIntersectWith(nodeRect);
        if (globalRect) {
            return true;
        }
    }
    // if current node is in visible dirtyRegion, it CANNOT be skipped
    bool localIntersect = visibleDirtyRegion_.IsIntersectWith(nodeRect);
    if (localIntersect) {
        return true;
    }
    // if current node is transparent
    if (IsTransparent() || IsCurrentNodeInTransparentRegion(nodeRect)) {
        return dirtyRegionBelowCurrentLayer_.IsIntersectWith(nodeRect);
    }
    return false;
}

bool RSSurfaceRenderNode::SubNodeNeedDraw(const RectI &r, PartialRenderType opDropType) const
{
    if (dirtyManager_ == nullptr) {
        return true;
    }
    switch (opDropType) {
        case PartialRenderType::SET_DAMAGE_AND_DROP_OP:
            return SubNodeIntersectWithDirty(r);
        case PartialRenderType::SET_DAMAGE_AND_DROP_OP_OCCLUSION:
            return SubNodeVisible(r);
        case PartialRenderType::SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY:
            // intersect with self visible dirty or other surfaces' extra dirty region after alignment
            return SubNodeVisible(r) && (SubNodeIntersectWithDirty(r) ||
                SubNodeIntersectWithExtraDirtyRegion(r));
        case PartialRenderType::DISABLED:
        case PartialRenderType::SET_DAMAGE:
        default:
            return true;
    }
    return true;
}

void RSSurfaceRenderNode::ResetSurfaceOpaqueRegion(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation, const bool isFocusWindow)
{
    Occlusion::Rect absRectR {absRect};
    Occlusion::Region oldOpaqueRegion { opaqueRegion_ };

    // The transparent region of surfaceNode should include shadow area
    Occlusion::Rect dirtyRect {GetOldDirty()};
    transparentRegion_ = Occlusion::Region{dirtyRect};

    if (IsTransparent()) {
        opaqueRegion_ = Occlusion::Region();
    } else {
        if (IsAppWindow() && HasContainerWindow()) {
            opaqueRegion_ = ResetOpaqueRegion(absRect, screenRotation, isFocusWindow);
        } else {
            auto corner = GetWindowCornerRadius();
            if (!corner.IsZero()) {
                opaqueRegion_ = SetCornerRadiusOpaqueRegion(absRect, std::ceil(corner.x_));
            } else {
                opaqueRegion_ = Occlusion::Region{absRectR};
            }
        }
        transparentRegion_.SubSelf(opaqueRegion_);
    }
    Occlusion::Rect screen{screeninfo};
    Occlusion::Region screenRegion{screen};
    transparentRegion_.AndSelf(screenRegion);
    opaqueRegion_.AndSelf(screenRegion);
    opaqueRegionChanged_ = !oldOpaqueRegion.Xor(opaqueRegion_).IsEmpty();
}

Vector4f RSSurfaceRenderNode::GetWindowCornerRadius()
{
    if (!GetRenderProperties().GetCornerRadius().IsZero()) {
        return GetRenderProperties().GetCornerRadius();
    }
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetParent().lock());
    if (parent != nullptr && parent->IsLeashWindow()) {
        return parent->GetRenderProperties().GetCornerRadius();
    }
    return Vector4f();
}

Occlusion::Region RSSurfaceRenderNode::ResetOpaqueRegion(const RectI& absRect,
    const ScreenRotation screenRotation, const bool isFocusWindow) const
{
    if (isFocusWindow) {
        return SetFocusedWindowOpaqueRegion(absRect, screenRotation);
    } else {
        return SetUnfocusedWindowOpaqueRegion(absRect, screenRotation);
    }
}

void RSSurfaceRenderNode::ContarinerConfig::Update(bool hasContainer, float density)
{
    this->hasContainerWindow_ = hasContainer;
    this->density = density;

    // px = vp * density
    float containerTitleHeight_ = CONTAINER_TITLE_HEIGHT * density;
    float containerContentPadding_ = CONTENT_PADDING * density;
    float containerBorderWidth_ = CONTAINER_BORDER_WIDTH * density;
    float containerOutRadius_ = CONTAINER_OUTER_RADIUS * density;
    float containerInnerRadius_ = CONTAINER_INNER_RADIUS * density;

    this->outR = RoundFloor(containerOutRadius_);
    this->inR = RoundFloor(containerInnerRadius_);
    this->bp = RoundFloor(containerBorderWidth_ + containerContentPadding_);
    this->bt = RoundFloor(containerBorderWidth_ + containerTitleHeight_);
}

/*
    If a surfacenode with containerwindow is not focus window, then its opaque
region is absRect minus four roundCorner corresponding small rectangles.
This corners removed region can be assembled with two crossed rectangles.
Furthermore, when the surfacenode is not focus window, the inner content roundrect's
boundingbox rect can be set opaque.
*/
Occlusion::Region RSSurfaceRenderNode::SetUnfocusedWindowOpaqueRegion(const RectI& absRect,
    const ScreenRotation screenRotation) const
{
    Occlusion::Rect opaqueRect1{ absRect.left_ + containerConfig_.outR,
        absRect.top_,
        absRect.GetRight() - containerConfig_.outR,
        absRect.GetBottom()};
    Occlusion::Rect opaqueRect2{ absRect.left_,
        absRect.top_ + containerConfig_.outR,
        absRect.GetRight(),
        absRect.GetBottom() - containerConfig_.outR};
    Occlusion::Region r1{opaqueRect1};
    Occlusion::Region r2{opaqueRect2};
    Occlusion::Region opaqueRegion = r1.Or(r2);

    switch (screenRotation) {
        case ScreenRotation::ROTATION_0: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_90: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bt,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bt};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bt,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
        default: {
            Occlusion::Rect opaqueRect3{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r3{opaqueRect3};
            opaqueRegion.OrSelf(r3);
            break;
        }
    }
    return opaqueRegion;
}

/*
    If a surfacenode with containerwindow is a focused window, then its containerWindow region
should be set transparent, including: title, content padding area, border, and content corners.
Note this region is not centrosymmetric, hence it should be differentiated under different
screen rotation state as top/left/botton/right has changed when screen rotated.
*/
Occlusion::Region RSSurfaceRenderNode::SetFocusedWindowOpaqueRegion(const RectI& absRect,
    const ScreenRotation screenRotation) const
{
    Occlusion::Region opaqueRegion;
    switch (screenRotation) {
        case ScreenRotation::ROTATION_0: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp - containerConfig_.inR};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bp + containerConfig_.inR,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_90: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bt + containerConfig_.inR,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bt,
                absRect.top_ + containerConfig_.bp + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp - containerConfig_.inR};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_180: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bt - containerConfig_.inR};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bp + containerConfig_.inR,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bt};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        case ScreenRotation::ROTATION_270: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bp + containerConfig_.inR,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bt - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bt,
                absRect.GetBottom() - containerConfig_.bp - containerConfig_.inR};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
        default: {
            Occlusion::Rect opaqueRect1{
                absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt + containerConfig_.inR,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp - containerConfig_.inR};
            Occlusion::Rect opaqueRect2{
                absRect.left_ + containerConfig_.bp + containerConfig_.inR,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp - containerConfig_.inR,
                absRect.GetBottom() - containerConfig_.bp};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            opaqueRegion = r1.Or(r2);
            break;
        }
    }
    return opaqueRegion;
}

Occlusion::Region RSSurfaceRenderNode::SetCornerRadiusOpaqueRegion(const RectI& absRect, float radius) const
{
    Occlusion::Rect opaqueRect1{ absRect.left_ + radius,
        absRect.top_,
        absRect.GetRight() - radius,
        absRect.GetBottom()};
    Occlusion::Rect opaqueRect2{ absRect.left_,
        absRect.top_ + radius,
        absRect.GetRight(),
        absRect.GetBottom() - radius};
    Occlusion::Region r1{opaqueRect1};
    Occlusion::Region r2{opaqueRect2};
    Occlusion::Region opaqueRegion = r1.Or(r2);
    return opaqueRegion;
}

void RSSurfaceRenderNode::ResetSurfaceContainerRegion(const RectI& screeninfo, const RectI& absRect,
    const ScreenRotation screenRotation)
{
    if (!HasContainerWindow()) {
        containerRegion_ = Occlusion::Region{};
        return;
    }
    Occlusion::Region absRegion{Occlusion::Rect{absRect}};
    Occlusion::Rect innerRect;
    switch (screenRotation) {
        case ScreenRotation::ROTATION_0: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            break;
        }
        case ScreenRotation::ROTATION_90: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bt,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            break;
        }
        case ScreenRotation::ROTATION_180: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bt};
            break;
        }
        case ScreenRotation::ROTATION_270: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bp,
                absRect.GetRight() - containerConfig_.bt,
                absRect.GetBottom() - containerConfig_.bp};
            break;
        }
        default: {
            innerRect = Occlusion::Rect{ absRect.left_ + containerConfig_.bp,
                absRect.top_ + containerConfig_.bt,
                absRect.GetRight() - containerConfig_.bp,
                absRect.GetBottom() - containerConfig_.bp};
            break;
        }
    }
    Occlusion::Region innerRectRegion{innerRect};
    containerRegion_ = absRegion.Sub(innerRectRegion);
}

// [planning] Remove this after skia is upgraded, the clipRegion is supported
void RSSurfaceRenderNode::ResetChildrenFilterRects()
{
    childrenFilterRects_.clear();
}

void RSSurfaceRenderNode::UpdateChildrenFilterRects(const RectI& rect)
{
    if (!rect.IsEmpty()) {
        childrenFilterRects_.emplace_back(rect);
    }
}

const std::vector<RectI>& RSSurfaceRenderNode::GetChildrenNeedFilterRects() const
{
    return childrenFilterRects_;
}

// manage abilities' nodeid info
void RSSurfaceRenderNode::ResetAbilityNodeIds()
{
    abilityNodeIds_.clear();
}

void RSSurfaceRenderNode::UpdateAbilityNodeIds(NodeId id)
{
    abilityNodeIds_.emplace_back(id);
}

const std::vector<NodeId>& RSSurfaceRenderNode::GetAbilityNodeIds() const
{
    return abilityNodeIds_;
}

void RSSurfaceRenderNode::ResetChildHardwareEnabledNodes()
{
    childHardwareEnabledNodes_.clear();
}

void RSSurfaceRenderNode::AddChildHardwareEnabledNode(std::weak_ptr<RSSurfaceRenderNode> childNode)
{
    childHardwareEnabledNodes_.emplace_back(childNode);
}

std::vector<std::weak_ptr<RSSurfaceRenderNode>> RSSurfaceRenderNode::GetChildHardwareEnabledNodes() const
{
    return childHardwareEnabledNodes_;
}

void RSSurfaceRenderNode::SetLocalZOrder(float localZOrder)
{
    localZOrder_ = localZOrder;
}

float RSSurfaceRenderNode::GetLocalZOrder() const
{
    return localZOrder_;
}

void RSSurfaceRenderNode::OnApplyModifiers()
{
    auto& properties = GetMutableRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(properties.GetBoundsGeometry());

    // Multiply context alpha into alpha
    properties.SetAlpha(properties.GetAlpha() * contextAlpha_);

    // Apply the context matrix into the bounds geometry
    geoPtr->SetContextMatrix(contextMatrix_);
}

#ifndef USE_ROSEN_DRAWING
std::optional<SkRect> RSSurfaceRenderNode::GetContextClipRegion() const
{
    return contextClipRect_;
}
#else
std::optional<Drawing::Rect> RSSurfaceRenderNode::GetContextClipRegion() const
{
    return contextClipRect_;
}
#endif

bool RSSurfaceRenderNode::LeashWindowRelatedAppWindowOccluded(std::shared_ptr<RSSurfaceRenderNode>& appNode)
{
    if (!IsLeashWindow()) {
        return false;
    }
    for (auto& child : GetChildren()) {
        auto childNode = child.lock();
        const auto& childNodeSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(childNode);
        if (childNodeSurface->GetVisibleRegion().IsEmpty()) {
            appNode = childNodeSurface;
            return true;
        }
    }
    return false;
}

std::shared_ptr<RSSurfaceRenderNode> RSSurfaceRenderNode::GetLeashWindowNestedAppSurface()
{
    if (!IsLeashWindow()) {
        return nullptr;
    }
    for (auto& child : GetChildren()) {
        auto childNode = child.lock();
        if (childNode) {
            auto childNodeSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(childNode);
            if (childNodeSurface) {
                return childNodeSurface;
            }
        }
    }
    return nullptr;
}

bool RSSurfaceRenderNode::IsCurrentFrameStatic()
{
    if (dirtyManager_ == nullptr || !dirtyManager_->GetLastestHistory().IsEmpty()) {
        return false;
    }
    if (IsMainWindowType()) {
        return true;
    } else if (IsLeashWindow()) {
        auto appSurfaceNode = GetLeashWindowNestedAppSurface();
        return appSurfaceNode ? appSurfaceNode->IsCurrentFrameStatic() : true;
    } else if (IsSelfDrawingType()) {
        return isCurrentFrameBufferConsumed_;
    } else {
        return false;
    }
}

void RSSurfaceRenderNode::UpdateCacheSurfaceDirtyManager(int bufferAge)
{
    if (!cacheSurfaceDirtyManager_ || !dirtyManager_) {
        return;
    }
    cacheSurfaceDirtyManager_->Clear();
    cacheSurfaceDirtyManager_->MergeDirtyRect(dirtyManager_->GetLastestHistory());
    cacheSurfaceDirtyManager_->SetBufferAge(bufferAge);
    cacheSurfaceDirtyManager_->UpdateDirty(false);
    // for leashwindow type, nested app surfacenode's cacheSurfaceDirtyManager update is required
    auto appSurfaceNode = GetLeashWindowNestedAppSurface();
    if (appSurfaceNode) {
        appSurfaceNode->UpdateCacheSurfaceDirtyManager(bufferAge);
    }
}

} // namespace Rosen
} // namespace OHOS
