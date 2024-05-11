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

#include "ge_hps_blur_shader_filter.h"

#include "ge_kawase_blur_shader_filter.h"
#include "ge_log.h"
#include "ge_system_properties.h"
#include "src/core/SkOpts.h"

#include "draw/surface.h"
#include "effect/color_matrix.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {

GEHpsBlurShaderFilter::GEHpsBlurShaderFilter(const Drawing::HpsBlurFilterParams& params)
    : radius_(params.radius), saturation_(params.saturation), brightness_(params.brightness)
{
    std::string mixString(
        R"(
        uniform shader blurredInput;
        uniform shader originalInput;
        uniform float mixFactor;
        uniform float inColorFactor;
 
        highp float random(float2 xy) {
            float t = dot(xy, float2(78.233, 12.9898));
            return fract(sin(t) * 43758.5453);
        }
        half4 main(float2 xy) {
            highp float noiseGranularity = inColorFactor / 255.0;
            half4 finalColor = mix(originalInput.eval(xy), blurredInput.eval(xy), mixFactor);
            float noise  = mix(-noiseGranularity, noiseGranularity, random(xy));
            finalColor.rgb += noise;
            return finalColor;
        }
    )");

    auto mixEffect = Drawing::RuntimeEffect::CreateForShader(mixString);
    if (!mixEffect) {
        LOGE("HpsBlurFilter::RuntimeShader mixEffect create failed");
        return;
    }
    mixEffect_ = std::move(mixEffect);
}

std::shared_ptr<Drawing::Image> GEHpsBlurShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if ((image == nullptr) || (image->GetWidth() == 0) || (image->GetHeight() == 0)) {
        LOGE("GEHpsBlurShaderFilter::ProcessImage image is invalid");
        return nullptr;
    }
    Drawing::HpsBlurParameter hpsParam = Drawing::HpsBlurParameter(src, dst, radius_, saturation_, brightness_);

    return ApplyHpsBlur(canvas, image, hpsParam);
}

Drawing::Matrix GEHpsBlurShaderFilter::GetShaderTransform(
    const Drawing::Canvas* canvas, const Drawing::Rect& blurRect, float scaleW, float scaleH)
{
    Drawing::Matrix matrix;
    matrix.SetScale(scaleW, scaleH);
    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(blurRect.GetLeft(), blurRect.GetTop());
    matrix.PostConcat(translateMatrix);

    return matrix;
}

std::shared_ptr<Drawing::Image> GEHpsBlurShaderFilter::ApplyHpsBlur(
    Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::HpsBlurParameter& param)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        LOGE("GEHpsBlurShaderFilter::ApplyHpsBlur surface is null");
        return nullptr;
    }

    std::shared_ptr<Drawing::Surface> offscreenSurface =
        surface->MakeSurface(param.src.GetWidth(), param.src.GetHeight());
    if (offscreenSurface == nullptr) {
        LOGE("GEHpsBlurShaderFilter::ApplyHpsBlur offscreenSurface is null");
        return nullptr;
    }

    std::shared_ptr<Drawing::Canvas> offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        LOGE("GEHpsBlurShaderFilter::ApplyHpsBlur offscreenCanvas is null");
        return nullptr;
    }

    auto offscreenHpsParam =
        Drawing::HpsBlurParameter(param.src, param.src, param.sigma, param.saturation, param.brightness);
    if (!offscreenCanvas->DrawBlurImage(*image, offscreenHpsParam)) {
        LOGE("GEHpsBlurShaderFilter::ApplyHpsBlur Hps DrawBlurImage failed");
        return nullptr;
    }

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (imageCache == nullptr) {
        LOGE("GEHpsBlurShaderFilter::ApplyHpsBlur imageCache is null");
        return nullptr;
    }

    return ApplyBlur(canvas, image, imageCache, param);
}

std::shared_ptr<Drawing::Image> GEHpsBlurShaderFilter::ApplyBlur(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, std::shared_ptr<Drawing::Image>& blurImage,
    const Drawing::HpsBlurParameter& param) const
{
    if ((blurImage == nullptr) || (blurImage->GetWidth() == 0) || (blurImage->GetHeight() == 0)) {
        LOGE("GEHpsBlurShaderFilter::ApplyBlur image is invalid");
        return nullptr;
    }

    auto src = param.src;
    auto dst = param.dst;
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const auto blurMatrix = GetShaderTransform(
        &canvas, dst, dst.GetWidth() / blurImage->GetWidth(), dst.GetHeight() / blurImage->GetHeight());
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *blurImage, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, blurMatrix);

    Drawing::Matrix inputMatrix;
    inputMatrix.Translate(-src.GetLeft(), -src.GetTop());
    inputMatrix.PostScale(dst.GetWidth() / image->GetWidth(), dst.GetHeight() / image->GetHeight());

    Drawing::Matrix matrix;
    matrix.Translate(dst.GetLeft(), dst.GetTop());
    inputMatrix.PostConcat(matrix);

    Drawing::RuntimeShaderBuilder mixBuilder(mixEffect_);
    mixBuilder.SetChild("blurredInput", blurShader);
    mixBuilder.SetChild("originalInput", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
                                             Drawing::TileMode::CLAMP, linear, inputMatrix));

    float mixFactor = (abs(kMaxCrossFadeRadius) <= 1e-6) ? 1.f : (param.sigma / kMaxCrossFadeRadius);
    mixBuilder.SetUniform("mixFactor", std::min(1.0f, mixFactor));
    static constexpr float factor = 1.75f; // 1.75 from experience
    mixBuilder.SetUniform("inColorFactor", factor);
    auto output = mixBuilder.MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);

    return output;
}

} // namespace Rosen
} // namespace OHOS