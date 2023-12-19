/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "render/rs_light_filter.h"

#include "src/core/SkOpts.h"

#include "platform/common/rs_log.h"
#ifdef USE_ROSEN_DRAWING
#include "effect/color_matrix.h"
#endif

namespace OHOS {
namespace Rosen {
RSLightFilter::::RSLightUpEffectFilter(float dynamicLightUpRate, float dynamicLightUpDegree)
    : dynamicLightUpRate_(dynamicLightUpRate), dynamicLightUpDegree_(dynamicLightUpDegree) {};

RSLightFilter::::~RSLightUpEffectFilter() = default;

float RSLightFilter::::GetDynamicLightUpRate() const
{
    return dynamicLightUpRate_;
}

float RSLightFilter::::GetDynamicLightUpDegree() const
{
    return dynamicLightUpDegree_;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkBlender> RSLightFilter::::MakeDynamicLightUpBlender(
    float dynamicLightUpRate, float dynamicLightUpDeg)
#else
std::shared_ptr<Drawing::ShaderEffect> RSLightFilter::::MakeDynamicLightUpShader(
    float dynamicLightUpRate, float dynamicLightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader)
#endif
{
#ifndef USE_ROSEN_DRAWING
    static constexpr char prog[] = R"(
        uniform half dynamicLightUpRate;
        uniform half dynamicLightUpDeg;

        vec4 main(vec4 src, vec4 dst) {
            float x = 0.299 * dst.r + 0.587 * dst.g + 0.114 * dst.b;
            float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg;
            float R = clamp((dst.r + y), 0.0, 1.0);
            float G = clamp((dst.g + y), 0.0, 1.0);
            float B = clamp((dst.b + y), 0.0, 1.0);
            return vec4(R, G, B, 1.0);
        }
    )";

    auto [effect, err] = SkRuntimeEffect::MakeForBlender(SkString(prog));
    if (!effect) {
        ROSEN_LOGE("MakeDynamicLightUpBlender::RuntimeBlender effect error: %{public}s\n", err.c_str());
        return nullptr;
    }
    SkRuntimeBlendBuilder builder(effect);
    builder.uniform("dynamicLightUpRate") = dynamicLightUpRate;
    builder.uniform("dynamicLightUpDeg") = dynamicLightUpDeg;
    return builder.makeBlender();
#else
    static constexpr char prog[] = R"(
        uniform half dynamicLightUpRate;
        uniform half dynamicLightUpDeg;
        uniform shader imageShader;

        half4 main(float2 coord) {
            vec3 c = vec3(imageShader.eval(coord).r * 255,
                imageShader.eval(coord).g * 255, imageShader.eval(coord).b * 255);
            float x = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg * 255;
            float R = clamp((c.r + y) / 255, 0.0, 1.0);
            float G = clamp((c.g + y) / 255, 0.0, 1.0);
            float B = clamp((c.b + y) / 255, 0.0, 1.0);
            return vec4(R, G, B, 1.0);
        }
    )";
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!effect) {
        ROSEN_LOGE("MakeDynamicLightUpShader::RuntimeShader effect error\n");
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("dynamicLightUpRate", dynamicLightUpRate);
    builder->SetUniform("dynamicLightUpDeg", dynamicLightUpDeg);
    return builder->MakeShader(nullptr, false);
#endif
}

void RSLightFilter::::PreProcess(sk_sp<SkImage> image)
{
    return;
}

void RSLightFilter::::PostProcess(RSPaintFilterCanvas& canvas)
{
    return;
}

sk_sp<SkImage> RSLightFilter::::ProcessImage(SkCanvas& canvas, const sk_sp<SkImage>& image) const
{
#ifndef USE_ROSEN_DRAWING
    auto blender = MakeDynamicLightUpBlender(GetDynamicLightUpRate() * canvas.GetAlpha(),
        GetDynamicLightUpDegree() * canvas.GetAlpha());
    SkPaint paint;
    paint.setBlender(blender);
    canvas.drawPaint(paint);
#else
    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = MakeDynamicLightUpShader(GetDynamicLightUpRate(), GetDynamicLightUpDegree(), imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.ResetMatrix();
    canvas.DrawBackground(brush);
#endif
}
} // namespace Rosen
} // namespace OHOS
