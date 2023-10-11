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
#include "common/rs_optional_trace.h"
#include "include/gpu/GrDirectContext.h"

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
// Advanced Filter
#define PROPERTY_HIGPU_VERSION "const.gpu.vendor"
#define PROPERTY_DEBUG_SUPPORT_AF "persist.sys.graphic.supports_af"
static constexpr uint32_t BLUR_SAMPLE_COUNT = 5;

// Advanced Filter: we can get normalized uv offset from width and height
struct OffsetInfo {
    float offsetX;
    float offsetY;
    int width;
    int height;
};

// Advanced Filter
static bool IsAdvancedFilterUsable()
{
    std::string gpuVersion = RSSystemProperties::GetRSEventProperty(PROPERTY_HIGPU_VERSION);
    // The AF Feature is only enabled on higpu v200 platform
    if (gpuVersion.compare("higpu.v200") != 0) {
        return false;
    }

    // If persist.sys.graphic.supports_af=0
    // we will not use it
    return RSSystemProperties::GetBoolSystemProperty(PROPERTY_DEBUG_SUPPORT_AF, true);
}

static const bool IS_ADVANCED_FILTER_USABLE_CHECK_ONCE = IsAdvancedFilterUsable();

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

    auto [blurEffect, error] = SkRuntimeEffect::MakeForShader(blurString);
    if (!blurEffect) {
        ROSEN_LOGE("KawaseBlurFilter::RuntimeShader blurEffect error: %{public}s\n", error.c_str());
        return;
    }
    blurEffect_ = std::move(blurEffect);

    // Advanced Filter
    if (IS_ADVANCED_FILTER_USABLE_CHECK_ONCE) {
        setupBlurEffectAdvancedFilter();
    }

    auto [mixEffect, error2] = SkRuntimeEffect::MakeForShader(mixString);
    if (!mixEffect) {
        ROSEN_LOGE("KawaseBlurFilter::RuntimeShader mixEffect error: %s{public}\n", error2.c_str());
        return;
    }
    mixEffect_ = std::move(mixEffect);
}

KawaseBlurFilter::~KawaseBlurFilter() = default;

// Advanced Filter
void KawaseBlurFilter::setupBlurEffectAdvancedFilter()
{
    SkString blurStringAF(R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset[5];

        half4 main(float2 xy) {
            half4 c = half4(0, 0, 0, 0);
            for (int i = 0; i < 5; ++i) {
                c += imageInput.eval(float2(xy.x + in_blurOffset[i].x, xy.y + in_blurOffset[i].y));
            }
            return half4(c.rgb * 0.2, 1.0);
        }
    )");

    SkRuntimeEffect::Options ops;
    ops.useAF = true;
    auto [blurEffectAF, errorAF] = SkRuntimeEffect::MakeForShader(blurStringAF, ops);
    if (!blurEffectAF) {
        ROSEN_LOGE("%s: RuntimeShader error: %s", __func__, errorAF.c_str());
        return;
    }
    blurEffectAF_ = std::move(blurEffectAF);
}

static void getNormalizedOffset(SkV2* offsets, const uint32_t offsetCount, const OffsetInfo& offsetInfo)
{
    if (offsets == nullptr || offsetCount != BLUR_SAMPLE_COUNT) {
        ROSEN_LOGE("%s: Invalid offsets.", __func__);
        return;
    }
    if (std::fabs(offsetInfo.width) < 1e-6 || std::fabs(offsetInfo.height) < 1e-6) {
        ROSEN_LOGE("%s: Invalid width or height.", __func__);
        return;
    }
    SkV2 normalizedOffsets[BLUR_SAMPLE_COUNT] = {
        SkV2{0.0f, 0.0f},
        SkV2{offsetInfo.offsetX / offsetInfo.width, offsetInfo.offsetY / offsetInfo.height},
        SkV2{-offsetInfo.offsetX / offsetInfo.width, offsetInfo.offsetY / offsetInfo.height},
        SkV2{offsetInfo.offsetX / offsetInfo.width, -offsetInfo.offsetY / offsetInfo.height},
        SkV2{-offsetInfo.offsetX / offsetInfo.width, -offsetInfo.offsetY / offsetInfo.height}
    };
    for (uint32_t i = 0; i < BLUR_SAMPLE_COUNT; ++i) {
        offsets[i] = normalizedOffsets[i];
    }
}

SkMatrix KawaseBlurFilter::GetShaderTransform(const SkCanvas* canvas, const SkRect& blurRect, float scale)
{
    auto matrix = SkMatrix::Scale(scale, scale);
    matrix.postConcat(SkMatrix::Translate(blurRect.fLeft, blurRect.fTop));
    return matrix;
}

