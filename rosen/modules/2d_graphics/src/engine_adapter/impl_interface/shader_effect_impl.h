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

#ifndef SHADER_EFFECT_IMPL_H
#define SHADER_EFFECT_IMPL_H

#include <memory>

#include "draw/blend_mode.h"
#include "draw/color.h"
#include "shader_effect_data.h"
#include "utils/point.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class TileMode;
class ShaderEffectImpl {
public:
    ShaderEffectImpl() noexcept {}
    virtual ~ShaderEffectImpl() {}

    virtual void InitWithColor(ColorQuad color) = 0;
    virtual void InitWithBlend(const ShaderEffectData* d1, const ShaderEffectData* d2, BlendMode mode) = 0;
    virtual void InitWithLinearGradient(const Point& startPt, const Point& endPt,
        const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode) = 0;
    virtual void InitWithRadialGradient(const Point& centerPt, scalar radius,
        const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode) = 0;
    virtual void InitWithTwoPointConical(const Point& startPt, scalar startRadius,
        const Point& endPtr, scalar endRadius,
        const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode) = 0;
    virtual void InitWithSweepGradient(const Point& centerPt,
        const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode,
        scalar startAngle, scalar endAngle) = 0;
    const ShaderEffectData * GetShaderEffectData() const { return shaderEffectData_.get(); }
protected:
    std::shared_ptr<ShaderEffectData> shaderEffectData_;
};
}
}
}
#endif