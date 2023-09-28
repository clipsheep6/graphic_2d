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

#include "texgine/typography_style.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
TextStyle TypographyStyle::ConvertToTextStyle() const
{
    TextStyle style;
    style.fontSize = fontSize;
    style.fontFamilies = fontFamilies;
    style.halfLeading = halfLeading;
    style.heightScale = heightScale;
    style.heightOnly = heightOnly;
    style.fontWeight = fontWeight;
    style.fontStyle = fontStyle;

    return style;
}

TextAlign TypographyStyle::GetEquivalentAlign() const
{
    TextAlign textAlign = align;
    bool isLTR = direction == TextDirection::LTR;
    TextAlign trim = textAlign & TextAlign::TRIM;
    textAlign &= ~TextAlign::TRIM;
    if (textAlign == TextAlign::START) {
        textAlign = isLTR ? TextAlign::LEFT : TextAlign::RIGHT;
    } else if (textAlign == TextAlign::END) {
        textAlign = isLTR ? TextAlign::RIGHT : TextAlign::LEFT;
    } else if (textAlign == TextAlign::JUSTIFY) {
        textAlign = isLTR ? TextAlign::JUSTIFY : TextAlign::RIGHT;
    }
    return textAlign | trim;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
