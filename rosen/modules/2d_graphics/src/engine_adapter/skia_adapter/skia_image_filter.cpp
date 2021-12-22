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

#include "skia_image_filter.h"

#include "include/effects/SkImageFilters.h"
#include "include/effects/SkBlurImageFilter.h"

#include "skia_image_filter_data.h"

namespace OHOS {
namespace Rosen {
SkiaImageFilter::SkiaImageFilter() noexcept
{
    imageFilterData_ = std::make_shared<SkiaImageFilterData>();
    imageFilterData_->type = ImageFilterData::SKIA_IMAGE_FILTER;
}

void SkiaImageFilter::InitWithBlur(scalar sigmaX , scalar sigmaY, const ImageFilterData* d)
{
    sk_sp<SkImageFilter> input(static_cast<SkImageFilter*>(d->GetImageFilter()));
    auto f = SkBlurImageFilter::Make(sigmaX, sigmaY, input);
    imageFilterData_->SetImageFilter(f.release());
}

void SkiaImageFilter::InitWithColor(const ColorFilterData* d1, const ImageFilterData* d2)
{
    sk_sp<SkColorFilter> cf(static_cast<SkColorFilter*>(d1->GetColorFilter()));
    sk_sp<SkImageFilter> input(static_cast<SkImageFilter*>(d2->GetImageFilter()));
    auto f = SkImageFilters::ColorFilter(cf, input);
    imageFilterData_->SetImageFilter(f.release());
}

void SkiaImageFilter::InitWithOffset(scalar dx, scalar dy, const ImageFilterData* d)
{
    sk_sp<SkImageFilter> input(static_cast<SkImageFilter*>(d->GetImageFilter()));
    auto f = SkImageFilters::Offset(dx, dy, input);
    imageFilterData_->SetImageFilter(f.release());
}

void SkiaImageFilter::InitWithArithmetic(scalar k1, scalar k2, scalar k3, scalar k4, bool enforcePMColor,
                                         const ImageFilterData* d1, const ImageFilterData* d2)
{
    sk_sp<SkImageFilter> background(static_cast<SkImageFilter*>(d1->GetImageFilter()));
    sk_sp<SkImageFilter> foreground(static_cast<SkImageFilter*>(d2->GetImageFilter()));
    auto f = SkImageFilters::Arithmetic(k1, k2, k3, k4, enforcePMColor, background, foreground);
    imageFilterData_->SetImageFilter(f.release());
}

void SkiaImageFilter::InitWithCompose(const ImageFilterData* d1, const ImageFilterData* d2)
{
    sk_sp<SkImageFilter> outer(static_cast<SkImageFilter*>(d1->GetImageFilter()));
    sk_sp<SkImageFilter> inner(static_cast<SkImageFilter*>(d2->GetImageFilter()));
    auto f = SkImageFilters::Compose(outer, inner);
    imageFilterData_->SetImageFilter(f.release());
}

sk_sp<SkImageFilter> SkiaImageFilter::MutableImageFilter()
{
    return sk_sp<SkImageFilter>(static_cast<SkImageFilter*>(imageFilterData_->GetImageFilter()));
}
}
}