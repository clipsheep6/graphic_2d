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
#include "include/core/SkData.h"
#include "include/core/SkStream.h"
#include "src/ports/SkFontMgr_custom.h"

#include "font_collection.h"
#include "mock/mock_sk_font_mgr.h"
#include "mock/mock_sk_font_style_set.h"

struct MockVars {
    int catchedSize_ = -1;
    bool hasRetval_ = false;
    int hasCount_ = 0;
    sk_sp<SkFontMgr> skFontMgr_ = sk_make_sp<Texgine::MockSkFontMgr>();
    sk_sp<SkTypeface> skTypeface_ = sk_make_sp<SkTypeface_Empty>();
    std::vector<std::shared_ptr<Texgine::FontStyleSet>> fontStyleSets_;
    std::shared_ptr<Texgine::FontCollection> fontCollection_;
} mockVars;

sk_sp<SkFontMgr> SkFontMgr::RefDefault()
{
    return mockVars.skFontMgr_;
}

SkTypeface *SkFontMgr::matchFamilyStyleCharacter(const char familyName[], const SkFontStyle &style,
                                                 const char *bcp47[], int bcp47Count,
                                                 SkUnichar character) const
{
    mockVars.catchedSize_ = bcp47Count;
    return mockVars.skTypeface_.get();
}

namespace Texgine {
bool Typeface::Has(uint32_t ch)
{
    mockVars.hasCount_++;
    return mockVars.hasRetval_;
}

void InitMockVars(MockVars vars)
{
    mockVars = vars;
    mockVars.fontCollection_ = std::make_shared<FontCollection>(std::move(mockVars.fontStyleSets_));
}

std::vector<std::shared_ptr<FontStyleSet>> CreateSets(sk_sp<MockSkFontStyleSet> msfss,
                                                      sk_sp<SkTypeface> skTypeface,
                                                      int count = 0)
{
    if (msfss) {
        EXPECT_CALL(*msfss.get(), matchStyle).Times(count).WillRepeatedly(::testing::Return(skTypeface.get()));
    }

    std::vector<std::shared_ptr<FontStyleSet>> sets;
    sets.push_back(std::make_shared<FontStyleSet>(msfss.release()));
    return sets;
}

class FontCollectionTest : public testing::Test {
};

// 过程测试
// 创建FontCollection对象
// 判定不抛出异常
TEST_F(FontCollectionTest, FontCollection)
{
    InitMockVars({});
    EXPECT_NO_THROW(FontCollection fc(std::move(mockVars.fontStyleSets_)));
}

// 过程测试
// 调用GetTypefaceForChar函数，第二个参数传空
// 调用GetTypefaceForChar函数
// 判定bcp47.size为0
// 判定返回值为非空指针, 调用Get返回的SkTypeface与传入的相同
TEST_F(FontCollectionTest, GetTypefaceForChar1)
{
    InitMockVars({});
    auto tf = mockVars.fontCollection_->GetTypefaceForChar('a', "", {});
    EXPECT_NE(tf, nullptr);
    EXPECT_EQ(tf->Get(), mockVars.skTypeface_);
    EXPECT_EQ(mockVars.catchedSize_, 0);
}

// 过程测试
// 控制RefDefault返回nullptr
// 调用GetTypefaceForChar函数
// 判定返回值为空指针
TEST_F(FontCollectionTest, GetTypefaceForChar2)
{
    InitMockVars({.skFontMgr_ = nullptr});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', "zh_CN", {}), nullptr);
    EXPECT_EQ(mockVars.catchedSize_, -1);
}

// 过程测试
// 控制matchFamilyStyleCharacter返回nullptr
// 调用GetTypefaceForChar函数
// 判定返回值为空指针
TEST_F(FontCollectionTest, GetTypefaceForChar3)
{
    InitMockVars({.skTypeface_ = nullptr});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', "zh_CN", {}), nullptr);
    EXPECT_NE(mockVars.catchedSize_, 0);
}

// 过程测试
// 调用两次GetTypefaceForChar函数
// 第一次判定返回非空指针，bcp47.size不为0
// 第二次判定返回非空指针，bcp47.size == -1
// 判定两次得到的SkTypeface相同
TEST_F(FontCollectionTest, GetTypefaceForChar4)
{
    InitMockVars({});
    auto tf1 = mockVars.fontCollection_->GetTypefaceForChar('a', "en_US", {});
    EXPECT_NE(tf1, nullptr);
    EXPECT_NE(mockVars.catchedSize_, 0);
    InitMockVars({});
    auto tf2 = mockVars.fontCollection_->GetTypefaceForChar('a', "en_US", {});
    EXPECT_NE(tf2, nullptr);
    EXPECT_EQ(mockVars.catchedSize_, -1);
    EXPECT_EQ(tf1->Get(), tf2->Get());
}

// 过程测试
// 创建时，传入一个指向空指针的智能指针的vector
// 调用GetTypefaceForChar函数
// 判定返回值为空指针
TEST_F(FontCollectionTest, GetTypefaceForChar5)
{
    auto stf = sk_make_sp<SkTypeface_Empty>();
    InitMockVars({.skFontMgr_ = nullptr,
                  .fontStyleSets_ = CreateSets(nullptr, stf)});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', "en_UK", {}), nullptr);
}

// 过程测试
// 控制matchStyle函数返回nullptr
// 调用GetTypefaceForChar函数
// 判定返回值为空指针
TEST_F(FontCollectionTest, GetTypefaceForChar6)
{
    InitMockVars({.skFontMgr_ = nullptr,
                  .fontStyleSets_ = CreateSets(sk_make_sp<MockSkFontStyleSet>(), nullptr, 1)});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', "en_UK", {}), nullptr);
}

// 过程测试
// Has返回false
// 调用GetTypefaceForChar函数
// 判定返回值非空指针
TEST_F(FontCollectionTest, GetTypefaceForChar7)
{
    auto stf = sk_make_sp<SkTypeface_Empty>();
    InitMockVars({.skFontMgr_ = nullptr,
                  .fontStyleSets_ = CreateSets(sk_make_sp<MockSkFontStyleSet>(), stf, 1)});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', "en_UK", {}), nullptr);
}

// 过程测试
// 控制Has返回值为true
// 创建对象时传入非空vector
// 调用两次GetTypefaceForChar函数
// 判定返回值为非空指针
// 判定Has调用两次
// 判定两次得到的SkTypeface相同
TEST_F(FontCollectionTest, GetTypefaceForChar8)
{
    auto stf = sk_make_sp<SkTypeface_Empty>();
    InitMockVars({.hasRetval_ = true, .skFontMgr_ = nullptr,
                  .fontStyleSets_ = CreateSets(sk_make_sp<MockSkFontStyleSet>(), stf, 1)});
    auto tf1 = mockVars.fontCollection_->GetTypefaceForChar('a', "locale1", {});
    auto tf2 = mockVars.fontCollection_->GetTypefaceForChar('a', "locale1", {});
    EXPECT_NE(tf1, nullptr);
    EXPECT_NE(tf2, nullptr);
    EXPECT_EQ(mockVars.hasCount_, 2);
    EXPECT_EQ(tf1->Get(), tf2->Get());
}
} // namespace Texgine
