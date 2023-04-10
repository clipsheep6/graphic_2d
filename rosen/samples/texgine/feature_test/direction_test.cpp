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

#include <texgine/dynamic_file_font_provider.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"
#include "text_define.h"

using namespace Texgine;

namespace {
constexpr auto exampleText = "hello" Watch Person " wolrd تحول " WomanSportVariant
                             " كبير! hello 123 آخر الأخبار من محطة سكة حديد شيان هنا!" Watch;

class DirectionTest : public TestFeature {
public:
    DirectionTest() : TestFeature("DirectionTest")
    {
    }

    void Layout()
    {
        for (auto &dir : {TextDirection::LTR, TextDirection::RTL}) {
            TypographyStyle tystyle = {
                .direction_ = dir,
            };

            auto dfprovider = DynamicFileFontProvider::Create();
            dfprovider->LoadFont("Segoe UI Emoji", RESOURCE_PATH_PREFIX "seguiemj.ttf");
            auto fps = FontProviders::Create();
            fps->AppendFontProvider(dfprovider);
            fps->AppendFontProvider(SystemFontProvider::GetInstance());
            auto builder = TypographyBuilder::Create(tystyle, std::move(fps));

            TextStyle style = {
                .fontFamilies_ = {"Segoe UI Emoji"},
                .fontSize_ = 24,
            };

            builder->PushStyle(style);
            builder->AppendSpan(exampleText);
            builder->PopStyle();

            std::stringstream ss;
            ss << "typography direction: " << (dir == TextDirection::LTR ? "LTR" : "RTL");

            auto typography = builder->Build();
            typography->Layout(600);
            typographies_.push_back({
                .typography = typography,
                .comment = ss.str(),
            });
        }
    }
} g_test;
} // namespace
