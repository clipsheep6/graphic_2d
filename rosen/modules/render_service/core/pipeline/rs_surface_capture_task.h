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
#define EGL_EGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include "common/rs_common_def.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkSurface.h"
#ifdef ROSEN_OHOS
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include <GLES/gl.h>
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#include "surface_buffer.h"
#include "window.h"
#endif
#else
#include "draw/canvas.h"
#include "draw/surface.h"
#include "utils/matrix.h"
#endif
#include "ipc_callbacks/surface_capture_callback.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pixel_map.h"
#include "rs_base_render_engine.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
bool CopyDataToPixelMap(sk_sp<SkImage> img, const std::unique_ptr<Media::PixelMap>& pixelmap);
#else
bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img, const std::unique_ptr<Media::PixelMap>& pixelmap);
#endif
class RSSurfaceCaptureVisitor : public RSNodeVisitor {
    public:
        RSSurfaceCaptureVisitor(float scaleX, float scaleY, bool isUniRender);
        ~RSSurfaceCaptureVisitor() noexcept override = default;
        void PrepareChildren(RSRenderNode& node) override {}
        void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
        void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {}
        void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
        void PrepareRootRenderNode(RSRootRenderNode& node) override {}
        void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
        void PrepareEffectRenderNode(RSEffectRenderNode& node) override {}

        void ProcessChildren(RSRenderNode& node) override;
        void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
        void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override;
        void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
        void ProcessRootRenderNode(RSRootRenderNode& node) override;
        void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
        void ProcessEffectRenderNode(RSEffectRenderNode& node) override;

#ifndef USE_ROSEN_DRAWING
        void SetSurface(SkSurface* surface);
#else
        void SetSurface(Drawing::Surface* surface);
#endif
        void IsDisplayNode(bool isDisplayNode)
        {
            isDisplayNode_ = isDisplayNode;
        }

        bool IsUniRender() const
        {
            return isUniRender_;
        }

        bool GetHasingSecurityOrSkipLayer() const
        {
            return hasSecurityOrSkipLayer_;
        }

        void SetHasingSecurityOrSkipLayer(const bool &hasSecurityOrSkipLayer)
        {
            hasSecurityOrSkipLayer_ = hasSecurityOrSkipLayer;
        }

    private:
        void ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode& node);
        void CaptureSingleSurfaceNodeWithUni(RSSurfaceRenderNode& node);
        void CaptureSurfaceInDisplayWithUni(RSSurfaceRenderNode& node);
        void ProcessSurfaceRenderNodeWithoutUni(RSSurfaceRenderNode& node);
        void CaptureSingleSurfaceNodeWithoutUni(RSSurfaceRenderNode& node);
        void CaptureSurfaceInDisplayWithoutUni(RSSurfaceRenderNode& node);
        void DrawWatermarkIfNeed(RSDisplayRenderNode& node);
        void FindHardwareEnabledNodes();
        void AdjustZOrderAndDrawSurfaceNode();
        // Reuse DrawSpherize function in RSUniRenderVisitor.
        // Since the surfaceCache has been updated by the main screen drawing,
        // the updated surfaceCache can be reused directly without reupdating.
        void DrawSpherize(RSRenderNode& node);
        // Reuse DrawChildRenderNode function partially in RSUniRenderVisitor.
        void DrawChildRenderNode(RSRenderNode& node);
        // Reuse UpdateCacheRenderNodeMapWithBlur function partially in RSUniRenderVisitor and rename it.
        void ProcessCacheFilterRects(RSRenderNode& node);
        // Reuse DrawBlurInCache function partially in RSUniRenderVisitor.
        bool DrawBlurInCache(RSRenderNode& node);
        std::unique_ptr<RSPaintFilterCanvas> canvas_ = nullptr;
        bool isDisplayNode_ = false;
        float scaleX_ = 1.0f;
        float scaleY_ = 1.0f;
        bool isUniRender_ = false;
        bool hasSecurityOrSkipLayer_ = false;
        bool isUIFirst_ = false;
        std::unordered_set<NodeId> curCacheFilterRects_ = {};

#ifndef USE_ROSEN_DRAWING
        SkMatrix captureMatrix_ = SkMatrix::I();
#else
        Drawing::Matrix captureMatrix_ = Drawing::Matrix();
#endif

        std::shared_ptr<RSBaseRenderEngine> renderEngine_;

        std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_;
};

class RSSurfaceCaptureTask {
public:
    explicit RSSurfaceCaptureTask(NodeId nodeId, float scaleX, float scaleY)
        : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY) {}
    ~RSSurfaceCaptureTask() = default;

    bool Run(sptr<RSISurfaceCaptureCallback> callback);

private:
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor_ = nullptr;

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap);
#else
    std::shared_ptr<Drawing::Surface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap);
#endif

    std::unique_ptr<Media::PixelMap> CreatePixelMapBySurfaceNode(std::shared_ptr<RSSurfaceRenderNode> node,
        bool isUniRender = false);

    std::unique_ptr<Media::PixelMap> CreatePixelMapByDisplayNode(std::shared_ptr<RSDisplayRenderNode> node,
        bool isUniRender = false, bool hasSecurityOrSkipLayer = false);

    bool FindSecurityOrSkipLayer();

    // It is currently only used on folding screen.
    int32_t ScreenCorrection(ScreenRotation screenRotation);

    NodeId nodeId_;

    float scaleX_;

    float scaleY_;

    ScreenRotation screenCorrection_ = ScreenRotation::ROTATION_0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SURFACE_CAPTURE_TASK
