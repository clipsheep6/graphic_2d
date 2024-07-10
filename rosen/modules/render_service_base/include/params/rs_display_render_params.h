/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_DISPLAY_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_DISPLAY_RENDER_PARAMS_H

#include <memory>

#include "common/rs_macros.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_node.h"
#include "screen_manager/rs_screen_info.h"
#include "pipeline/rs_surface_render_node.h"
namespace OHOS::Rosen {
class RSB_EXPORT RSDisplayRenderParams : public RSRenderParams {
public:
    explicit RSDisplayRenderParams(NodeId id);
    ~RSDisplayRenderParams() override = default;

    void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    std::vector<RSBaseRenderNode::SharedPtr>& GetAllMainAndLeashSurfaces();
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& GetAllMainAndLeashSurfaceDrawables();
    void SetAllMainAndLeashSurfaces(std::vector<RSBaseRenderNode::SharedPtr>& allMainAndLeashSurfaces);
    void SetAllMainAndLeashSurfaceDrawables(
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allMainAndLeashSurfaces);
    int32_t GetDisplayOffsetX() const
    {
        return offsetX_;
    }
    int32_t GetDisplayOffsetY() const
    {
        return offsetY_;
    }
    uint64_t GetScreenId() const
    {
        return screenId_;
    }
    uint64_t GetMirroredId() const
    {
        return mirroredId_;
    }
    const ScreenInfo& GetScreenInfo() const
    {
        return screenInfo_;
    }
    std::weak_ptr<RSDisplayRenderNode> GetMirrorSource()
    {
        return mirrorSource_;
    }
    NodeId GetMirrorSourceId() const
    {
        return mirrorSourceId_;
    }
    RSDisplayRenderNode::CompositeType GetCompositeType() const
    {
        return compositeType_;
    };
    ScreenRotation GetScreenRotation() const override
    {
        return screenRotation_;
    }
    ScreenRotation GetNodeRotation() const
    {
        return nodeRotation_;
    }
    const std::map<ScreenId, bool>& GetDisplayHasSecSurface() const
    {
        return displayHasSecSurface_;
    }
    const std::map<ScreenId, bool>& GetDisplayHasSkipSurface() const
    {
        return displayHasSkipSurface_;
    }
    const std::map<ScreenId, bool>& GetDisplayHasProtectedSurface() const
    {
        return displayHasProtectedSurface_;
    }
    const std::map<ScreenId, bool>& GethasCaptureWindow() const
    {
        return hasCaptureWindow_;
    }
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetHardwareEnabledNodes()
    {
        return hardwareEnabledNodes_;
    }
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetHardwareEnabledTopNodes()
    {
        return hardwareEnabledTopNodes_;
    }
    const sptr<SurfaceBuffer>& GetBuffer() const
    {
        return buffer_;
    }
    const sptr<SyncFence>& GetAcquireFence() const
    {
        return acquireFence_;
    }
    const sptr<IConsumerSurface>& GetConsumer() const
    {
        return consumer_;
    }
    bool GetSecurityDisplay() const
    {
        return isSecurityDisplay_;
    }
    bool IsFirstTimeToProcessor() const {
        return isFirstTimeToProcessor_;
    }
    void SetOriginScreenRotation(const ScreenRotation& rotate) {
        originScreenRotation_ = rotate;
        isFirstTimeToProcessor_ = false;
    }
    ScreenRotation GetOriginScreenRotation() const {
        return originScreenRotation_;
    }
#ifdef NEW_RENDER_CONTEXT
    void SetVirtualSurface(std::shared_ptr<RSRenderSurface>& virtualSurface, uint64_t pSurfaceUniqueId)
    {
        virtualSurface_ = virtualSurface;
        virtualSurfaceUniqueId_ = pSurfaceUniqueId;
    }
    std::shared_ptr<RSRenderSurface> GetVirtualSurface(uint64_t pSurfaceUniqueId)
    {
        return virtualSurfaceUniqueId_ != pSurfaceUniqueId ? nullptr : virtualSurface_;
    }
#else
    void SetVirtualSurface(std::shared_ptr<RSSurface>& virtualSurface, uint64_t pSurfaceUniqueId)
    {
        virtualSurface_ = virtualSurface;
        virtualSurfaceUniqueId_ = pSurfaceUniqueId;
    }
    std::shared_ptr<RSSurface> GetVirtualSurface(uint64_t pSurfaceUniqueId)
    {
        return virtualSurfaceUniqueId_ != pSurfaceUniqueId ? nullptr : virtualSurface_;
    }
#endif
    void SetMainAndLeashSurfaceDirty(bool isDirty);
    bool GetMainAndLeashSurfaceDirty() const;
    bool HasSecurityLayer();
    bool HasSkipLayer();
    bool HasProtectedLayer();
    bool HasCaptureWindow();
    void SetNeedOffscreen(bool needOffscreen);
    bool GetNeedOffscreen() const;

    void SetRotationChanged(bool changed);
    bool IsRotationChanged() const;

    void SetHDRPresent(bool hasHdrPresent);
    bool GetHDRPresent() const;

    void SetNewColorSpace(const GraphicColorGamut& newColorSpace);
    GraphicColorGamut GetNewColorSpace() const;
    void SetNewPixelFormat(const GraphicPixelFormat& newPixelFormat);
    GraphicPixelFormat GetNewPixelFormat() const;

    // dfx
    std::string ToString() const override;

private:
    std::map<ScreenId, bool> displayHasSecSurface_;
    std::map<ScreenId, bool> displayHasSkipSurface_;
    std::map<ScreenId, bool> displayHasProtectedSurface_;
    std::map<ScreenId, bool> hasCaptureWindow_;
    std::vector<RSBaseRenderNode::SharedPtr> allMainAndLeashSurfaces_;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allMainAndLeashSurfaceDrawables_;
    int32_t offsetX_ = -1;
    int32_t offsetY_ = -1;
    ScreenRotation nodeRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    ScreenRotation screenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    uint64_t screenId_ = 0;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    sptr<SyncFence> acquireFence_ = nullptr;
    sptr<IConsumerSurface> consumer_ = nullptr;
    bool isSecurityDisplay_ = false;
    bool isFirstTimeToProcessor_ = false;
    ScreenRotation originScreenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    uint64_t virtualSurfaceUniqueId_ = 0;
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> virtualSurface_ = nullptr;
#else
    std::shared_ptr<RSSurface> virtualSurface_ = nullptr;
#endif
    std::weak_ptr<RSDisplayRenderNode> mirrorSource_;
    NodeId mirrorSourceId_ = INVALID_NODEID;
    ScreenInfo screenInfo_;
    ScreenId mirroredId_ = INVALID_SCREEN_ID;
    RSDisplayRenderNode::CompositeType compositeType_ = RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE;
    bool isMainAndLeashSurfaceDirty_ = false;
    bool needOffscreen_ = false;
    bool isRotationChanged_ = false;
    bool hasHdrPresent_ = false;

    friend class RSUniRenderVisitor;
    friend class RSDisplayRenderNode;
    
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_;
    // vector of hardwareEnabled nodes above displayNodeSurface like pointer window
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledTopNodes_;
    GraphicColorGamut newColorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    GraphicPixelFormat newPixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_DISPLAY_RENDER_PARAMS_H
