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

#include "texgine/dynamic_font_provider.h"

#include <gtest/gtest.h>
#include <iostream>

#include "include/core/SkFontStyle.h"
#include "include/core/SkStream.h"
#include "src/ports/SkFontMgr_custom.h"

#include "dynamic_font_style_set.h"
#include "font_style_set.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "typeface.h"

SkTypeface_Empty::SkTypeface_Empty() :SkTypeface_Custom({}, {}, {}, {}, {}) {}

std::unique_ptr<SkMemoryStream> skMemoryStream = nullptr;
sk_sp<SkTypeface> skTypeface = nullptr;
struct Style {
    std::unique_ptr<SkMemoryStream> skMemoryStream_ = std::make_unique<SkMemoryStream>();
    sk_sp<SkTypeface> skTypeface_ = sk_make_sp<SkTypeface_Empty>();
};

void InitMyMockVars(Style style)
{
    skMemoryStream = std::move(style.skMemoryStream_);
    skTypeface = style.skTypeface_;
}

std::unique_ptr<SkMemoryStream> SkMemoryStream::MakeCopy(const void *data, size_t length)
{
    return std::move(skMemoryStream);
}

sk_sp<SkTypeface> SkTypeface::MakeFromStream(std::unique_ptr<SkStreamAsset> stream, int index)
{
    return skTypeface;
}

namespace Texgine {
class DynamicFontProviderTest : public testing::Test {
public:
    std::shared_ptr<DynamicFontProvider> dynamicFontProvider = DynamicFontProvider::Create();
};

// 过程测试
// 调用Create函数
// 判定返回值为非空指针
TEST_F(DynamicFontProviderTest, Create)
{
    EXPECT_NE(DynamicFontProvider::Create(), nullptr);
}

// 异常测试
// 调用LoadFont函数，第二个参数为nullptr
// 判定不抛出异常，并且返回值为1
TEST_F(DynamicFontProviderTest, LoadFont1)
{
    InitMyMockVars({});
    EXPECT_NO_THROW({ EXPECT_EQ(dynamicFontProvider->LoadFont("LF1", nullptr, 4), 1); });
}

// 异常测试
// 调用LoadFont函数，第三个参数为0
// 判定不抛出异常，并且返回值为1
TEST_F(DynamicFontProviderTest, LoadFont2)
{
    InitMyMockVars({});
    EXPECT_NO_THROW({ EXPECT_EQ(dynamicFontProvider->LoadFont("LF2", this, 0), 1); });
}

// 异常测试
// 重写MakeCopy函数返回nullptr，调用LoadFont函数
// 判定不抛出异常，并且返回值为2
TEST_F(DynamicFontProviderTest, LoadFont3)
{
    InitMyMockVars({.skMemoryStream_ = nullptr});
    EXPECT_NO_THROW({ EXPECT_EQ(dynamicFontProvider->LoadFont("LF3", this, 4), 2); });
}

// 异常测试
// 重写MakeFromStream函数返回nullptr，调用LoadFont函数
// 判定不抛出异常，并且返回值为2
TEST_F(DynamicFontProviderTest, LoadFont4)
{
    InitMyMockVars({.skTypeface_ = nullptr});
    EXPECT_NO_THROW({ EXPECT_EQ(dynamicFontProvider->LoadFont("LF4", this, 4), 2); });
}

// 逻辑测试
// 调用LoadFont函数
// 判定不抛出异常，并且返回值为0
TEST_F(DynamicFontProviderTest, LoadFont5)
{
    InitMyMockVars({});
    EXPECT_NO_THROW({
        auto ret = dynamicFontProvider->LoadFont("LF5", this, 4);
        EXPECT_EQ(ret, 0);
    });
}

// 过程测试
// 未调用LoadFont函数加载任何Family
// 调用MatchFamily函数查找名为“aaa”的Family
// 判定返回值为空指针
TEST_F(DynamicFontProviderTest, MatchFamily1)
{
    EXPECT_EQ(dynamicFontProvider->MatchFamily("aaa"), nullptr);
}

// 过程测试
// 调用LoadFont函数加载名为“aaa”的Family
// 调用MatchFamily函数查找名为“aaa”的Family
// 判定返回值为非空指针
TEST_F(DynamicFontProviderTest, MatchFamily2)
{
    InitMyMockVars({});
    dynamicFontProvider->LoadFont("aaa", this, 4);
    EXPECT_NE(dynamicFontProvider->MatchFamily("aaa"), nullptr);
}

// 过程测试
// 调用MatchFamily函数查找名为“bbb”的Family
// 判定返回值为空指针
TEST_F(DynamicFontProviderTest, MatchFamily3)
{
    InitMyMockVars({});
    dynamicFontProvider->LoadFont("aaa", this, 4);
    EXPECT_EQ(dynamicFontProvider->MatchFamily("bbb"), nullptr);
}
} // Texgine
