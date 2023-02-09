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

#include <iostream>
#include <vector>

#include <gtest/gtest.h>
#include <hb.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkString.h>
#include <include/core/SkTypeface.h>
#include <src/ports/SkFontMgr_custom.h>

#include "opentype_parser/cmap_parser.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "typeface.h"

struct hb_blob_t {};

struct Mockvars {
    const char *name_ = "";
    int sizeIndex_ = 0;
    int lengthIndex_ = 0;
    int idIndex_ = 0;
    std::vector<int> tableSize_ = {1};
    std::vector<int> dataLength_ = {1};
    int parseRetval_ = 0;
    std::vector<int> glyphId_ = {0};
    sk_sp<SkTypeface> skTypeface_ = sk_make_sp<SkTypeface_Empty>();
    std::unique_ptr<hb_blob_t> blob_ = std::make_unique<hb_blob_t>();
    std::unique_ptr<Texgine::Typeface> typeface_ = nullptr;
} mockvars;

void InitMyMockVars(Mockvars vars)
{
    mockvars = std::move(vars);
    mockvars.typeface_ = std::make_unique<Texgine::Typeface>(mockvars.skTypeface_);
}

sk_sp<SkTypeface> SkTypeface::MakeFromFile(const char path[], int index)
{
    return mockvars.skTypeface_;
}

size_t SkTypeface::getTableSize(SkFontTableTag tag) const
{
    assert(mockvars.sizeIndex_ < mockvars.tableSize_.size());
    return mockvars.tableSize_[mockvars.sizeIndex_++];
}

size_t SkTypeface::getTableData(SkFontTableTag tag, size_t offset, size_t length,void* data) const
{
    return mockvars.dataLength_[mockvars.lengthIndex_++];
}

void SkTypeface::getFamilyName(SkString* name) const
{
    *name = mockvars.name_;
}

extern "C" {
hb_blob_t * hb_blob_create (const char *data, unsigned int length, hb_memory_mode_t mode, void *user_data, hb_destroy_func_t destroy)
{
    return mockvars.blob_.get();
}

void hb_blob_destroy(hb_blob_t *)
{
}
}

namespace Texgine {
int CmapParser::Parse(const char *data, int32_t size)
{
    return mockvars.parseRetval_;
}

int32_t CmapParser::GetGlyphId(int32_t codepoint) const
{
    return mockvars.glyphId_[mockvars.idIndex_++];
}

class TypefaceTest : public testing::Test {
public:
};

// 逻辑测试
// 设置sktypeface_为非空
// 判定返回值不为空指针，可以得到name
TEST_F(TypefaceTest, MakeFromFile1)
{
    InitMyMockVars({ .name_ = "cxt" });
    auto typeface = Typeface::MakeFromFile("aaa");
    EXPECT_NE(typeface.get(), nullptr);
    EXPECT_EQ(typeface->GetName(), "cxt");
}

// 异常测试
// 设置sktypeface_为空
// 判定得不到name并抛出异常
TEST_F(TypefaceTest, MakeFromFile2)
{
    InitMyMockVars({ .name_ = "cxxt", .skTypeface_ = nullptr });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, Typeface::MakeFromFile("aaa"));
    EXPECT_NE(mockvars.typeface_->GetName(), "cxxt");
}

// 异常测试
// 设置typeface_为空
// 判定返回值为false
TEST_F(TypefaceTest, Has1)
{
    InitMyMockVars({ .name_ = "one", .skTypeface_ = nullptr });
    EXPECT_EQ(mockvars.typeface_->Has(0x0006), false);
}

// 异常测试
// 设置size为0
// 判定抛出异常
TEST_F(TypefaceTest, Has2)
{
    InitMyMockVars({ .name_ = "two", .tableSize_ = {0} });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, mockvars.typeface_->Has(0x0006));
}

// 异常测试
// 设置retv为2，不等于size
// 判定抛出异常
TEST_F(TypefaceTest, Has3)
{
    InitMyMockVars({ .name_ = "three", .tableSize_ = {3}, .dataLength_ = {2} });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, mockvars.typeface_->Has(0x0006));
}

// 异常测试
// 设置blob_为空
// 判定抛出异常
TEST_F(TypefaceTest, Has4)
{
    InitMyMockVars({ .name_ = "four", .blob_ = nullptr });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, mockvars.typeface_->Has(0x0006));
}

// 过程测试
// 设置parseRetval_ 为-1
// 判定返回值为false
TEST_F(TypefaceTest, Has5)
{
    InitMyMockVars({ .name_ = "five", .parseRetval_ = -1 });
    EXPECT_EQ(mockvars.typeface_->Has(0x0006), false);
}

// 过程测试
// 设置parseRetval_ 为0
// 判定返回值为true
TEST_F(TypefaceTest, Has6)
{
    InitMyMockVars({ .name_ = "six" });
    EXPECT_EQ(mockvars.typeface_->Has(0x0006), true);
}

// 逻辑测试
// 设置两次都是成功走通
// 第一次返回值为true，第二次返回值为false，判定sizeIndex_为1
TEST_F(TypefaceTest, Has7)
{
    InitMyMockVars({ .name_ = "seven", .tableSize_ = {1, 1}, .dataLength_ = {1, 1}, .glyphId_ = {0, -1} });
    auto bool1 = mockvars.typeface_->Has(0x0006);
    auto bool2 = mockvars.typeface_->Has(0x0006);
    EXPECT_EQ(bool1, true);
    EXPECT_EQ(bool2, false);
    EXPECT_EQ(mockvars.sizeIndex_, 1);
}

// 逻辑测试
// 设置第一次失败，第二次成功走通
// 第一次会抛出异常，第二次返回值为true，判定sizeIndex_为2
TEST_F(TypefaceTest, Has8)
{
    InitMyMockVars({ .name_ = "eight", .tableSize_ = {2, 1}, .dataLength_ = {1, 1} });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, mockvars.typeface_->Has(0x0006));
    auto bool1 = mockvars.typeface_->Has(0x0006);
    EXPECT_EQ(bool1, true);
    EXPECT_EQ(mockvars.sizeIndex_, 2);
}

// 逻辑测试
// 设置第一次失败，第二次失败
// 第一次会抛出异常，第二次也会抛出异常，判定sizeIndex_为2
TEST_F(TypefaceTest, Has9)
{
    InitMyMockVars({ .name_ = "nine", .tableSize_ = {0, 0} });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, mockvars.typeface_->Has(0x0006));
    ASSERT_EXCEPTION(ExceptionType::APIFailed, mockvars.typeface_->Has(0x0006));
    EXPECT_EQ(mockvars.sizeIndex_, 2);
}
} // namespace Texgine
