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

#include "bidi_processer.h"
#include "mock/mock_any_span.h"
#include "texgine_exception.h"
#include "text_span.h"
#include "text_breaker.h"
#include "text_converter.h"

#include <gtest/gtest.h>
#include <unicode/ubidi.h>

struct UBiDi {};
std::unique_ptr<UBiDi> bidi = nullptr;
U_STABLE UBiDi * U_EXPORT2
ubidi_open(void)
{
    return bidi.get();
}

U_STABLE void U_EXPORT2
ubidi_close(UBiDi *pBiDi) {}

auto status = U_ZERO_ERROR;
U_STABLE void U_EXPORT2
ubidi_setPara(UBiDi *pBiDi, const UChar *text, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embeddingLevels,
              UErrorCode *pErrorCode)
{
    *pErrorCode = status;
}

int size = 0;
U_STABLE int32_t U_EXPORT2
ubidi_countRuns(UBiDi *pBiDi, UErrorCode *pErrorCode)
{
    return size;
}

std::vector<int> start;
std::vector<int> length;
int count = 0;
UBiDiDirection rtl = UBIDI_RTL;
U_STABLE UBiDiDirection U_EXPORT2
ubidi_getVisualRun(UBiDi *pBiDi, int32_t runIndex,
                   int32_t *pLogicalStart, int32_t *pLength)
{
    assert(count < size);
    *pLogicalStart = (start[count]);
    *pLength = (length[count]);
    count++;
    return rtl;
}

struct Style {
    int size_ = 1;
    std::unique_ptr<UBiDi> bidi_ = std::make_unique<UBiDi>();
    UErrorCode status_ = U_ZERO_ERROR;
    UBiDiDirection rtl_ = UBIDI_RTL;
    std::vector<int> start_ = {0};
    std::vector<int> length_ = {4};
};

void InitMyMockVars(Style style)
{
    count = 0;
    size = style.size_;
    bidi = std::move(style.bidi_);
    status = style.status_;
    rtl = style.rtl_;
    start = style.start_;
    length = style.length_;
}

namespace Texgine {
class BidiProcesserTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        tpstyle_ = std::make_shared<TypographyStyle>();
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("m"), .glyphs_ = {{0x013B, 13.664}}});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("o"), .glyphs_ = {{0x0145, 9.456}}});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("s"), .glyphs_ = {{0x0166, 7.28}}});
        cgs1_.PushBack({.chars_ = TextConverter::ToUTF16("t"), .glyphs_ = {{0x016E, 5.88}}});
    }

    static inline std::shared_ptr<TypographyStyle> tpstyle_ = nullptr;
    static inline TextStyle textStyle_;
    static inline CharGroups cgs1_ = CharGroups::CreateEmpty();
    std::vector<VariantSpan> spans_;
    BidiProcesser bp;
};

// 异常测试
// 设置cgs为空
// 判定抛出TexgineException异常
TEST_F(BidiProcesserTest, DoBidiProcess1)
{
    CharGroups cgs = CharGroups::CreateEmpty();

    try {
        bp.DoBidiProcess(cgs, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::InvalidArgument, err.code);
    }
}

// 异常测试
// 设置cgs为{}
// 判定抛出TexgineException异常
TEST_F(BidiProcesserTest, DoBidiProcess2)
{
    CharGroups cgs = {};

    try {
        bp.DoBidiProcess(cgs, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::InvalidArgument, err.code);
    }
}

// 异常测试
// 控制ubidi_open()返回值为nullptr
// 判定抛出TexgineException异常
TEST_F(BidiProcesserTest, DoBidiProcess3)
{
    InitMyMockVars({.bidi_ = nullptr});

    try {
        bp.DoBidiProcess(cgs1_, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::APIFailed, err.code);
    }
}

// 异常测试
// 控制ubidi_setPara，中status返回值为U_ILLEGAL_ARGUMENT_ERROR, ubidi_open()返回值不为nullptr
// 判定抛出TexgineException异常
TEST_F(BidiProcesserTest, DoBidiProcess4)
{
    InitMyMockVars({.status_ = U_ILLEGAL_ARGUMENT_ERROR});

    try {
        bp.DoBidiProcess(cgs1_, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::APIFailed, err.code);
    }
}

// 过程测试
// 控制ubidi_countRuns返回值为0
// 判定无异常抛出，并且返回值的大小为0
TEST_F(BidiProcesserTest, DoBidiProcess5)
{
    InitMyMockVars({.size_ = 0,});

    EXPECT_NO_THROW({
        auto ret = bp.DoBidiProcess(cgs1_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 0);
    });
}