void KawaseBlurFilter::CheckInputImage(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param,
    sk_sp<SkImage>& checkedImage)
{
    auto src = param.src;
    auto srcRect = SkIRect::MakeLTRB(src.left(), src.top(), src.right(), src.bottom());
    if (image->bounds() != srcRect) {
        if (auto resizedImage = image->makeSubset(srcRect, canvas.recordingContext()->asDirectContext())) {
            checkedImage = resizedImage;
            ROSEN_LOGD("KawaseBlurFilter::resize image success");
        } else {
            ROSEN_LOGE("KawaseBlurFilter::resize image failed, use original image");
        }
    }
}

void KawaseBlurFilter::OutputOriginalImage(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param)
{
    auto src = param.src;
    auto dst = param.dst;
    SkPaint paint;
    if (param.colorFilter) {
        paint.setColorFilter(param.colorFilter);
    }
    SkMatrix inputMatrix = SkMatrix::Translate(-src.fLeft, -src.fTop);
    inputMatrix.postConcat(SkMatrix::Translate(dst.fLeft, dst.fTop));
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    const auto inputShader = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, &inputMatrix);
    paint.setShader(inputShader);
    canvas.drawRect(dst, paint);
}

bool KawaseBlurFilter::ApplyKawaseBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param)
{
    if (!blurEffect_ || !mixEffect_ || !image) {
        ROSEN_LOGE("KawaseBlurFilter::shader error, use Gauss instead");
        return false;
    }
    static auto useKawaseOriginal = RSSystemProperties::GetKawaseOriginalEnabled();
    if (param.radius <= 0 || useKawaseOriginal) {
        ROSEN_LOGD("KawaseBlurFilter::input invalid radius : %{public}d", param.radius);
        OutputOriginalImage(canvas, image, param);
        return true;
    }
    auto src = param.src;
    auto dst = param.dst;
    auto input = image;
    CheckInputImage(canvas, image, param, input);
    ComputeRadiusAndScale(param.radius);
    RS_OPTIONAL_TRACE_BEGIN("ApplyKawaseBlur " + GetDescription());
    int maxPasses = supportLargeRadius ? kMaxPassesLargeRadius : kMaxPasses;
    float dilatedConvolutionFactor = supportLargeRadius ? kDilatedConvolutionLargeRadius : kDilatedConvolution;
    if (abs(dilatedConvolutionFactor) <= 1e-6) {
        return false;
    }
    float tmpRadius = static_cast<float>(blurRadius_) / dilatedConvolutionFactor;
    int numberOfPasses = std::min(maxPasses, std::max(static_cast<int>(ceil(tmpRadius)), 1)); // 1 : min pass num
    float radiusByPasses = tmpRadius / numberOfPasses;
    ROSEN_LOGD("KawaseBlurFilter::kawase radius : %{public}f, scale : %{public}f, pass num : %{public}d",
        blurRadius_, blurScale_, numberOfPasses);
    auto width = std::max(static_cast<int>(std::ceil(dst.width())), input->width());
    auto height = std::max(static_cast<int>(std::ceil(dst.height())), input->height());
    SkImageInfo scaledInfo = input->imageInfo().makeWH(std::ceil(width * blurScale_), std::ceil(height * blurScale_));
    SkMatrix blurMatrix = SkMatrix::Translate(-src.fLeft, -src.fTop);
    blurMatrix.postScale(blurScale_, blurScale_);
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);

    // Advanced Filter: check is AF usable only the first time
    bool isUsingAF = IS_ADVANCED_FILTER_USABLE_CHECK_ONCE && blurEffectAF_ != nullptr;
    SkRuntimeShaderBuilder blurBuilder(isUsingAF ? blurEffectAF_ : blurEffect_);
    blurBuilder.child("imageInput") = input->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, blurMatrix);

    if (isUsingAF) {
        SkV2 firstPassOffsets[BLUR_SAMPLE_COUNT];
        OffsetInfo firstPassOffsetInfo = {radiusByPasses * blurScale_, radiusByPasses * blurScale_,
            scaledInfo.width(), scaledInfo.height()};
        getNormalizedOffset(firstPassOffsets, BLUR_SAMPLE_COUNT, firstPassOffsetInfo);
        blurBuilder.uniform("in_blurOffset") = firstPassOffsets;
    } else {
        blurBuilder.uniform("in_blurOffset") = SkV2{radiusByPasses * blurScale_, radiusByPasses * blurScale_};
        blurBuilder.uniform("in_maxSizeXY") = SkV2{width * blurScale_, height * blurScale_};
    }

    sk_sp<SkImage> tmpBlur(blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false));
    // And now we'll build our chain of scaled blur stages
    for (auto i = 1; i < numberOfPasses; i++) {
        const float stepScale = static_cast<float>(i) * blurScale_;
        blurBuilder.child("imageInput") = tmpBlur->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear);

        // Advanced Filter
        if (isUsingAF) {
            SkV2 offsets[BLUR_SAMPLE_COUNT];
            OffsetInfo offsetInfo = {radiusByPasses * stepScale, radiusByPasses * stepScale,
                scaledInfo.width(), scaledInfo.height()};
            getNormalizedOffset(offsets, BLUR_SAMPLE_COUNT, offsetInfo);
            blurBuilder.uniform("in_blurOffset") = offsets;
        } else {
            blurBuilder.uniform("in_blurOffset") = SkV2{radiusByPasses * stepScale, radiusByPasses * stepScale};
            blurBuilder.uniform("in_maxSizeXY") = SkV2{width * blurScale_, height * blurScale_};
        }
        tmpBlur = blurBuilder.makeImage(canvas.recordingContext(), nullptr, scaledInfo, false);
    }
    RS_OPTIONAL_TRACE_END();
    return ApplyBlur(canvas, input, tmpBlur, param);
}

