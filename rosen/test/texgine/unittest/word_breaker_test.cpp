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

#include "param_test_macros.h"
#include "word_breaker.h"

namespace Texgine {
class MockBreakIterator : public icu::BreakIterator {
public:
    bool operator ==(const icu::BreakIterator &rhs) const override { return false; }
    MOCK_METHOD(BreakIterator *, clone, (), (const, override));
    MOCK_METHOD(UClassID, getDynamicClassID, (), (const, override));
    MOCK_METHOD(icu::CharacterIterator &, getText, (), (const, override));
    MOCK_METHOD(UText *, getUText, (UText *fillIn, UErrorCode &status), (const, override));
    MOCK_METHOD(void, setText, (const icu::UnicodeString &text), (override));
    MOCK_METHOD(void, setText, (UText *text, UErrorCode &status), (override));
    MOCK_METHOD(void, adoptText, (icu::CharacterIterator* it), (override));
    MOCK_METHOD(int32_t, first, (), (override));
    MOCK_METHOD(int32_t, last, (), (override));
    MOCK_METHOD(int32_t, previous, (), (override));
    MOCK_METHOD(int32_t, next, (), (override));
    MOCK_METHOD(int32_t, current, (), (const, override));
    MOCK_METHOD(int32_t, following, (int32_t offset), (override));
    MOCK_METHOD(int32_t, preceding, (int32_t offset), (override));
    MOCK_METHOD(UBool, isBoundary, (int32_t offset), (override));
    MOCK_METHOD(int32_t, next, (int32_t offset), (override));
    MOCK_METHOD(BreakIterator *, createBufferClone, (void *stackBuffer, int32_t &BufferSize, UErrorCode &status), (override));
    MOCK_METHOD(BreakIterator &, refreshInputText, (UText *input, UErrorCode &status), (override));
};

struct Mockvars {
    std::unique_ptr<MockBreakIterator> iterator_ = std::make_unique<MockBreakIterator>();
    UErrorCode createLineInstanceStatus_ = U_ZERO_ERROR;
    icu::Locale catchedLocale_;
} mockvars;

void InitMockvars(struct Mockvars &&vars)
{
    mockvars = std::move(vars);
}
} // namespace Texgine

icu::BreakIterator*
icu::BreakIterator::createLineInstance(const icu::Locale& key, UErrorCode& status)
{
    Texgine::mockvars.catchedLocale_ = key;
    status = Texgine::mockvars.createLineInstanceStatus_;
    return Texgine::mockvars.iterator_.get();
}

namespace Texgine {
class WordBreakerTest : public testing::Test {
public:
    std::vector<uint16_t> u16str = {u'A', u'a'};
};

// 尝试捕获locale
TEST_F(WordBreakerTest, SetLocale)
{
    WordBreaker wbreaker;

    icu::Locale defaultLocale = icu::Locale::getDefault();
    icu::Locale targetLocale;
    if (icu::Locale::getDefault() == icu::Locale::getChinese()) {
        targetLocale = icu::Locale::getEnglish();
    } else {
        targetLocale = icu::Locale::getChinese();
    }

    wbreaker.SetLocale(targetLocale);
    InitMockvars({.iterator_ = nullptr, .catchedLocale_ = defaultLocale});
    ASSERT_NE(mockvars.catchedLocale_, targetLocale);
    ASSERT_EXCEPTION(ExceptionType::APIFailed, wbreaker.GetBoundary(u16str););
    ASSERT_EQ(mockvars.catchedLocale_, targetLocale);
}

// 返回值为nullptr，抛出APIFailed异常
TEST_F(WordBreakerTest, GetBoundary1)
{
    WordBreaker wbreaker;

    InitMockvars({.iterator_ = nullptr});
    ASSERT_EXCEPTION(ExceptionType::APIFailed, wbreaker.GetBoundary(u16str););
}

// 状态为U_ILLEGAL_ARGUMENT_ERROR，抛出APIFailed异常
TEST_F(WordBreakerTest, GetBoundary2)
{
    WordBreaker wbreaker;

    InitMockvars({.createLineInstanceStatus_ = U_ILLEGAL_ARGUMENT_ERROR});
    ASSERT_EXCEPTION(ExceptionType::APIFailed, wbreaker.GetBoundary(u16str););
}

// 没设置Range，抛出InvalidArgument异常
TEST_F(WordBreakerTest, GetBoundary3)
{
    WordBreaker wbreaker;

    InitMockvars({});
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, wbreaker.GetBoundary(u16str););
}

// 设置倒置范围，抛出InvalidArgument异常
TEST_F(WordBreakerTest, GetBoundary4)
{
    WordBreaker wbreaker;

    InitMockvars({});
    wbreaker.SetRange(2, 0);
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, wbreaker.GetBoundary(u16str););
}

// 设置过大范围，抛出InvalidArgument异常
TEST_F(WordBreakerTest, GetBoundary5)
{
    WordBreaker wbreaker;

    InitMockvars({});
    wbreaker.SetRange(0, 3);
    ASSERT_EXCEPTION(ExceptionType::InvalidArgument, wbreaker.GetBoundary(u16str););
}

// 逻辑测试
TEST_F(WordBreakerTest, GetBoundary6)
{
    WordBreaker wbreaker;
    wbreaker.SetRange(0, 2);
    InitMockvars({});
    EXPECT_CALL(*mockvars.iterator_, setText(testing::_)).Times(1);
    EXPECT_CALL(*mockvars.iterator_, first())
        .Times(1)
        .WillOnce(testing::Return(1));
    EXPECT_CALL(*mockvars.iterator_, next())
        .Times(2)
        .WillOnce(testing::Return(2))
        .WillOnce(testing::Return(icu::BreakIterator::DONE));
    auto boundary = wbreaker.GetBoundary(u16str);
    ASSERT_EQ(boundary.size(), 1);
    ASSERT_EQ(boundary[0].leftIndex_, 1);
    ASSERT_EQ(boundary[0].rightIndex_, 2);
}
} // namespace Texgine
