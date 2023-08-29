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

#ifndef FONT_STYLE_SET_IMPL_H
#define FONT_STYLE_SET_IMPL_H

#include "base_impl.h"
#include "text/typeface.h"
#include "text/font_string.h"
#include "text/font_style.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontStyleSet;
class FontStyleSetImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    FontStyleSetImpl() noexcept {};
    ~FontStyleSetImpl() override {};

    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }
    
    virtual int Count() const = 0;

    virtual void GetStyle(const int index, std::shared_ptr<FontStyle> style, std::shared_ptr<FontString> name) const = 0;

    virtual std::shared_ptr<Typeface> CreateTypeface(int index) = 0;

    virtual std::shared_ptr<Typeface> MatchStyle(const std::shared_ptr<FontStyle> pattern) = 0;

    virtual std::shared_ptr<FontStyleSet> CreateEmpty() = 0;

};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // FONT_IMPL_H