bool KawaseBlurFilter::ApplyBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const sk_sp<SkImage>& blurImage,
    const KawaseParameter& param) const
{
    auto src = param.src;
    auto dst = param.dst;
    if (abs(blurScale_) <= 1e-6) {
        return false;
    }
    float invBlurScale = 1.0f / blurScale_;
    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    const auto blurMatrix = GetShaderTransform(&canvas, dst, invBlurScale);
    const auto blurShader = blurImage->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, &blurMatrix);
    SkPaint paint;
    paint.setAlphaf(param.alpha);
    if (param.colorFilter) {
        paint.setColorFilter(param.colorFilter);
    }
    static auto addRandomColor = RSSystemProperties::GetRandomColorEnabled();
    if (addRandomColor) {
        SkMatrix inputMatrix = SkMatrix::Translate(-src.fLeft, -src.fTop);
        inputMatrix.postConcat(SkMatrix::Translate(dst.fLeft, dst.fTop));
        SkRuntimeShaderBuilder mixBuilder(mixEffect_);
        mixBuilder.child("blurredInput") = blurShader;
        mixBuilder.child("originalInput") = image->makeShader(
            SkTileMode::kClamp, SkTileMode::kClamp, linear, inputMatrix);
        float mixFactor = (abs(kMaxCrossFadeRadius) <= 1e-6) ? 1.f : (blurRadius_ / kMaxCrossFadeRadius);
        mixBuilder.uniform("mixFactor") = std::min(1.0f, mixFactor);
        static auto factor = RSSystemProperties::GetKawaseRandomColorFactor();
        mixBuilder.uniform("inColorFactor") = factor;
        ROSEN_LOGD("KawaseBlurFilter::kawase random color factor : %{public}f", factor);
        paint.setShader(mixBuilder.makeShader(nullptr, image->isOpaque()));
    } else {
        paint.setShader(blurShader);
    }
    canvas.drawRect(dst, paint);
    return true;
}

void KawaseBlurFilter::ComputeRadiusAndScale(int radius)
{
    static constexpr int noiseFactor = 3; // 3 : smooth the radius change
    blurRadius_ = radius * 4 / noiseFactor * noiseFactor; // 4 : scale between gauss radius and kawase
    AdjustRadiusAndScale();
}

void KawaseBlurFilter::AdjustRadiusAndScale()
{
    static constexpr int radiusStep1 = 50; // 50 : radius step1
    static constexpr int radiusStep2 = 150; // 150 : radius step2
    static constexpr int radiusStep3 = 400; // 400 : radius step3
    static constexpr float scaleFactor1 = 0.25f; // 0.25 : downSample scale for step1
    static constexpr float scaleFactor2 = 0.125f; // 0.125 : downSample scale for step2
    static constexpr float scaleFactor3 = 0.0625f; // 0.0625 : downSample scale for step3
    float scale = baseBlurScale;
    if (blurRadius_ > radiusStep3) {
        scale = scaleFactor3;
    } else if (blurRadius_ > radiusStep2) {
        scale = scaleFactor2;
    } else if (blurRadius_ > radiusStep1) {
        scale = scaleFactor1;
    }
    blurScale_ = scale;
}

std::string KawaseBlurFilter::GetDescription() const
{
    return "blur radius is " + std::to_string(blurRadius_);
}
#endif
} // namespace Rosen
} // namespace OHOS