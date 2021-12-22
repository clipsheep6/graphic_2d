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

#ifndef SKIA_COLOR_FILTER_H
#define SKIA_COLOR_FILTER_H

#include "include/core/SkColorFilter.h"

#include "impl_interface/color_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaColorFilter : public ColorFilterImpl {
public:
    SkiaColorFilter() noexcept;
    ~SkiaColorFilter() {};

    void InitWithBlendMode(ColorQuad c, BlendMode mode) override;
    void InitWithColorMatrix(ColorMatrix& m) override;
    void InitWithLinearToSrgbGamma() override;
    void InitWithSrgbGammaToLinear() override;
    void InitWithCompose(const ColorFilterData* d1, const ColorFilterData* d2) override;
    void Compose(const ColorFilterData * d) override;
private:
    sk_sp<SkColorFilter> MutableColorFilter();
};
}
}
}
#endif
