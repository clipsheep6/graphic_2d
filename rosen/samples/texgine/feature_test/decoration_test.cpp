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
struct StyledText {
    std::string text;
    TextStyle style;
} texts[] = {
    {
        .text = "Decoration对照",
        .style = {},
    },
    {
        .text = "Decoration上划线",
        .style = {
            .decoration_ = TextDecoration::Overline,
        },
    },
    {
        .text = "Decoration删除线",
        .style = {
            .decoration_ = TextDecoration::Linethrough,
        },
    },
    {
        .text = "Decoration下划线",
        .style = {
            .decoration_ = TextDecoration::Underline,
        },
    },
    {
        .text = "Decoration两两混合1",
        .style = {
            .decoration_ = TextDecoration::Overline |
                TextDecoration::Linethrough,
        },
    },
    {
        .text = "Decoration两两混合2",
        .style = {
            .decoration_ = TextDecoration::Overline |
                TextDecoration::Underline,
        },
    },
    {
        .text = "Decoration两两混合3",
        .style = {
            .decoration_ = TextDecoration::Linethrough |
                TextDecoration::Underline,
        },
    },
    {
        .text = "Decoration全混合",
        .style = {
            .decoration_ = TextDecoration::Overline |
                TextDecoration::Linethrough |
                TextDecoration::Underline,
        },
    },
    {
        .text = "Decoration实下滑线",
        .style = {
            .decoration_ = TextDecoration::Underline,
            .decorationStyle_ = TextDecorationStyle::Solid,
        },
    },
    {
        .text = "Decoration虚下滑线",
        .style = {
            .decoration_ = TextDecoration::Underline,
            .decorationStyle_ = TextDecorationStyle::Dashed,
        },
    },
    {
        .text = "Decoration点下滑线",
        .style = {
            .decoration_ = TextDecoration::Underline,
            .decorationStyle_ = TextDecorationStyle::Dotted,
        },
    },
    {
        .text = "Decoration波浪下滑线",
        .style = {
            .decoration_ = TextDecoration::Underline,
            .decorationStyle_ = TextDecorationStyle::Wavy,
        },
    },
    {
        .text = "Decoration双实下滑线",
        .style = {
            .decoration_ = TextDecoration::Underline,
            .decorationStyle_ = TextDecorationStyle::Double,
        },
    },
    {
        .text = "Decoration粗下滑线",
        .style = {
            .decoration_ = TextDecoration::Underline,
            .decorationThicknessScale_ = 5.0f,
        },
    },
    {
        .text = "Decoration红下划线",
        .style = {
            .decoration_ = TextDecoration::Underline,
            .decorationColor_ = 0xffff0000,
        },
    },
};

class DecorationTest : public TestFeature {
public:
    DecorationTest() : TestFeature("DecorationTest")
    {
    }

    void Layout()
    {
        option_.needBorder = false;

        TypographyStyle tstyle;
        for (auto &[text, style] : texts) {
            auto builder = TypographyBuilder::Create(tstyle);
            builder->PushStyle(style);
            builder->AppendSpan(text);
            auto typography = builder->Build();
            typography->Layout(200);
            typographies_.push_back({
                .typography = typography,
            });
        }
    }
} g_test;
} // namespace
