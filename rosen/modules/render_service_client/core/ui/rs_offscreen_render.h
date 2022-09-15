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

#ifndef RS_OFFSCREEN_RENDER
#define RS_OFFSCREEN_RENDER

#include "pixel_map.h"
#include "common/rs_common_def.h"
#include "include/core/SkCanvas.h"
#include "render/rs_skia_filter.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {

namespace Rosen {

class RS_EXPORT RSOffscreenRender {
public:
    RSOffscreenRender() = default;
    ~RSOffscreenRender() = default;
    std::shared_ptr<Media::PixelMap> GetLocalCapture(NodeId nodeId, float scaleX, float scaleY);

private:
    class RSOffscreenRenderVisitor : public RSNodeVisitor {
    public:
        RSOffscreenRenderVisitor(float scaleX, float scaleY, float pixelMapX, float pixelMapY);
        ~RSOffscreenRenderVisitor() noexcept override = default;
        virtual void PrepareBaseRenderNode(RSBaseRenderNode& node) override {}
        virtual void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {}
        virtual void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
        virtual void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
        virtual void PrepareRootRenderNode(RSRootRenderNode& node) override {}

        void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
        void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
        void ProcessRootRenderNode(RSRootRenderNode& node) override;
        void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override {}
        void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
        void SetSurface(SkSurface* surface);

    private:
        float scaleX_ = 1.0f;
        float scaleY_ = 1.0f;
        std::unique_ptr<RSPaintFilterCanvas> canvas_ = nullptr;
        float pixelMapX_;
        float pixelMapY_;
    };

    NodeId nodeId_;
    float scaleX_;
    float scaleY_;
    float pixelMapX_;
    float pixelMapY_; 
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_OFFSCREEN_RENDER