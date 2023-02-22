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

#include "texgine/typography_style.h"

namespace Texgine {
TextStyle TypographyStyle::ConvertToTextStyle() const
{
    TextStyle style;
    style.fontSize_ = fontSize_;
    style.fontFamilies_ = fontFamilies_;
    style.heightScale_ = heightScale_;
    style.heightOnly_ = heightOnly_;
    style.fontWeight_ = fontWeight_;
    style.fontStyle_ = fontStyle_;

    return style;
}

TextAlign TypographyStyle::GetEquivalentAlign() const
{
    TextAlign align = align_;
    bool isLTR = direction_ == TextDirection::LTR;
    TextAlign trim = align & TextAlign::Trim;
    align &= ~TextAlign::Trim;
    if (align == TextAlign::Start) {
        align = isLTR ? TextAlign::Left : TextAlign::Right;
    } else if (align == TextAlign::End) {
        align = isLTR ? TextAlign::Right : TextAlign::Left;
    } else if (align == TextAlign::Justify) {
        align = isLTR ? TextAlign::Justify : TextAlign::Right;
    }
    return align | trim;
}
} // namespace Texgine
