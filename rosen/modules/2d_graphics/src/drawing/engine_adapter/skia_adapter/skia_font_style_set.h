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

#ifndef SKIA_FONT_STYLE_SET_H
#define SKIA_FONT_STYLE_SET_H

#include <include/core/SkFontMgr.h>
#include "text/font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontStyleSet : public FontStyleSetImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaFontStyleSet() noexcept {};
    explicit SkiaFontStyleSet(SkFontStyleSet *set) noexcept;
    ~SkiaFontStyleSet() override;
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    int Count() const override;

    void GetStyle(const int index, std::shared_ptr<FontStyle> style, std::shared_ptr<FontString> name) const override;

    std::shared_ptr<Typeface> CreateTypeface(int index) override;

    std::shared_ptr<Typeface> MatchStyle(const std::shared_ptr<FontStyle> pattern) override;

    std::shared_ptr<FontStyleSet> CreateEmpty() override;
private:
    SkFontStyleSet *skFontStyleSet_ = nullptr;
};
} // Drawing
} // Rosen
} // OHOS

#endif
