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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "mock/mock_any_span.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "typography_builder_impl.h"
#include "typography_impl.h"

namespace Texgine {
struct MockVars {
    TextStyle ys_;
    std::vector<VariantSpan> catchedSpans_;
    std::vector<TextStyle> catchedStyles_;
} mockvars;

void InitMockVars(struct MockVars &&vars)
{
    mockvars = std::move(vars);
}

TypographyImpl::TypographyImpl(TypographyStyle &ys,
                               std::vector<VariantSpan> &spans,
                               std::unique_ptr<FontProviders> providers)
{
    mockvars.catchedSpans_ = spans;
}

void VariantSpan::SetTextStyle(const TextStyle &style) noexcept(true)
{
    mockvars.catchedStyles_.push_back(style);
}

TextStyle TypographyStyle::ConvertToTextStyle() const
{
    return mockvars.ys_;
}

class TypographyBuilderImplTest : public testing::Test {
public:
    std::shared_ptr<TypographyBuilderImpl> builder_ =
        std::make_shared<TypographyBuilderImpl>(TypographyStyle{}, FontProviders::Create());
};

TEST_F(TypographyBuilderImplTest, Create)
{
    EXPECT_NE(TypographyBuilder::Create(), nullptr);
}

TEST_F(TypographyBuilderImplTest, PushPopStyle)
{
    TextStyle s1 = {.fontSize_ = 1};
    TextStyle s2 = {.fontSize_ = 2};
    TextStyle s3 = {.fontSize_ = 3};
    auto normalAnySpan = std::make_shared<MockAnySpan>();

    // YS
    InitMockVars({.ys_ = s1});
    TypographyBuilderImpl builder1({}, FontProviders::Create());
    builder1.AppendSpan(normalAnySpan);
    EXPECT_NE(builder1.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedStyles_[0], s1);

    // YS Push
    InitMockVars({.ys_ = s1});
    TypographyBuilderImpl builder2({}, FontProviders::Create());
    builder2.PushStyle(s2);
    builder2.AppendSpan(normalAnySpan);
    EXPECT_NE(builder2.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedStyles_[0], s2);

    // YS Push Push
    InitMockVars({.ys_ = s1});
    TypographyBuilderImpl builder3({}, FontProviders::Create());
    builder3.PushStyle(s2);
    builder3.PushStyle(s3);
    builder3.AppendSpan(normalAnySpan);
    EXPECT_NE(builder3.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedStyles_[0], s3);

    // YS Push Push Pop
    InitMockVars({.ys_ = s1});
    TypographyBuilderImpl builder4({}, FontProviders::Create());
    builder4.PushStyle(s2);
    builder4.PushStyle(s3);
    builder4.PopStyle();
    builder4.AppendSpan(normalAnySpan);
    EXPECT_NE(builder4.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedStyles_[0], s2);
}

TEST_F(TypographyBuilderImplTest, AppendSpan)
{
    TextStyle xs = {};
    std::shared_ptr<AnySpan> nullptrAnySpan = nullptr;
    std::shared_ptr<TextSpan> nullptrTextSpan = nullptr;

    // A(nullptr)
    InitMockVars({});
    TypographyBuilderImpl builder1({}, FontProviders::Create());
    builder1.AppendSpan(nullptrAnySpan);
    EXPECT_NE(builder1.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedSpans_.size(), 0u);

    // T(nullptr)
    InitMockVars({});
    TypographyBuilderImpl builder2({}, FontProviders::Create());
    builder2.AppendSpan(nullptrTextSpan);
    EXPECT_NE(builder2.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedSpans_.size(), 0u);

    // A
    auto as1 = std::make_shared<MockAnySpan>();
    InitMockVars({});
    TypographyBuilderImpl builder3({}, FontProviders::Create());
    builder3.AppendSpan(as1);
    EXPECT_NE(builder3.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedSpans_.size(), 1u);
    EXPECT_EQ(mockvars.catchedSpans_[0], as1);

    // T
    auto ts1 = TextSpan::MakeEmpty();
    InitMockVars({});
    TypographyBuilderImpl builder4({}, FontProviders::Create());
    builder4.AppendSpan(ts1);
    EXPECT_NE(builder4.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedSpans_.size(), 1u);
    EXPECT_EQ(mockvars.catchedSpans_[0], ts1);

    // A A
    auto as2 = std::make_shared<MockAnySpan>();
    auto as3 = std::make_shared<MockAnySpan>();
    InitMockVars({});
    TypographyBuilderImpl builder5({}, FontProviders::Create());
    builder5.AppendSpan(as2);
    builder5.AppendSpan(as3);
    EXPECT_NE(builder5.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedSpans_.size(), 2u);
    EXPECT_EQ(mockvars.catchedSpans_[0], as2);
    EXPECT_EQ(mockvars.catchedSpans_[1], as3);

    // T T
    auto ts2 = TextSpan::MakeEmpty();
    auto ts3 = TextSpan::MakeEmpty();
    InitMockVars({});
    TypographyBuilderImpl builder6({}, FontProviders::Create());
    builder6.AppendSpan(ts2);
    builder6.AppendSpan(ts3);
    EXPECT_NE(builder6.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedSpans_.size(), 1u);
    EXPECT_EQ(mockvars.catchedSpans_[0], ts2);

    // T Style T
    auto ts4 = TextSpan::MakeEmpty();
    auto ts5 = TextSpan::MakeEmpty();
    InitMockVars({});
    TypographyBuilderImpl builder7({}, FontProviders::Create());
    builder7.AppendSpan(ts4);
    builder7.PushStyle(xs);
    builder7.AppendSpan(ts5);
    EXPECT_NE(builder7.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedSpans_.size(), 2u);
    EXPECT_EQ(mockvars.catchedSpans_[0], ts4);
    EXPECT_EQ(mockvars.catchedSpans_[1], ts5);

    // T A T
    auto ts6 = TextSpan::MakeEmpty();
    auto as4 = std::make_shared<MockAnySpan>();
    auto ts7 = TextSpan::MakeEmpty();
    InitMockVars({});
    TypographyBuilderImpl builder8({}, FontProviders::Create());
    builder8.AppendSpan(ts6);
    builder8.AppendSpan(as4);
    builder8.AppendSpan(ts7);
    EXPECT_NE(builder8.Build(), nullptr);
    EXPECT_EQ(mockvars.catchedSpans_.size(), 3u);
    EXPECT_EQ(mockvars.catchedSpans_[0], ts6);
    EXPECT_EQ(mockvars.catchedSpans_[1], as4);
    EXPECT_EQ(mockvars.catchedSpans_[2], ts7);
}
} // namespace Texgine
