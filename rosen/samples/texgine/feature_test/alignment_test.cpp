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

#include <sstream>

#include <include/core/SkCanvas.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"
#include "my_any_span.h"

using namespace Texgine;

namespace {
constexpr const char *text = "Hi 世界";

struct AlignmentTestData {
    AnySpanAlignment alignment_;
    double offset_ = 0;
    TextBaseline baseline_ = TextBaseline::Alphabetic;
} testDatas[] = {
    { .alignment_ = AnySpanAlignment::AboveBaseline },
    { .alignment_ = AnySpanAlignment::AboveBaseline, .baseline_ = TextBaseline::Ideographic },
    { .alignment_ = AnySpanAlignment::OffsetAtBaseline },
    { .alignment_ = AnySpanAlignment::OffsetAtBaseline, .offset_ = 40 },
    { .alignment_ = AnySpanAlignment::BelowBaseline },
    { .alignment_ = AnySpanAlignment::TopOfRowBox },
    { .alignment_ = AnySpanAlignment::CenterOfRowBox },
    { .alignment_ = AnySpanAlignment::BottomOfRowBox },
};

AnySpanAlignment onelineAlignments[] = {
    AnySpanAlignment::TopOfRowBox,
    AnySpanAlignment::TopOfRowBox,
    AnySpanAlignment::CenterOfRowBox,
    AnySpanAlignment::BottomOfRowBox,
    AnySpanAlignment::OffsetAtBaseline,
    AnySpanAlignment::AboveBaseline,
    AnySpanAlignment::BelowBaseline,
};

std::map<AnySpanAlignment, std::string> alignmentToString = {
    {AnySpanAlignment::AboveBaseline, "AB"},
    {AnySpanAlignment::BelowBaseline, "BB"},
    {AnySpanAlignment::OffsetAtBaseline, "BL"},
    {AnySpanAlignment::TopOfRowBox, "TO"},
    {AnySpanAlignment::BottomOfRowBox, "BO"},
    {AnySpanAlignment::CenterOfRowBox, "MI"},
};

std::map<TextBaseline, std::string> baselineToString = {
    {TextBaseline::Alphabetic, "Alpha"},
    {TextBaseline::Ideographic, "Ideog"},
};

class AlignmentTest : public TestFeature {
public:
    AlignmentTest() : TestFeature("AlignmentTest")
    {
    }

    void Layout()
    {
        option_.needRainbowChar = true;

        TextStyle textStyle = {
            .fontSize_ = 20,
            .decoration_ = TextDecoration::Baseline,
        };

        for (const auto &data : testDatas) {
            auto heights = {20, 40, 60};
            for (const auto &height : heights) {
                auto builder = TypographyBuilder::Create();
                builder->PushStyle(textStyle);
                builder->AppendSpan(text);

                auto anySpan = std::make_shared<MyAnySpan>(20,
                    height, data.alignment_, data.baseline_, data.offset_);
                builder->AppendSpan(anySpan);

                auto typography = builder->Build();
                typography->Layout(150);

                std::stringstream ss;
                ss << alignmentToString[data.alignment_];
                if (data.alignment_ == AnySpanAlignment::OffsetAtBaseline) {
                    ss << "(" << data.offset_ << ")";
                }
                ss << " " << baselineToString[data.baseline_];
                ss << " " << height << "px";

                typographies_.push_back({
                    .typography = typography,
                    .comment = ss.str(),
                    .atNewline = (height == *heights.begin()),
                });
            }
        }

        auto builder = TypographyBuilder::Create();
        std::string title = "单行多个不同对齐方式";
        for (const auto &alignment : onelineAlignments) {
            auto anySpan = std::make_shared<MyAnySpan>(20, 30, alignment);
            builder->AppendSpan(anySpan);
            builder->PushStyle(textStyle);
            builder->AppendSpan(text);
            title += alignmentToString[alignment] + " ";
        }

        auto typography = builder->Build();
        typography->Layout(800);
        typographies_.push_back({
            .typography = typography,
            .comment = title,
        });
    }
} g_test;
} // namespace
