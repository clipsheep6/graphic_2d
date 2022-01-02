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

#ifndef COLOR_FILTER_IMPL_H
#define COLOR_FILTER_IMPL_H

#include "color_filter_data.h"
#include "draw/blend_mode.h"
#include "draw/color.h"
#include "effect/color_matrix.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorFilterImpl {
public:
    ColorFilterImpl() noexcept {}
    virtual ~ColorFilterImpl() {}

    virtual void InitWithBlendMode(ColorQuad c, BlendMode mode) = 0;
    virtual void InitWithColorMatrix(ColorMatrix& m) = 0;
    virtual void InitWithLinearToSrgbGamma() = 0;
    virtual void InitWithSrgbGammaToLinear() = 0;
    virtual void InitWithCompose(const ColorFilterData* d1, const ColorFilterData* d2) = 0;
    virtual void Compose(const ColorFilterData * d) = 0;
    const ColorFilterData * GetColorFilterData() const { return colorFilterData_.get(); }
protected:
    std::shared_ptr<ColorFilterData> colorFilterData_;
};
}
}
}
#endif