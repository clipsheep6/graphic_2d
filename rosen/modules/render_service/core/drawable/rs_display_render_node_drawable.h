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

#include "common/rs_occlusion_region.h"
#include "drawable/rs_render_node_drawable.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_processor_factory.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
class RSDisplayRenderNodeDrawable : public RSRenderNodeDrawable {
public:
    explicit RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    ~RSDisplayRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) const override;

private:
    std::unique_ptr<RSRenderFrame> RequestFrame(std::shared_ptr<RSDisplayRenderNode> displayNodeSp,
        std::shared_ptr<RSProcessor> processor, ScreenInfo& screenInfo) const;

    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::DISPLAY_NODE, OnGenerate>;
    static Registrar instance_;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_;
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_DISPLAY_RENDER_NODE_DRAWABLE_H
