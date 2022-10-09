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

namespace OHOS {
namespace Rosen {
RSMaterialFilter::RSMaterialFilter(int style, float dipScale)
    : RSSkiaFilter(RSMaterialFilter::CreateMaterialStyle(style, dipScale)), style_(style), dipScale_(dipScale)
{
    type_ = FilterType::MATERIAL;
}

RSMaterialFilter::RSMaterialFilter(sk_sp<SkImageFilter> imageFilter)
    : RSSkiaFilter(imageFilter)
{
    type_ = FilterType::MATERIAL;
    style_ = 0;
    dipScale_ = 0;
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

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialFilter(float radius, float sat, SkColor maskColor)
{
    maskColor_ = maskColor;
    sk_sp<SkImageFilter> blurFilter = SkBlurImageFilter::Make(radius, radius, nullptr, nullptr,
        SkBlurImageFilter::kClamp_TileMode); // blur
    SkColorMatrix cm;
    cm.setSaturation(sat);
    sk_sp<SkColorFilter> satFilter = SkColorFilters::Matrix(cm); // saturation

    return SkImageFilters::ColorFilter(satFilter, blurFilter);
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
        default:
            break;
    }
    return 0;
}

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialRadius(int style, float dipScale, float radius)
{
    sk_sp<SkImageFilter> spFilter;
    switch (style) {
        case STYLE_CARD_THIN_LIGHT:
            spFilter = RSMaterialFilter::CreateMaterialFilter(
                RSMaterialFilter::RadiusVp2Sigma(radius, dipScale),
                CARDTHINLIGHT.saturation, CARDTHINLIGHT.maskColor);
            break;
        case STYLE_CARD_LIGHT:
            spFilter = RSMaterialFilter::CreateMaterialFilter(
                RSMaterialFilter::RadiusVp2Sigma(radius, dipScale),
                CARDLIGHT.saturation, CARDLIGHT.maskColor);
            break;
        case STYLE_CARD_THICK_LIGHT:
            spFilter = RSMaterialFilter::CreateMaterialFilter(
                RSMaterialFilter::RadiusVp2Sigma(radius, dipScale),
                CARDTHICKLIGHT.saturation, CARDTHICKLIGHT.maskColor);
            break;
        default:
            break;
    }
    return spFilter;
}

std::shared_ptr<RSFilter> RSMaterialFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::MATERIAL)) {
         return shared_from_this();
    }
    auto filterM = std::static_pointer_cast<RSMaterialFilter>(rhs);
    float radiusThis = GetRadius(style_);
    float radiusOther = GetRadius(filterM->GetStyle());
    sk_sp<SkImageFilter> spFilter = RSMaterialFilter::CreateMaterialRadius(style_, dipScale_, radiusThis + radiusOther) ;
    if (spFilter == nullptr) {
        return shared_from_this();
    }
    return std::make_shared<RSMaterialFilter>(spFilter);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::MATERIAL)) {
        return shared_from_this();
    }
    auto filterM = std::static_pointer_cast<RSMaterialFilter>(rhs);
    float radiusThis = GetRadius(style_);
    float radiusOther = GetRadius(filterM->GetStyle());
    sk_sp<SkImageFilter> spFilter = RSMaterialFilter::CreateMaterialRadius(style_, dipScale_, radiusThis - radiusOther) ;
    if (spFilter == nullptr) {
        return shared_from_this();
    }
    return std::make_shared<RSMaterialFilter>(spFilter);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Multiply(float rhs)
{
    float radiusThis = GetRadius(style_);
    sk_sp<SkImageFilter> spFilter = RSMaterialFilter::CreateMaterialRadius(style_, dipScale_, radiusThis * rhs) ;
    if (spFilter == nullptr) {
        return shared_from_this();
    }
    return std::make_shared<RSMaterialFilter>(spFilter);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Negate()
{
    float radiusThis = GetRadius(style_);
    sk_sp<SkImageFilter> spFilter = RSMaterialFilter::CreateMaterialRadius(style_, dipScale_, -radiusThis) ;
    if (spFilter == nullptr) {
        return shared_from_this();
    }
    return std::make_shared<RSMaterialFilter>(spFilter);
}
} // namespace Rosen
} // namespace OHOS
