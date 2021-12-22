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

#include "skia_shader_effect.h"

#include <vector>

#include "include/effects/SkGradientShader.h"
#include "include/core/SkTileMode.h"

#include "effect/shader_effect.h"
#include "skia_shader_effect_data.h"

namespace OHOS {
namespace Rosen {
SkiaShaderEffect::SkiaShaderEffect() noexcept
{
    shaderEffectData_ = std::make_shared<SkiaShaderEffectData>();
    shaderEffectData_->type = ShaderEffectData::SKIA_SHADER_EFFECT;
}

void SkiaShaderEffect::InitWithColor(ColorQuad color)
{
    auto s = SkShaders::Color(color);
    shaderEffectData_->SetShader(s.release());
}

void SkiaShaderEffect::InitWithBlend(const ShaderEffectData* d1, const ShaderEffectData* d2, BlendMode mode)
{
    sk_sp<SkShader> dst(static_cast<SkShader*>(d1->GetShader()));
    sk_sp<SkShader> src(static_cast<SkShader*>(d2->GetShader()));
    auto s = SkShaders::Blend(static_cast<SkBlendMode>(mode), dst, src);
}

void SkiaShaderEffect::InitWithLinearGradient(const Point& startPt, const Point& endPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    SkPoint pts[2];
    pts[0].set(startPt.GetX(), startPt.GetY());
    pts[1].set(endPt.GetX(), endPt.GetY());

    int count = (colors.size() == pos.size()) ? colors.size() : 0;
    SkColor c[count];
    SkScalar p[count];
    for (auto i = 0; i < count; ++i)
    {
        c[i] = colors[i];
        p[i] = pos[i];
    }
    auto s = SkGradientShader::MakeLinear(pts, c, p, colors.size(), static_cast<SkTileMode>(mode));
    shaderEffectData_->SetShader(s.release());
}

void SkiaShaderEffect::InitWithRadialGradient(const Point& centerPt, scalar radius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    SkPoint center;
    center.set(centerPt.GetX(), centerPt.GetY());

    int count = (colors.size() == pos.size()) ? colors.size() : 0;
    SkColor c[count];
    SkScalar p[count];
    for (auto i = 0; i < count; ++i)
    {
        c[i] = colors[i];
        p[i] = pos[i];
    }
    auto s = SkGradientShader::MakeRadial(center, radius, c, p, count,
        static_cast<SkTileMode>(mode));
    shaderEffectData_->SetShader(s.release());
}

void SkiaShaderEffect::InitWithTwoPointConical(const Point& startPt, scalar startRadius,
    const Point& endPt, scalar endRadius,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode)
{
    SkPoint start;
    SkPoint end;
    start.set(startPt.GetX(), startPt.GetY());
    end.set(endPt.GetX(), endPt.GetY());

    int count = (colors.size() == pos.size()) ? colors.size() : 0;
    SkColor c[count];
    SkScalar p[count];
    for (auto i = 0; i < count; ++i)
    {
        c[i] = colors[i];
        p[i] = pos[i];
    }
    auto s = SkGradientShader::MakeTwoPointConical(start, startRadius, end, endRadius,
        c, p, count, static_cast<SkTileMode>(mode));
    shaderEffectData_->SetShader(s.release());
}

void SkiaShaderEffect::InitWithSweepGradient(const Point& centerPt,
    const std::vector<ColorQuad>& colors, const std::vector<scalar>& pos, TileMode mode,
    scalar startAngle, scalar endAngle)
{
    int count = (colors.size() == pos.size()) ? colors.size() : 0;
    SkColor c[count];
    SkScalar p[count];
    for (auto i = 0; i < count; ++i)
    {
        c[i] = colors[i];
        p[i] = pos[i];
    }
    auto s = SkGradientShader::MakeSweep(centerPt.GetX(), centerPt.GetY(), c, p, count,
        static_cast<SkTileMode>(mode), startAngle, endAngle, 0, nullptr);
    shaderEffectData_->SetShader(s.release());
}

sk_sp<SkShader> SkiaShaderEffect::MutableShaderEffect()
{
    return sk_sp<SkShader>(static_cast<SkShader*>(shaderEffectData_->GetShader()));
}
}
}