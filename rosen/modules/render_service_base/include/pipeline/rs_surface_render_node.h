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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H

#include <functional>
#include <memory>
#include <surface.h>

#include "display_type.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "include/core/SkRect.h"
#include "pipeline/rs_surface_handler.h"
#include "refbase.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class RSCommand;
class RSSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::SURFACE_NODE;

    explicit RSSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    explicit RSSurfaceRenderNode(const RSSurfaceRenderNodeConfig& config, std::weak_ptr<RSContext> context = {});
    virtual ~RSSurfaceRenderNode();

    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override;
    void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override;
    void ConsumeNodeNotOnTree();

    std::string GetName() const
    {
        return name_;
    }

    void SetOffSetX(int32_t offset)
    {
        offsetX_ = offset;
    }

    int32_t GetOffSetX()
    {
        return offsetX_;
    }

    void SetOffSetY(int32_t offset)
    {
        offsetY_ = offset;
    }

    int32_t GetOffSetY()
    {
        return offsetY_;
    }

    void SetOffset(int32_t offsetX, int32_t offsetY)
    {
        offsetX_ = offsetX;
        offsetY_ = offsetY;
    }

    void SetProxy()
    {
        isProxy_ = true;
    }
    bool IsProxy() const
    {
        return isProxy_;
    }

    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override;
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override;

    RSRenderNodeType GetType() const override
    {
        return RSRenderNodeType::SURFACE_NODE;
    }

    void SetMatrix(const SkMatrix& transform, bool sendMsg = true);
    const SkMatrix& GetMatrix() const;

    void SetAlpha(float alpha, bool sendMsg = true);
    float GetAlpha() const;

    void SetClipRegion(Vector4f clipRegion, bool sendMsg = true);

    void SetSecurityLayer(bool isSecurityLayer);
    bool GetSecurityLayer() const;

    const Vector4f& GetClipRegion() const
    {
        return clipRect_;
    }

    void SetDstRect(const RectI& dstRect)
    {
        dstRect_ = dstRect;
    }

    const RectI& GetDstRect() const
    {
        return dstRect_;
    }

    void SetGlobalAlpha(float alpha)
    {
        if (globalAlpha_ == alpha) {
        return;
        }
        globalAlpha_ = alpha;
    }

    float GetGlobalAlhpa() const
    {
        return globalAlpha_;
    }

    NodeId GetId() const override
    {
        return RSBaseRenderNode::GetId();
    }

    void SetParentId(NodeId parentId, bool sendMsg = true);
    NodeId GetParentId() const;

    void UpdateSurfaceDefaultSize(float width, float height);

    static void SendPropertyCommand(std::unique_ptr<RSCommand>& command);

    BlendType GetBlendType();
    void SetBlendType(BlendType blendType);

    // Only SurfaceNode in RS calls "RegisterBufferAvailableListener"
    // to save callback method sent by RT or UI which depands on the value of "isFromRenderThread".
    void RegisterBufferAvailableListener(
        sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread);

    // Only SurfaceNode in RT calls "ConnectToNodeInRenderService" to send callback method to RS
    void ConnectToNodeInRenderService();

    void NotifyRTBufferAvailable();
    bool IsNotifyRTBufferAvailable() const;

    void NotifyUIBufferAvailable();
    bool IsNotifyUIBufferAvailable() const;

    // UI Thread would not be notified when SurfaceNode created by Video/Camera in RenderService has available buffer.
    // And RenderThread does not call mainFunc_ if nothing in UI thread is changed
    // which would cause callback for "clip" on parent SurfaceNode cannot be triggered
    // for "clip" is executed in RenderThreadVisitor::ProcessSurfaceRenderNode.
    // To fix this bug, we set callback which would call RSRenderThread::RequestNextVSync() to forcely "refresh"
    // RenderThread when SurfaceNode in RenderService has available buffer and execute RSIBufferAvailableCallback.
    void SetCallbackForRenderThreadRefresh(std::function<void(void)> callback);
    bool NeedSetCallbackForRenderThreadRefresh();

private:
    RectI CalculateClipRegion(RSPaintFilterCanvas& canvas);
    friend class RSRenderTransition;

    std::mutex mutexRT_;
    std::mutex mutexUI_;
    std::mutex mutex_;
    SkMatrix matrix_;
    float alpha_ = 1.0f;
    bool isSecurityLayer_ = false;
    NodeId parentId_ = 0;
    RectI dstRect_;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    float globalAlpha_ = 1.0f;
    Vector4f clipRect_;
    std::string name_;
    bool isProxy_ = false;
    BlendType blendType_ = BlendType::BLEND_SRCOVER;
    std::atomic<bool> isNotifyRTBufferAvailable_ = false;
    std::atomic<bool> isNotifyUIBufferAvailable_ = false;
    sptr<RSIBufferAvailableCallback> callbackFromRT_;
    sptr<RSIBufferAvailableCallback> callbackFromUI_;
    std::function<void(void)> callbackForRenderThreadRefresh_ = nullptr;
    RectI clipRegionFromParent_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_RENDER_NODE_H
