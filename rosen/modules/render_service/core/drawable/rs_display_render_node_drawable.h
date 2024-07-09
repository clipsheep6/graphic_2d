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

#ifndef RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_occlusion_region.h"
#include "drawable/rs_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSDisplayRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    ~RSDisplayRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;
    void DrawHardwareEnabledNodes(Drawing::Canvas& canvas,
        std::shared_ptr<RSDisplayRenderNode> displayNodeSp, RSDisplayRenderParams* params);
    void DrawHardwareEnabledNodes(Drawing::Canvas& canvas);
    void DrawHardwareEnabledNodesMissedInCacheImage(Drawing::Canvas& canvas);
    void SwitchColorFilter(RSPaintFilterCanvas& canvas) const;
    void SetHighContrastIfEnabled(RSPaintFilterCanvas& canvas) const;


    const std::shared_ptr<RSSurfaceHandler> GetRSSurfaceHandlerOnDraw() const
    {
        return surfaceHandler_;
    }

    std::shared_ptr<RSSurfaceHandler> GetMutableRSSurfaceHandlerOnDraw()
    {
        return surfaceHandler_;
    }


#ifndef ROSEN_CROSS_PLATFORM
    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    sptr<IBufferConsumerListener> GetConsumerListener() const
    {
        return consumerListener_;
    }
#endif
    bool IsSurfaceCreated() const
    {
        return surfaceCreated_;
    }

#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> GetRSSurface() const
    {
        return surface_;
    }
    void SetVirtualSurface(std::shared_ptr<RSRenderSurface>& virtualSurface)
    {
        virtualSurface_ = virtualSurface;
    }
    std::shared_ptr<RSRenderSurface> GetVirtualSurface()
    {
        return virtualSurface_;
    }
#else
    std::shared_ptr<RSSurface> GetRSSurface() const
    {
        return surface_;
    }
    void SetVirtualSurface(std::shared_ptr<RSSurface>& virtualSurface)
    {
        virtualSurface_ = virtualSurface;
    }
    std::shared_ptr<RSSurface> GetVirtualSurface()
    {
        return virtualSurface_;
    }
#endif

private:
    explicit RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    bool CheckDisplayNodeSkip(std::shared_ptr<RSDisplayRenderNode> displayNode, RSDisplayRenderParams* params,
        std::shared_ptr<RSProcessor> processor);
    std::unique_ptr<RSRenderFrame> RequestFrame(std::shared_ptr<RSDisplayRenderNode> displayNodeSp,
        RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor);
    void FindHardwareEnabledNodes();
    void AdjustZOrderAndDrawSurfaceNode(
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodes,
        Drawing::Canvas& canvas, RSDisplayRenderParams& params) const;
    void WiredScreenProjection(std::shared_ptr<RSDisplayRenderNode> displayNodeSp, RSDisplayRenderParams& params,
        std::shared_ptr<RSProcessor> processor);
    void ScaleAndRotateMirrorForWiredScreen(RSDisplayRenderNode& node, RSDisplayRenderNode& mirroredNode);
    std::vector<RectI> CalculateVirtualDirtyForWiredScreen(RSDisplayRenderNode& displayNode,
        std::unique_ptr<RSRenderFrame>& renderFrame, RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix);
    void DrawWatermarkIfNeed(RSDisplayRenderParams& params, RSPaintFilterCanvas& canvas) const;
    void RotateMirrorCanvas(ScreenRotation& rotation, float mainWidth, float mainHeight);

    void DrawMirrorScreen(std::shared_ptr<RSDisplayRenderNode>& displayNode, RSDisplayRenderParams& params,
        std::shared_ptr<RSProcessor> processor);
    std::vector<RectI> CalculateVirtualDirty(RSDisplayRenderNode& displayNode,
        std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser,
        RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix);
    using DrawFuncPtr = void(RSDisplayRenderNodeDrawable::*)(Drawing::Canvas&);
    void DrawMirror(std::shared_ptr<RSDisplayRenderNode>& displayNode, RSDisplayRenderParams& params,
        std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, DrawFuncPtr drawFunc,
        RSRenderThreadParams& uniParam);
    void DrawExpandScreen(RSUniRenderVirtualProcessor& processor);
    void SetVirtualScreenType(RSDisplayRenderNode& node, const ScreenInfo& screenInfo);
    void DrawCurtainScreen() const;
    void RemoveClearMemoryTask() const;
    void PostClearMemoryTask() const;
    std::shared_ptr<Drawing::Image> GetCacheImageFromMirrorNode(
        std::shared_ptr<RSDisplayRenderNode> mirrorNode);
    void ResetRotateIfNeed(RSDisplayRenderNode& mirroredNode, RSUniRenderVirtualProcessor& mirroredProcessor,
        Drawing::Region& clipRegion);
    void ProcessCacheImage(Drawing::Image& cacheImageProcessed);
    void SetCanvasBlack(RSProcessor& processor);
    // Prepare for off-screen render
    void ClearTransparentBeforeSaveLayer();
    void PrepareOffscreenRender(const RSRenderNode& node);
    void FinishOffscreenRender(const Drawing::SamplingOptions& sampling);
    bool SkipDisplayIfScreenOff() const;
    bool CheckIfHasSpecialLayer(RSDisplayRenderParams& params);
    void SetDisplayNodeSkipFlag(RSRenderThreadParams& uniParam, bool flag);
    void CreateUIFirstLayer(std::shared_ptr<RSProcessor>& processor);

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::DISPLAY_NODE, OnGenerate>;
    static Registrar instance_;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_;
    std::shared_ptr<Drawing::Surface> offscreenSurface_; // temporary holds offscreen surface
    std::shared_ptr<RSPaintFilterCanvas> canvasBackup_; // backup current canvas before offscreen rende
    std::unordered_set<NodeId> virtualScreenBlackList_ = {};
    std::unordered_set<NodeId> castScreenBlackList_ = {};
    bool castScreenEnableSkipWindow_ = false;
    bool hasSpecialLayer_ = false;
    bool isLastFrameHasSecSurface_ = false;
    bool isDisplayNodeSkip_ = false;
    bool isDisplayNodeSkipStatusChanged_ = false;
    Drawing::Matrix lastMatrix_;
    Drawing::Matrix lastMirrorMatrix_;
    bool useFixedOffscreenSurfaceSize_ = false;

    bool surfaceCreated_ = false;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> surface_;
    std::shared_ptr<RSRenderSurface> virtualSurface_;
#else
    std::shared_ptr<RSSurface> surface_;
    std::shared_ptr<RSSurface> virtualSurface_;
#endif

#ifndef ROSEN_CROSS_PLATFORM
    sptr<IBufferConsumerListener> consumerListener_;
#endif
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H