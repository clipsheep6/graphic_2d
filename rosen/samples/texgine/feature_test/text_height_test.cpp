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
#include "feature_test_framework.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
struct TextHeightTestData {
    std::string text;
    TextStyle style;
} g_datas[] = {
    {
        .text = "文本字高默认.",
        .style = {.heightOnly = true,},
    },
    {
        .text = "文本字高test1.",
        .style = {
            .heightOnly = true, .fontSize = 10.0, .heightScale = 1.0,
        },
    },
    {
        .text = "文本字高test2.",
        .style = {
            .heightOnly = true, .fontSize = 10.0, .heightScale = 1.5,
        },
    },
    {
        .text = "文本字高test3.",
        .style = { .heightOnly = true, .fontSize = 10.0, .heightScale = 2.0,
        },
    },
    {
        .text = "文本字高test4.",
        .style = { .heightOnly = true, .fontSize = 15.0, .heightScale = 2.0,
        },
    },
    {
        .text = "文本字高test5.",
        .style = {.heightOnly = true, .fontSize = 20.0, .heightScale = 2.0,
        },
    },
    {
        .text = "文本字高test6.",
        .style = {.heightOnly = true, .fontSize = 20.0, .heightScale = 2.5,
        },
    },
};

class TextHeightTest : public TestFeature {
public:
    TextHeightTest() : TestFeature("TextHeightTest")
    {
    }

    void Layout()
    {
        for (auto &[text, style] : g_datas) {
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
