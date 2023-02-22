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
#include "include/core/SkFontMgr.h"

#include "font_style_set.h"

namespace Texgine {
class FontStyleSetTest : public testing::Test {
};

// 过程测试
// 创建FontStyleSet对象，触发构造，析构
// 调用Get函数
// 判定不抛出异常，返回值为非空指针
TEST_F(FontStyleSetTest, FontStyleSet)
{
    EXPECT_NO_THROW({
        FontStyleSet fss(SkFontStyleSet::CreateEmpty());
        EXPECT_NE(fss.Get(), nullptr);
    });

}

// 过程测试
// 创建FontStyleSet对象，构造时传入空指针
// 判定不抛出异常
TEST_F(FontStyleSetTest, FontStyleSetDestruct)
{
    EXPECT_NO_THROW({
        FontStyleSet fss(nullptr);
        EXPECT_EQ(fss.Get(), nullptr);
    });
}
} // namespace Texgine
