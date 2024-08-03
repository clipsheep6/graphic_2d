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
#ifndef GRAPHICS_EFFECT_GE_MESA_BLUR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_MESA_BLUR_SHADER_FILTER_H

#include <memory>

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
class GEMESABlurShaderFilter : public GEShaderFilter {
public:
    GEMESABlurShaderFilter(const Drawing::GEMESABlurShaderFilterParams& params);
    ~GEMESABlurShaderFilter() override = default;
    int GetRadius() const;

    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    struct NewBlurParams {
        int numberOfPasses = 1;     // 1: initial number of passes
        float offsets[12] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};    // 0: initial offsets
    };
    bool InitBlurEffect();
    bool InitMixEffect();
    bool InitSimpleFilter();
    bool InitGreyAdjustmentEffect();

    void CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Image>& checkedImage, const Drawing::Rect& src) const;
    std::shared_ptr<Drawing::Image> OutputImageWithoutBlur(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const;

    std::shared_ptr<Drawing::ShaderEffect> ApplySimpleFilter(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const std::shared_ptr<Drawing::ShaderEffect>& prevShader,
        const Drawing::ImageInfo& scaledInfo, const Drawing::SamplingOptions& linear) const;
    std::shared_ptr<Drawing::ShaderEffect> ApplyGreyAdjustmentFilter(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const std::shared_ptr<Drawing::ShaderEffect>& prevShader,
        const Drawing::ImageInfo& scaledInfo, const Drawing::SamplingOptions& linear) const;
    std::shared_ptr<Drawing::ShaderEffect> GetShaderEffect(const std::shared_ptr<Drawing::Image>& image,
        const Drawing::SamplingOptions& linear, const Drawing::Matrix& matrix) const;

    std::shared_ptr<Drawing::Image> DownSampling2X(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const Drawing::Rect& src,
        const Drawing::ImageInfo& scaledInfo, const Drawing::SamplingOptions& linear,
        const NewBlurParams& blur) const;
    std::shared_ptr<Drawing::Image> DownSampling4X(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const Drawing::Rect& src,
        const Drawing::ImageInfo& scaledInfo, const Drawing::SamplingOptions& linear,
        const NewBlurParams& blur) const;
    std::shared_ptr<Drawing::Image> DownSampling8X(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const Drawing::Rect& src,
        const Drawing::ImageInfo& scaledInfo, const Drawing::ImageInfo& middleInfo,
        const Drawing::SamplingOptions& linear, const NewBlurParams& blur) const;
    std::shared_ptr<Drawing::Image> DownSamplingMoreX(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const Drawing::Rect& src,
        const Drawing::ImageInfo& scaledInfo, const Drawing::ImageInfo& middleInfo,
        const Drawing::ImageInfo& middleInfo2, const Drawing::SamplingOptions& linear,
        const NewBlurParams& blur) const;
    std::shared_ptr<Drawing::Image> DownSampling(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const Drawing::Rect& src,
        const Drawing::ImageInfo& scaledInfo, int& width, int& height,
        const Drawing::SamplingOptions& linear, const NewBlurParams& blur) const;
    std::shared_ptr<Drawing::Image> ScaleAndAddRandomColor(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Image>& blurImage,
        const Drawing::Rect& src, const Drawing::Rect& dst, int& width, int& height) const;

    void ComputeRadiusAndScale(int radius);
    void AdjustRadiusAndScale();
    std::string GetDescription() const;
    bool IsInputValid(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
        const Drawing::Rect& dst);

    void SetBlurParams(NewBlurParams& bParam);
    void SetBlurParamsFivePassSmall(NewBlurParams& bParam);
    void SetBlurParamsFivePassLarge(NewBlurParams& bParam);

    Drawing::Matrix BuildMatrix(const Drawing::Rect& src, const Drawing::ImageInfo& scaledInfo,
        const std::shared_ptr<Drawing::Image>& input) const;
    Drawing::Matrix BuildMiddleMatrix(
        const Drawing::ImageInfo& scaledInfo, const Drawing::ImageInfo& middleInfo) const;
    Drawing::Matrix BuildStretchMatrix(const Drawing::Rect& src,
        const Drawing::Rect& dst, int inputWidth, int inputHeight) const;
    void CalculatePixelStretch(int width, int height);

    int radius_;
    float blurRadius_ = 0.0f;
    float blurScale_ = 0.25f;

    float greyCoef1_;
    float greyCoef2_;
    bool isGreyX_ = false;
    float stretchOffsetX_;
    float stretchOffsetY_;
    float stretchOffsetZ_;
    float stretchOffsetW_;
    float offsetX_;
    float offsetY_;
    float offsetZ_;
    float offsetW_;
    Drawing::TileMode tileMode_;
    float width_;
    float height_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_MESA_BLUR_SHADER_FILTER_H
