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

#ifndef RS_HARDWARE_PROCESSOR_H
#define RS_HARDWARE_PROCESSOR_H

#include <vector>

#include "display_type.h"
#include "hdi_backend.h"
#include "hdi_layer_info.h"
#include "hdi_screen.h"
#include <surface.h>

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_render_service_util.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {

class RSHardwareProcessor : public RSProcessor {
public:
    RSHardwareProcessor();
    ~RSHardwareProcessor() override;
    void ProcessSurface(RSSurfaceRenderNode& node) override;
    void ProcessSurface(RSDisplayRenderNode& node) override;
    void Init(ScreenId id, int32_t offsetX, int32_t offsetY) override;
    void PostProcess() override;
    void CropLayers();

private:
    void Redraw(sptr<Surface>& surface, const struct PrepareCompleteParam& param, void* data);
    void OnRotate();
    void CalculateSrcRect(ComposeInfo& info, const Vector4f& ratio);
    void ScaleDownLayers();
    HdiBackend* backend_ = nullptr;
    sptr<RSScreenManager> screenManager_;
    ScreenInfo currScreenInfo_;
    std::shared_ptr<HdiOutput> output_;
    std::vector<LayerInfoPtr> layers_;
    ScreenRotation rotation_ {ScreenRotation::ROTATION_0};
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif