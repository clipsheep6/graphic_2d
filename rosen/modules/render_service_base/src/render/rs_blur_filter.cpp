/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"
#include "render/rs_blur_filter.h"

#if defined(NEW_SKIA)
#include "include/core/SkTileMode.h"
#include "include/effects/SkImageFilters.h"
#else
#include "include/effects/SkBlurImageFilter.h"
#endif
namespace OHOS {
namespace Rosen {
std::shared_ptr<RSBlurFilter> RSBlurFilter::CreateInstance(float blurRadiusX, float blurRadiusY)
{
    constexpr float epsilon = 0.001f;
    if (ROSEN_EQ(blurRadiusX, 0.f, epsilon) && ROSEN_EQ(blurRadiusY, 0.f, epsilon)) {
        return nullptr;
    }
    return std::shared_ptr<RSBlurFilter>(new RSBlurFilter(blurRadiusX, blurRadiusY));
}

#if defined(NEW_SKIA)
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY): RSSkiaFilter(SkImageFilters::Blur(blurRadiusX,
    blurRadiusY, SkTileMode::kClamp, nullptr)), blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;
}
#else
RSBlurFilter::RSBlurFilter(float blurRadiusX, float blurRadiusY): RSSkiaFilter(SkBlurImageFilter::Make(blurRadiusX,
    blurRadiusY, nullptr, nullptr, SkBlurImageFilter::kClamp_TileMode)), blurRadiusX_(blurRadiusX),
    blurRadiusY_(blurRadiusY)
{
    type_ = FilterType::BLUR;
}
#endif

RSBlurFilter::~RSBlurFilter() {}

float RSBlurFilter::GetBlurRadiusX()
{
    return blurRadiusX_;
}

float RSBlurFilter::GetBlurRadiusY()
{
    return blurRadiusY_;
}

std::string RSBlurFilter::GetDescription()
{
    return "RSBlurFilter blur radius is " + std::to_string(blurRadiusX_) + " sigma";
}

std::shared_ptr<RSSkiaFilter> RSBlurFilter::Compose(const std::shared_ptr<RSSkiaFilter>& inner)
{
    std::shared_ptr<RSBlurFilter> blur = RSBlurFilter::CreateInstance(blurRadiusX_, blurRadiusY_);
    blur->imageFilter_ = SkImageFilters::Compose(imageFilter_, inner->GetImageFilter());
    return blur;
}

std::shared_ptr<RSFilter> RSBlurFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return RSBlurFilter::CreateInstance(blurRadiusX_ + blurR->GetBlurRadiusX(),
        blurRadiusY_ + blurR->GetBlurRadiusY());
}

std::shared_ptr<RSFilter> RSBlurFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    if ((rhs == nullptr) || (rhs->GetFilterType() != FilterType::BLUR)) {
        return shared_from_this();
    }
    auto blurR = std::static_pointer_cast<RSBlurFilter>(rhs);
    return RSBlurFilter::CreateInstance(blurRadiusX_ - blurR->GetBlurRadiusX(),
        blurRadiusY_ - blurR->GetBlurRadiusY());
}

std::shared_ptr<RSFilter> RSBlurFilter::Multiply(float rhs)
{
    return RSBlurFilter::CreateInstance(blurRadiusX_ * rhs, blurRadiusY_ * rhs);
}

std::shared_ptr<RSFilter> RSBlurFilter::Negate()
{
    return RSBlurFilter::CreateInstance(-blurRadiusX_, -blurRadiusY_);
}
} // namespace Rosen
} // namespace OHOS
