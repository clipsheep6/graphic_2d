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
#include <memory>

#include "impl_interface/font_style_set_impl.h"
#include "skia_font_style.h"
//#include "skia_string.h"
#include "skia_typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontStyleSet : public FontStyleSetImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    explicit SkiaFontStyleSet(SkFontStyleSet *set);
    SkiaFontStyleSet() noexcept {}
    ~SkiaFontStyleSet();
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    int Count() const override;

    //void GetStyle(const int index, std::shared_ptr<SkiaFontStyle> style, std::shared_ptr<SkiaString> name) const;

    std::shared_ptr<SkiaTypeface> CreateTypeface(int index);

    std::shared_ptr<SkiaTypeface> MatchStyle(const std::shared_ptr<SkiaFontStyle> pattern);

    static std::shared_ptr<SkiaFontStyleSet> CreateEmpty();

    SkFontStyleSet *GetFontStyleSet() const
    {
        return set_;
    }

private:
    SkFontStyleSet *set_ = nullptr;
};
} // Drawing
} // Rosen
} // OHOS

#endif
