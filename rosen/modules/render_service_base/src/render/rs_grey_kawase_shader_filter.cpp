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
#include "render/rs_grey_kawase_shader_filter.h"

#include "effect/color_matrix.h"
#include "effect/runtime_shader_builder.h"
#include "include/gpu/GrDirectContext.h"
#include "platform/common/rs_system_properties.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "src/core/SkOpts.h"


namespace OHOS {
namespace Rosen {
RSGreyKawaseShaderFilter::RSGreyKawaseShaderFilter(int radius, float greyCoefLow, float greyCoefHigh)
    : radius_(radius), greyCoefLow_(greyCoefLow), greyCoefHigh_(greyCoefHigh)
{
    type_ = ShaderFilterType::GREYKAWASE;
    hash_ = SkOpts::hash(&radius_, sizeof(radius_), 0);
    hash_ = SkOpts::hash(&greyCoefLow_, sizeof(greyCoefLow_), hash_);
    hash_ = SkOpts::hash(&greyCoefHigh_, sizeof(greyCoefHigh_), hash_);
}

RSGreyKawaseShaderFilter::~RSGreyKawaseShaderFilter() = default;

int RSGreyKawaseShaderFilter::GetRadius() const
{
    return radius_;
}

float RSGreyKawaseShaderFilter::GetGreyCoefLow() const
{
    return greyCoefLow_;
}

float RSGreyKawaseShaderFilter::GetGreyCoefHigh() const
{
    return greyCoefHigh_;
}

void RSGreyKawaseShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto kawaseFilter = std::make_shared<Drawing::GEVisualEffect>("GREY_KAWASE", Drawing::DrawingPaintType::BRUSH);
    kawaseFilter->SetParam("GREY_KAWASE_RADIUS", (int)radius_); // blur radius
    kawaseFilter->SetParam("GREY_KAWASE_COEF_1", greyCoefLow_);
    kawaseFilter->SetParam("GREY_KAWASE_COEF_2", greyCoefHigh_);
    visualEffectContainer->AddToChainedFilter(kawaseFilter);
}
} // namespace Rosen
} // namespace OHOS