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

#ifndef FONT_MGR_IMPL_H
#define FONT_MGR_IMPL_H

#include "impl_interface/base_impl.h"
#include "text/font_style_set.h"
#include "text/typeface.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontMgrImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    ~FontMgrImpl() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }

    virtual Typeface* MatchFamilyStyleCharacter(const char familyName[], const FontStyle& fontStyle,
                                                const char* bcp47[], int bcp47Count,
                                                int32_t character) = 0;
    virtual FontStyleSet* MatchFamily(const char familyName[]) const = 0;

protected:
    FontMgrImpl() noexcept = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif