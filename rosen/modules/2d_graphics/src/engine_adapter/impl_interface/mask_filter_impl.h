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

#ifndef MASK_FILTER_IMPL_H
#define MASK_FILTER_IMPL_H

#include <memory>

#include "effect/mask_filter.h"
#include "mask_filter_data.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
enum class BlurType;
class MaskFilterImpl {
public:
    MaskFilterImpl() noexcept {}
    virtual ~MaskFilterImpl() {}

    virtual void InitWithBlur(BlurType t, scalar radius) = 0;
    virtual void InitWithCompose(const MaskFilterData* d1, const MaskFilterData* d2) = 0;
    const MaskFilterData * GetMaskFilterData() const { return maskFilterData_.get(); }
protected:
    std::shared_ptr<MaskFilterData> maskFilterData_;
};
}
}
#endif