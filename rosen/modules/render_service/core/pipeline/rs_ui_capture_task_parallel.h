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

#ifndef RS_UI_CAPTURE_TASK_PARALLEL
#define RS_UI_CAPTURE_TASK_PARALLEL
#define EGL_EGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include "drawable/rs_render_node_drawable.h"
#include "pipeline/rs_surface_capture_task.h"
#include "pixel_map.h"
#include "system/rs_system_parameters.h"

namespace OHOS {
namespace Rosen {
class RSUiCaptureTaskParallel {
public:
    explicit RSUiCaptureTaskParallel(NodeId nodeId, const RSSurfaceCaptureConfig& captureConfig)
        : nodeId_(nodeId), captureConfig_(captureConfig) {}
    ~RSUiCaptureTaskParallel() = default;

    static void Capture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig);

    bool CreateResources();
    bool Run(sptr<RSISurfaceCaptureCallback> callback);

private:
    std::shared_ptr<Drawing::Surface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap) const;
    std::unique_ptr<Media::PixelMap> CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const;

    std::unique_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::shared_ptr<DrawableV2::RSRenderNodeDrawable> nodeDrawable_ = nullptr;
    NodeId nodeId_;
    RSSurfaceCaptureConfig captureConfig_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_UI_CAPTURE_TASK_PARALLEL