// 异常测试
// 控制ubidi_countRuns返回值为4,控制ubidi_getVisualRun(bidi, i, &start, &length)中start=-1, length=1
// 判定抛出TexgineException异常
TEST_F(BidiProcesserTest, DoBidiProcess6)
{
    InitMyMockVars({.start_ = {-1}, .length_ = {1},});

    try {
        bp.DoBidiProcess(cgs1_, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::APIFailed, err.code);
    }
}

// 异常测试
// 控制ubidi_countRuns返回值为1,控制ubidi_getVisualRun(bidi, i, &start, &length)中start=1, length=-1
// 判定抛出TexgineException异常
TEST_F(BidiProcesserTest, DoBidiProcess7)
{
    InitMyMockVars({.start_ = {1}, .length_ = {-1},});

    try {
        bp.DoBidiProcess(cgs1_, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::APIFailed, err.code);
    }
}

// 逻辑测试
// 控制ubidi_countRuns返回值为4,每次修改start{0，1，2，3}和length{1, 1, 1, 1}的值，使IsIntersect的返回值为false
// 判定不抛出异常, 并且返回值为1
TEST_F(BidiProcesserTest, DoBidiProcess8)
{
    InitMyMockVars({.size_ = 4, .start_ = {0, 1, 2, 3}, .length_ = {1, 1, 1, 1}});
    auto c = cgs1_.GetSub(0, 1);
    EXPECT_NO_THROW({
        auto ret = bp.DoBidiProcess(c, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 1);
    });
}

// 逻辑测试
// 控制ubidi_getVisualRun的返回值为UBIDI_RTL
// 判定不抛出异常，并且获取返回值中rtl为true
TEST_F(BidiProcesserTest, DoBidiProcess9)
{
    InitMyMockVars({});

    EXPECT_NO_THROW({
        auto ret = bp.DoBidiProcess(cgs1_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 1);
        ASSERT_EQ(ret[0].rtl_, true);
    });
}

// 逻辑测试
// 控制ubidi_getVisualRun的返回值为UBIDI_LTR
// 判定不抛出异常，并且返回值中rtl为false
TEST_F(BidiProcesserTest, DoBidiProcess10)
{
    InitMyMockVars({.rtl_ = UBIDI_LTR,});

    EXPECT_NO_THROW({
        auto ret = bp.DoBidiProcess(cgs1_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 1);
        ASSERT_EQ(ret[0].rtl_, false);
    });
}

// 逻辑测试
// 设置spans中的内容是AnySpan、TextSpan、AnySpan
// 判定不抛出异常，返回值的大小为3，并且转换成相应的类型之后不为空指针
TEST_F(BidiProcesserTest, ProcessBidiText1)
{
    InitMyMockVars({});
    spans_ = {std::make_shared<MockAnySpan>(), TextSpan::MakeFromCharGroups(cgs1_), std::make_shared<MockAnySpan>()};

    EXPECT_NO_THROW({
        auto ret = bp.ProcessBidiText(spans_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 3);
        ASSERT_NE(ret[0].TryToAnySpan(), nullptr);
        ASSERT_NE(ret[1].TryToTextSpan(), nullptr);
        ASSERT_NE(ret[2].TryToAnySpan(), nullptr);
    });
}

// 过程测试
// 设置spans为空
// 判定不抛出异常，返回值的大小为0
TEST_F(BidiProcesserTest, ProcessBidiText2)
{
    EXPECT_NO_THROW({
        auto ret = bp.ProcessBidiText(spans_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 0);
    });
}

// 过程测试
// 所有参数均正常设置，测试正确流程
// 判定不抛出异常，返回值的大小为3，返回值中span类型正确
TEST_F(BidiProcesserTest, ProcessBidiText3)
{
    InitMyMockVars({.size_ = 4, .start_ = {0, 1, 2, 3}, .length_ = {1, 1, 1, 1}});
    auto c = cgs1_.GetSub(0, 1);
    spans_ = {std::make_shared<MockAnySpan>(), TextSpan::MakeFromCharGroups(c), std::make_shared<MockAnySpan>()};

    EXPECT_NO_THROW({
        auto ret = bp.ProcessBidiText(spans_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 3);
        ASSERT_NE(ret[0].TryToAnySpan(), nullptr);
        auto span1 = ret[1].TryToTextSpan();
        ASSERT_NE(span1, nullptr);
        ASSERT_EQ(span1->IsRTL(), true);
        ASSERT_NE(ret[2].TryToAnySpan(), nullptr);
    });
}
} // namespace Texgine
