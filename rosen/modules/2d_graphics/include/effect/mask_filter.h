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

#ifndef MASK_FILTER_H
#define MASK_FILTER_H

#include <memory>

#include "engine_adapter/impl_interface/mask_filter_impl.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class MaskFilterImpl;
struct MaskFilterData;

enum class BlurType {
    NORMAL,
    SOLID,
    OUTER,
    INNER,
};

class MaskFilter {
public:
    enum class FilterType {
        NO_TYPE,
        BLUR,
        COMPOSE,
    };

    static std::shared_ptr<MaskFilter> CreateBlurMaskFilter(BlurType blurType, scalar radius);
    static std::shared_ptr<MaskFilter> CreateComposeMaskFilter(MaskFilter& f1, MaskFilter& f2);

    ~MaskFilter() {}
    FilterType GetType() const;
    const MaskFilterData* GetMaskFilterData() const;

    MaskFilter(FilterType t, BlurType blurType, scalar radius) noexcept;
    MaskFilter(FilterType t, MaskFilter& f1, MaskFilter& f2) noexcept;
protected:
    MaskFilter() noexcept;
private:
    FilterType type_;
    std::unique_ptr<MaskFilterImpl> impl_;
};
}
}
}
#endif