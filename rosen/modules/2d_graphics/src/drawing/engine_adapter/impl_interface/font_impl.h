/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FONT_IMPL_H
#define FONT_IMPL_H

#include "utils/scalar.h"
#include "base_impl.h"
#include "text/font_metrics.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Typeface;

class FontImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    FontImpl() noexcept {};
    ~FontImpl() override {};

    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }

    virtual void SetTypeface(const std::shared_ptr<Typeface> tf) = 0;

    virtual void SetSize(scalar textSize) = 0;

    virtual scalar GetMetrics(FontMetrics *metrics) const = 0;

};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // FONT_IMPL_H
