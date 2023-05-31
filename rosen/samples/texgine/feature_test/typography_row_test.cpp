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

#include "feature_test_framework.h"
#include "texgine/system_font_provider.h"
#include "texgine/typography_builder.h"

using namespace Texgine;

namespace {

constexpr auto exampleText = "Typography is the open and technique of arranging type to make written "
                             "language legible, readable and appealing when displayed. The "
                             "arrangement of type involves selecting typefaces, point sizes, "
                             "line lengths, line-spacing (leading), and letter-spacing "
                             "(tracking), as well as adjusting the space between pairs of "
                             "letters (kerning).";

struct LayoutTestData {
    std::string text;
    TypographyStyle typographystyle;
} datas[] = {
    {
        .text = "GREEDY",
        .typographystyle = { .breakStrategy_ = BreakStrategy::GREEDY },
    },
    {
        .text = "HIGHQUALITY",
	.typographystyle = { .breakStrategy_ = BreakStrategy::HIGHQUALITY },
    },
    {
        .text = "BALANCED",
	.typographystyle = { .breakStrategy_ = BreakStrategy::BALANCED },
    },
};

class LayoutTest : public TestFeature {
public:
    LayoutTest() : TestFeature("LayoutTest")
    {
    }

    void Layout()
    {
	TextStyle style = {
            .fontSize_ = 16,
        };

        for (auto &[text, typographystyle] : datas) {
             option_.needRainbowChar = true;
             auto builder = TypographyBuilder::Create(typographystyle);
             builder->PushStyle(style);
             builder->AppendSpan(exampleText);
             builder->PopStyle();

	     auto typography = builder->Build();
             typography->Layout(300);
             typographies_.push_back({
                 .typography = typography,
		 .comment = text,
             });
        }
    }
} g_test;
} // namespace
