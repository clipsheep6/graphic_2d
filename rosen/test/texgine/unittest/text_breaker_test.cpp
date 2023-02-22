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

#include <variant>

#include <gtest/gtest.h>

#include "font_collection.h"
#include "font_styles.h"
#include "mock/mock_any_span.h"
#include "mock/mock_measurer.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "text_breaker.h"
#include "text_converter.h"

namespace Texgine {
std::vector<std::string> families_;
std::shared_ptr<FontCollection> FontProviders::GenerateFontCollection(
    const std::vector<std::string> &families) const noexcept(true)
{
    families_ = families;
    std::vector<std::shared_ptr<FontStyleSet>> sets;
    return std::make_shared<FontCollection>(std::move(sets));
}

std::unique_ptr<Measurer> measurer = nullptr;
std::unique_ptr<Measurer> Measurer::Create(const std::vector<uint16_t> &text, const FontCollection &fontCollection)
{
    return std::move(measurer);
}

std::vector<Boundary> boundaries;

class MeasurerForTest : public Measurer {
public:
    virtual ~MeasurerForTest() = default;
    MeasurerForTest() : Measurer({}, {{}}) {}
    void SetMeasurerArgs(int ret, const CharGroups &cgs)
    {
        ret_ = ret;
        cgs_ = cgs;
    }

    int Measure(CharGroups &cgs) override
    {
        cgs = cgs_;
        return ret_;
    }

    const std::vector<Boundary> &GetWordBoundary() const override
    {
        return boundaries;
    }

private:
    CharGroups cgs_;
    int ret_ = 0;
};

class TextBreakerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        tpstyle_.fontFamilies_ = {"seguiemj"};
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("m"), .glyphs_ = {{0x013B, 13.664}}, .visibleWidth_ = 14,});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("o"), .glyphs_ = {{0x0145, 9.456}}, .visibleWidth_ = 10,});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("s"), .glyphs_ = {{0x0166, 7.28}}, .visibleWidth_ = 8,});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("t"), .glyphs_ = {{0x016E, 5.88}}, .visibleWidth_ = 6,});

        cgs2_ = cgs1_;
        cgs2_.PushBack({.chars_ = TextConverter::ToUTF16(" "), .glyphs_ = {{0x0002, 4.32}}, .invisibleWidth_ = 5,});
    }

    void PrepareWordBreak(int ret, CharGroups &cgs, WordBreakType type = WordBreakType::BreakWord)
    {
        tpstyle_.wordBreakType_ = type;
        auto m = std::make_unique<MeasurerForTest>();
        m->SetMeasurerArgs(ret, cgs);
        measurer = std::move(m);
    }

    static inline TypographyStyle tpstyle_;
    static inline TextStyle textStyle_;
    std::shared_ptr<TextSpan> tsNullptr_ = nullptr;
    std::shared_ptr<TextSpan> tsNormal_ = TextSpan::MakeFromText("most");
    CharGroups emptyCgs_ = CharGroups::CreateEmpty();
    static inline CharGroups cgs1_ = CharGroups::CreateEmpty();
    static inline CharGroups cgs2_ = CharGroups::CreateEmpty();
    FontCollection fontCollection_ = std::vector<std::shared_ptr<FontStyleSet>>{};
    std::vector<VariantSpan> spans_;
    TextBreaker breaker;
    std::vector<Boundary> boundaries_;
};

// 逻辑测试
// 设置textStyle的fontFamilies_为空
// 判定返回值不为空指针，并且最后传出的families为tpstyle的families
TEST_F(TextBreakerTest, GenerateFontCollection1)
{
    textStyle_.fontFamilies_ = {};

    EXPECT_NO_THROW({
        auto ret = breaker.GenerateFontCollection(tpstyle_, textStyle_, FontProviders::Create());
        ASSERT_NE(ret, nullptr);
        ASSERT_EQ(tpstyle_.fontFamilies_, families_);
    });
}


// 异常测试
// 设置providers_为空
// 判定返回值为空指针
TEST_F(TextBreakerTest, GenerateFontCollection2)
{
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, breaker.GenerateFontCollection(tpstyle_, textStyle_, nullptr));
}

// 过程测试
// 设置textStyle的fontFamilies_不为空，并且providers_不为空，typoStyle不为空
// 判定返回值不为空指针，并且传出的families==textStyle的fontFamilies_
TEST_F(TextBreakerTest, GenerateFontCollection3)
{
    tpstyle_.fontFamilies_ = {"seguiemj"};
    textStyle_.fontFamilies_ = {"robot"};

    EXPECT_NO_THROW({
        auto ret = breaker.GenerateFontCollection(tpstyle_, textStyle_, FontProviders::Create());
        ASSERT_NE(ret, nullptr);
        ASSERT_EQ(textStyle_.fontFamilies_, families_);
    });
}

