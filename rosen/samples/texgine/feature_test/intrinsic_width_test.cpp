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

#include "texgine/any_span.h"
#include "texgine/system_font_provider.h"
#include "texgine/typography_builder.h"

#include "feature_test_framework.h"
#include "my_any_span.h"

using namespace Texgine;

namespace {
constexpr auto exampleText = "hello world, hello openharmony!";
constexpr auto exampleText2 = "你 好 世 界 你好鸿蒙！";

struct IntrinsicTestData {
    TypographyStyle ys_;
    std::string title_;
    double widthLimit_ = 150;
    const char *text_ = exampleText;
    std::shared_ptr<MyAnySpan> as_ = std::make_shared<MyAnySpan>(20, 80);
} datas[] = {
    {
        .ys_ = {
            .ellipsis_ = u"",
        },
        .title_ = "不换行的对照组",
        .widthLimit_ = 700,
    },
    {
        .ys_ = {
            .ellipsis_ = u"",
            .wordBreakType_ = WordBreakType::BREAKWORD,
        },
        .title_ = "breakWord",
    },
    {
        .ys_ = {
            .ellipsis_ = u"",
            .wordBreakType_ = WordBreakType::BREAKALL,
        },
        .title_ = "breakAll",
    },
    {
        .ys_ = {
            .maxLines_ = 1,
        },
        .title_ = "maxline = 1",
    },
    {
        .ys_ = {
            .ellipsis_ = u"",
            .wordBreakType_ = WordBreakType::BREAKALL,
        },
        .title_ = "breakAll，中文",
        .widthLimit_ = 120,
        .text_ = exampleText2,
    },
};

class IntrinsicWidthTest : public TestFeature {
public:
    IntrinsicWidthTest() : TestFeature("IntrinsicWidthTest")
    {
    }

    void Layout()
    {
        for (auto &[ys, title, limit, text, as] : datas) {
            auto builder = TypographyBuilder::Create(ys);
            builder->PushStyle({});
            builder->AppendSpan(text);
            builder->AppendSpan(as);
            builder->PopStyle();

            auto typography = builder->Build();
            typography->Layout(limit);
            auto onPaint = [&](const struct TypographyData &tyData, TexgineCanvas &canvas, double x, double y) {
                // typography
                tyData.typography->Paint(canvas, x, y);

                TexginePaint paint;
                paint.SetAntiAlias(true);
                paint.SetStyle(TexginePaint::kFill_Style);

                // max
                paint.SetColor(0x5500FF00);
                auto rect1 = TexgineRect::MakeXYWH(x, y, tyData.typography->GetMaxIntrinsicWidth(), 10);
                canvas.DrawRect(rect1, paint);

                // min
                paint.SetColor(0x5500FFCC);
                auto rect2 = TexgineRect::MakeXYWH(x, y + 10, tyData.typography->GetMinIntrinsicWidth(), 10);
                canvas.DrawRect(rect2, paint);

                // placeholder
                auto rects = tyData.typography->GetTextRectsOfPlaceholders();
                for (auto &[rect, _] : rects) {
                    rect.GetRect()->offset(x, y);
                    canvas.DrawRect(rect, paint);
                }
            };

            typographies_.push_back({
                .typography = typography,
                .comment = title,
                .atNewline = true,
                .onPaint = onPaint,
            });
        }
    }
} g_test;
} // namespace
