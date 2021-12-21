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

#include "skia_color_filter.h"
#include "skia_color_filter_data.h"

namespace OHOS {
namespace Rosen {
SkiaColorFilter::SkiaColorFilter() noexcept
{
    colorFilterData_ = std::make_shared<SkiaColorFilterData>();
    colorFilterData_->type = ColorFilterData::SKIA_COLOR_FILTER;
}

void SkiaColorFilter::InitWithBlendMode(ColorQuad c, BlendMode mode)
{
    auto f = SkColorFilters::Blend(static_cast<SkColor>(c), static_cast<SkBlendMode>(mode));
    colorFilterData_->SetColorFilter(f.release());
}

void SkiaColorFilter::InitWithColorMatrix(ColorMatrix& m)
{
    scalar dst[20];
    m.GetArray(dst);
    auto f = SkColorFilters::Matrix(dst);
    colorFilterData_->SetColorFilter(f.release());
}

void SkiaColorFilter::InitWithLinearToSrgbGamma()
{
    auto f = SkColorFilters::LinearToSRGBGamma();
    colorFilterData_->SetColorFilter(f.release());
}

void SkiaColorFilter::InitWithSrgbGammaToLinear()
{
    auto f = SkColorFilters::SRGBToLinearGamma();
    colorFilterData_->SetColorFilter(f.release());
}

void SkiaColorFilter::InitWithCompose(const ColorFilterData* d1, const ColorFilterData* d2)
{
    sk_sp<SkColorFilter> outer(static_cast<SkColorFilter*>(d1->GetColorFilter()));
    sk_sp<SkColorFilter> inner(static_cast<SkColorFilter*>(d2->GetColorFilter()));
    auto f = SkColorFilters::Compose(outer, inner);
    colorFilterData_->SetColorFilter(f.release());
}

void SkiaColorFilter::Compose(const ColorFilterData * d)
{
     sk_sp<SkColorFilter> skFilter(static_cast<SkColorFilter*>(d->GetColorFilter()));
     auto f = MutableColorFilter()->makeComposed(skFilter);
     colorFilterData_->SetColorFilter(f.release());
}

sk_sp<SkColorFilter> SkiaColorFilter::MutableColorFilter()
{
    return sk_sp<SkColorFilter>(static_cast<SkColorFilter*>(colorFilterData_->GetColorFilter()));
}
}
}