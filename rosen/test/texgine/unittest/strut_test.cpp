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

#include "text_span.h"
#include "texgine/typography_builder.h"

using namespace testing;

namespace Texgine {
namespace {
class StrutTest : public testing::Test {
};

TEST_F(StrutTest, Height)
{
    auto builder = TypographyBuilder::Create({});
    builder->PushStyle({});
    builder->AppendSpan("hello");
    auto ty = builder->Build();
    ty->Layout(250);

    EXPECT_NEAR(ty->GetHeight(), 19.0, 0.5);
    EXPECT_NEAR(ty->GetAlphabeticBaseline(), 14.8, 0.5);
    EXPECT_NEAR(ty->GetIdeographicBaseline(), 18.7, 0.5);
}

TEST_F(StrutTest, Width)
{
    {
        auto builder = TypographyBuilder::Create({});
        builder->PushStyle({});
        builder->AppendSpan("hello world hello world hello");
        auto ty = builder->Build();
        ty->Layout(250);

        EXPECT_NEAR(ty->GetActualWidth(), 205.0, 0.5);
    }

    {
        auto builder = TypographyBuilder::Create({});
        builder->PushStyle({});
        builder->AppendSpan("hello world hello world hello");
        builder->AppendSpan("world hello world hello");
        auto ty = builder->Build();
        ty->Layout(250);

        EXPECT_NEAR(ty->GetActualWidth(), 250.2, 0.5);
        EXPECT_EQ(ty->GetWidthLimit(), 250);
    }
}
} // namespace
} // namespace Texgine