// 逻辑测试
// 让Measurer的Measure失败
// 判定返回值不为0
TEST_F(TextBreakerTest, Measure1)
{
    boundaries = {{ 0, 5 }};
    PrepareWordBreak(1, emptyCgs_);

    EXPECT_NO_THROW({
        auto ret = breaker.Measure({}, tsNormal_->u16vect_, fontCollection_, emptyCgs_, boundaries_);
        ASSERT_NE(ret, 0);
    });
}

// 逻辑测试
// 让Measurer的GetWordBoundary返回值为空，measure成功
// 判定返回值为1
TEST_F(TextBreakerTest, Measure2)
{
    boundaries = {};
    PrepareWordBreak(0, emptyCgs_);

    EXPECT_NO_THROW({
        auto ret = breaker.Measure({}, tsNormal_->u16vect_, fontCollection_, emptyCgs_, boundaries_);
        ASSERT_EQ(ret, 1);
    });
}

// 逻辑测试
// 让Measurer的GetWordBoundary返回值不为空，measure成功
// 判定返回值为0
TEST_F(TextBreakerTest, Measure3)
{
    boundaries = {{ 0, 5 }};
    PrepareWordBreak(0, emptyCgs_);

    EXPECT_NO_THROW({
        auto ret = breaker.Measure({}, tsNormal_->u16vect_, fontCollection_, emptyCgs_, boundaries_);
        ASSERT_EQ(ret, 0);
    });
}

// 过程测试
// wordcgs为空，未进入循环
// 判定返回值为0
TEST_F(TextBreakerTest, BreakWord1)
{
    EXPECT_NO_THROW({
        breaker.BreakWord(emptyCgs_, tpstyle_, textStyle_, spans_);
        ASSERT_EQ(spans_.size(), 0);
    });
}


// 逻辑测试
// 设置wordcgs中的内容为“most”不包含空格，选择分词策略为WordBreakType::breakWord
// 判定返回值为0，spans_的大小为1，并且这1个span的preBreak==postBreak
TEST_F(TextBreakerTest, BreakWord2)
{
    EXPECT_NO_THROW({
        breaker.BreakWord(cgs1_, tpstyle_, textStyle_, spans_);
        ASSERT_EQ(spans_.size(), 1);
        auto span = spans_[0].TryToTextSpan();
        ASSERT_EQ(span->GetPreBreak(), span->GetPostBreak());
    });
}

// 逻辑测试
// 设置wordcgs中的内容为“most ”包含空格，选择分词策略为WordBreakType::breakWord
// 判定返回值为0，spans_的大小为1，并且这1个span的preBreak！=postBreak
TEST_F(TextBreakerTest, BreakWord3)
{
    EXPECT_NO_THROW({
        breaker.BreakWord(cgs2_, tpstyle_, textStyle_, spans_);
        ASSERT_EQ(spans_.size(), 1);
        auto span = spans_[0].TryToTextSpan();
        ASSERT_NE(span->GetPreBreak(), span->GetPostBreak());
    });
}

// 逻辑测试
// 设置wordcgs中的内容为“most”不包含空格，选择分词策略为WordBreakType::breakAll
// 判定返回值为0，spans_的大小为4，并且每一个span的preBreak==postBreak
TEST_F(TextBreakerTest, BreakWord4)
{
    tpstyle_.wordBreakType_ = WordBreakType::BreakAll;

    EXPECT_NO_THROW({
        breaker.BreakWord(cgs1_, tpstyle_, textStyle_, spans_);
        ASSERT_EQ(spans_.size(), 4);
        for (int i = 0; i < 4; i++) {
            auto span = spans_[i].TryToTextSpan();
            ASSERT_EQ(span->GetPreBreak(), span->GetPostBreak());
        }
    });
}

// 逻辑测试
// 设置wordcgs中的内容为“most ”包含空格，选择分词策略为WordBreakType::breakAll
// 判定返回值为0，spans_的大小为5，并且前4个span的preBreak==postBreak，最后一个span的preBreak！=postBreak
TEST_F(TextBreakerTest, BreakWord5)
{
    tpstyle_.wordBreakType_ = WordBreakType::BreakAll;

    EXPECT_NO_THROW({
        breaker.BreakWord(cgs2_, tpstyle_, textStyle_, spans_);
        ASSERT_EQ(spans_.size(), 5);
        for (int i = 0; i < 4; i++) {
            auto span = spans_[i].TryToTextSpan();
            ASSERT_EQ(span->GetPreBreak(), span->GetPostBreak());
        }
        auto span = spans_[4].TryToTextSpan();
        ASSERT_NE(span->GetPreBreak(), span->GetPostBreak());
    });
}

