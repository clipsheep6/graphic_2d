/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROSEN_TEXT_ADAPTER_TEXGINE_CONVERT_H
#define ROSEN_TEXT_ADAPTER_TEXGINE_CONVERT_H

#include "font_collection.h"
#include "rosen_text/typography_create.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_style.h"
#include "texgine/any_span.h"
#include "texgine/text_style.h"
#include "texgine/typography.h"
#include "texgine/typography_style.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTexgine {
#define DEFINE_CONVERT_FUNC(from, to) to Convert(const from &)

// from interface to adapter txt
DEFINE_CONVERT_FUNC(std::shared_ptr<OHOS::Rosen::FontCollection>,
                    std::shared_ptr<OHOS::Rosen::AdapterTexgine::FontCollection>);

// from texgine to rosen_text
DEFINE_CONVERT_FUNC(Texgine::IndexAndAffinity, IndexAndAffinity);
DEFINE_CONVERT_FUNC(Texgine::Boundary,         Boundary);
DEFINE_CONVERT_FUNC(Texgine::TextRect,         TextRect);
DEFINE_CONVERT_FUNC(Texgine::Affinity,         Affinity);
DEFINE_CONVERT_FUNC(Texgine::TextDirection,    TextDirection);

// from rosen_text to texgine
DEFINE_CONVERT_FUNC(TextRectHeightStyle,          Texgine::TextRectHeightStyle);
DEFINE_CONVERT_FUNC(TextRectWidthStyle,           Texgine::TextRectWidthStyle);
DEFINE_CONVERT_FUNC(WordBreakType,                Texgine::WordBreakType);
DEFINE_CONVERT_FUNC(BreakStrategy,                Texgine::BreakStrategy);
DEFINE_CONVERT_FUNC(TypographyStyle,              Texgine::TypographyStyle);
DEFINE_CONVERT_FUNC(TextStyle,                    Texgine::TextStyle);
DEFINE_CONVERT_FUNC(FontWeight,                   Texgine::FontWeight);
DEFINE_CONVERT_FUNC(FontStyle,                    Texgine::FontStyle);
DEFINE_CONVERT_FUNC(TextAlign,                    Texgine::TextAlign);
DEFINE_CONVERT_FUNC(TextBaseline,                 Texgine::TextBaseline);
DEFINE_CONVERT_FUNC(TextDirection,                Texgine::TextDirection);
DEFINE_CONVERT_FUNC(TextDecorationStyle,          Texgine::TextDecorationStyle);
DEFINE_CONVERT_FUNC(TextDecoration,               Texgine::TextDecoration);
DEFINE_CONVERT_FUNC(PlaceholderVerticalAlignment, Texgine::AnySpanAlignment);

#undef DEFINE_CONVERT_FUNC
} // namespace AdapterTexgine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TEXGINE_CONVERT_H
