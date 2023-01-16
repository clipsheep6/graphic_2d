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

#include "pipeline/rs_surface_render_node.h"

#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "include/gpu/GrContext.h"

#include "command/rs_surface_node_command.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSSurfaceRenderNode::RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context)
    : RSRenderNode(config.id, context),
      RSSurfaceHandler(config.id),
      name_(config.name),
      nodeType_(config.nodeType),
      dirtyManager_(std::make_shared<RSDirtyRegionManager>())
{}

RSSurfaceRenderNode::RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSSurfaceRenderNode(RSSurfaceRenderNodeConfig{id, "SurfaceNode"}, context)
{}

RSSurfaceRenderNode::~RSSurfaceRenderNode() {}

#ifdef ROSEN_OHOS
void RSSurfaceRenderNode::SetConsumer(const sptr<Surface>& consumer)
{
    consumer_ = consumer;
}
#endif

static SkRect getLocalClipBounds(const RSPaintFilterCanvas& canvas)
{
    SkIRect ibounds = canvas.getDeviceClipBounds();
    if (ibounds.isEmpty()) {
        return SkRect::MakeEmpty();
    }

    SkMatrix inverse;
    // if we can't invert the CTM, we can't return local clip bounds
    if (!(canvas.getTotalMatrix().invert(&inverse))) {
        return SkRect::MakeEmpty();
    }
    SkRect bounds;
    SkRect r = SkRect::Make(ibounds);
    inverse.mapRect(&bounds, r);
    return bounds;
}

void RSSurfaceRenderNode::PrepareRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    renderNodeSaveCount_ = canvas.SaveCanvasAndAlpha();

    // apply intermediate properties from RT to canvas
    canvas.MultiplyAlpha(GetContextAlpha());
    canvas.concat(GetContextMatrix());
    auto clipRectFromRT = GetContextClipRegion();
    if (clipRectFromRT.width() > std::numeric_limits<float>::epsilon() &&
        clipRectFromRT.height() > std::numeric_limits<float>::epsilon()) {
        canvas.clipRect(clipRectFromRT);
    }

    // apply node properties to canvas
    const RSProperties& properties = GetRenderProperties();
    canvas.MultiplyAlpha(properties.GetAlpha());
    auto currentGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(properties.GetBoundsGeometry());
    if (currentGeoPtr != nullptr) {
        currentGeoPtr->UpdateByMatrixFromSelf();
        auto matrix = currentGeoPtr->GetMatrix();
        matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
        matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
        canvas.concat(matrix);
    }

    // clip by bounds
    canvas.clipRect(
        SkRect::MakeWH(std::floor(properties.GetBoundsWidth()), std::floor(properties.GetBoundsHeight())));

    // extract srcDest and dstRect from SkCanvas, localCLipBounds as SrcRect, deviceClipBounds as DstRect
    auto localClipRect = getLocalClipBounds(canvas);
    RectI srcRect = {
        std::clamp<int>(localClipRect.left(), 0, properties.GetBoundsWidth()),
        std::clamp<int>(localClipRect.top(), 0, properties.GetBoundsHeight()),
        std::clamp<int>(localClipRect.width(), 0, properties.GetBoundsWidth() - localClipRect.left()),
        std::clamp<int>(localClipRect.height(), 0, properties.GetBoundsHeight() - localClipRect.top())
    };
    SetSrcRect(srcRect);
    auto deviceClipRect = canvas.getDeviceClipBounds();
    RectI dstRect = { deviceClipRect.left(), deviceClipRect.top(), deviceClipRect.width(), deviceClipRect.height() };
    SetDstRect(dstRect);

    // save TotalMatrix and GlobalAlpha for compositor
    SetTotalMatrix(canvas.getTotalMatrix());
    SetGlobalAlpha(canvas.GetAlpha());
}

void RSSurfaceRenderNode::PrepareRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    canvas.RestoreCanvasAndAlpha(renderNodeSaveCount_);
}

void RSSurfaceRenderNode::CollectSurface(
    const std::shared_ptr<RSBaseRenderNode>& node, std::vector<RSBaseRenderNode::SharedPtr>& vec, bool isUniRender)
{
    if (nodeType_ == RSSurfaceNodeType::STARTING_WINDOW_NODE) {
        if (isUniRender) {
            vec.emplace_back(shared_from_this());
        }
        return;
    }
    if (nodeType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        for (auto& child : node->GetSortedChildren()) {
            child->CollectSurface(child, vec, isUniRender);
        }
        return;
    }
#ifdef ROSEN_OHOS
    auto& consumer = GetConsumer();
    if (consumer != nullptr && consumer->GetTunnelHandle() != nullptr) {
        return;
    }
    auto num = find(vec.begin(), vec.end(), shared_from_this());
    if (num != vec.end()) {
        return;
    }
    if (isUniRender && ShouldPaint()) {
        vec.emplace_back(shared_from_this());
    } else {
        if (GetBuffer() != nullptr && ShouldPaint()) {
            vec.emplace_back(shared_from_this());
        }
    }
#endif
}