// 异常测试
// 设置currentCgs为空
// 判定抛出TexgineException类型异常
TEST_F(TextBreakerTest, GenerateSpan1)
{
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument,
        breaker.GenerateSpan(emptyCgs_, tpstyle_, textStyle_, spans_));
}

// 异常测试
// 设置currentCgsbu为{}
// 判定抛出TexgineException类型异常
TEST_F(TextBreakerTest, GenerateSpan2)
{
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument,
        breaker.GenerateSpan({}, tpstyle_, textStyle_, spans_));
}

// 过程测试
// 设置currentCgsbu为空
// 判定不抛出异常, spans_的大小为1
TEST_F(TextBreakerTest, GenerateSpan3)
{
    EXPECT_NO_THROW({
        breaker.GenerateSpan(cgs2_, tpstyle_, textStyle_, spans_);
        ASSERT_EQ(spans_.size(), 1);
    });
}

// 逻辑测试
// 传入一个AnySpan
// 判定返回值为0，出参spans_中的内容转化后不为空指针
TEST_F(TextBreakerTest, WordBreak1)
{
    spans_ = {std::make_shared<MockAnySpan>()};
    EXPECT_NO_THROW({
        breaker.WordBreak(spans_, tpstyle_, FontProviders::Create());
        ASSERT_EQ(spans_.size(), 1);
        auto span = spans_[0].TryToAnySpan();
        ASSERT_NE(span, nullptr);
    });
}

// 逻辑测试
// 在fontCollection不为空指针的情况下，让Measure返回值不为0
// 判定返回值不为0，出参spans_的大小为0
TEST_F(TextBreakerTest, WordBreak2)
{
    PrepareWordBreak(1, cgs1_);

    spans_ = {tsNormal_};
    EXPECT_NO_THROW({
        breaker.WordBreak(spans_, tpstyle_, FontProviders::Create());
        ASSERT_EQ(spans_.size(), 0);
    });
}

// 过程测试
// 传入spans为空
// 判定返回值为0，出参spans_的大小为0
TEST_F(TextBreakerTest, WordBreak3)
{
    EXPECT_NO_THROW({
        breaker.WordBreak(spans_, tpstyle_, FontProviders::Create());
        ASSERT_EQ(spans_.size(), 0);
    });
}

// 过程测试
// 在fontCollection不为空指针的情况下，让Measure成功，boundaries = {{0, 4}}
// 分词策略为WordBreakType::breakWord
// 判定返回值为0，出参spans_的大小为1
TEST_F(TextBreakerTest, WordBreak4)
{
    auto type = WordBreakType::BreakWord;
    boundaries = {{ 0, 4 }};
    PrepareWordBreak(0, cgs1_, type);

    spans_ = {tsNormal_};
    EXPECT_NO_THROW({
        breaker.WordBreak(spans_, tpstyle_, FontProviders::Create());
        ASSERT_EQ(spans_.size(), 1);
    });
}

// 过程测试
// 在fontCollection不为空指针的情况下，让Measure成功，boundaries = {{0, 4}}
// 分词策略为WordBreakType::breakAll
// 判定返回值为0，出参spans_的大小为4
TEST_F(TextBreakerTest, WordBreak5)
{
    auto type = WordBreakType::BreakAll;
    boundaries = {{ 0, 4 }};
    PrepareWordBreak(0, cgs1_, type);

    spans_ = {tsNormal_};
    EXPECT_NO_THROW({
        breaker.WordBreak(spans_, tpstyle_, FontProviders::Create());
        ASSERT_EQ(spans_.size(), 4);
    });
}

// 过程测试
// 在fontCollection不为空指针的情况下，让Measure成功，boundaries = {}
// 判定返回值为0，出参spans_的大小为0
TEST_F(TextBreakerTest, WordBreak6)
{
    auto type = WordBreakType::BreakAll;
    boundaries = {};
    PrepareWordBreak(0, cgs1_, type);

    spans_ = {tsNormal_};
    EXPECT_NO_THROW({
        breaker.WordBreak(spans_, tpstyle_, FontProviders::Create());
        ASSERT_EQ(spans_.size(), 0);
    });
}
} // namespace Texgine
