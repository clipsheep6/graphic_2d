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

#include <include/core/SkMaskFilter.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"

using namespace Texgine;

namespace {
struct MyPaint {
    uint32_t color = 0xFF000000;
    bool isAntiAlias = false;
    float blurRadius = 0;
    SkPaint::Style style = SkPaint::kFill_Style;
    float strokeWidth = 0;
};

SkPaint ConvertMyPaintToSkPaint(const MyPaint& data)
{
    SkPaint paint;
    paint.setAntiAlias(data.isAntiAlias);
    paint.setColor(data.color);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, data.blurRadius));
    paint.setStrokeWidth(data.strokeWidth);
    paint.setStyle(data.style);
    return paint;
}

struct FontStyleTestData {
    std::string text;
    TextStyle style;
} datas[] = {
    {
        .text = "无样式",
        .style = {
            .fontSize_ = 32,
        }
    },
    {
        .text = "前景样式：颜色",
        .style = {
            .fontSize_ = 32,
            .foreground_ = ConvertMyPaintToSkPaint({
                .color = 0xFF00FF00,
            }),
        }
    },
    {
        .text = "前景样式：模糊",
        .style = {
            .fontSize_ = 32,
            .foreground_ = ConvertMyPaintToSkPaint({
                .blurRadius = 2.0f,
            }),
        }
    },
    {
        .text = "背景样式：颜色",
        .style = {
            .fontSize_ = 32,
            .background_ = ConvertMyPaintToSkPaint({
                .color = 0x5500FF00,
            }),
        }
    },
    {
        .text = "背景样式：边框",
        .style = {
            .fontSize_ = 32,
            .background_ = ConvertMyPaintToSkPaint({
                .color = 0x5500FF00,
                .style = SkPaint::kStroke_Style,
                .strokeWidth = 2.0f,
            }),
        }
    },
};

class FontStyleTest : public TestFeature {
public:
    FontStyleTest() : TestFeature("FontStyleTest")
    {
    }

    void Layout()
    {
        for (auto& [text, style] : datas) {
            auto builder = TypographyBuilder::Create();
            builder->PushStyle(style);
            builder->AppendSpan(text);
            auto typography = builder->Build();
            typography->Layout(300);
            typographies_.push_back({
                .typography = typography,
            });
        }
    }
} g_test;
} // namespace
