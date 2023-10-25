/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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
    const static inline std::u16string ELLIPSIS = u"\u2026";

#ifndef USE_GRAPHIC_TEXT_GINE
    FontWeight fontWeight_ = FontWeight::W400;
    FontStyle fontStyle_ = FontStyle::NORMAL;
    std::string fontFamily_ = "";
    double fontSize_ = 14.0; // default is libtxt text style fonst size
    double heightScale_ = 1.0;
    bool heightOnly_ = false;
    bool useLineStyle_ = false;

    FontWeight lineStyleFontWeight_ = FontWeight::W400;
    FontStyle lineStyleFontStyle_ = FontStyle::NORMAL;
    std::vector<std::string> lineStyleFontFamilies_;
    double lineStyleFontSize_ = 14.0; // default is libtxt text style font size
    double lineStyleHeightScale_ = 1.0;
    bool lineStyleHeightOnly_ = false;
    double lineStyleSpacingScale_ = -1.0;
    bool lineStyleOnly_ = false;

    TextAlign textAlign_ = TextAlign::START;
    TextDirection textDirection_ = TextDirection::LTR;
    size_t maxLines_ = 1e9;
    std::u16string ellipsis_;
    std::string locale_;

    BreakStrategy breakStrategy_ = BreakStrategy::GREEDY;
    WordBreakType wordBreakType_ = WordBreakType::BREAK_WORD;
#else
    FontWeight fontWeight = FontWeight::W400;
    FontStyle fontStyle = FontStyle::NORMAL;
    std::string fontFamily = "";
    double fontSize = 14.0; // default is libtxt text style fonst size
    double heightScale = 1.0;
    bool heightOnly = false;
    bool useLineStyle = false;

    FontWeight lineStyleFontWeight = FontWeight::W400;
    FontStyle lineStyleFontStyle = FontStyle::NORMAL;
    std::vector<std::string> lineStyleFontFamilies;
    double lineStyleFontSize = 14.0; // default is libtxt text style font size
    double lineStyleHeightScale = 1.0;
    bool lineStyleHeightOnly = false;
    double lineStyleSpacingScale = -1.0;
    bool lineStyleOnly = false;

    TextAlign textAlign = TextAlign::START;
    TextDirection textDirection = TextDirection::LTR;
    size_t maxLines = 1e9;
    std::u16string ellipsis;
    std::string locale;

    BreakStrategy breakStrategy = BreakStrategy::GREEDY;
    WordBreakType wordBreakType = WordBreakType::BREAK_WORD;
    EllipsisModal ellipsisModal = EllipsisModal::TAIL;
#endif

    TextStyle GetTextStyle() const;
    TextAlign GetEffectiveAlign() const;
    bool IsUnlimitedLines() const;
    bool IsEllipsized() const;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_STYLE_H
