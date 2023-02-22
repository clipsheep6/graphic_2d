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

#include "font_collection.h"
#include "measurer.h"
#include "mock/mock_any_span.h"
#include "mock/mock_measurer.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "texgine/font_providers.h"
#include "texgine/typography_types.h"
#include "text_shaper.h"
#include "typeface.h"

struct MockVars {
    std::vector<uint16_t> catchedBufferGlyphs_;
    std::vector<float> catchedBufferPos_;
    std::vector<std::string> catchedGenerateFontCollectionFamilies_;

    sk_sp<SkTextBlob> retvalSkTextBlobBuilderMake_ = nullptr;
    std::shared_ptr<Texgine::FontCollection> retvalGenerateFontCollection_ =
        std::make_shared<Texgine::FontCollection>(std::vector<std::shared_ptr<Texgine::FontStyleSet>>{});
    std::unique_ptr<Texgine::MockMeasurer> retvalMeasurerCreate_ = std::make_unique<Texgine::MockMeasurer>();
} mockvars;

void InitMockVars(struct MockVars &&vars)
{
    mockvars = std::move(vars);
}

const SkTextBlobBuilder::RunBuffer& SkTextBlobBuilder::allocRunPos(const SkFont& font, int count, const SkRect* bounds)
{
    static SkTextBlobBuilder::RunBuffer buffer;
    mockvars.catchedBufferGlyphs_.resize(count);
    mockvars.catchedBufferPos_.resize(count * 2);
    buffer.glyphs = mockvars.catchedBufferGlyphs_.data();
    buffer.pos = mockvars.catchedBufferPos_.data();
    return buffer;
}

void SkTextBlob::operator delete(void* p)
{
    delete[](reinterpret_cast<char *>(p));
}

void* SkTextBlob::operator new(size_t size, void* p)
{
    return new char[size];
}

sk_sp<SkTextBlob> SkTextBlob::MakeFromText(const void* text,
    size_t byteLength, const SkFont& font, SkTextEncoding encoding)
{
    return sk_sp<SkTextBlob>(new (nullptr) SkTextBlob({}));
}

SkTextBlob::SkTextBlob(const SkRect& bounds)
    : fBounds(bounds) , fUniqueID(0) , fCacheID(SK_InvalidUniqueID) {}

SkTextBlob::~SkTextBlob() {}

sk_sp<SkTextBlob> SkTextBlobBuilder::make()
{
    return mockvars.retvalSkTextBlobBuilderMake_;
}

namespace Texgine {
std::shared_ptr<FontCollection> FontProviders::GenerateFontCollection(
    const std::vector<std::string> &families) const noexcept(true)
{
    mockvars.catchedGenerateFontCollectionFamilies_ = families;
    return mockvars.retvalGenerateFontCollection_;
}

std::unique_ptr<Measurer> Measurer::Create(const std::vector<uint16_t> &text,
    const FontCollection &fontCollection)
{
    return std::move(mockvars.retvalMeasurerCreate_);
}

struct TextSpanInfo {
    CharGroups cgs_ = CharGroups::CreateEmpty();
};

class ControllerForTest {
public:
    static std::shared_ptr<TextSpan> GenerateTextSpan(TextSpanInfo info)
    {
        auto ts = std::make_shared<TextSpan>();
        ts->cgs_ = info.cgs_;
        return ts;
    }
};

auto GenerateTextSpan(TextSpanInfo info)
{
    return ControllerForTest::GenerateTextSpan(info);
}

class TextShaperTest : public testing::TestWithParam<std::shared_ptr<TextSpan>> {
public:
    static void SetUpTestCase()
    {
        cgs1_.PushBack({ .glyphs_ = { {1, 1, 0, 0.1, 0.1}, {2, 1, 0, 0.2, 0.2} }, .visibleWidth_ = 2 });
        cgs2_.PushBack({ .glyphs_ = { {1, 1, 0, 0.1, 0.1} }, .visibleWidth_ = 1 });
        cgs2_.PushBack({ .glyphs_ = { {2, 1, 0, 0.2, 0.2} }, .visibleWidth_ = 1 });
    }

