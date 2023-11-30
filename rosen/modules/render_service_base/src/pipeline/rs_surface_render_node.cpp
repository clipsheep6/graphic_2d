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
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "ipc_callbacks/rs_rt_refresh_callback.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"
#include "property/rs_property_drawable.h"

namespace OHOS {
namespace Rosen {
RSSurfaceRenderNode::RSSurfaceRenderNode(
    const RSSurfaceRenderNodeConfig& config, const std::weak_ptr<RSContext>& context)
    : RSRenderNode(config.id, context), RSSurfaceHandler(config.id), name_(config.name), bundleName_(config.bundleName),
      nodeType_(config.nodeType), dirtyManager_(std::make_shared<RSDirtyRegionManager>()),
      cacheSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    MemoryInfo info = {sizeof(*this), ExtractPid(config.id), config.id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(config.id, info);
}

RSSurfaceRenderNode::RSSurfaceRenderNode(NodeId id, const std::weak_ptr<RSContext>& context)
    : RSSurfaceRenderNode(RSSurfaceRenderNodeConfig { id, "SurfaceNode" }, context)
{}

RSSurfaceRenderNode::~RSSurfaceRenderNode()
{
#ifdef USE_SURFACE_TEXTURE
    SetSurfaceTexture(nullptr);
#endif
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
    int width = std::clamp<int>(std::ceil(localClipRect.width()), 0, std::ceil(properties.GetBoundsWidth() - left));
    int height = std::clamp<int>(std::ceil(localClipRect.height()), 0, std::ceil(properties.GetBoundsHeight() - top));
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
    auto currentGeoPtr = (properties.GetBoundsGeometry());
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
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender,
    bool onlyFirstLevel)
{
    if (IsScbScreen()) {
        for (auto& child : node->GetSortedChildren()) {
            child->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
        }
        return;
    }
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
        if (onlyFirstLevel) {
            return;
        }
        for (auto& child : node->GetSortedChildren()) {
            child->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
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

    if (GetSubSurfaceEnabled()) {
        if (onlyFirstLevel) {
            return;
        }
        for (auto &nodes : node->GetSubSurfaceNodes()) {
            for (auto &node : nodes.second) {
                auto surfaceNode = node.lock();
                if (surfaceNode != nullptr) {
                    surfaceNode->CollectSurface(surfaceNode, vec, isUniRender, onlyFirstLevel);
                }
            }
        }
    }
}

void RSSurfaceRenderNode::ClearChildrenCache()
{
    for (auto& child : GetChildren()) {
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
    // Temporary solution, GetChildren will generate fullChildrenList_, which will cause memory leak
    OnTreeStateChanged();
}

void RSSurfaceRenderNode::OnTreeStateChanged()
{
    RSRenderNode::OnTreeStateChanged();
#ifdef RS_ENABLE_GL
    if (grContext_ && !IsOnTheTree()) {
        if (auto context = GetContext().lock()) {
#ifndef USE_ROSEN_DRAWING
            RS_TRACE_NAME_FMT("need purgeUnlockedResources this SurfaceNode isn't on the tree Id:%" PRIu64 " Name:%s",
                GetId(), GetName().c_str());
#endif
            if (IsLeashWindow()) {
                context->MarkNeedPurge(RSContext::PurgeType::GENTLY);
            }
            if (GetName().substr(0, 3) == "SCB") {
                context->MarkNeedPurge(RSContext::PurgeType::STRONGLY);
            }
        }
    }
#endif
    if (IsAbilityComponent()) {
        if (auto instanceRootNode = GetInstanceRootNode()) {
            if (auto surfaceNode = instanceRootNode->ReinterpretCastTo<RSSurfaceRenderNode>()) {
                surfaceNode->UpdateAbilityNodeIds(GetId(), IsOnTheTree());
            }
        }
    }
}

void RSSurfaceRenderNode::OnResetParent()
{
    if (nodeType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        ClearChildrenCache();
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
#ifdef USE_SURFACE_TEXTURE
    auto texture = GetSurfaceTexture();
    if (texture) {
        texture->MarkUiFrameAvailable(available);
    }
#endif
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

#ifndef ROSEN_CROSS_PLATFORM
    RSPropertiesPainter::DrawBackground(property, canvas, true, IsSelfDrawingNode() && (GetBuffer() != nullptr));
#else
    RSPropertiesPainter::DrawBackground(property, canvas);
#endif
    RSPropertiesPainter::DrawMask(property, canvas);
    RSPropertiesPainter::DrawFilter(property, canvas, FilterType::BACKGROUND_FILTER);
#ifndef USE_ROSEN_DRAWING
    SetTotalMatrix(canvas.getTotalMatrix());
#else
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
    RSPropertiesPainter::DrawFilter(property, canvas, FilterType::FOREGROUND_FILTER);
    RSPropertiesPainter::DrawLinearGradientBlurFilter(property, canvas);
#ifndef USE_ROSEN_DRAWING
    canvas.save();
    if (GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE) {
        auto geoPtr = (property.GetBoundsGeometry());
        canvas.concat(geoPtr->GetMatrix());
    }
    RSPropertiesPainter::DrawBorder(property, canvas);
    canvas.restore();
#else
    canvas.Save();
    if (GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE) {
        auto geoPtr = (property.GetBoundsGeometry());
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
    AddDirtyType(RSModifierType::SCALE);
    AddDirtyType(RSModifierType::TRANSLATE);
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
    AddDirtyType(RSModifierType::ALPHA);
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
    AddDirtyType(RSModifierType::BOUNDS);
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextClipRegion>(GetId(), clipRegion);
    SendCommandFromRT(command, GetId());
}
void RSSurfaceRenderNode::SetBootAnimation(bool isBootAnimation)
{
    ROSEN_LOGD("SetBootAnimation:: id:%{public}" PRIu64 ", isBootAnimation:%{public}d",
        GetId(), isBootAnimation);
    isBootAnimation_ = isBootAnimation;
}

bool RSSurfaceRenderNode::GetBootAnimation() const
{
    return isBootAnimation_;
}

void RSSurfaceRenderNode::SetSecurityLayer(bool isSecurityLayer)
{
    isSecurityLayer_ = isSecurityLayer;
    SetDirty();
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetParent().lock());
    if (parent != nullptr && parent->IsLeashWindow()) {
        parent->SetSecurityLayer(isSecurityLayer);
        parent->SetDirty();
    }
}

bool RSSurfaceRenderNode::GetSecurityLayer() const
{
    return isSecurityLayer_;
}

void RSSurfaceRenderNode::SetSkipLayer(bool isSkipLayer)
{
    isSkipLayer_ = isSkipLayer;
    SetDirty();
    auto parent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetParent().lock());
    if (parent != nullptr && parent->IsLeashWindow()) {
        parent->SetSkipLayer(isSkipLayer);
        parent->SetDirty();
    }
}

bool RSSurfaceRenderNode::GetSkipLayer() const
{
    return isSkipLayer_;
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
#else
#ifdef USE_SURFACE_TEXTURE
    auto texture = GetSurfaceTexture();
    if (texture) {
        texture->UpdateSurfaceDefaultSize(width, height);
    }
#endif
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

void RSSurfaceRenderNode::RegisterBufferClearListener(
    sptr<RSIBufferClearCallback> callback)
{
    std::lock_guard<std::mutex> lock(mutexClear_);
    clearBufferCallback_ = callback;
}

void RSSurfaceRenderNode::SetNotifyRTBufferAvailable(bool isNotifyRTBufferAvailable)
{
    isNotifyRTBufferAvailable_ = isNotifyRTBufferAvailable;
    std::lock_guard<std::mutex> lock(mutexClear_);
    if (clearBufferCallback_) {
        clearBufferCallback_->OnBufferClear();
    }
}

void RSSurfaceRenderNode::ConnectToNodeInRenderService()
{
    ROSEN_LOGI("RSSurfaceRenderNode::ConnectToNodeInRenderService nodeId = %{public}" PRIu64, GetId());
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
        renderServiceClient->RegisterBufferClearListener(
            GetId(), [weakThis = weak_from_this()]() {
                auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(weakThis.lock());
                if (node == nullptr) {
                    return;
                }
                node->SetNotifyRTBufferAvailable(false);
            });
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

    if (isRefresh_) {
        ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %{public}" PRIu64 " RenderThread", GetId());
        RSRTRefreshCallback::Instance().ExcuteRefresh();
    }

    {
        std::lock_guard<std::mutex> lock(mutexRT_);
        if (callbackFromRT_) {
            ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %{public}" PRIu64 " RenderService",
                GetId());
            callbackFromRT_->OnBufferAvailable();
        }
        if (!isRefresh_ && !callbackFromRT_) {
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
            ROSEN_LOGD("RSSurfaceRenderNode::NotifyUIBufferAvailable nodeId = %{public}" PRIu64, GetId());
            callbackFromUI_->OnBufferAvailable();
#ifdef OHOS_PLATFORM
            if (IsAppWindow()) {
                RSJankStats::GetInstance().SetAppFirstFrame(ExtractPid(GetId()));
            }
#endif
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

void RSSurfaceRenderNode::SetCallbackForRenderThreadRefresh(bool isRefresh)
{
    isRefresh_ = isRefresh;
}

bool RSSurfaceRenderNode::NeedSetCallbackForRenderThreadRefresh()
{
    return !isRefresh_;
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

bool RSSurfaceRenderNode::IsSurfaceInStartingWindowStage() const
{
    auto parentPtr = this->GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        auto surfaceParentPtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentPtr);
        if (surfaceParentPtr->GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE &&
            !this->IsNotifyUIBufferAvailable()) {
            return true;
        }
    }
    return false;
}

bool RSSurfaceRenderNode::IsParentLeashWindowInScale() const
{
    auto parentPtr = this->GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        auto surfaceParentPtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parentPtr);
        if (surfaceParentPtr->IsLeashWindow() && surfaceParentPtr->IsScale()) {
            return true;
        }
    }
    return false;
}

Occlusion::Rect RSSurfaceRenderNode::GetSurfaceOcclusionRect(bool isUniRender)
{
    Occlusion::Rect occlusionRect;
    if (isUniRender) {
        occlusionRect = Occlusion::Rect {GetOldDirtyInSurface()};
    } else {
        occlusionRect = Occlusion::Rect {GetDstRect()};
    }
    return occlusionRect;
}

void RSSurfaceRenderNode::AccumulateOcclusionRegion(Occlusion::Region& accumulatedRegion,
    Occlusion::Region& curRegion,
    bool& hasFilterCacheOcclusion,
    bool isUniRender,
    bool filterCacheOcclusionEnabled)
{
    // when surfacenode is in starting window stage, do not occlude other window surfaces
    // fix gray block when directly open app (i.e. setting) from notification center
    if (IsSurfaceInStartingWindowStage()) {
        return;
    }
    if (!isUniRender) {
        bool diff =
#ifndef ROSEN_CROSS_PLATFORM
            (GetDstRect().width_ > GetBuffer()->GetWidth() || GetDstRect().height_ > GetBuffer()->GetHeight()) &&
#endif
            GetRenderProperties().GetFrameGravity() != Gravity::RESIZE &&
            ROSEN_EQ(GetGlobalAlpha(), 1.0f);
        if (!IsTransparent() && !diff) {
            accumulatedRegion.OrSelf(curRegion);
        }
    }

    if (GetName().find("hisearch") != std::string::npos) {
        return;
    }
    SetTreatedAsTransparent(false);
    // when a surfacenode is in animation (i.e. 3d animation), its dstrect cannot be trusted, we treated it as a full
    // transparent layer.
    if (GetAnimateState() || IsParentLeashWindowInScale()) {
        SetTreatedAsTransparent(true);
        ResetAnimateState();
        return;
    }

    // full surfacenode valid filter cache can be treated as opaque
    if (filterCacheOcclusionEnabled && IsTransparent() && GetFilterCacheValidForOcclusion()) {
        accumulatedRegion.OrSelf(curRegion);
        hasFilterCacheOcclusion = true;
    } else {
        accumulatedRegion.OrSelf(GetOpaqueRegion());
    }
    return;
}

WINDOW_LAYER_INFO_TYPE RSSurfaceRenderNode::GetVisibleLevelForWMS(RSVisibleLevel visibleLevel)
{
    switch (visibleLevel) {
        case RSVisibleLevel::RS_INVISIBLE:
            return WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE;
        case RSVisibleLevel::RS_ALL_VISIBLE:
            return WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE;
        case RSVisibleLevel::RS_SEMI_NONDEFAULT_VISIBLE:
        case RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE:
            return WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE;
        default:
            break;
    }
    return WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE;
}

bool RSSurfaceRenderNode::IsMultiInstance()
{
    return GetName().find("filemanager") != std::string::npos || GetName().find("browser") != std::string::npos;
}

void RSSurfaceRenderNode::SetVisibleRegionRecursive(const Occlusion::Region& region,
                                                    VisibleData& visibleVec,
                                                    std::map<uint32_t, RSVisibleLevel>& pidVisMap,
                                                    bool needSetVisibleRegion,
                                                    RSVisibleLevel visibleLevel)
{
    if (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE || IsAbilityComponent()) {
        SetOcclusionVisible(true);
        visibleVec.emplace_back(std::make_pair(GetId(), ALL_VISIBLE));
        return;
    }

    bool vis = region.GetSize() > 0;
    if (vis) {
        visibleVec.emplace_back(std::make_pair(GetId(), GetVisibleLevelForWMS(visibleLevel)));
    }

    // collect visible changed pid
    if (qosPidCal_ && GetType() == RSRenderNodeType::SURFACE_NODE && !IsMultiInstance()) {
        uint32_t tmpPid = ExtractPid(GetId());
        pidVisMap[tmpPid] = visibleLevel;
    }

    visibleRegionForCallBack_ = region;
    if (needSetVisibleRegion) {
        visibleRegion_ = region;
        SetOcclusionVisible(vis);
    }

    for (auto& child : GetChildren()) {
        if (auto surfaceChild = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child)) {
            surfaceChild->SetVisibleRegionRecursive(region, visibleVec, pidVisMap, needSetVisibleRegion, visibleLevel);
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
    if (IsTransparent() || IsCurrentNodeInTransparentRegion(nodeRect) || IsTreatedAsTransparent()) {
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
    ResetSurfaceContainerRegion(screeninfo, absRect, screenRotation);
}

void RSSurfaceRenderNode::CalcFilterCacheValidForOcclusion()
{
    if (!dirtyManager_) {
        return;
    }
    isFilterCacheStatusChanged_ = false;
    bool currentCacheValidForOcclusion = isFilterCacheFullyCovered_ && dirtyManager_->IsFilterCacheRectValid();
    if (isFilterCacheValidForOcclusion_ != currentCacheValidForOcclusion) {
        isFilterCacheValidForOcclusion_ = currentCacheValidForOcclusion;
        isFilterCacheStatusChanged_ = true;
    }
}

void RSSurfaceRenderNode::UpdateFilterNodes(const std::shared_ptr<RSRenderNode>& nodePtr)
{
    if (nodePtr == nullptr) {
        return;
    }
    filterNodes_.emplace(nodePtr->GetId(), nodePtr);
}

void RSSurfaceRenderNode::UpdateDrawingCacheNodes(const std::shared_ptr<RSRenderNode>& nodePtr)
{
    if (nodePtr == nullptr) {
        return;
    }
    drawingCacheNodes_.emplace(nodePtr->GetId(), nodePtr);
}

void RSSurfaceRenderNode::ResetDrawingCacheStatusIfNodeStatic(
    std::unordered_map<NodeId, std::unordered_set<NodeId>>& allRects)
{
    // traversal drawing cache nodes including app window
    EraseIf(drawingCacheNodes_, [this, &allRects](const auto& pair) {
        auto& node = pair.second;
        if (node == nullptr || !node->IsOnTheTree()) {
            return true;
        }
        node->SetDrawingCacheChanged(false);
        node->GetFilterRectsInCache(allRects);
        return false;
    });
}

void RSSurfaceRenderNode::UpdateFilterCacheStatusWithVisible(bool visible)
{
    if (visible == prevVisible_) {
        return;
    }
    prevVisible_ = visible;
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!visible && !filterNodes_.empty()) {
        for (auto& node : filterNodes_) {
            node.second->GetMutableRenderProperties().ClearFilterCache();
        }
    }
#endif
}

void RSSurfaceRenderNode::UpdateFilterCacheStatusIfNodeStatic(const RectI& clipRect)
{
    if (!dirtyManager_) {
        return;
    }
    // traversal filter nodes including app window
    EraseIf(filterNodes_, [this](const auto& pair) {
        auto& node = pair.second;
        if (node == nullptr || !node->IsOnTheTree() || !node->GetRenderProperties().NeedFilter()) {
            return true;
        }
        node->UpdateFilterCacheWithDirty(*dirtyManager_, false);
        node->UpdateFilterCacheWithDirty(*dirtyManager_, true);
        return false;
    });
    SetFilterCacheFullyCovered(false);
    if (IsTransparent() && dirtyManager_->IfCacheableFilterRectFullyCover(GetOldDirtyInSurface())) {
        SetFilterCacheFullyCovered(true);
        RS_LOGD("UpdateFilterCacheStatusIfNodeStatic surfacenode %{public}" PRIu64 " [%{public}s] rectsize %{public}s",
            GetId(), GetName().c_str(), GetOldDirtyInSurface().ToString().c_str());
    }
    CalcFilterCacheValidForOcclusion();
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

void RSSurfaceRenderNode::ContainerConfig::Update(bool hasContainer, float density)
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
screen rotation state as top/left/bottom/right has changed when screen rotated.
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

bool RSSurfaceRenderNode::CheckOpaqueRegionBaseInfo(
    const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation, const bool isFocusWindow)
{
    return opaqueRegionBaseInfo_.screenRect_ == screeninfo &&
        opaqueRegionBaseInfo_.absRect_ == absRect &&
        opaqueRegionBaseInfo_.screenRotation_ == screenRotation &&
        opaqueRegionBaseInfo_.isFocusWindow_ == isFocusWindow &&
        opaqueRegionBaseInfo_.isTransparent_ == IsTransparent() &&
        opaqueRegionBaseInfo_.hasContainerWindow_ == HasContainerWindow();
}

void RSSurfaceRenderNode::SetOpaqueRegionBaseInfo(
    const RectI& screeninfo, const RectI& absRect, const ScreenRotation screenRotation, const bool isFocusWindow)
{
    opaqueRegionBaseInfo_.screenRect_ = screeninfo;
    opaqueRegionBaseInfo_.absRect_ = absRect;
    opaqueRegionBaseInfo_.screenRotation_ = screenRotation;
    opaqueRegionBaseInfo_.isFocusWindow_ = isFocusWindow;
    opaqueRegionBaseInfo_.isTransparent_ = IsTransparent();
    opaqueRegionBaseInfo_.hasContainerWindow_ = HasContainerWindow();
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
void RSSurfaceRenderNode::UpdateAbilityNodeIds(NodeId id, bool isAdded)
{
    if (isAdded) {
        abilityNodeIds_.emplace(id);
    } else {
        abilityNodeIds_.erase(id);
    }
}

void RSSurfaceRenderNode::AddAbilityComponentNodeIds(std::unordered_set<NodeId> nodeIds)
{
    abilityNodeIds_.insert(nodeIds.begin(), nodeIds.end());
}

void RSSurfaceRenderNode::ResetAbilityNodeIds()
{
    abilityNodeIds_.clear();
}

const std::unordered_set<NodeId>& RSSurfaceRenderNode::GetAbilityNodeIds() const
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

const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& RSSurfaceRenderNode::GetChildHardwareEnabledNodes() const
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
    auto geoPtr = (properties.GetBoundsGeometry());

    // Multiply context alpha into alpha
    properties.SetAlpha(properties.GetAlpha() * contextAlpha_);

    // Apply the context matrix into the bounds geometry
    geoPtr->SetContextMatrix(contextMatrix_);
    if (!ShouldPaint()) {
        UpdateFilterCacheStatusWithVisible(false);
    }
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
    for (auto& childNode : GetChildren()) {
        const auto& childNodeSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(childNode);
        if (childNodeSurface && childNodeSurface->GetVisibleRegion().IsEmpty()) {
            appNode = childNodeSurface;
            return true;
        }
    }
    return false;
}

std::vector<std::shared_ptr<RSSurfaceRenderNode>> RSSurfaceRenderNode::GetLeashWindowNestedSurfaces()
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> res;
    if (!IsLeashWindow()) {
        return res;
    }
    for (auto& childNode : GetChildren()) {
        if (childNode) {
            auto childNodeSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(childNode);
            if (childNodeSurface) {
                res.emplace_back(childNodeSurface);
            }
        }
    }
    return res;
}

bool RSSurfaceRenderNode::IsCurrentFrameStatic()
{
    if (IsMainWindowType() && !IsOnlyBasicGeoTransfrom()) {
        return false;
    }
    if (IsAppWindow()) {
        auto hardwareEnabledNodes = GetChildHardwareEnabledNodes();
        for (auto& hardwareEnabledNode : hardwareEnabledNodes) {
            auto hardwareEnabledNodePtr = hardwareEnabledNode.lock();
            if (hardwareEnabledNodePtr && hardwareEnabledNodePtr->IsCurrentFrameBufferConsumed()) {
                return false;
            }
        }
    }
    if (IsMainWindowType()) {
        return true;
    } else if (IsLeashWindow()) {
        // leashwindow subthread cache considered static if and only if all nested surfacenode static
        // (include appwindow and starting window)
        auto nestedSurfaceNodes = GetLeashWindowNestedSurfaces();
        for (auto& nestedSurface: nestedSurfaceNodes) {
            if (nestedSurface && !nestedSurface->IsCurrentFrameStatic()) {
                return false;
            }
        }
        if (dirtyManager_ == nullptr || !dirtyManager_->GetCurrentFrameDirtyRegion().IsEmpty()) {
            return false;
        }
        return true;
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
    cacheSurfaceDirtyManager_->MergeDirtyRect(dirtyManager_->GetLatestDirtyRegion());
    cacheSurfaceDirtyManager_->SetBufferAge(bufferAge);
    cacheSurfaceDirtyManager_->UpdateDirty(false);
    // for leashwindow type, nested app surfacenode's cacheSurfaceDirtyManager update is required
    auto nestedSurfaceNodes = GetLeashWindowNestedSurfaces();
    for (auto& nestedSurface : nestedSurfaceNodes) {
        if (nestedSurface) {
            nestedSurface->UpdateCacheSurfaceDirtyManager(bufferAge);
        }
    }
}

void RSSurfaceRenderNode::SetIsOnTheTree(bool flag, NodeId instanceRootNodeId, NodeId firstLevelNodeId,
    NodeId cacheNodeId)
{
    instanceRootNodeId = (IsMainWindowType() || IsLeashWindow()) ? GetId() : instanceRootNodeId;
    if (IsLeashWindow()) {
        firstLevelNodeId = GetId();
    } else if (IsAppWindow()) {
        firstLevelNodeId = GetId();
        auto parentNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(GetParent().lock());
        if (parentNode && parentNode->IsLeashWindow()) {
            firstLevelNodeId = parentNode->GetId();
        }
    }
    isNewOnTree_ = flag && !isOnTheTree_;
    // if node is marked as cacheRoot, update subtree status when update surface
    // in case prepare stage upper cacheRoot cannot specify dirty subnode
    RSBaseRenderNode::SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
}

CacheProcessStatus RSSurfaceRenderNode::GetCacheSurfaceProcessedStatus() const
{
    return cacheProcessStatus_.load();
}

void RSSurfaceRenderNode::SetCacheSurfaceProcessedStatus(CacheProcessStatus cacheProcessStatus)
{
    cacheProcessStatus_.store(cacheProcessStatus);
}

bool RSSurfaceRenderNode::HasOnlyOneRootNode() const
{
    if (GetChildrenCount() != 1) {
        return false;
    }

    const auto child = GetChildren().front().lock();
    if (!child || child->GetType() != RSRenderNodeType::ROOT_NODE || child->GetChildrenCount() > 0) {
        return false;
    }

    return true;
}

bool RSSurfaceRenderNode::GetNodeIsSingleFrameComposer() const
{
    bool flag = false;
    if (RSSystemProperties::GetSingleFrameComposerCanvasNodeEnabled()) {
        auto idx = GetName().find("hwstylusfeature");
        if (idx != std::string::npos) {
            flag = true;
        }
    }
    return isNodeSingleFrameComposer_ || flag;
}
} // namespace Rosen
} // namespace OHOS
