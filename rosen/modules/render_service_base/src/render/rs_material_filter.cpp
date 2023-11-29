/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "render/rs_material_filter.h"

#include <unordered_map>

#include "src/core/SkOpts.h"

#include "common/rs_common_def.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "platform/common/rs_system_properties.h"

#if defined(NEW_SKIA)
#include "include/effects/SkImageFilters.h"
#include "include/core/SkTileMode.h"
#else
#include "include/effects/SkBlurImageFilter.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr float BLUR_SIGMA_SCALE = 0.57735f;
// style to MaterialParam map
static const std::unordered_map<MATERIAL_BLUR_STYLE, MaterialParam> MATERIAL_PARAM {
    // card blur params
    { STYLE_CARD_THIN_LIGHT,         { 23.0f,  1.05, 1.05, RSColor(0xFFFFFF33) } },
    { STYLE_CARD_LIGHT,              { 50.0f,  1.8,  1.0,  RSColor(0xFAFAFA99) } },
    { STYLE_CARD_THICK_LIGHT,        { 57.0f,  1.2,  1.1,  RSColor(0xFFFFFF8C) } },
    { STYLE_CARD_THIN_DARK,          { 75.0f,  1.35, 1.0,  RSColor(0x1A1A1A6B) } },
    { STYLE_CARD_DARK,               { 50.0f,  2.15, 1.0,  RSColor(0x1F1F1FD1) } },
    { STYLE_CARD_THICK_DARK,         { 75.0f,  2.15, 1.0,  RSColor(0x1F1F1FD1) } },
    // background blur params
    { STYLE_BACKGROUND_SMALL_LIGHT,  { 23.0f,  1.05, 1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_MEDIUM_LIGHT, { 29.0f,  1.1,  1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_LARGE_LIGHT,  { 57.0f,  1.2,  1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_XLARGE_LIGHT, { 120.0f, 1.3,  1.0,  RSColor(0x6666664C) } },
    { STYLE_BACKGROUND_SMALL_DARK,   { 15.0f,  1.1,  1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_MEDIUM_DARK,  { 55.0f,  1.15, 1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_LARGE_DARK,   { 75.0f,  1.5,  1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_XLARGE_DARK,  { 130.0f, 1.3,  1.0,  RSColor(0x0D0D0D80) } },
};

static const std::unordered_map<MATERIAL_BLUR_STYLE, MaterialParam> KAWASE_MATERIAL_PARAM {
    // card blur params
    { STYLE_CARD_THIN_LIGHT,         { 23.0f,  1.05, 1.05, RSColor(0xFFFFFF33) } },
    { STYLE_CARD_LIGHT,              { 50.0f,  1.8,  1.0,  RSColor(0xFAFAFA99) } },
    { STYLE_CARD_THICK_LIGHT,        { 57.0f,  1.2,  1.1,  RSColor(0xFFFFFF8C) } },
    { STYLE_CARD_THIN_DARK,          { 75.0f,  1.35, 1.0,  RSColor(0x1A1A1A6B) } },
    { STYLE_CARD_DARK,               { 50.0f,  2.15, 1.0,  RSColor(0x1F1F1FD1) } },
    { STYLE_CARD_THICK_DARK,         { 75.0f,  2.15, 1.0,  RSColor(0x1F1F1FD1) } },
    // background blur params
    { STYLE_BACKGROUND_SMALL_LIGHT,  { 12.0f,  1.05, 1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_MEDIUM_LIGHT, { 29.0f,  1.1,  1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_LARGE_LIGHT,  { 45.0f,  1.2,  1.0,  RSColor(0x80808033) } },
    { STYLE_BACKGROUND_XLARGE_LIGHT, { 120.0f, 1.3,  1.0,  RSColor(0x6666664C) } },
    { STYLE_BACKGROUND_SMALL_DARK,   { 15.0f,  1.1,  1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_MEDIUM_DARK,  { 55.0f,  1.15, 1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_LARGE_DARK,   { 75.0f,  1.5,  1.0,  RSColor(0x0D0D0D80) } },
    { STYLE_BACKGROUND_XLARGE_DARK,  { 130.0f, 1.3,  1.0,  RSColor(0x0D0D0D80) } },
};
} // namespace

#ifndef USE_ROSEN_DRAWING
const bool KAWASE_BLUR_ENABLED = RSSystemProperties::GetKawaseEnabled();
#else
const bool KAWASE_BLUR_ENABLED = false;
#endif

RSMaterialFilter::RSMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode, float ratio)
#ifndef USE_ROSEN_DRAWING
    : RSSkiaFilter(nullptr), colorMode_(mode)
#else
    : RSDrawingFilter(nullptr), colorMode_(mode)
#endif
{
    imageFilter_ = RSMaterialFilter::CreateMaterialStyle(static_cast<MATERIAL_BLUR_STYLE>(style), dipScale, ratio);
    type_ = FilterType::MATERIAL;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&style, sizeof(style), hash_);
    hash_ = SkOpts::hash(&colorMode_, sizeof(colorMode_), hash_);
    hash_ = SkOpts::hash(&ratio, sizeof(ratio), hash_);
}

RSMaterialFilter::RSMaterialFilter(MaterialParam materialParam, BLUR_COLOR_MODE mode)
#ifndef USE_ROSEN_DRAWING
    : RSSkiaFilter(nullptr), colorMode_(mode), radius_(materialParam.radius), saturation_(materialParam.saturation),
#else
    : RSDrawingFilter(nullptr), colorMode_(mode), radius_(materialParam.radius), saturation_(materialParam.saturation),
#endif
      brightness_(materialParam.brightness), maskColor_(materialParam.maskColor)
{
    imageFilter_ = RSMaterialFilter::CreateMaterialFilter(
        materialParam.radius, materialParam.saturation, materialParam.brightness);
    type_ = FilterType::MATERIAL;
    if (colorMode_ == FASTAVERAGE) {
        colorPickerTask_ = std::make_shared<RSColorPickerCacheTask>();
    }

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&materialParam, sizeof(materialParam), hash_);
    hash_ = SkOpts::hash(&colorMode_, sizeof(colorMode_), hash_);
}

RSMaterialFilter::~RSMaterialFilter() = default;

float RSMaterialFilter::RadiusVp2Sigma(float radiusVp, float dipScale)
{
    float radiusPx = radiusVp * dipScale;
#ifndef USE_ROSEN_DRAWING
    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + SK_ScalarHalf : 0.0f;
#else
    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + 0.5f : 0.0f;
#endif
}

std::string RSMaterialFilter::GetDescription()
{
    return "RSMaterialFilter blur radius is " + std::to_string(radius_) + " sigma";
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<RSSkiaFilter> RSMaterialFilter::Compose(const std::shared_ptr<RSSkiaFilter>& other) const
#else
std::shared_ptr<RSDrawingFilter> RSMaterialFilter::Compose(const std::shared_ptr<RSDrawingFilter>& other) const
#endif
{
    if (other == nullptr) {
        return nullptr;
    }
    MaterialParam materialParam = {radius_, saturation_, brightness_, maskColor_};
    std::shared_ptr<RSMaterialFilter> result = std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
#ifndef USE_ROSEN_DRAWING
    result->imageFilter_ = SkImageFilters::Compose(imageFilter_, other->GetImageFilter());
#else
    result->imageFilter_ = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter_, other->GetImageFilter());
#endif
    auto otherHash = other->Hash();
    result->hash_ = SkOpts::hash(&otherHash, sizeof(otherHash), hash_);
    return result;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkColorFilter> RSMaterialFilter::GetColorFilter(float sat, float brightness)
#else
std::shared_ptr<Drawing::ColorFilter> RSMaterialFilter::GetColorFilter(float sat, float brightness)
#endif
{
    float normalizedDegree = brightness - 1.0;
    const float brightnessMat[] = {
        1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
        0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
    };

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkColorFilter> brightnessFilter = SkColorFilters::Matrix(brightnessMat); // brightness
    SkColorMatrix cm;
    cm.setSaturation(sat);
    sk_sp<SkColorFilter> satFilter = SkColorFilters::Matrix(cm); // saturation
    sk_sp<SkColorFilter> filterCompose = SkColorFilters::Compose(satFilter, brightnessFilter);
#else
    Drawing::ColorMatrix bm;
    bm.SetArray(brightnessMat);
    std::shared_ptr<Drawing::ColorFilter> brightnessFilter = Drawing::ColorFilter::CreateMatrixColorFilter(bm);
    Drawing::ColorMatrix cm;
    cm.SetSaturation(sat);
    std::shared_ptr<Drawing::ColorFilter> satFilter = Drawing::ColorFilter::CreateMatrixColorFilter(cm);
    std::shared_ptr<Drawing::ColorFilter> filterCompose =
        Drawing::ColorFilter::CreateComposeColorFilter(*satFilter, *brightnessFilter);
#endif
    return filterCompose;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialFilter(float radius, float sat, float brightness)
{
    colorFilter_ = GetColorFilter(sat, brightness);
#if defined(NEW_SKIA)
    sk_sp<SkImageFilter> blurFilter = SkImageFilters::Blur(radius, radius, SkTileMode::kClamp, nullptr); // blur
#else
    sk_sp<SkImageFilter> blurFilter = SkBlurImageFilter::Make(radius, radius, nullptr, nullptr,
        SkBlurImageFilter::kClamp_TileMode); // blur
#endif

    return SkImageFilters::ColorFilter(colorFilter_, blurFilter);
}
#else
std::shared_ptr<Drawing::ImageFilter> RSMaterialFilter::CreateMaterialFilter(float radius, float sat, float brightness)
{
    colorFilter_ = GetColorFilter(sat, brightness);
    useKawase_ = RSSystemProperties::GetKawaseEnabled();
    std::shared_ptr<Drawing::ImageFilter> blurFilter =
        Drawing::ImageFilter::CreateBlurImageFilter(radius, radius, Drawing::TileMode::CLAMP, nullptr);
    
    return Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter_, blurFilter);
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialStyle(MATERIAL_BLUR_STYLE style, float dipScale, float ratio)
#else
std::shared_ptr<Drawing::ImageFilter> RSMaterialFilter::CreateMaterialStyle(
    MATERIAL_BLUR_STYLE style, float dipScale, float ratio)
#endif
{
    const auto& materialParams = KAWASE_BLUR_ENABLED ? KAWASE_MATERIAL_PARAM : MATERIAL_PARAM;
    if (materialParams.find(style) != materialParams.end()) {
        const auto& materialParam = materialParams.at(style);
        maskColor_ = RSColor(materialParam.maskColor.AsRgbaInt());
        maskColor_.MultiplyAlpha(ratio);
        radius_ = RSMaterialFilter::RadiusVp2Sigma(materialParam.radius, dipScale) * ratio;
        saturation_ = (materialParam.saturation - 1) * ratio + 1.0;
        brightness_ = (materialParam.brightness - 1) * ratio + 1.0;
        return RSMaterialFilter::CreateMaterialFilter(radius_, saturation_, brightness_);
    }
    return nullptr;
}

#ifndef USE_ROSEN_DRAWING
void RSMaterialFilter::PreProcess(sk_sp<SkImage> imageSnapshot)
{
    if (colorMode_ == AVERAGE && imageSnapshot != nullptr) {
        // update maskColor while persevere alpha
        SkColor colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
        maskColor_ = RSColor(
            SkColorGetR(colorPicker), SkColorGetG(colorPicker), SkColorGetB(colorPicker), maskColor_.GetAlpha());
    } else if (colorMode_ == FASTAVERAGE) {
        RSColor color;
        if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask_, imageSnapshot)) {
            if (colorPickerTask_->GetColor(color)) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            colorPickerTask_->SetStatus(CacheProcessStatus::WAITING);
        }
    }
}
#else
void RSMaterialFilter::PreProcess(std::shared_ptr<Drawing::Image> imageSnapshot)
{
    if (colorMode_ == AVERAGE && imageSnapshot != nullptr) {
        // update maskColor while persevere alpha
        auto colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
        maskColor_ = RSColor(Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
            Drawing::Color::ColorQuadGetB(colorPicker), maskColor_.GetAlpha());
    }
}
#endif

void RSMaterialFilter::PostProcess(RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setColor(maskColor_.AsArgbInt());
    canvas.drawPaint(paint);
#else
    Drawing::Brush brush;
    brush.SetColor(maskColor_.AsArgbInt());
    canvas.DrawBackground(brush);
#endif
}

std::shared_ptr<RSFilter> RSMaterialFilter::TransformFilter(float fraction) const
{
    MaterialParam materialParam;
    materialParam.radius = radius_ * fraction;
    materialParam.saturation = (saturation_ - 1) * fraction + 1.0;
    materialParam.brightness = (brightness_ - 1) * fraction + 1.0;
    materialParam.maskColor = RSColor(maskColor_.GetRed(), maskColor_.GetGreen(),
        maskColor_.GetBlue(), maskColor_.GetAlpha() * fraction);
    return std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
}

bool RSMaterialFilter::IsValid() const
{
    constexpr float epsilon = 0.05f;
    return radius_ > epsilon;
}

std::shared_ptr<RSFilter> RSMaterialFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::MATERIAL)) {
        return shared_from_this();
    }
    auto materialR = std::static_pointer_cast<RSMaterialFilter>(rhs);

    MaterialParam materialParam;
    materialParam.radius = radius_ + materialR->radius_;
    materialParam.saturation = saturation_ + materialR->saturation_;
    materialParam.brightness = brightness_ + materialR->brightness_;
    materialParam.maskColor = maskColor_ + materialR->maskColor_;
    return std::make_shared<RSMaterialFilter>(materialParam, materialR->colorMode_);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::MATERIAL)) {
        return shared_from_this();
    }
    auto materialR = std::static_pointer_cast<RSMaterialFilter>(rhs);
    MaterialParam materialParam;
    materialParam.radius = radius_ - materialR->radius_;
    materialParam.saturation = saturation_ - materialR->saturation_;
    materialParam.brightness = brightness_ - materialR->brightness_;
    materialParam.maskColor = maskColor_ - materialR->maskColor_;
    return std::make_shared<RSMaterialFilter>(materialParam, materialR->colorMode_);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Multiply(float rhs)
{
    MaterialParam materialParam;
    materialParam.radius = radius_ * rhs;
    materialParam.saturation = saturation_ * rhs;
    materialParam.brightness = brightness_ * rhs;
    materialParam.maskColor = maskColor_ * rhs;
    return std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Negate()
{
    MaterialParam materialParam;
    materialParam.radius = -radius_;
    materialParam.saturation = -saturation_;
    materialParam.brightness = -brightness_;
    materialParam.maskColor = RSColor(0x00000000) - maskColor_;
    return std::make_shared<RSMaterialFilter>(materialParam, colorMode_);
}

#ifndef USE_ROSEN_DRAWING
void RSMaterialFilter::DrawImageRect(
    SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const
#else
void RSMaterialFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
#endif
{
#ifndef USE_ROSEN_DRAWING
    auto paint = GetPaint();
#ifdef NEW_SKIA
    // if kawase blur failed, use gauss blur
    KawaseParameter param = KawaseParameter(src, dst, radius_, colorFilter_, paint.getAlphaf());
    if (KAWASE_BLUR_ENABLED && KawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, image, param)) {
        return;
    }
    canvas.drawImageRect(image.get(), src, dst, SkSamplingOptions(), &paint, SkCanvas::kStrict_SrcRectConstraint);
#else
    canvas.drawImageRect(image.get(), src, dst, &paint);
#endif
#else
    auto brush = GetBrush();
    // if kawase blur failed, use gauss blur
    KawaseParameter param = KawaseParameter(src, dst, radius_, colorFilter_, brush.GetColor().GetAlphaF());
    if (useKawase_ && KawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, image, param)) {
        return;
    }
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*image, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
#endif
}

float RSMaterialFilter::GetRadius() const
{
    return radius_;
}

bool RSMaterialFilter::CanSkipFrame() const
{
    constexpr float HEAVY_BLUR_THRESHOLD = 25.0f;
    return radius_ > HEAVY_BLUR_THRESHOLD;
};

bool RSMaterialFilter::IsNearEqual(const std::shared_ptr<RSFilter>& other, float threshold) const
{
    auto otherMaterialFilter = std::static_pointer_cast<RSMaterialFilter>(other);
    if (otherMaterialFilter == nullptr) {
        ROSEN_LOGE("RSMaterialFilter::IsNearEqual: the types of filters are different.");
        return true;
    }
    return ROSEN_EQ(radius_, otherMaterialFilter->radius_, 1.0f) &&
           ROSEN_EQ(saturation_, otherMaterialFilter->saturation_, threshold) &&
           ROSEN_EQ(brightness_, otherMaterialFilter->brightness_, threshold) &&
           maskColor_.IsNearEqual(otherMaterialFilter->maskColor_, 0);
}

bool RSMaterialFilter::IsNearZero(float threshold) const
{
    return ROSEN_EQ(radius_, 0.0f, threshold);
}

void RSMaterialFilter::ReleaseColorPicker()
{
    if (colorPickerTask_ != nullptr) {
        colorPickerTask_->SetStatus(CacheProcessStatus::WAITING);
        colorPickerTask_->Reset();
#ifdef IS_OHOS
        if (colorPickerTask_->GetHandler() != nullptr) {
            auto task = colorPickerTask_;
            task->GetHandler()->PostTask(
                [task]() { task->ResetGrContext(); }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
#endif
    }
}

void RSMaterialFilter::SetDynamicDarkColorFilter(sk_sp<SkImage> imageSnapshot)
{
    if (!colorPickerTask_) {
        colorPickerTask_ = std::make_shared<RSColorPickerCacheTask>();
    }
    RSColor color;
    if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask_, imageSnapshot)) {
        if (!colorPickerTask_->GetColor(color)) {
            colorPickerTask_->SetStatus(CacheProcessStatus::WAITING);
            return;
        }
    }
    float relaLum = RSColorPicker::CalcRelativeLum(color.AsRgbaInt());
    float lightDegree = (1 + 0.05) / (relaLum + 0.05);
    sk_sp<SkColorFilter> brightnessFilter;
    if (lightDegree  < 1.9) {
        // LightColorDegree less than 1.9 means light color picture.
        float lightupDegree = brightness_ > 1 ? (brightness_ - 1) : brightness_;
        float matrix[20] = {
            lightupDegree, 0, 0, 0, 0,
            0, lightupDegree, 0, 0, 0,
            0, 0, lightupDegree, 0, 0,
            0, 0,   0,           1, 0 };
        brightnessFilter = SkColorFilters::Matrix(matrix);
    } else if (lightDegree > 7) {
        // LightColorDegree greater than 7 means dark color picture.
        SkString blurString(R"(
            uniform half dynamicLightUpRate;
            uniform half dynamicLightUpDeg;
            half4 main(half4 inColor) {
                half4 c = inColor * 255; 
                float x = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
                float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg * 255;
                float R = clamp((c.r + y) / 255, 0.0, 1.0);
                float G = clamp((c.g + y) / 255, 0.0, 1.0);
                float B = clamp((c.b + y) / 255, 0.0, 1.0);
                return vec4(R, G, B, 1.0);
            }
        )");
        auto [gEffect, error] = SkRuntimeEffect::MakeForColorFilter(blurString);
        if (!error.isEmpty()) {
            SkDebugf("shader make failed %s\n", error.c_str());
            return;
        }
        struct Uniforms { float dynamicLightUpRate, dynamicLightUpDeg; };
        Uniforms uniforms = {brightness_ - 1, brightness_ -1};
        brightnessFilter = gEffect->makeColorFilter(SkData::MakeWithCopy(&uniforms, sizeof(uniforms)));
    } else {
        // this should be added, because we should recover colorFilter_ to original one
        // when lightDegree change from dark/light to normal 
        float normalizedDegree = brightness_ - 1.0;
        const float brightnessMat[] = {
            1.000000f, 0.000000f, 0.000000f, 0.000000f, normalizedDegree,
            0.000000f, 1.000000f, 0.000000f, 0.000000f, normalizedDegree,
            0.000000f, 0.000000f, 1.000000f, 0.000000f, normalizedDegree,
            0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f,
        };
        brightnessFilter = SkColorFilters::Matrix(brightnessMat);
    }
    SkColorMatrix cm;
    cm.setSaturation(saturation_);
    sk_sp<SkColorFilter> satFilter = SkColorFilters::Matrix(cm);
    colorFilter_ = SkColorFilters::Compose(satFilter, brightnessFilter);
    hash_ = SkOpts::hash(&colorFilter_, sizeof(colorFilter_), hash_);
}
} // namespace Rosen
} // namespace OHOS