    static inline CharGroups cgs1_ = CharGroups::CreateEmpty();
    static inline CharGroups cgs2_ = CharGroups::CreateEmpty();
    static inline std::shared_ptr<TypographyStyle> ysNormal_ = std::make_shared<TypographyStyle>();
    static inline std::shared_ptr<TypographyStyle> ysNoProvider_ = std::make_shared<TypographyStyle>();
};

TEST_F(TextShaperTest, DoShape1)
{
    TextShaper shaper;
    auto fp = FontProviders::Create();
    std::shared_ptr<TextSpan> tsNullptr;
    auto tsNormal = TextSpan::MakeFromText("normal");
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, shaper.DoShape(tsNullptr, {}, {}, fp));
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, shaper.DoShape(tsNormal, {}, {}, nullptr));
}

// 逻辑测试
// TextStyle设置为空
TEST_F(TextShaperTest, DoShape2)
{
    InitMockVars({});
    EXPECT_CALL(*mockvars.retvalMeasurerCreate_, Measure).Times(1).WillOnce(testing::Return(0));
    TypographyStyle ys;
    ys.fontFamilies_ = {"Roboto"};
    auto span = GenerateTextSpan({.cgs_ = cgs1_});

    EXPECT_NO_THROW({
        TextShaper shaper;
        shaper.DoShape(span, {}, ys, FontProviders::Create());
        ASSERT_EQ(ys.fontFamilies_, mockvars.catchedGenerateFontCollectionFamilies_);
    });
}

// 逻辑测试
// TextStyle设置不为空
TEST_F(TextShaperTest, DoShape3)
{
    InitMockVars({});
    EXPECT_CALL(*mockvars.retvalMeasurerCreate_, Measure).Times(1).WillOnce(testing::Return(0));
    TextStyle style = {.fontFamilies_ = {"Sans"}};
    auto span = GenerateTextSpan({.cgs_ = cgs1_});

    EXPECT_NO_THROW({
        TextShaper shaper;
        shaper.DoShape(span, style, {}, FontProviders::Create());
        ASSERT_EQ(style.fontFamilies_ , mockvars.catchedGenerateFontCollectionFamilies_);
    });
}

// 逻辑测试
TEST_F(TextShaperTest, DoShape4)
{
    InitMockVars({.retvalGenerateFontCollection_ = nullptr});
    auto span = GenerateTextSpan({});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.DoShape(span, {}, {}, FontProviders::Create());
        ASSERT_EQ(ret, 1);
    });
}

// 逻辑测试
TEST_F(TextShaperTest, DoShape5)
{
    InitMockVars({});
    EXPECT_CALL(*mockvars.retvalMeasurerCreate_, Measure).Times(1).WillOnce(testing::Return(1));
    auto span = GenerateTextSpan({.cgs_ = cgs1_});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.DoShape(span, {}, {}, FontProviders::Create());
        ASSERT_EQ(ret, 1);
    });
}

// 逻辑测试
TEST_F(TextShaperTest, DoShape6)
{
    InitMockVars({});
    EXPECT_CALL(*mockvars.retvalMeasurerCreate_, Measure).Times(1).WillOnce(testing::Return(0));
    auto span = GenerateTextSpan({.cgs_ = cgs1_});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.DoShape(span, {}, {}, FontProviders::Create());
        ASSERT_EQ(ret, 0);
    });
}

TEST_F(TextShaperTest, GenerateTextBlob1)
{
    double spanWidth = 0.0;
    std::vector<double> glyphWidths;
    TextShaper shaper;
    ASSERT_EXCEPTION(ExceptionType::APIFailed, shaper.GenerateTextBlob({},
        CharGroups::CreateEmpty(), spanWidth, glyphWidths));
}

TEST_F(TextShaperTest, GenerateTextBlob2)
{
    EXPECT_NO_THROW({
        double spanWidth = 0.0;
        std::vector<double> glyphWidths;
        TextShaper shaper;
        shaper.GenerateTextBlob({}, cgs1_, spanWidth, glyphWidths);
        ASSERT_EQ(spanWidth, 2);
        ASSERT_EQ(glyphWidths, (std::vector<double>{2.0}));
        ASSERT_EQ(mockvars.catchedBufferPos_, (std::vector<float>{0.1, -0.1, 1.2, -0.2}));
        ASSERT_EQ(mockvars.catchedBufferGlyphs_, (std::vector<uint16_t>{1, 2}));
    });
}

