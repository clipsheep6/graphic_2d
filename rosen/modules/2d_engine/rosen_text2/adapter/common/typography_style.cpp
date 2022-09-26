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

#include "rosen_text/typography_style.h"

namespace OHOS {
namespace Rosen {
TextStyle TypographyStyle::GetTextStyle() const
{
    TextStyle style = {
        .fontWeight_ = fontWeight_,
        .fontStyle_ = fontStyle_,
        .fontFamilies_ = { fontFamily_ },
        .fontSize_ = fontSize_,
        .height_ = height_,
        .hasHeightOverride_ = hasHeightOverride_,
        .locale_ = locale_,
    };
    if (fontSize_ >= 0) {
        style.fontSize_ = fontSize_;
    }

    return style;
}

bool TypographyStyle::UnlimitedLines() const
{
    return maxLines_ == 1e9;
}

bool TypographyStyle::Ellipsized() const
{
    return !ellipsis_.empty();
}

TextAlign TypographyStyle::EffectiveAlign() const
{
    if (textAlign_ == TextAlign::Start) {
        return (textDirection_ == TextDirection::LTR) ? TextAlign::Left : TextAlign::Right;
    } else if (textAlign_ == TextAlign::End) {
        return (textDirection_ == TextDirection::RTL) ? TextAlign::Left : TextAlign::Right;
    } else {
        return textAlign_;
    }
}
} // namespace Rosen
} // namespace OHOS
