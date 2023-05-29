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

#ifndef RS_SURFACE_CAPTURE_TASK
#define RS_SURFACE_CAPTURE_TASK

#include "common/rs_common_def.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkSurface.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_recording_canvas.h"
#include "pixel_map.h"
#include "rs_base_render_engine.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceCaptureVisitor : public RSNodeVisitor {
    public:
        RSSurfaceCaptureVisitor(float scaleX, float scaleY, bool isUniRender);
        ~RSSurfaceCaptureVisitor() noexcept override = default;
        void PrepareBaseRenderNode(RSBaseRenderNode& node) override {}
        void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
        void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {}
        void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
        void PrepareRootRenderNode(RSRootRenderNode& node) override {}
        void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}

        void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
        void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
        void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override;
        void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
        void ProcessRootRenderNode(RSRootRenderNode& node) override;
        void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;

        void SetSurface(SkSurface* surface);
        void SetCanvas(std::shared_ptr<RSRecordingCanvas> canvas);
        void IsDisplayNode(bool isDisplayNode)
        {
            isDisplayNode_ = isDisplayNode;
        }

        void MarkForceCPU()
        {
            isForceCPU_ = true;
        }

        bool IsUniRender() const
        {
            return isUniRender_;
        }

        bool GetHasingSecurityLayer() const
        {
            return hasSecurityLayer_;
        }

        void SetHasingSecurityLayer(const bool &hasSecurityLayer)
        {
            hasSecurityLayer_ = hasSecurityLayer;
        }

    private:
        void ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode& node);
        void CaptureSingleSurfaceNodeWithUni(RSSurfaceRenderNode& node);
        void CaptureSurfaceInDisplayWithUni(RSSurfaceRenderNode& node);
        void ProcessSurfaceRenderNodeWithoutUni(RSSurfaceRenderNode& node);
        void CaptureSingleSurfaceNodeWithoutUni(RSSurfaceRenderNode& node);
        void CaptureSurfaceInDisplayWithoutUni(RSSurfaceRenderNode& node);
        void DrawWatermarkIfNeed(float screenWidth, float screenHeight);
        void FindHardwareEnabledNodes();
        void AdjustZOrderAndDrawSurfaceNode();
        std::unique_ptr<RSPaintFilterCanvas> canvas_ = nullptr;
        bool isDisplayNode_ = false;
        bool isForceCPU_ = false;
        float scaleX_ = 1.0f;
        float scaleY_ = 1.0f;
        bool isUniRender_ = false;
        bool hasSecurityLayer_ = false;

        std::shared_ptr<RSBaseRenderEngine> renderEngine_;

        std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_;
};
class RSSurfaceCaptureTask {
public:
    explicit RSSurfaceCaptureTask(NodeId nodeId, float scaleX, float scaleY)
        : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY) {}
    ~RSSurfaceCaptureTask() = default;

    std::unique_ptr<Media::PixelMap> Run();

private:
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor_ = nullptr;

    sk_sp<SkSurface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap);

    std::unique_ptr<Media::PixelMap> CreatePixelMapBySurfaceNode(std::shared_ptr<RSSurfaceRenderNode> node,
        bool isUniRender = false);

    std::unique_ptr<Media::PixelMap> CreatePixelMapByDisplayNode(std::shared_ptr<RSDisplayRenderNode> node,
        bool isUniRender = false, bool hasSecurityLayer = false);

    bool FindSecurityLayer();

    NodeId nodeId_;

    float scaleX_;

    float scaleY_;

    friend class RSRenderServiceConnection;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SURFACE_CAPTURE_TASK