TEST_F(TextShaperTest, GenerateTextBlob3)
{
    EXPECT_NO_THROW({
        double spanWidth = 0.0;
        std::vector<double> glyphWidths;
        TextShaper shaper;
        shaper.GenerateTextBlob({}, cgs2_, spanWidth, glyphWidths);
        ASSERT_EQ(spanWidth, 2);
        ASSERT_EQ(glyphWidths, (std::vector<double>{1.0, 1.0}));
        ASSERT_EQ(mockvars.catchedBufferPos_, (std::vector<float>{0.1, -0.1, 1.2, -0.2}));
        ASSERT_EQ(mockvars.catchedBufferGlyphs_, (std::vector<uint16_t>{1, 2}));
    });
}

// 异常测试，设置参数中的span为nullptr
TEST_F(TextShaperTest, Shape1)
{
    std::shared_ptr<TextSpan> tsNullptr = nullptr;
    std::shared_ptr<AnySpan> asNullptr = nullptr;
    TextShaper shaper;
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, shaper.Shape(tsNullptr, {}, FontProviders::Create()));
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, shaper.Shape(asNullptr, {}, FontProviders::Create()));
}

// 逻辑测试 AnySpan
TEST_F(TextShaperTest, Shape2)
{
    InitMockVars({.retvalGenerateFontCollection_ = nullptr});

    EXPECT_NO_THROW({
        std::shared_ptr<MockAnySpan> mas = std::make_shared<MockAnySpan>();
        TextShaper shaper;
        auto ret = shaper.Shape(mas, {}, FontProviders::Create());
        ASSERT_EQ(ret, 0);
    });
}

// 逻辑测试 控制GenerateFontCollection返回值为nullptr从而使DoShape返回值为1
TEST_F(TextShaperTest, Shape3)
{
    InitMockVars({.retvalGenerateFontCollection_ = nullptr});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.Shape(GenerateTextSpan({}), {}, FontProviders::Create());
        ASSERT_EQ(ret, 1);
    });
}

// span的cgs_的异常测试
TEST_F(TextShaperTest, Shape4)
{
    InitMockVars({});
    TextShaper shaper;
    ASSERT_EXCEPTION(ExceptionType::InvalidCharGroups,
        shaper.Shape(GenerateTextSpan({.cgs_ = {}}), {}, FontProviders::Create()));

    InitMockVars({});
    EXPECT_CALL(*mockvars.retvalMeasurerCreate_, Measure).Times(1).WillOnce(testing::Return(0));
    cgs1_.Get(0).typeface_ = nullptr;

    ASSERT_EXCEPTION(ExceptionType::InvalidArgument,
        shaper.Shape(GenerateTextSpan({.cgs_ = cgs1_}), {}, FontProviders::Create()));
}

TEST_F(TextShaperTest, Shape5)
{
    InitMockVars({});
    EXPECT_CALL(*mockvars.retvalMeasurerCreate_, Measure).Times(1).WillOnce(testing::Return(0));
    cgs1_.Get(0).typeface_ = std::make_unique<Typeface>(nullptr);

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.Shape(GenerateTextSpan({.cgs_ = cgs1_}), {}, FontProviders::Create());
        ASSERT_EQ(ret, 1);
    });
}

TEST_F(TextShaperTest, Shape6)
{
    InitMockVars({});
    EXPECT_CALL(*mockvars.retvalMeasurerCreate_, Measure).Times(1).WillOnce(testing::Return(0));
    mockvars.retvalSkTextBlobBuilderMake_ = SkTextBlob::MakeFromText("hello", 5, {});

    EXPECT_NO_THROW({
        TextShaper shaper;
        auto ret = shaper.Shape(GenerateTextSpan({.cgs_ = cgs1_}), {}, FontProviders::Create());
        ASSERT_EQ(ret, 0);
    });
}
} // namespace Texgine
