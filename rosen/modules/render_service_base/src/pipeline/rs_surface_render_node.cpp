/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "command/rs_surface_node_command.h"
#include "common/rs_obj_abs_geometry.h"
#include "display_type.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "property/rs_transition_properties.h"
#include "transaction/rs_transaction_proxy.h"
#include "transaction/rs_render_service_client.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
static const int rectBounds = 2;

RSSurfaceRenderNode::RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context) : RSRenderNode(id, context) {}
RSSurfaceRenderNode::RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context)
    : RSRenderNode(config.id, context), name_(config.name)
{}

RSSurfaceRenderNode::~RSSurfaceRenderNode() {}

void RSSurfaceRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
    canvas.SaveAlpha();
    canvas.MultiplyAlpha(GetRenderProperties().GetAlpha() * GetContextAlpha());

    auto currentClipRegion0 = canvas.getDeviceClipBounds();
    auto matrix0 = canvas.getTotalMatrix();
    RS_LOGE("chenlu ProcessRenderBeforeChildren 0 name:%s tran[%f %f] clip[%d %d %d %d]",
        GetName().c_str(),
        matrix0.getTranslateX(), matrix0.getTranslateY(),
        currentClipRegion0.left(), currentClipRegion0.top(), currentClipRegion0.width(), currentClipRegion0.height());

    auto clipRectFromRT = GetContextClipRegion();
    canvas.concat(GetContextMatrix());
    if (!clipRectFromRT.isEmpty()){
        canvas.clipRect(clipRectFromRT);
    }

    auto currentClipRegion1 = canvas.getDeviceClipBounds();
    auto matrix1 = canvas.getTotalMatrix();
    RS_LOGE("chenlu ProcessRenderBeforeChildren 1 name:%s tran[%f %f] clip[%d %d %d %d] clipRT[%f %f %f %f] tranRT[%f %f]",
        GetName().c_str(),
        matrix1.getTranslateX(), matrix1.getTranslateY(),
        currentClipRegion1.left(), currentClipRegion1.top(), currentClipRegion1.width(), currentClipRegion1.height(),
        clipRectFromRT.left(), clipRectFromRT.top(), clipRectFromRT.width(), clipRectFromRT.height(),
        GetContextMatrix().getTranslateX(), GetContextMatrix().getTranslateY());

    RectI clipRegion = CalculateClipRegion(canvas);
    SkRect rect;
    auto currentClipRegion2 = canvas.getDeviceClipBounds();
    SkPoint points[] = {{clipRegion.left_, clipRegion.top_}, {clipRegion.GetRight(), clipRegion.GetBottom()}};
    rect.setBounds(points, rectBounds);
    RS_LOGE("chenlu ProcessRenderBeforeChildren 2 name:%s rect[%f %f %f %f] currentClipRegion2[%d %d %d %d]",
        GetName().c_str(),
        rect.left(), rect.top(), rect.width(), rect.height(),
        currentClipRegion2.left(), currentClipRegion2.top(), currentClipRegion2.width(), currentClipRegion2.height());
    canvas.clipRect(rect);
    auto currentClipRegion = canvas.getDeviceClipBounds();
    RS_LOGE("chenlu ProcessRenderBeforeChildren 3 name:%s currentClipRegion2[%d %d %d %d]",
        GetName().c_str(),
        currentClipRegion.left(), currentClipRegion.top(), currentClipRegion.width(), currentClipRegion.height());
    SetDstRect({ currentClipRegion.left(), currentClipRegion.top(), currentClipRegion.width(),
        currentClipRegion.height() });
    SetGlobalAlpha(canvas.GetAlpha()); 
}

RectI RSSurfaceRenderNode::CalculateClipRegion(RSPaintFilterCanvas& canvas)
{
    const RSProperties& properties = GetRenderProperties();
    RectI originDstRect(0, 0, properties.GetBoundsWidth(), properties.GetBoundsHeight());
    canvas.translate(properties.GetBoundsPositionX(), properties.GetBoundsPositionY());
    auto transitionProperties = GetAnimationManager().GetTransitionProperties();
    Vector2f center(originDstRect.width_ * 0.5f, originDstRect.height_ * 0.5f);
    RS_LOGE("chenlu CalculateClipRegion name:%s clip[%d %d %d %d] center[%f %f]",
        GetName().c_str(),
        properties.GetBoundsPositionX(), properties.GetBoundsPositionY(), originDstRect.width_, originDstRect.height_,
        center.x_, center.y_);
    RSPropertiesPainter::DrawTransitionProperties(transitionProperties, center, canvas);
    return originDstRect;
}

void RSSurfaceRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
    canvas.RestoreAlpha();
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
    visitor->ProcessSurfaceRenderNode(*this);
}

void RSSurfaceRenderNode::SetContextMatrix(const SkMatrix& matrix, bool sendMsg)
{
    if (contextMatrix_ == matrix) {
        return;
    }
    contextMatrix_ = matrix;
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextMatrix>(GetId(), matrix);
    SendCommandFromRT(command);
}

const SkMatrix& RSSurfaceRenderNode::GetContextMatrix() const
{
    return contextMatrix_;
}

void RSSurfaceRenderNode::SetSrcRatio(const Vector4f ratio, bool sendMsg)
{
    if (srcRatio_ == ratio) {
        return;
    }
    srcRatio_ = ratio;
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetSrcRatio>(GetId(), ratio);
    SendCommandFromRT(command);
}

const Vector4f& RSSurfaceRenderNode::GetSrcRatio() const
{
    return srcRatio_;
}

void RSSurfaceRenderNode::SetContextAlpha(float alpha, bool sendMsg)
{
    if (contextAlpha_ == alpha) {
        return;
    }
    contextAlpha_ = alpha;
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextAlpha>(GetId(), alpha);
    SendCommandFromRT(command);
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
    if (!sendMsg) {
        return;
    }
    // send a Command
    std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeSetContextClipRegion>(GetId(), clipRegion);
    SendCommandFromRT(command);
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

void RSSurfaceRenderNode::UpdateSurfaceDefaultSize(float width, float height)
{
    consumer_->SetDefaultWidthAndHeight(width, height);
}

void RSSurfaceRenderNode::SendCommandFromRT(std::unique_ptr<RSCommand>& command)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommandFromRT(command);
    }
}

BlendType RSSurfaceRenderNode::GetBlendType()
{
    return blendType_;
}

void RSSurfaceRenderNode::SetBlendType(BlendType blendType)
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

void RSSurfaceRenderNode::ConnectToNodeInRenderService()
{
    ROSEN_LOGI("RSSurfaceRenderNode::ConnectToNodeInRenderService nodeId = %llu", GetId());
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
    if (isNotifyRTBufferAvailable_) {
        return;
    }
    isNotifyRTBufferAvailable_ = true;

    if (callbackForRenderThreadRefresh_) {
        ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %llu RenderThread", GetId());
        callbackForRenderThreadRefresh_();
    }

    {
        std::lock_guard<std::mutex> lock(mutexRT_);
        if (callbackFromRT_) {
            ROSEN_LOGI("RSSurfaceRenderNode::NotifyRTBufferAvailable nodeId = %llu RenderService", GetId());
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
            ROSEN_LOGI("RSSurfaceRenderNode::NotifyUIBufferAvailable nodeId = %llu", GetId());
            callbackFromUI_->OnBufferAvailable();
        } else {
            isNotifyUIBufferAvailable_ = false;
        }
    }
}

bool RSSurfaceRenderNode::IsNotifyRTBufferAvailable() const
{
    return isNotifyRTBufferAvailable_;
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

void RSSurfaceRenderNode::ConsumeNodeNotOnTree()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (GetAvailableBufferCount() <= 0) {
        return;
    }
    const auto& surfaceConsumer = GetConsumer();
    if (surfaceConsumer == nullptr) {
        RS_LOGE("RSSurfaceRenderNode::ConsumeNodesNotOnTree (node: %llu): surfaceConsumer is null!", GetId());
        return;
    }
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    auto ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
    if (cbuffer == nullptr || ret != SURFACE_ERROR_OK) {
        RS_LOGW("RSSurfaceRenderNode::ConsumeNodesNotOnTree: AcquireBuffer failed, (node: %llu):!", GetId());
        return;
    }
    ret = surfaceConsumer->ReleaseBuffer(cbuffer, SyncFence::INVALID_FENCE);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        RS_LOGW("RSSurfaceRenderNode::ConsumeNodesNotOnTree(node: %llu): ReleaseBuffer failed(ret: %d)",
            GetId(), ret);
    }
    SetBuffer(cbuffer);
    SetFence(acquireFence);
    RS_LOGI("RSSurfaceRenderNode::ConsumeNodesNotOnTree(node: %llu): consume buffer successfully (ret: %d)",
            GetId(), ret);
    ReduceAvailableBuffer();
}

} // namespace Rosen
} // namespace OHOS
