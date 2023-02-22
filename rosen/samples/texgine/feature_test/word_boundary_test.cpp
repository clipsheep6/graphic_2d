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

#include <sstream>

#include <include/core/SkCanvas.h>

#include "texgine/system_font_provider.h"
#include "texgine/typography_builder.h"

#include "feature_test_framework.h"
#include "my_any_span.h"

using namespace Texgine;

namespace {
std::vector<size_t> positions = {0, 1, 2, 3, 4, 5, 6, 7, 11, 12, 13, 17, 18, 19};

class WordBoundaryTest : public TestFeature {
public:
    WordBoundaryTest() : TestFeature("WordBoundaryTest")
    {
    }

    void Layout()
    {
        auto builder = TypographyBuilder::Create();
        builder->AppendSpan("你好");
        builder->PushStyle({});
        builder->PopStyle();
        builder->AppendSpan("世界, ");
        builder->AppendSpan(std::make_shared<MyAnySpan>(50, 50));
        builder->AppendSpan("He");
        builder->PushStyle({});
        builder->PopStyle();
        builder->AppendSpan("llo World.");
        auto typography = builder->Build();
        typography->Layout(200);

        for (const auto &index : positions) {
            auto onPaint = [index](const struct TypographyData &tyData, SkCanvas &canvas, double x, double y) {
                const auto &ty = tyData.typography;
                ty->Paint(canvas, x, y);

                SkPaint paint;
                paint.setAntiAlias(true);
                paint.setColor(0x5500FF00);
                paint.setStyle(SkPaint::kFill_Style);
                constexpr auto ws = TextRectWidthStyle::Tight;
                constexpr auto hs = TextRectHeightStyle::Tight;

                const auto &[left, right] = ty->GetWordBoundaryByIndex(index);
                auto wordRects = ty->GetTextRectsByBoundary(Boundary{left, right}, hs, ws);
                for (auto &[rect, _] : wordRects) {
                    rect.offset(x, y);
                    canvas.drawRect(rect, paint);
                }

                paint.setColor(0xFF000000);
                paint.setStyle(SkPaint::kStroke_Style);
                auto indexRect = ty->GetTextRectsByBoundary(Boundary{index, index + 1}, hs, ws);
                for (auto &[rect, _] : indexRect) {
                    rect.offset(x, y);
                    canvas.drawRect(rect, paint);
                }
            };

            std::stringstream ss;
            ss << "index: " << index;
            typographies_.push_back({
                .typography = typography,
                .comment = ss.str(),
                .onPaint = onPaint,
            });
        }
    }
} g_test;
} // namespace
