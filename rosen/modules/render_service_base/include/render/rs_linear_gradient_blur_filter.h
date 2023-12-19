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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H

#include <memory>
#include "include/core/SkColorFilter.h"
#include "include/effects/SkImageFilters.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSLinearGradientBlurFilter : public RSShaderFilter {
public:
    RSLinearGradientBlurFilter(RSLinearGradientBlurPara& rsLinearGradientBlurPara);
    RSLinearGradientBlurFilter(const RSLinearGradientBlurFilter&) = delete;
    RSLinearGradientBlurFilter operator=(const RSLinearGradientBlurFilter&) = delete;
    ~RSLinearGradientBlurFilter() override;
#ifndef USE_ROSEN_DRAWING
    static bool GetGradientDirectionPoints(SkPoint (&pts)[2],
                                const SkRect& clipBounds, GradientDirection direction);
    static sk_sp<SkShader> MakeAlphaGradientShader(const SkRect& clipBounds,
                                const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias);
    static sk_sp<SkShader> MakeHorizontalMeanBlurShader(float radiusIn,
                                            sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader);
    static sk_sp<SkShader> MakeVerticalMeanBlurShader(float radiusIn,
                                            sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader);
    static sk_sp<SkShader> MakeLightUpEffectShader(float lightUpDeg, sk_sp<SkShader> imageShader);
    static sk_sp<SkShader> MakeBinarizationShader(float low, float high, float threshold,
        float thresholdLow, float thresholdHigh, float imageWidth, float imageHeight, sk_sp<SkShader> imageShader);
    static void DrawHorizontalLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
        float radius, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding);
    static void DrawVerticalLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
        float radius, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding);
    static uint8_t CalcDirectionBias(const SkMatrix& mat);
    static void DrawLinearGradientBlur(SkSurface* surface, RSPaintFilterCanvas& canvas,
        const std::shared_ptr<RSLinearGradientBlurPara>& para, sk_sp<SkShader> alphaGradientShader,
        const SkIRect& clipIPadding);
    static sk_sp<SkShader> MakeMeanBlurShader(sk_sp<SkShader> srcImageShader,
        sk_sp<SkShader> blurImageShader, sk_sp<SkShader> gradientShader);
#else
    static bool GetGradientDirectionPoints(
        Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GradientDirection direction);
    static std::shared_ptr<Drawing::ShaderEffect> MakeAlphaGradientShader(const Drawing::Rect& clipBounds,
        const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias);
    static std::shared_ptr<Drawing::ShaderEffect> MakeHorizontalMeanBlurShader(float radiusIn,
        std::shared_ptr<Drawing::ShaderEffect> shader, std::shared_ptr<Drawing::ShaderEffect> gradientShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeVerticalMeanBlurShader(float radiusIn,
        std::shared_ptr<Drawing::ShaderEffect> shader, std::shared_ptr<Drawing::ShaderEffect> gradientShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeLightUpEffectShader(
        float lightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeBinarizationShader(float low, float high, float threshold,
        float thresholdLow, float thresholdHigh, float imageWidth, float imageHeight,
        std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static void DrawHorizontalLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
        float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding);
    static void DrawVerticalLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
        float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding);
    static uint8_t CalcDirectionBias(const Drawing::Matrix& mat);
    static void DrawLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
        const std::shared_ptr<RSLinearGradientBlurPara>& para,
        std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding);
    static std::shared_ptr<Drawing::ShaderEffect> MakeMeanBlurShader(
        std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
        std::shared_ptr<Drawing::ShaderEffect> gradientShader);

    void PreProcess(sk_sp<SkImage> image) override;
    void PostProcess(RSPaintFilterCanvas& canvas) override;
    sk_sp<SkImage> ProcessImage(SkCanvas& canvas, const sk_sp<SkImage>& image) const override;

private:
    std::shared_ptr<RSLinearGradientBlurPara> rsLinearGradientBlurPara_;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H