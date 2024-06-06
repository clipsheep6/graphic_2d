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
#include "render/rs_water_ripple_shader_filter.h"
 
#include "platform/common/rs_log.h"
#include "effect/color_matrix.h"
#include "effect/runtime_shader_builder.h"
#include "include/gpu/GrDirectContext.h"
#include "platform/common/rs_system_properties.h"
#include "src/core/SkOpts.h"
#include "property/rs_properties.h"
 
namespace OHOS {
namespace Rosen {
RSWaterRippleShaderFilter::RSWaterRippleShaderFilter(
    const float progress, const float waveNum, const float rippleCenterX, const float rippleCenterY)
{
    type_ = ShaderFilterType::WATER_RIPPLE;
    progress_ = progress;
    waveNum_ = waveNum;
    rippleCenterX_ = rippleCenterX;
    rippleCenterY_ = rippleCenterY;
    hash_ = SkOpts::hash(&progress_, sizeof(progress_), hash_);
    hash_ = SkOpts::hash(&waveNum_, sizeof(waveNum_), hash_);
    hash_ = SkOpts::hash(&rippleCenterX_, sizeof(rippleCenterX_), hash_);
    hash_ = SkOpts::hash(&rippleCenterY_, sizeof(rippleCenterY_), hash_);
}
 
RSWaterRippleShaderFilter::~RSWaterRippleShaderFilter() = default;
 
float RSWaterRippleShaderFilter::GetProgress() const 
{
    return progress_;
}
 
float RSWaterRippleShaderFilter::GetWaveNum() const 
{
    return waveNum_;
}
 
float RSWaterRippleShaderFilter::GetRippleCenterX() const 
{
    return rippleCenterX_;
}
 
float RSWaterRippleShaderFilter::GetRippleCenterY() const 
{
    return rippleCenterY_;
} 
 
void RSWaterRippleShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto waterRippleFilter = std::make_shared<Drawing::GEVisualEffect>("WATER_RIPPLE", Drawing::DrawingPaintType::BRUSH);
    waterRippleFilter->SetParam("PROGRESS", progress_);
    waterRippleFilter->SetParam("WAVE_NUM", waveNum_);
    waterRippleFilter->SetParam("RIPPLE_CENTER_X", rippleCenterX_);
    waterRippleFilter->SetParam("RIPPLE_CENTER_Y", rippleCenterY_);
    visualEffectContainer->AddToChainedFilter(waterRippleFilter);
}
} // namespace Rosen
} // namespace OHOS