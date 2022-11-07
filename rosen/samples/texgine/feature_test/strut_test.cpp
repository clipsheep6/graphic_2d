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
struct Rect {
    TextRectHeightStyle heightStyle;
    TextRectWidthStyle widthStyle;
    std::string title = "";
} rectinfos[] = {
    {
        .heightStyle = TextRectHeightStyle::Tight,
        .widthStyle = TextRectWidthStyle::Tight,
        .title = "height:Tight, width:Tight",
    },
    {
        .heightStyle = TextRectHeightStyle::Tight,
        .widthStyle = TextRectWidthStyle::MaxWidth,
        .title = "height:Tight, width:Max",
    },
    {
        .heightStyle = TextRectHeightStyle::CoverTopAndBottom,
        .widthStyle = TextRectWidthStyle::Tight,
        .title = "height:Max, width:Tight",
    },
    {
        .heightStyle = TextRectHeightStyle::CoverHalfTopAndBottom,
        .widthStyle = TextRectWidthStyle::Tight,
        .title = "height:IncludeSpacingMiddle, width:Tight",
    },
    {
        .heightStyle = TextRectHeightStyle::CoverTop,
        .widthStyle = TextRectWidthStyle::Tight,
        .title = "height:IncludeSpacingTop, width:Tight",
    },
    {
        .heightStyle = TextRectHeightStyle::CoverBottom,
        .widthStyle = TextRectWidthStyle::Tight,
        .title = "height:IncludeSpacingBottom, width:Tight",
    },
    {
        .heightStyle = TextRectHeightStyle::FollowByLineStyle,
        .widthStyle = TextRectWidthStyle::Tight,
        .title = "height:StructForce, width:Tight",
    },
};

class StrutTest : public TestFeature {
public:
    StrutTest() : TestFeature("StrutTest")
    {
    }

    void Layout()
    {
        option_.needRainbowChar = true;

        TextStyle style = {
            .fontSize_ = 64,
        };

        TypographyStyle tpStyle = {
            .lineStyle_ = {
                .only_ = true,
                .fontSize_ = 64,
            },
        };

        for (auto &[heightStyle, widthStyle, title] : rectinfos) {
            auto builder = TypographyBuilder::Create(tpStyle);
            builder->PushStyle(style);
            builder->AppendSpan("jhello hello hello JHELLO");
            auto typography = builder->Build();
            typography->Layout(300);
            typographies_.push_back({
                .typography = typography,
                .comment = title,
                .ws = widthStyle,
                .hs = heightStyle,
            });
        }
    }
};
} // namespace
