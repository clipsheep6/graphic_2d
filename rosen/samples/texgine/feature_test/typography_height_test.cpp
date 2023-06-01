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

#include <texgine/typography_builder.h>
#include "texgine/typography_types.h"
#include "feature_test_framework.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
struct TypographyHeightTestData {
    std::string text;
    TypographyStyle typographystyle;
} g_datas[] = {
    {
        .text = "段落高默认.",
        .typographystyle.useLineStyle = true,
        .typographystyle.lineStyle.heightOnly = true,
    },
    {
        .text = "段落高test1.",
        .typographystyle.useLineStyle = true,
        .typographystyle.lineStyle.heightOnly = true,
        .typographystyle.lineStyle.fontSize = 10.0,
        .typographystyle.lineStyle.heightScale = 1.5,
    },
    {
        .text = "段落高test2.",
        .typographystyle.useLineStyle = true,
        .typographystyle.lineStyle.heightOnly = true,
        .typographystyle.lineStyle.fontSize = 15.0,
        .typographystyle.lineStyle.heightScale = 1.5,
    },
    {
        .text = "段落高test3.",
        .typographystyle.useLineStyle = true,
        .typographystyle.lineStyle.heightOnly = true,
        .typographystyle.lineStyle.fontSize = 15.0,
        .typographystyle.lineStyle.heightScale = 2.0,
    },
    {
        .text = "段落高test4.",
        .typographystyle.useLineStyle = true,
        .typographystyle.lineStyle.heightOnly = true,
        .typographystyle.lineStyle.fontSize = 20.0,
        .typographystyle.lineStyle.heightScale = 2.0,
    },
    {
        .text = "段落高test5.",
        .typographystyle.useLineStyle = true,
        .typographystyle.lineStyle.heightOnly = true,
        .typographystyle.lineStyle.fontSize = 25.0,
        .typographystyle.lineStyle.heightScale = 2.0,
    },
    {
        .text = "段落高test6.",
        .typographystyle.useLineStyle = true,
        .typographystyle.lineStyle.heightOnly = true,
        .typographystyle.lineStyle.fontSize = 30.0,
        .typographystyle.lineStyle.heightScale = 3.0,
    },
};

class TypographyHeightTest : public TestFeature {
public:
    TypographyHeightTest() : TestFeature("TypographyHeightTest")
    {
    }

    void Layout()
    {
        for (auto &[text, typographystyle] : g_datas) {
             auto builder = TypographyBuilder::Create(typographystyle);
             TextStyle tstyle;
             builder->PushStyle(tstyle);
             builder->AppendSpan(text);
             builder->PopStyle();
             auto typography = builder->Build();
             typography->Layout(300);
             typographies_.push_back({
                 .typography = typography,
            });
        }
    }
} g_test;
} // namespace
