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

#include "font_collection.h"
#include "texgine_font_manager.h"

namespace Texgine {
struct MockVars {
    int catchedSize_ = -1;
    bool hasRetval_ = false;
    int hasCount_ = 0;
    std::shared_ptr<TexgineFontManager> fontMgr_ = std::make_shared<TexgineFontManager>();
    std::shared_ptr<TexgineTypeface> SCRetvalTypeface_ = std::make_shared<TexgineTypeface>();
    std::shared_ptr<TexgineTypeface> styleRetvalTypeface_ = std::make_shared<TexgineTypeface>();
    std::vector<std::shared_ptr<VariantFontStyleSet>> fontStyleSets_;
    std::shared_ptr<Texgine::FontCollection> fontCollection_;
} mockVars;

std::shared_ptr<TexgineFontManager> TexgineFontManager::RefDefault()
{
    return mockVars.fontMgr_;
}

std::shared_ptr<TexgineTypeface> TexgineFontManager::MatchFamilyStyleCharacter(const char familyName[],
    TexgineFontStyle &style, const char *bcp47[], int bcp47Count, int32_t character)
{
    mockVars.catchedSize_ = bcp47Count;
    return mockVars.SCRetvalTypeface_;
}

std::shared_ptr<TexgineTypeface> VariantFontStyleSet::MatchStyle(std::shared_ptr<TexgineFontStyle> pattern)
{
    return mockVars.styleRetvalTypeface_;
}

bool Typeface::Has(uint32_t ch)
{
    mockVars.hasCount_++;
    return mockVars.hasRetval_;
}

std::vector<std::shared_ptr<VariantFontStyleSet>> CreateSets()
{
    std::vector<std::shared_ptr<VariantFontStyleSet>> sets;
    auto tfss = std::make_shared<TexgineFontStyleSet>(nullptr);
    sets.push_back(std::make_shared<VariantFontStyleSet>(tfss));
    return sets;
}

void InitMockVars(MockVars vars)
{
    mockVars = vars;
    mockVars.fontCollection_ = std::make_shared<FontCollection>(std::move(mockVars.fontStyleSets_));
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
// 调用GetTypefaceForChar函数，locale为空
// 调用GetTypefaceForChar函数
// 判定bcp47.size为0
// 判定返回值为非空指针, 调用Get返回的typeface与传入的相同
TEST_F(FontCollectionTest, GetTypefaceForChar1)
{
    InitMockVars({});
    auto tf = mockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "");
    EXPECT_NE(tf, nullptr);
    EXPECT_EQ(tf->Get(), mockVars.SCRetvalTypeface_);
    EXPECT_EQ(mockVars.catchedSize_, 0);
}

// 过程测试
// 控制RefDefault返回nullptr
// 调用GetTypefaceForChar函数
// 判定返回值为空指针
TEST_F(FontCollectionTest, GetTypefaceForChar2)
{
    InitMockVars({.fontMgr_ = nullptr});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', {}, "zh_CN", ""), nullptr);
    EXPECT_EQ(mockVars.catchedSize_, -1);
}

// 过程测试
// 控制matchFamilyStyleCharacter返回nullptr
// 调用GetTypefaceForChar函数
// 判定返回值为空指针
TEST_F(FontCollectionTest, GetTypefaceForChar3)
{
    InitMockVars({.SCRetvalTypeface_ = nullptr});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "zh_CN"), nullptr);
    EXPECT_NE(mockVars.catchedSize_, 0);
}

// 过程测试
// 调用两次GetTypefaceForChar函数
// 第一次判定返回非空指针，bcp47.size不为0
// 第二次判定返回非空指针，bcp47.size == -1
// 判定两次得到的typeface相同
TEST_F(FontCollectionTest, GetTypefaceForChar4)
{
    InitMockVars({});
    auto tf1 = mockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "zh_HK");
    EXPECT_NE(tf1, nullptr);
    EXPECT_NE(mockVars.catchedSize_, 0);
    InitMockVars({});
    auto tf2 = mockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "zh_HK");
    EXPECT_NE(tf2, nullptr);
    EXPECT_EQ(mockVars.catchedSize_, -1);
    EXPECT_EQ(tf1->Get(), tf2->Get());
}

// 过程测试
// 控制matchStyle函数返回nullptr
// 调用GetTypefaceForChar函数
// 判定返回值为空指针
TEST_F(FontCollectionTest, GetTypefaceForChar5)
{
    InitMockVars({.fontMgr_ = nullptr, .styleRetvalTypeface_ = nullptr, .fontStyleSets_ = CreateSets()});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "en_US"), nullptr);
}

// 过程测试
// Has返回false
// 调用GetTypefaceForChar函数
// 判定返回值非空指针
TEST_F(FontCollectionTest, GetTypefaceForChar6)
{
    InitMockVars({.fontMgr_ = nullptr, .fontStyleSets_ = CreateSets()});
    EXPECT_EQ(mockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "en_US"), nullptr);
}

// 过程测试
// 控制Has返回值为true
// 创建对象时传入非空vector
// 调用两次GetTypefaceForChar函数
// 判定返回值为非空指针
// 判定Has调用两次
// 判定两次得到的SkTypeface相同
TEST_F(FontCollectionTest, GetTypefaceForChar7)
{
    InitMockVars({.hasRetval_ = true, .fontMgr_ = nullptr, .fontStyleSets_ = CreateSets()});
    auto tf1 = mockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "locale1");
    auto tf2 = mockVars.fontCollection_->GetTypefaceForChar('a', {}, "", "locale1");
    EXPECT_NE(tf1, nullptr);
    EXPECT_NE(tf2, nullptr);
    EXPECT_EQ(mockVars.hasCount_, 2);
    EXPECT_EQ(tf1->Get(), tf2->Get());
}
} // namespace Texgine
