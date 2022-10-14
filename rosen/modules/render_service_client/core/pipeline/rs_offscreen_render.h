/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_RS_OFFSCREEN_RENDER
#define RENDER_SERVICE_CLIENT_CORE_RS_OFFSCREEN_RENDER

#include "common/rs_common_def.h"
#include "include/core/SkCanvas.h"
#include "render/rs_skia_filter.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {

class RSOffscreenRender {
public:
    RSOffscreenRender(float scaleX, float scaleY, NodeId nodeId) : scaleX_(scaleX), scaleY_(scaleY), nodeId_(nodeId) {};
    ~RSOffscreenRender() = default;
    std::shared_ptr<Media::PixelMap> GetLocalCapture();

private:
    std::shared_ptr<Media::PixelMap> CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node);
    sk_sp<SkSurface> CreateSurface(const std::shared_ptr<Media::PixelMap> pixelmap);

    class RSOffscreenRenderVisitor : public RSNodeVisitor {
    public:
        RSOffscreenRenderVisitor(float scaleX, float scaleY, NodeId nodeId);
        ~RSOffscreenRenderVisitor() noexcept override = default;
        void PrepareBaseRenderNode(RSBaseRenderNode& node) override {}
        void PrepareDisplayRenderNode(RSDisplayRenderNode& node) override {}
        void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
        void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
        void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
        void PrepareRootRenderNode(RSRootRenderNode& node) override {}

        void ProcessBaseRenderNode(RSBaseRenderNode& node) override;
        void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override;
        void ProcessRootRenderNode(RSRootRenderNode& node) override;
        void ProcessProxyRenderNode(RSProxyRenderNode& node) override {};
        void ProcessDisplayRenderNode(RSDisplayRenderNode& node) override {}
        void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override;
        void SetSurface(SkSurface* surface);

    private:
        float scaleX_ = 1.0f;
        float scaleY_ = 1.0f;
        NodeId nodeId_;
        std::unique_ptr<RSPaintFilterCanvas> canvas_ = nullptr;
    };

    float scaleX_;
    float scaleY_;
    NodeId nodeId_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_OFFSCREEN_RENDER
