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
#include "render/rs_light_up_effect_filter.h"

#include "src/core/SkOpts.h"

#include "platform/common/rs_log.h"
#ifdef USE_ROSEN_DRAWING
#include "effect/color_matrix.h"
#endif

namespace OHOS {
namespace Rosen {
RSLightUpEffectFilter::RSLightUpEffectFilter(float lightUpDegree)
    : lightUpDegree_(lightUpDegree) {};

RSLightUpEffectFilter::~RSLightUpEffectFilter() = default;

float RSLightUpEffectFilter::GetLightUpDegree()
{
    return lightUpDegree_;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSLightUpEffectFilter::MakeLightUpEffectShader(float lightUpDeg, sk_sp<SkShader> imageShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeLightUpEffectShader(
    float lightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader)
#endif
{
    static constexpr char prog[] = R"(
        uniform half lightUpDeg;
        uniform shader imageShader;
        vec3 rgb2hsv(in vec3 c)
        {
            vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
            vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
            vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
            float d = q.x - min(q.w, q.y);
            float e = 1.0e-10;
            return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
        }
        vec3 hsv2rgb(in vec3 c)
        {
            vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
            return c.z * mix(vec3(1.0), rgb, c.y);
        }
        half4 main(float2 coord)
        {
            vec3 hsv = rgb2hsv(imageShader.eval(coord).rgb);
            float satUpper = clamp(hsv.y * 1.2, 0.0, 1.0);
            hsv.y = mix(satUpper, hsv.y, lightUpDeg);
            hsv.z += lightUpDeg - 1.0;
            return vec4(hsv2rgb(hsv), 1.0);
        }
    )";
#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {imageShader};
    size_t childCount = 1;
    return effect->makeShader(SkData::MakeWithCopy(
        &lightUpDeg, sizeof(lightUpDeg)), children, childCount, nullptr, false);
#else
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    std::shared_ptr<Drawing::ShaderEffect> children[] = {imageShader};
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&lightUpDeg, sizeof(lightUpDeg));
    return effect->MakeShader(data, children, childCount, nullptr, false);
#endif
}

void RSLightUpEffectFilter::PreProcess(sk_sp<SkImage> image)
{
    return;
}

void RSLightUpEffectFilter::PostProcess(RSPaintFilterCanvas& canvas)
{
    return;
}

sk_sp<SkImage> RSLightUpEffectFilter::ProcessImage(SkCanvas& canvas, const sk_sp<SkImage>& image) const
{
#ifndef USE_ROSEN_DRAWING
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeLightUpEffectShader(GetLightUpDegree(), imageShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.resetMatrix();
    canvas.drawPaint(paint);
#else
    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = MakeLightUpEffectShader(GetLightUpDegree(), imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.ResetMatrix();
    canvas.DrawBackground(brush);
#endif
}
} // namespace Rosen
} // namespace OHOS
