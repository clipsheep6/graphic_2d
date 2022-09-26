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
    double heightScale_ = 1;
    bool heightOnly_ = false;
    bool useLineStyle_ = false;

    FontWeight lineStyleFontWeight_ = FontWeight::W400;
    FontStyle lineStyleFontStyle_ = FontStyle::Normal;
    std::vector<std::string> lineStyleFontFamilies_;
    double lineStyleFontSize_ = 14;
    double lineStyleHeightScale_ = 1;
    bool lineStyleHeightOnly_ = false;
    double lineStyleSpacingScale_ = -1;
    bool lineStyleOnly_ = false;

    TextAlign textAlign_ = TextAlign::Start;
    TextDirection textDirection_ = TextDirection::LTR;
    size_t maxLines_ = 1e9;
    std::u16string ellipsis_;
    std::string locale_;

    BreakStrategy breakStrategy_ = BreakStrategy::Greedy;
    WordBreakType wordBreakType_ = WordBreakType::BreakWord;

    TextStyle GetTextStyle() const;
    TextAlign GetEffectiveAlign() const;
    bool IsUnlimitedLines() const;
    bool IsEllipsized() const;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_TEXT_EXPORT_ROSEN_TEXT_TYPOGRAPHY_STYLE_H
