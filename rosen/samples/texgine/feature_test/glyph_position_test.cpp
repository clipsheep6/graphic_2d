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

#include <sstream>

#include <texgine/dynamic_file_font_provider.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"
#include "text_define.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
struct GlyphPosition {
    double x = 0;
    double y = 5;
    TypographyStyle ys;
} g_datas[] {
    { .x = -5,  .y = -5 },
    { .x = -5,  .y = 15 },
    { .x = 5,   .y = -5 },
    { .x = -5,  .y = 85 },
    { .x = 155, .y = -5 },
    { .x = 155, .y = 15 },
    { .x = 155, .y = 85 },
    { .x = 96,  .y = 8 },
    { .x = 100, .y = 8 },
    { .x = 137, .y = 15 },
    { .x = 96,  .y = 8, .ys = { .align_ = TextAlign::CENTER, }, },
    { .x = 100, .y = 8, .ys = { .align_ = TextAlign::CENTER, }, },
    { .x = 123, .y = 70, .ys = { .align_ = TextAlign::CENTER, }, },
    { .x = 22,  .y = 70, .ys = { .align_ = TextAlign::CENTER, }, },
    { .x = 46,  .y = 70, },
    { .x = 52,  .y = 70, },
    { .x = 55,  .y = 70, },
    { .x = 52,  .y = 67, },
};

constexpr auto TEXT = "hello world hello world 你好世界 你好世界， hello openharmony, hello openharmony, 你好鸿蒙";

class GlyphPositionTest : public TestFeature {
public:
    GlyphPositionTest() : TestFeature("GlyphPositionTest")
    {
    }

    void Layout()
    {
        TextStyle xs;
        xs.fontFamilies_ = {"Segoe UI Emoji"};
        xs.fontSize_ = 14;

        for (auto &data : g_datas) {
            auto dfProvider = DynamicFileFontProvider::Create();
            dfProvider->LoadFont("Segoe UI Emoji", RESOURCE_PATH_PREFIX "seguiemj.ttf");
            auto fps = FontProviders::Create();
            fps->AppendFontProvider(dfProvider);
            fps->AppendFontProvider(SystemFontProvider::GetInstance());

            auto builder = TypographyBuilder::Create(data.ys, std::move(fps));

            builder->PushStyle(xs);
            builder->AppendSpan(TEXT);
            builder->PopStyle();

            std::string emojiText = Woman Skin1 ZWJ RedHeart ZWJ Man Skin0;
            builder->PushStyle(xs);
            builder->AppendSpan(emojiText);
            builder->PopStyle();

            auto onPaint = [data](const struct TypographyData &tyData, TexgineCanvas &canvas, double x, double y) {
                auto &typography = tyData.typography;
                typography->Paint(canvas, x, y);
                TexginePaint paint;
                paint.SetAntiAlias(true);
                paint.SetColor(0x7F00FF00);
                paint.SetStrokeWidth(5);
                canvas.DrawLine(x + data.x - 10, y + data.y, x + data.x + 10, y + data.y, paint);
                canvas.DrawLine(x + data.x, y + data.y - 10, x + data.x, y + data.y + 10, paint);
                paint.SetColor(0xFF00FF);
                paint.SetAlpha(255 * 0.3);
                canvas.Save();
                canvas.Translate(x, y);
                auto ia = typography->GetGlyphIndexByCoordinate(data.x, data.y);
                auto rects = typography->GetTextRectsByBoundary(Boundary{ia.index_, ia.index_ + 1},
                    TextRectHeightStyle::COVER_TOP_AND_BOTTOM, TextRectWidthStyle::TIGHT);
                if (rects.size()) {
                    canvas.DrawRect(rects.back().rect_, paint);
                }
                canvas.Restore();
            };

            std::stringstream ss;
            ss << "(" << data.x << ", " << data.y << ")";
            if (data.ys.align_ == TextAlign::CENTER) {
                ss << " align(center)";
            }

            auto typography = builder->Build();
            double widthLimit = 150.0;
            typography->Layout(widthLimit);
            typographies_.push_back({
                .typography = typography,
                .comment = ss.str(),
                .onPaint = onPaint,
            });
        }
    }
} g_test;
} // namespace
