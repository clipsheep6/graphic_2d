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
#include "render/rs_mesa_blur_shader_filter.h"

#include "effect/color_matrix.h"
#include "effect/runtime_shader_builder.h"
#include "include/gpu/GrDirectContext.h"
#include "platform/common/rs_system_properties.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
RSMESABlurShaderFilter::RSMESABlurShaderFilter(int radius)
    : radius_(radius)
{
    type_ = ShaderFilterType::MESA;
    hash_ = SkOpts::hash(&radius_, sizeof(radius_), 0);
}

RSMESABlurShaderFilter::RSMESABlurShaderFilter(int radius, float greyCoefLow, float greyCoefHigh)
    : radius_(radius), greyCoefLow_(greyCoefLow), greyCoefHigh_(greyCoefHigh)
{
    type_ = ShaderFilterType::MESA;
    hash_ = SkOpts::hash(&radius_, sizeof(radius_), 0);
    hash_ = SkOpts::hash(&greyCoefLow_, sizeof(greyCoefLow_), hash_);
    hash_ = SkOpts::hash(&greyCoefHigh_, sizeof(greyCoefHigh_), hash_);
}

RSMESABlurShaderFilter::~RSMESABlurShaderFilter() = default;

int RSMESABlurShaderFilter::GetRadius() const
{
    return radius_;
}

void RSMESABlurShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto mesaFilter = std::make_shared<Drawing::GEVisualEffect>("MESA_BLUR", Drawing::DrawingPaintType::BRUSH);
    mesaFilter->SetParam("MESA_BLUR_RADIUS", (int)radius_); // blur radius
    mesaFilter->SetParam("MESA_BLUR_GREY_COEF_1", greyCoefLow_);
    mesaFilter->SetParam("MESA_BLUR_GREY_COEF_2", greyCoefHigh_);
    if (pixelStretchParam_) {
        mesaFilter->SetParam("OFFSET_X", pixelStretchParam_->offsetX_);
        mesaFilter->SetParam("OFFSET_Y", pixelStretchParam_->offsetY_);
        mesaFilter->SetParam("OFFSET_Z", pixelStretchParam_->offsetZ_);
        mesaFilter->SetParam("OFFSET_W", pixelStretchParam_->offsetW_);
        mesaFilter->SetParam("TILE_MODE", pixelStretchParam_->tileMode_);
        mesaFilter->SetParam("WIDTH", pixelStretchParam_->width_);
        mesaFilter->SetParam("HEIGHT", pixelStretchParam_->height_);
    } else {
        mesaFilter->SetParam("OFFSET_X", 0.f);
        mesaFilter->SetParam("OFFSET_Y", 0.f);
        mesaFilter->SetParam("OFFSET_Z", 0.f);
        mesaFilter->SetParam("OFFSET_W", 0.f);
        mesaFilter->SetParam("TILE_MODE", 0);
        mesaFilter->SetParam("WIDTH", 0.f);
        mesaFilter->SetParam("HEIGHT", 0.f);
    }
    visualEffectContainer->AddToChainedFilter(mesaFilter);
}
} // namespace Rosen
} // namespace OHOS