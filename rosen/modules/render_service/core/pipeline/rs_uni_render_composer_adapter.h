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

#ifndef RS_CORE_PIPELINE_UNI_RENDER_COMPOSER_ADAPTER_H
#define RS_CORE_PIPELINE_UNI_RENDER_COMPOSER_ADAPTER_H

#include "hdi_backend.h"
#include "rs_composer_adapter.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_base_render_util.h"
#include "rs_hardware_thread.h"
#include "rs_main_thread.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {
class RSComposerAdapter;
class RSRcdSurfaceRenderNode;
namespace DrawableV2 {
class RSDisplayRenderNodeDrawable;
class RSSurfaceRenderNodeDrawable;
}
class RSUniRenderComposerAdapter {
public:
    RSUniRenderComposerAdapter() = default;
    ~RSUniRenderComposerAdapter() noexcept = default;
    LayerInfoPtr CreateLayer(RSDisplayRenderNode& node);
    LayerInfoPtr CreateLayer(RSSurfaceRenderNode& node) const;
    LayerInfoPtr CreateLayer(RSRcdSurfaceRenderNode& node);
    bool Init(const ScreenInfo& screenInfo, int32_t offsetX, int32_t offsetY, float mirrorAdaptiveCoefficient);

    LayerInfoPtr CreateLayer(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
    LayerInfoPtr CreateLayer(DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable) const;
    void CommitLayers(const std::vector<LayerInfoPtr>& layers);
    void SetMetaDataInfoToLayer(const LayerInfoPtr& layer, const sptr<SurfaceBuffer>& buffer,
        const sptr<IConsumerSurface>& surface) const;
private:
    bool IsOutOfScreenRegion(const ComposeInfo& info) const;
    static RectI SrcRectRotateTransform(RSSurfaceRenderNode& node);
    static RectI SrcRectRotateTransform(DrawableV2::RSSurfaceRenderNodeDrawable& node);

    ComposeInfo BuildComposeInfo(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
    ComposeInfo BuildComposeInfo(RSSurfaceRenderNode& node) const;
    ComposeInfo BuildComposeInfo(DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable) const;
    ComposeInfo BuildComposeInfo(RSRcdSurfaceRenderNode& node) const;

    void SetComposeInfoToLayer(
        const LayerInfoPtr& layer,
        const ComposeInfo& info,
        const sptr<IConsumerSurface>& surface) const;
    static void SetBufferColorSpace(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
    void LayerRotate(const LayerInfoPtr& layer, RSSurfaceRenderNode& node) const;
    void LayerRotate(const LayerInfoPtr& layer, DrawableV2::RSRenderNodeDrawableAdapter& node) const;
    void DealWithNodeGravity(const RSSurfaceRenderNode& node, ComposeInfo& info) const;
    void DealWithNodeGravity(const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, ComposeInfo& info) const;
    LayerInfoPtr CreateBufferLayer(RSSurfaceRenderNode& node) const;
    LayerInfoPtr CreateBufferLayer(DrawableV2::RSSurfaceRenderNodeDrawable& node) const;

    void LayerCrop(const LayerInfoPtr& layer) const;
    static void LayerScaleDown(const LayerInfoPtr& layer, RSSurfaceRenderNode& node);
    static void LayerScaleDown(const LayerInfoPtr& layer, DrawableV2::RSSurfaceRenderNodeDrawable& node);
    void LayerScaleFit(const LayerInfoPtr& layer) const;
    static void LayerPresentTimestamp(const LayerInfoPtr& layer, const sptr<Surface>& surface);

    static void GetComposerInfoSrcRect(ComposeInfo &info, const RSSurfaceRenderNode& node);
    static void GetComposerInfoSrcRect(
        ComposeInfo& info, const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable);
    bool GetComposerInfoNeedClient(const ComposeInfo& info, RSRenderParams& params) const;
    bool CheckStatusBeforeCreateLayer(RSSurfaceRenderNode& node) const;
    bool CheckStatusBeforeCreateLayer(DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable) const;
    void SetPreBufferInfo(RSSurfaceHandler& surfaceHandler, ComposeInfo& info) const;

    std::shared_ptr<HdiOutput> output_;
    ScreenInfo screenInfo_;
    HdiBackend *hdiBackend_ = nullptr;
    // The offset on dst screen for all layers.
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    float mirrorAdaptiveCoefficient_ = 1.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_UNI_RENDER_COMPOSER_ADAPTER_H
