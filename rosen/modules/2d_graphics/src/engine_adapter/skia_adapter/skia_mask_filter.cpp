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

#include "skia_mask_filter.h"

#include "src/core/SkBlurMask.h"

#include "skia_mask_filter_data.h"
#include "effect/mask_filter.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaMaskFilter::SkiaMaskFilter() noexcept
{
    maskFilterData_ = std::make_shared<SkiaMaskFilterData>();
    maskFilterData_->type = MaskFilterData::SKIA_MASK_FILTER;
}

void SkiaMaskFilter::InitWithBlur(BlurType t, scalar radius)
{
    SkScalar sima = SkBlurMask::ConvertRadiusToSigma(radius);
    auto f = SkMaskFilter::MakeBlur(static_cast<SkBlurStyle>(t), sima);
    maskFilterData_->SetMaskFilter(f.release());
}

void SkiaMaskFilter::InitWithCompose(const MaskFilterData* d1, const MaskFilterData* d2)
{
    sk_sp<SkMaskFilter> outer(static_cast<SkMaskFilter*>(d1->GetMaskFilter()));
    sk_sp<SkMaskFilter> inner(static_cast<SkMaskFilter*>(d2->GetMaskFilter()));
    auto f = SkMaskFilter::MakeCompose(outer, inner);
    maskFilterData_->SetMaskFilter(f.release());
}

sk_sp<SkMaskFilter> SkiaMaskFilter::MutableMaskFilter()
{
    return sk_sp<SkMaskFilter>(static_cast<SkMaskFilter*>(maskFilterData_->GetMaskFilter()));
}
}
}
}