void RSSurfaceRenderNode::ClearChildrenCache(const std::shared_ptr<RSBaseRenderNode>& node)
{
    for (auto& child : node->GetSortedChildren()) {
        auto surfaceNode = child->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode == nullptr) {
            continue;
        }
#ifdef ROSEN_OHOS
        auto& consumer = surfaceNode->GetConsumer();
        if (consumer != nullptr) {
            consumer->GoBackground();
        }
#endif
    }
}

void RSSurfaceRenderNode::ResetParent()
{
    RSBaseRenderNode::ResetParent();

    if (nodeType_ == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
        ClearChildrenCache(shared_from_this());
    } else {
#ifdef ROSEN_OHOS
        auto& consumer = GetConsumer();
        if (consumer != nullptr &&
            (GetSurfaceNodeType() != RSSurfaceNodeType::SELF_DRAWING_NODE &&
            GetSurfaceNodeType() != RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE &&
            GetSurfaceNodeType() != RSSurfaceNodeType::ABILITY_COMPONENT_NODE)) {
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

void RSSurfaceRenderNode::SetContextBounds(const Vector4f bounds)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetBounds>(GetId(), bounds);
    SendCommandFromRT(command, GetId());
}

std::shared_ptr<RSDirtyRegionManager> RSSurfaceRenderNode::GetDirtyManager() const
{
    return dirtyManager_;
}

void RSSurfaceRenderNode::SetContextMatrix(const SkMatrix& matrix, bool sendMsg)
{
    if (contextMatrix_ == matrix) {
        return;
    }
    contextMatrix_ = matrix;
    SetDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextMatrix>(GetId(), matrix);
    SendCommandFromRT(command, GetId());
}

const SkMatrix& RSSurfaceRenderNode::GetContextMatrix() const
{
    return contextMatrix_;
}

void RSSurfaceRenderNode::SetContextAlpha(float alpha, bool sendMsg)
{
    if (contextAlpha_ == alpha) {
        return;
    }
    contextAlpha_ = alpha;
    SetDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextAlpha>(GetId(), alpha);
    SendCommandFromRT(command, GetId());
}

float RSSurfaceRenderNode::GetContextAlpha() const
{
    return contextAlpha_;
}

void RSSurfaceRenderNode::SetContextClipRegion(SkRect clipRegion, bool sendMsg)
{
    if (contextClipRect_ == clipRegion) {
        return;
    }
    contextClipRect_ = clipRegion;
    SetDirty();
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextClipRegion>(GetId(), clipRegion);
    SendCommandFromRT(command, GetId());
}

const SkRect& RSSurfaceRenderNode::GetContextClipRegion() const
{
    return contextClipRect_;
}

void RSSurfaceRenderNode::SetSecurityLayer(bool isSecurityLayer)
{
    isSecurityLayer_ = isSecurityLayer;
}

bool RSSurfaceRenderNode::GetSecurityLayer() const
{
    return isSecurityLayer_;
}

#ifdef ROSEN_OHOS
void RSSurfaceRenderNode::SetColorSpace(ColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

ColorGamut RSSurfaceRenderNode::GetColorSpace() const
{
    return colorSpace_;
}

void RSSurfaceRenderNode::UpdateSurfaceDefaultSize(float width, float height)
{
    if (consumer_ != nullptr) {
        consumer_->SetDefaultWidthAndHeight(width, height);
    }
}

GraphicBlendType RSSurfaceRenderNode::GetBlendType()
{
    return blendType_;
}

void RSSurfaceRenderNode::SetBlendType(GraphicBlendType blendType)
{
    blendType_ = blendType;
}

void RSSurfaceRenderNode::RegisterBufferAvailableListener(
    sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread)
{
    if (isFromRenderThread) {
        std::lock_guard<std::mutex> lock(mutexRT_);
        callbackFromRT_ = callback;
    } else {
        std::lock_guard<std::mutex> lock(mutexUI_);
        callbackFromUI_ = callback;
    }
}
#endif

void RSSurfaceRenderNode::ConnectToNodeInRenderService()
{
#ifdef ROSEN_OHOS
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
#endif
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
#ifdef ROSEN_OHOS
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
#endif
}

void RSSurfaceRenderNode::NotifyUIBufferAvailable()
{
    if (isNotifyUIBufferAvailable_) {
        return;
    }
    isNotifyUIBufferAvailable_ = true;
#ifdef ROSEN_OHOS
    {
        std::lock_guard<std::mutex> lock(mutexUI_);
        if (callbackFromUI_) {
            ROSEN_LOGD("RSSurfaceRenderNode::NotifyUIBufferAvailable nodeId = %" PRIu64, GetId());
            callbackFromUI_->OnBufferAvailable();
        } else {
            isNotifyUIBufferAvailable_ = false;
        }
    }
#endif
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailable() const
{
    return isNotifyRTBufferAvailable_;
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailablePre() const
{
    return isNotifyRTBufferAvailablePre_;
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

void RSSurfaceRenderNode::SetVisibleRegionRecursive(const Occlusion::Region& region,
                                                    VisibleData& visibleVec,
                                                    std::map<uint32_t, bool>& pidVisMap)
{
    if (nodeType_ == RSSurfaceNodeType::SELF_DRAWING_NODE ||
        nodeType_ == RSSurfaceNodeType::ABILITY_COMPONENT_NODE) {
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
    if (r.IsEmpty()) {
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

Occlusion::Region RSSurfaceRenderNode::ResetOpaqueRegion(const RectI& absRect,
    const ContainerWindowConfigType containerWindowConfigType, const bool isFocusWindow)
{
    if (containerWindowConfigType == ContainerWindowConfigType::DISABLED) {
        Occlusion::Rect opaqueRect{absRect};
        Occlusion::Region opaqueRegion = Occlusion::Region{opaqueRect};
        return opaqueRegion;
    }
    if (isFocusWindow) {
        Occlusion::Rect opaqueRect{ absRect.left_ + containerContentPadding_ + containerBorderWidth_,
            absRect.top_ + containerTitleHeight_ + containerInnerRadius_ + containerBorderWidth_,
            absRect.GetRight() - containerContentPadding_ - containerBorderWidth_,
            absRect.GetBottom() - containerContentPadding_ - containerBorderWidth_};
        Occlusion::Region opaqueRegion{opaqueRect};
        return opaqueRegion;
    } else {
        if (containerWindowConfigType == ContainerWindowConfigType::ENABLED_LEVEL_0) {
            Occlusion::Rect opaqueRect{ absRect.left_ + containerContentPadding_ + containerBorderWidth_,
                absRect.top_ + containerTitleHeight_ + containerBorderWidth_,
                absRect.GetRight() - containerContentPadding_ - containerBorderWidth_,
                absRect.GetBottom() - containerContentPadding_ - containerBorderWidth_};
            Occlusion::Region opaqueRegion{opaqueRect};
            return opaqueRegion;
        } else if (containerWindowConfigType == ContainerWindowConfigType::ENABLED_UNFOCUSED_WINDOW_LEVEL_1) {
            Occlusion::Rect opaqueRect{ absRect.left_,
                absRect.top_ + containerOutRadius_,
                absRect.GetRight(),
                absRect.GetBottom() - containerOutRadius_};
            Occlusion::Region opaqueRegion{opaqueRect};
            return opaqueRegion;
        } else {
            Occlusion::Rect opaqueRect1{ absRect.left_ + containerOutRadius_,
                absRect.top_,
                absRect.GetRight() - containerOutRadius_,
                absRect.GetBottom()};
            Occlusion::Rect opaqueRect2{ absRect.left_,
                absRect.top_ + containerOutRadius_,
                absRect.GetRight(),
                absRect.GetBottom() - containerOutRadius_};
            Occlusion::Region r1{opaqueRect1};
            Occlusion::Region r2{opaqueRect2};
            Occlusion::Region opaqueRegion = r1.Or(r2);
            return opaqueRegion;
        }
    }
}

void RSSurfaceRenderNode::ResetSurfaceOpaqueRegion(const RectI& screeninfo, const RectI& absRect,
    ContainerWindowConfigType containerWindowConfigType, bool isFocusWindow)
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
            opaqueRegion_ = ResetOpaqueRegion(absRect, containerWindowConfigType, isFocusWindow);
        } else {
            opaqueRegion_ = Occlusion::Region{absRectR};
        }
        transparentRegion_.SubSelf(opaqueRegion_);
    }
    Occlusion::Rect screen{screeninfo};
    Occlusion::Region screenRegion{screen};
    transparentRegion_.AndSelf(screenRegion);
    opaqueRegion_.AndSelf(screenRegion);
    opaqueRegionChanged_ = !oldOpaqueRegion.Xor(opaqueRegion_).IsEmpty();
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

} // namespace Rosen
} // namespace OHOS
