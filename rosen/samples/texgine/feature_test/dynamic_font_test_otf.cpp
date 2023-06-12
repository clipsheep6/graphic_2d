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

#include "texgine/dynamic_file_font_provider.h"
#include "texgine/system_font_provider.h"
#include "texgine/typography_builder.h"

#include "feature_test_framework.h"

using namespace Texgine;

namespace {
constexpr auto exampleText = "Hello World otf 1234 كبير! hello 123 آخر الأخبار من محطة سكة حديد شيان هنا!";

struct FontsInfo {
    std::string path = "";
    std::string fontFamily;
    std::string title = "";
} infos[] = {
    {
        .title = "default",
    },
    {
        .path = RESOURCE_PATH_PREFIX "Roboto.otf",
        .fontFamily = "Segoe UI Emoji",
        .title = "Segoe UI Emoji",
    },
    {
        .path = RESOURCE_PATH_PREFIX "SourceHanSansSC-Regular.otf",
        .fontFamily = "SourceHanSansSC-Regular",
        .title = "SourceHanSansSC-Regular",
    },
};

class DynamicFontTest : public TestFeature {
public:
    DynamicFontTest() : TestFeature("DynamicFontTest") {}

    void Layout()
    {
        for (auto& info : infos) {
            TypographyStyle tystyle;
            auto dfprovider = DynamicFileFontProvider::Create();
            dfprovider->LoadFont(info.fontFamily, info.path);
            auto fps = FontProviders::Create();
            fps->AppendFontProvider(dfprovider);
            fps->AppendFontProvider(SystemFontProvider::GetInstance());
            auto builder = TypographyBuilder::Create(tystyle, std::move(fps));

            TextStyle style;
            style.fontFamilies_ = { info.fontFamily };
            builder->PushStyle(style);
            builder->AppendSpan(exampleText);
            builder->PopStyle();

            auto typography = builder->Build();
            typography->Layout(300);
            typographies_.push_back({
                .typography = typography,
                .comment = info.title,
            });
        }
    }
} g_test;
} // namespace
