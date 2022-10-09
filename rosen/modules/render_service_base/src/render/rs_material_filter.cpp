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

#include "include/effects/SkBlurImageFilter.h"
#include "render/rs_material_filter.h"

#include "hilog/log.h"
#include "platform/common/rs_log.h"
using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace Rosen {
constexpr int INDEX_R = 0;
constexpr int INDEX_G = 6;
constexpr int INDEX_B = 12;
constexpr int INDEX_A = 18;
constexpr int INDEX_R_offset = 4;
constexpr int INDEX_G_offset = 9;
constexpr int INDEX_B_offset = 14;
constexpr float BLUR_SIGMA_SCALE = 0.57735f;

RSMaterialFilter::RSMaterialFilter(int style, float dipScale)
    : RSSkiaFilter(RSMaterialFilter::CreateMaterialStyle(style, dipScale)), style_(style), dipScale_(dipScale)
{
    type_ = FilterType::MATERIAL;
    radius_ = 0;
    saturation_ = 0;
}

RSMaterialFilter::RSMaterialFilter(sk_sp<SkImageFilter> imageFilter, int style, float dipScale, float radius)
    : RSSkiaFilter(imageFilter), style_(style), dipScale_(dipScale), radius_(radius)
{
    type_ = FilterType::MATERIAL;
    saturation_ = 0;
}    

RSMaterialFilter::~RSMaterialFilter() {}

int RSMaterialFilter::GetStyle() const
{
    return style_;
}

float RSMaterialFilter::GetDipScale() const
{
    return dipScale_;
}

float RSMaterialFilter::RadiusVp2Sigma(float radiusVp, float dipScale) const
{
    float radiusPx = radiusVp * dipScale;

    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + SK_ScalarHalf : 0.0f;
}

/*
* replace drawPaint/drawColor(kSrcOver)
* formula: kSrcOver r = sa*s + (1-sa)*d
* s: source d:destination sa: source alpha
*/
sk_sp<SkColorFilter> RSMaterialFilter::MaskColorFilter(SkColor maskColor)
{
    SkColor4f maskColor4f = SkColor4f::FromColor(maskColor);
    SkScalar colorMatrix[20] = { 0 };
    colorMatrix[INDEX_R] = 1 - maskColor4f.fA;
    colorMatrix[INDEX_G] = 1 - maskColor4f.fA;
    colorMatrix[INDEX_B] = 1 - maskColor4f.fA;
    colorMatrix[INDEX_R_offset] = maskColor4f.fR * maskColor4f.fA;
    colorMatrix[INDEX_G_offset] = maskColor4f.fG * maskColor4f.fA;
    colorMatrix[INDEX_B_offset] = maskColor4f.fB * maskColor4f.fA;
    colorMatrix[INDEX_A] = 1;

    return SkColorFilters::Matrix(colorMatrix);
}

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialFilter(float radius, float sat, SkColor maskColor)
{
    sk_sp<SkImageFilter> blurFilter = SkBlurImageFilter::Make(radius, radius, nullptr, nullptr,
        SkBlurImageFilter::kClamp_TileMode); // blur
    SkColorMatrix cm;
    cm.setSaturation(sat);
    sk_sp<SkColorFilter> satFilter = SkColorFilters::Matrix(cm); // saturation
    sk_sp<SkColorFilter> maskFilter = RSMaterialFilter::MaskColorFilter(maskColor); // mask
    sk_sp<SkColorFilter> filterCompose = SkColorFilters::Compose(maskFilter, satFilter);

    return SkImageFilters::ColorFilter(filterCompose, blurFilter);
}

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialStyle(int style, float dipScale)
{
    switch (style) {
        case STYLE_CARD_THIN_LIGHT:
            return RSMaterialFilter::CreateMaterialFilter(
                RSMaterialFilter::RadiusVp2Sigma(CARDTHINLIGHT.radius, dipScale),
                CARDTHINLIGHT.saturation, CARDTHINLIGHT.maskColor);
        case STYLE_CARD_LIGHT:
            return RSMaterialFilter::CreateMaterialFilter(
                RSMaterialFilter::RadiusVp2Sigma(CARDLIGHT.radius, dipScale),
                CARDLIGHT.saturation, CARDLIGHT.maskColor);
        case STYLE_CARD_THICK_LIGHT:
            return RSMaterialFilter::CreateMaterialFilter(
                RSMaterialFilter::RadiusVp2Sigma(CARDTHICKLIGHT.radius, dipScale),
                CARDTHICKLIGHT.saturation, CARDTHICKLIGHT.maskColor);
        case STYLE_CARD_INDEFINITE:
            return RSMaterialFilter::CreateMaterialFilter(
                RSMaterialFilter::RadiusVp2Sigma(radius_, dipScale),
                CARDTHICKLIGHT.saturation, CARDTHICKLIGHT.maskColor);
        default:
            break;
    }
    return nullptr;
}

float RSMaterialFilter::GetRadius(int style)
{
    switch (style) {
        case STYLE_CARD_THIN_LIGHT:
            return CARDTHINLIGHT.radius;
        case STYLE_CARD_LIGHT:
            return CARDLIGHT.radius;
        case STYLE_CARD_THICK_LIGHT:
            return CARDTHICKLIGHT.radius;
        case STYLE_CARD_INDEFINITE:
            return radius_;
        default:
            break;
    }
    return radius_;
}

SkColor RSMaterialFilter::GetMaskColor(int style)
{
    switch (style) {
        case STYLE_CARD_THIN_LIGHT:
            return CARDTHINLIGHT.maskColor;
        case STYLE_CARD_LIGHT:
            return CARDLIGHT.maskColor;
        case STYLE_CARD_THICK_LIGHT:
            return CARDTHICKLIGHT.maskColor;
        case STYLE_CARD_INDEFINITE:
            return maskColor_;
        default:
            break;
    }
    return maskColor_;
}

float RSMaterialFilter::GetSaturation(int style)
{
    switch (style) {
        case STYLE_CARD_THIN_LIGHT:
            return CARDTHINLIGHT.saturation;
        case STYLE_CARD_LIGHT:
            return CARDLIGHT.saturation;
        case STYLE_CARD_THICK_LIGHT:
            return CARDTHICKLIGHT.saturation;
        case STYLE_CARD_INDEFINITE:
            return saturation_;
        default:
            break;
    }
    return saturation_;
}

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialRadius(int style, float dipScale, float radius)
{
    sk_sp<SkImageFilter> spFilter;
    return spFilter;
}

std::shared_ptr<RSFilter> RSMaterialFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::MATERIAL)) {
         return shared_from_this();
    }
    auto filterM = std::static_pointer_cast<RSMaterialFilter>(rhs);

    // add r1 + r2
    float thisRadius = GetRadius(style_);
    float thisSat = GetSaturation(style_);
    SkColor thisMaskColor = GetMaskColor(style_);

    float thatRadius = filterM->GetRadius(filterM->GetStyle());    //float thatRadius = GetRadius(filterM->GetStyle());
    float newRadius = thisRadius + thatRadius;

    sk_sp<SkImageFilter> spFilter = RSMaterialFilter::CreateMaterialFilter(
        RSMaterialFilter::RadiusVp2Sigma(newRadius, dipScale_),
        thisSat, thisMaskColor);

    char szLog[2048] = {0};
    snprintf(szLog, 2048, "ADD %d/%d. %f+%f=%f", style_, filterM->GetStyle(), thisRadius, thatRadius, newRadius);
    ROSEN_LOGI("--------material [%s]", szLog);

    if (spFilter == nullptr) {
        return shared_from_this();
    }
    return std::make_shared<RSMaterialFilter>(spFilter, STYLE_CARD_INDEFINITE, dipScale_, newRadius);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::MATERIAL)) {
        return shared_from_this();
    }
    auto filterM = std::static_pointer_cast<RSMaterialFilter>(rhs);

    // sub r1 - r2
    float thisRadius = GetRadius(style_);
    float thisSat = GetSaturation(style_);
    SkColor thisMaskColor = GetMaskColor(style_);

    float thatRadius = filterM->GetRadius(filterM->GetStyle());    //float thatRadius = GetRadius(filterM->GetStyle());
    float newRadius = thisRadius - thatRadius;

    sk_sp<SkImageFilter> spFilter = RSMaterialFilter::CreateMaterialFilter(
        RSMaterialFilter::RadiusVp2Sigma(newRadius, dipScale_),
        thisSat, thisMaskColor);

    char szLog[2048] = {0};
    snprintf(szLog, 2048, "SUB %d/%d. %f-%f=%f", style_, filterM->GetStyle(), thisRadius, thatRadius, newRadius);
    ROSEN_LOGI("--------material [%s]", szLog);

    if (spFilter == nullptr) {
        return shared_from_this();
    }
    return std::make_shared<RSMaterialFilter>(spFilter, STYLE_CARD_INDEFINITE, dipScale_, newRadius);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Multiply(float rhs)
{
    // multi r1 * rhs
    float thisRadius = GetRadius(style_);
    float thisSat = GetSaturation(style_);
    SkColor thisMaskColor = GetMaskColor(style_);

    float newRadius = thisRadius * rhs;

    sk_sp<SkImageFilter> spFilter = RSMaterialFilter::CreateMaterialFilter(
        RSMaterialFilter::RadiusVp2Sigma(newRadius, dipScale_),
        thisSat, thisMaskColor);

    char szLog[2048] = {0};
    snprintf(szLog, 2048, "MULTI %d. %f x %f=%f", style_, thisRadius, rhs, newRadius);   // ARK  没转
    ROSEN_LOGI("--------material [%s]", szLog);

    if (spFilter == nullptr) {
        return shared_from_this();
    }
    return std::make_shared<RSMaterialFilter>(spFilter, STYLE_CARD_INDEFINITE, dipScale_, newRadius);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Negate()
{
    // neg  r1
    float thisRadius = GetRadius(style_);
    float thisSat = GetSaturation(style_);
    SkColor thisMaskColor = GetMaskColor(style_);

    float newRadius = -thisRadius;

    sk_sp<SkImageFilter> spFilter = RSMaterialFilter::CreateMaterialFilter(
        RSMaterialFilter::RadiusVp2Sigma(newRadius, dipScale_),
        thisSat, thisMaskColor);

    char szLog[2048] = {0};
    snprintf(szLog, 2048, "NEG %d. %f, %f", style_, thisRadius, newRadius);
    ROSEN_LOGI("--------material [%s]", szLog);

    if (spFilter == nullptr) {
        return shared_from_this();
    }
    return std::make_shared<RSMaterialFilter>(spFilter, STYLE_CARD_INDEFINITE, dipScale_, newRadius);
}

void RSMaterialFilter::print()
{
    char szLog[2048] = {0};
    snprintf(szLog, 2048, "PRINT sytle=%d radius=%f, sat=%f", style_, radius_, saturation_);
    ROSEN_LOGI("--------material [%s]", szLog);
}
} // namespace Rosen
} // namespace OHOS
