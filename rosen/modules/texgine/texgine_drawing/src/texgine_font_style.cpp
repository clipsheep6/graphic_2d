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

#include "texgine_font_style.h"

namespace Texgine {
TexgineFontStyle::TexgineFontStyle()
{
    fontStyle_ = std::make_shared<SkFontStyle>();
}

TexgineFontStyle::TexgineFontStyle(int weight, int width, Slant slant)
{
    fontStyle_ = std::make_shared<SkFontStyle>(weight, width, static_cast<SkFontStyle::Slant>(slant));
}

TexgineFontStyle::TexgineFontStyle(std::shared_ptr<SkFontStyle> style)
{
    fontStyle_ = style;
}

std::shared_ptr<SkFontStyle> TexgineFontStyle::GetFontStyle()
{
    return fontStyle_;
}

void TexgineFontStyle::SetFontStyle(std::shared_ptr<SkFontStyle> fontStyle)
{
    fontStyle_ = fontStyle;
}

void TexgineFontStyle::SetStyle(SkFontStyle style)
{
    *fontStyle_ = style;
}
} // namespace Texgine
