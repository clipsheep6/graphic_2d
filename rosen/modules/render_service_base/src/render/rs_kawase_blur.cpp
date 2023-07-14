/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "render/rs_kawase_blur.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
KawaseBlurFilter::KawaseBlurFilter()
{
    SkString blurString(R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;
        uniform float2 in_maxSizeXY;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2(clamp(in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            c += imageInput.eval(float2(clamp(-in_blurOffset.x + xy.x, 0, in_maxSizeXY.x),
                                        clamp(-in_blurOffset.y + xy.y, 0, in_maxSizeXY.y)));
            return half4(c.rgb * 0.2, 1.0);
        }
    )");

    SkString mixString(R"(
        uniform shader blurredInput;
        uniform shader originalInput;
        uniform float mixFactor;
        half4 main(float2 xy) {
            return half4(mix(originalInput.eval(xy), blurredInput.eval(xy), mixFactor));
        }
    )");

    auto [blurEffect, error] = SkRuntimeEffect::MakeForShader(blurString);
    if (!blurEffect) {
        ROSEN_LOGE("KawaseBlurFilter::RuntimeShader error: %s\n", error.c_str());
        return;
    }
    blurEffect_ = std::move(blurEffect);

    auto [mixEffect, error2] = SkRuntimeEffect::MakeForShader(mixString);
    if (!mixEffect) {
        ROSEN_LOGE("KawaseBlurFilter::RuntimeShader error: %s\n", error2.c_str());
        return;
    }
    mixEffect_ = std::move(mixEffect);
}

KawaseBlurFilter::~KawaseBlurFilter() = default;

SkMatrix KawaseBlurFilter::GetShaderTransform(const SkCanvas* canvas, const SkRect& blurRect, float scale)
{
    auto matrix = SkMatrix::Scale(scale, scale);
    matrix.postConcat(SkMatrix::Translate(blurRect.fLeft, blurRect.fTop));
    return matrix;
}

bool KawaseBlurFilter::ApplyKawaseBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param)
{
    if (!blurEffect_ || !mixEffect_) {
        return false;
    }
    RS_TRACE_NAME("ApplyKawaseBlur " + GetDescription());
    ComputeRadiusAndScale(param.radius);
    auto src = param.src;
    auto dst = param.dst;
    int maxPasses = supporteLargeRadius ? kMaxPassesLargeRadius : kMaxPasses;
    float dilatedConvolutionFactor = supporteLargeRadius ? kDilatedConvolutionLargeRadius : kDilatedConvolution;
    float tmpRadius = blurRadius_ / dilatedConvolutionFactor;
    int numberOfPasses = std::min(maxPasses, std::max(static_cast<int>(ceil(tmpRadius)), 1)); // 1 : min pass num
    float radiusByPasses = tmpRadius / numberOfPasses;
    SkImageInfo scaledInfo = image->imageInfo().makeWH(std::ceil(dst.width() * blurScale_),
        std::ceil(dst.height() * blurScale_));
    SkMatrix blurMatrix = SkMatrix::Translate(-src.fLeft, -src.fTop);
    blurMatrix.postScale(blurScale_, blurScale_);
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    SkRuntimeShaderBuilder blurBuilder(blurEffect_);
    blurBuilder.child("imageInput") = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, blurMatrix);
    blurBuilder.uniform("in_blurOffset") = SkV2{radiusByPasses * blurScale_, radiusByPasses * blurScale_};
    blurBuilder.uniform("in_maxSizeXY") = SkV2{dst.width() * blurScale_, dst.height() * blurScale_};
    sk_sp<SkImage> tmpBlur(blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));
    // And now we'll build our chain of scaled blur stages
    for (auto i = 1; i < numberOfPasses; i++) {
        const float stepScale = i * blurScale_;
        blurBuilder.child("imageInput") = tmpBlur->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear);
        blurBuilder.uniform("in_blurOffset") = SkV2{radiusByPasses * stepScale, radiusByPasses * stepScale};
        blurBuilder.uniform("in_maxSizeXY") = SkV2{dst.width() * blurScale_, dst.height() * blurScale_};
        tmpBlur = blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false);
    }
    return ApplyBlur(canvas, image, tmpBlur, param);
}

bool KawaseBlurFilter::ApplyBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const sk_sp<SkImage>& blurImage,
    const KawaseParameter& param) const
{
    auto src = param.src;
    auto dst = param.dst;
    float invBlurScale = 1 / blurScale_;
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    SkImageInfo scaledInfo = image->imageInfo().makeWH(std::ceil(dst.width() * blurScale_),
        std::ceil(dst.height() * blurScale_));
    const auto blurMatrix = GetShaderTransform(&canvas, dst, invBlurScale);
    const auto blurShader = blurImage->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, &blurMatrix);
    SkPaint paint;
    if (param.colorFilter) {
        paint.setColorFilter(param.colorFilter);
    }
    if (blurRadius_ < kMaxCrossFadeRadius) {
        SkMatrix inputMatrix = SkMatrix::Translate(-src.fLeft, -src.fTop);
        inputMatrix.postConcat(SkMatrix::Translate(dst.fLeft, dst.fTop));
        SkRuntimeShaderBuilder mixBuilder(mixEffect_);
        mixBuilder.child("blurredInput") = blurShader;
        mixBuilder.child("originalInput") = image->makeShader(
            SkTileMode::kClamp, SkTileMode::kClamp, linear, inputMatrix);
        mixBuilder.uniform("mixFactor") = std::min(1.0f, blurRadius_ / kMaxCrossFadeRadius);
        paint.setShader(mixBuilder.makeShader(nullptr, true));
    } else {
        paint.setShader(blurShader);
    }
    canvas.drawRect(dst, paint);
    return true;
}

void KawaseBlurFilter::ComputeRadiusAndScale(int radius)
{
    float factor = std::min(1.0f, static_cast<float>(radius) / kMaxGaussRadius);
    float optimizedFactor = 1.0f - (1.0f - factor) * (1.0f - factor);
    blurRadius_ = kMaxKawaseRadius * optimizedFactor;
    AdjustRadiusAndScale();
}

void KawaseBlurFilter::AdjustRadiusAndScale()
{
    int radius = blurRadius_;
    float scale = baseBlurScale;
    if (radius > radiusStep1) {
        scale = 0.1f; // 0.1 : downSample radio step
        radius -= smoothScope / (radius - radiusStep1);
    } else if (radius > radiusStep2) {
        scale = 0.03f; // 0.03 : downSample radio step
        radius -= smoothScope / (radius - radiusStep2);
    }
    blurRadius_ = radius;
    blurScale_ = scale;
}

std::string KawaseBlurFilter::GetDescription() const
{
    return "blur radius is " + std::to_string(blurRadius_);
}
#endif
} // namespace Rosen
} // namespace OHOS