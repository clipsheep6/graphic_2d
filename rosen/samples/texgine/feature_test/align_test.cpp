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

#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"

using namespace Texgine;

namespace {
constexpr auto exampleText = "    Landscape engineering quality management, the most likely "
                             "problem is not clear responsible person, responsible unit, resulting "
                             "in problems again and again, therefore, should establish a strict quality "
                             "management responsibility system and norms, through the system to ensure the quality "
                             "of     Landscape engineering construction.";
struct AlignTestData {
    std::string title;
    TypographyStyle tStyle;
} datas[] = {
    {
        .title = "Start对齐布局",
        .tStyle = {
            .align_ = TextAlign::Start,
        },
    },
    {
        .title = "左对齐布局",
        .tStyle = {
            .align_ = TextAlign::Left,
        },
    },
    {
        .title = "右对齐布局",
        .tStyle = {
            .align_ = TextAlign::Right,
        },
    },
    {
        .title = "居中对齐布局",
        .tStyle = {
            .align_ = TextAlign::Center,
        },
    },
    {
        .title = "End对齐布局",
        .tStyle = {
            .align_ = TextAlign::End,
        },
    },
};

class AlignTest : public TestFeature {
public:
    AlignTest() : TestFeature("AlignTest")
    {
    }

    void Layout()
    {
        SkPaint background;
        background.setColor(0xff00ff00);
        background.setStrokeWidth(2.0f);
        background.setStyle(SkPaint::kStroke_Style);
        TextStyle style = {
            .fontSize_ = 16,
            .background_ = background,
        };

        for (auto &[title, tstyle] : datas) {
            for (auto dir : {TextDirection::LTR, TextDirection::RTL}) {
                tstyle.direction_ = dir;
                auto builder = TypographyBuilder::Create(tstyle);
                builder->PushStyle(style);
                builder->AppendSpan(exampleText);
                auto typography = builder->Build();
                typography->Layout(325);
                typographies_.push_back({
                    .typography = typography,
                    .comment = (dir == TextDirection::LTR ? "LTR " : "RTL ") + title,
                });
            }
        }
    }
} g_test;
} // namespace
