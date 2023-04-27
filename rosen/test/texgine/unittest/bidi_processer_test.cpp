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

#include "bidi_processer.h"
#include "mock/mock_any_span.h"
#include "texgine_exception.h"
#include "text_breaker.h"
#include "text_converter.h"
#include "text_span.h"

#include <gtest/gtest.h>
#include <unicode/ubidi.h>

using namespace testing;
using namespace testing::ext;

struct UBiDi {};
std::unique_ptr<UBiDi> g_bidi = nullptr;
U_STABLE UBiDi * U_EXPORT2
ubidi_open(void)
{
    return g_bidi.get();
}

U_STABLE void U_EXPORT2
ubidi_close(UBiDi *pBiDi) {}

auto g_status = U_ZERO_ERROR;
U_STABLE void U_EXPORT2
ubidi_setPara(UBiDi *pBiDi, const UChar *text, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embeddingLevels,
              UErrorCode *pErrorCode)
{
    *pErrorCode = g_status;
}

int g_size = 0;
U_STABLE int32_t U_EXPORT2
ubidi_countRuns(UBiDi *pBiDi, UErrorCode *pErrorCode)
{
    return g_size;
}

std::vector<int> g_start;
std::vector<int> g_length;
int g_count = 0;
UBiDiDirection g_rtl = UBIDI_RTL;
U_STABLE UBiDiDirection U_EXPORT2
ubidi_getVisualRun(UBiDi *pBiDi, int32_t runIndex,
                   int32_t *pLogicalStart, int32_t *pLength)
{
    assert(g_count < g_size);
    *pLogicalStart = (g_start[g_count]);
    *pLength = (g_length[g_count]);
    g_count++;
    return g_rtl;
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
    g_count = 0;
    g_size = style.size_;
    g_bidi = std::move(style.bidi_);
    g_status = style.status_;
    g_rtl = style.rtl_;
    g_start = style.start_;
    g_length = style.length_;
}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
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

/**
 * @tc.name: DoBidiProcess1
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess1, TestSize.Level1)
{
    CharGroups cgs = CharGroups::CreateEmpty();

    try {
        bp.DoBidiProcess(cgs, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::InvalidArgument, err.code);
    }
}

/**
 * @tc.name: DoBidiProcess2
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess2, TestSize.Level1)
{
    CharGroups cgs = {};

    try {
        bp.DoBidiProcess(cgs, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::InvalidArgument, err.code);
    }
}

/**
 * @tc.name: DoBidiProcess3
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess3, TestSize.Level1)
{
    InitMyMockVars({.bidi_ = nullptr});

    try {
        bp.DoBidiProcess(cgs1_, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::APIFailed, err.code);
    }
}

/**
 * @tc.name: DoBidiProcess4
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess4, TestSize.Level1)
{
    InitMyMockVars({.status_ = U_ILLEGAL_ARGUMENT_ERROR});

    try {
        bp.DoBidiProcess(cgs1_, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::APIFailed, err.code);
    }
}

/**
 * @tc.name: DoBidiProcess5
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess5, TestSize.Level1)
{
    // set ubidi_countRuns return 0
    InitMyMockVars({.size_ = 0,});

    EXPECT_NO_THROW({
        auto ret = bp.DoBidiProcess(cgs1_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 0);
    });
}

/**
 * @tc.name: DoBidiProcess6
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess6, TestSize.Level1)
{
    // -1, 1: Set the output parameter of ubidi_getVisualRun
    InitMyMockVars({.start_ = {-1}, .length_ = {1},});

    try {
        bp.DoBidiProcess(cgs1_, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::APIFailed, err.code);
    }
}

/**
 * @tc.name: DoBidiProcess7
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess7, TestSize.Level1)
{
    InitMyMockVars({.start_ = {1}, .length_ = {-1},});

    try {
        bp.DoBidiProcess(cgs1_, TextDirection::LTR);
    } catch(const TexgineException &err) {
        ASSERT_EQ(ExceptionType::APIFailed, err.code);
    }
}

/**
 * @tc.name: DoBidiProcess8
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess8, TestSize.Level1)
{
    InitMyMockVars({.size_ = 4, .start_ = {0, 1, 2, 3}, .length_ = {1, 1, 1, 1}});
    auto c = cgs1_.GetSub(0, 1);
    EXPECT_NO_THROW({
        auto ret = bp.DoBidiProcess(c, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 1);
    });
}

/**
 * @tc.name: DoBidiProcess9
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess9, TestSize.Level1)
{
    InitMyMockVars({});

    EXPECT_NO_THROW({
        auto ret = bp.DoBidiProcess(cgs1_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 1);
        ASSERT_EQ(ret[0].rtl_, true);
    });
}

/**
 * @tc.name: DoBidiProcess10
 * @tc.desc: Verify the DoBidiProcess
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, DoBidiProcess10, TestSize.Level1)
{
    InitMyMockVars({.rtl_ = UBIDI_LTR,});

    EXPECT_NO_THROW({
        auto ret = bp.DoBidiProcess(cgs1_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 1);
        ASSERT_EQ(ret[0].rtl_, false);
    });
}

/**
 * @tc.name: ProcessBidiText1
 * @tc.desc: Verify the ProcessBidiText
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, ProcessBidiText1, TestSize.Level1)
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

/**
 * @tc.name: ProcessBidiText2
 * @tc.desc: Verify the ProcessBidiText
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, ProcessBidiText2, TestSize.Level1)
{
    EXPECT_NO_THROW({
        auto ret = bp.ProcessBidiText(spans_, TextDirection::LTR);
        ASSERT_EQ(ret.size(), 0);
    });
}

/**
 * @tc.name: ProcessBidiText3
 * @tc.desc: Verify the ProcessBidiText
 * @tc.type:FUNC
 * @tc.require: issueI6V6J4
 */
HWTEST_F(BidiProcesserTest, ProcessBidiText3, TestSize.Level1)
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
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
