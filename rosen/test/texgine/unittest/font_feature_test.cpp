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

#include <gtest/gtest.h>

#include "texgine/dynamic_file_font_provider.h"
#include "text_span.h"
#include "texgine/typography_builder.h"

using namespace testing;

namespace Texgine {
namespace {
class FontFeatureTest : public testing::Test {
};

TEST_F(FontFeatureTest, FontFeature)
{
    auto dfprovider = DynamicFileFontProvider::Create();
    dfprovider->LoadFont("Roboto", "out/Roboto-Black.ttf");

    TextStyle style1;
    style1.fontSize_ = 64;
    style1.fontFamilies_ = {"Roboto"};
    style1.fontFeature_.SetFeature("pnum", 1);
    TypographyStyle tystyle;
    auto fps = FontProviders::Create();
    fps->AppendFontProvider(dfprovider);
    auto builder1 = TypographyBuilder::Create(tystyle, std::move(fps));
    builder1->PushStyle(style1);
    builder1->AppendSpan("Hello 1234");
    builder1->PopStyle();
    auto typography1 = builder1->Build();
    typography1->Layout(500);

    TextStyle style2;
    style2.fontSize_ = 64;
    style2.fontFamilies_ = {"Roboto"};
    style2.fontFeature_.SetFeature("tnum", 1);
    fps = FontProviders::Create();
    fps->AppendFontProvider(dfprovider);
    auto builder2 = TypographyBuilder::Create(tystyle, std::move(fps));
    builder2->PushStyle(style2);
    builder2->AppendSpan("Hello 1234");
    builder2->PopStyle();
    auto typography2 = builder2->Build();
    typography2->Layout(500);

    EXPECT_NEAR(typography1->GetActualWidth(), 310.28125, 0.001);
    EXPECT_NEAR(typography2->GetActualWidth(), 315.125, 0.001);
}
} // namespace
} // namespace Texgine
