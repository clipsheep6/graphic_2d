/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_STYLE_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_STYLE_H

#include <string>
#include <vector>

#include "text_style.h"
#include "typography_types.h"

namespace OHOS {
namespace Rosen {
struct TypographyStyle {
    FontWeight fontWeight_ = FontWeight::W400;
    FontStyle fontStyle_ = FontStyle::Normal;
    std::string fontFamily_ = "";
    double fontSize_ = 14;
    double height_ = 1;
    bool hasHeightOverride_ = false;
    bool strutEnabled_ = false;

    FontWeight strutFontWeight_ = FontWeight::W400;
    FontStyle strutFontStyle_ = FontStyle::Normal;
    std::vector<std::string> strutFontFamilies_;
    double strutFontSize_ = 14;
    double strutHeight_ = 1;
    bool strutHasHeightOverride_ = false;
    double strutLeading_ = -1;
    bool forceStrutHeight_ = false;

    TextAlign textAlign_ = TextAlign::Start;
    TextDirection textDirection_ = TextDirection::LTR;
    size_t maxLines_ = 1e9;
    std::u16string ellipsis_;
    std::string locale_;

    BreakStrategy breakStrategy_ = BreakStrategy::Greedy;
    WordBreakType wordBreakType_ = WordBreakType::BreakWord;

    TextStyle GetTextStyle() const;
    bool UnlimitedLines() const;
    bool Ellipsized() const;
    TextAlign EffectiveAlign() const;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_STYLE_H
