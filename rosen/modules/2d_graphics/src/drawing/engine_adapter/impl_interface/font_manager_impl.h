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

#ifndef FONT_MANAGER_IMPL_H
#define FONT_MANAGER_IMPL_H

#include "base_impl.h"
#include "text/typeface.h"
#include "text/font_style.h"
#include "text/font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontManager;
class FontManagerImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    FontManagerImpl() noexcept {};
    ~FontManagerImpl() override {};

    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }
    virtual std::shared_ptr<FontManager> RefDefault() = 0;

    virtual std::shared_ptr<Typeface> MatchFamilyStyleCharacter(const std::string &familyName,
        const FontStyle &style, const char* bcp47[], int bcp47Count, int32_t character) = 0;
    virtual std::shared_ptr<FontStyleSet> MatchFamily(const std::string &familyName) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // FONT_METRICS_IMPL_H
