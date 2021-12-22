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

#include "effect/mask_filter.h"

#include "impl_factory.h"
#include "impl_interface/mask_filter_impl.h"

namespace OHOS {
namespace Rosen {
MaskFilter::MaskFilter(FilterType t, BlurType blurType, scalar radius) noexcept : MaskFilter()
{
    type_ = t;
    impl_->InitWithBlur(blurType, radius);
}

MaskFilter::MaskFilter(FilterType t, MaskFilter& f1, MaskFilter& f2) noexcept : MaskFilter()
{
    type_ = t;
    impl_->InitWithCompose(f1.GetMaskFilterData(), f2.GetMaskFilterData());
}

MaskFilter::MaskFilter() noexcept
    : type_(MaskFilter::FilterType::NO_TYPE),
    impl_(ImplFactory::CreateMaskFilterImpl()) {}

MaskFilter::FilterType MaskFilter::GetType() const
{
    return type_;
}

const MaskFilterData* MaskFilter::GetMaskFilterData() const
{
    return impl_->GetMaskFilterData();
}

std::shared_ptr<MaskFilter> MaskFilter::CreateBlurMaskFilter(BlurType blurType, scalar radius)
{
    return std::make_shared<MaskFilter>(MaskFilter::FilterType::BLUR, blurType, radius);
}

std::shared_ptr<MaskFilter> MaskFilter::CreateComposeMaskFilter(MaskFilter& f1, MaskFilter& f2)
{
    return std::make_shared<MaskFilter>(MaskFilter::FilterType::COMPOSE, f1, f2);
}
}
}