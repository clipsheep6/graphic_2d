/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "skia_font_style.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaFontStyle::SkiaFontStyle() noexcept: fontSkStyle_(std::make_shared<SkFontStyle>())
{ 
}

SkiaFontStyle::SkiaFontStyle(
    int weight, int width, Slant slant) noexcept: fontSkStyle_(
        std::make_shared<SkFontStyle>(weight, width, static_cast<SkFontStyle::Slant>(slant)))
{
}

std::shared_ptr<SkFontStyle> SkiaFontStyle::GetSkFontStyle() const
{
     return fontSkStyle_;
}

void SkiaFontStyle::SetSkFontStyle(const std::shared_ptr<SkFontStyle> fontStyle)
{
    fontSkStyle_ = fontStyle;
}

void SkiaFontStyle::SetSkStyle(const SkFontStyle &style)
{
    if (fontSkStyle_ == nullptr) {
        return;
    }
    *fontSkStyle_ = style;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
