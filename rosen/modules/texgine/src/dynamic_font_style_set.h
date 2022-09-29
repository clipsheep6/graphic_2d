/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_SRC_DYNAMIC_FONT_STYLE_SET_H
#define ROSEN_MODULES_TEXGINE_SRC_DYNAMIC_FONT_STYLE_SET_H

#include <include/core/SkFontMgr.h>
#include <include/core/SkTypeface.h>

#include "typeface.h"

class SkTypeface;

namespace Texgine {
class DynamicFontStyleSet : public SkFontStyleSet {
public:
    DynamicFontStyleSet(std::unique_ptr<Typeface> typeface) noexcept(true);

    int count() noexcept(true) override;
    void getStyle(int index, SkFontStyle *style, SkString *name) noexcept(false) override;
    SkTypeface* createTypeface(int index) noexcept(true) override;
    SkTypeface* matchStyle(const SkFontStyle& pattern) noexcept(true) override;

private:
    std::unique_ptr<Typeface> typeface_ = nullptr;
};
} // namespace Texgine

#endif // ROSEN_MODULES_TEXGINE_SRC_DYNAMIC_FONT_STYLE_SET_H
