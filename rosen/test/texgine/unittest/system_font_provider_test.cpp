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
#include "include/core/SkTypeface.h"

#include "font_style_set.h"
#include "mock/mock_sk_font_mgr.h"
#include "param_test_macros.h"
#include "texgine/system_font_provider.h"

struct MockVars {
    sk_sp<SkFontStyleSet> skFontStyleSet_{SkFontStyleSet::CreateEmpty()};
    sk_sp<SkFontMgr> skFontMgr_ = sk_make_sp<Texgine::MockSkFontMgr>();
} mockVars;

void InitMockVars(MockVars vars)
{
    mockVars = std::move(vars);
}

sk_sp<SkFontMgr> SkFontMgr::RefDefault()
{
    return mockVars.skFontMgr_;
}

SkFontStyleSet *SkFontMgr::matchFamily(const char familyName[]) const
{
    return mockVars.skFontStyleSet_.release();
}

namespace Texgine {
class SystemFontProviderTest : public testing::Test {
public:
    std::shared_ptr<SystemFontProvider> systemFontProvider = SystemFontProvider::GetInstance();
};

// 过程测试
// 调用GetInstance函数
// 判定返回值为非空指针
TEST_F(SystemFontProviderTest, GetInstance)
{
    EXPECT_NE(SystemFontProvider::GetInstance(), nullptr);
}

// 异常测试
// 控制RefDefault函数返回空指针
// 调用MatchFamily函数
// 判定返回空指针
TEST_F(SystemFontProviderTest, MatchFamily1)
{
    InitMockVars({.skFontMgr_ = nullptr});
    auto fss = systemFontProvider->MatchFamily("");
    EXPECT_EQ(fss, nullptr);
}

// 异常测试
// 控制matchFamily函数返回空指针
// 调用MatchFamily函数
// 调用Get方法
// 判定返回空指针
TEST_F(SystemFontProviderTest, MatchFamily2)
{
    InitMockVars({.skFontStyleSet_ = nullptr});
    auto fss = systemFontProvider->MatchFamily("");
    EXPECT_EQ(fss->Get(), nullptr);
}

// 逻辑测试
// 调用MatchFamily函数
// 判定返回非空指针
// 调用Get方法
// 判定返回非空指针
TEST_F(SystemFontProviderTest, MatchFamily3)
{
    InitMockVars({});
    auto fontStyleSet = systemFontProvider->MatchFamily("");
    EXPECT_NE(fontStyleSet, nullptr);
    EXPECT_NE(fontStyleSet->Get(), nullptr);
}
} // namespace Texgine
