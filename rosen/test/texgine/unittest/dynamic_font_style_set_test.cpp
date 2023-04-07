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

#include <dynamic_font_style_set.h>

#include "param_test_macros.h"

using namespace testing;
using namespace testing::ext;

namespace Texgine {
struct Mockvars {
    std::shared_ptr<TexgineTypeface> typeface_ = std::make_shared<TexgineTypeface>();
    std::shared_ptr<TexgineFontStyle> fontStyle_ = std::make_shared<TexgineFontStyle>();
    std::shared_ptr<Texgine::DynamicFontStyleSet> normset_ = nullptr;
} dfssMockvars;


auto Init(struct Mockvars vars)
{
    return [vars = std::move(vars)] {
        dfssMockvars = std::move(vars);
        auto typeface = std::make_unique<Typeface>(dfssMockvars.typeface_);
        dfssMockvars.normset_ = std::make_shared<DynamicFontStyleSet>(std::move(typeface));
    };
}

std::shared_ptr<TexgineFontStyle> TexgineTypeface::FontStyle()
{
    return dfssMockvars.fontStyle_;
}

Typeface::Typeface(std::shared_ptr<TexgineTypeface> tf)
{
    typeface_ = dfssMockvars.typeface_;
}

#define PARAMCLASS DynamicFontStyleSet
class DynamicFontStyleSetTest : public testing::Test {
public:
};

#define PARAMFUNC Count
HWTEST_F(DynamicFontStyleSetTest, Count, TestSize.Level1)
{
    DEFINE_TESTINFO0();

    DynamicFontStyleSet nullset(nullptr);
    RUN_TESTINFO0(nullset, {.checkFunc = CreateChecker(0)});

    DynamicFontStyleSet normset(std::make_unique<Typeface>(dfssMockvars.typeface_));
    RUN_TESTINFO0(normset, {.checkFunc = CreateChecker(1)});
}
#undef PARAMFUNC

#define PARAMFUNC GetStyle
auto CreateStyleChecker(std::shared_ptr<TexgineFontStyle> s)
{
    return [s] (int, std::shared_ptr<TexgineFontStyle> style, std::shared_ptr<TexgineString>, DynamicFontStyleSet &) {
        return *s->GetFontStyle() == *style->GetFontStyle();
    };
}

HWTEST_F(DynamicFontStyleSetTest, GetStyle, TestSize.Level1)
{
    DEFINE_VOID_TESTINFO3(int, std::shared_ptr<TexgineFontStyle>, std::shared_ptr<TexgineString>);

    TexgineFontStyle::Slant slant {};
    auto s1 = std::make_shared<TexgineFontStyle>(0, 0, slant);
    auto s2 = std::make_shared<TexgineFontStyle>(100, 100, slant);
    DynamicFontStyleSet nullset(nullptr);

#define CORE_TEST(obj, mock, a1, expect)               \
    {                                                  \
        std::shared_ptr<TexgineFontStyle> style = s1;  \
        RUN_VOID_TESTINFO3(obj, {                      \
            .init = Init({.fontStyle_ = mock}),        \
            .arg1 = a1, .arg2 = style,                 \
            .checkFunc = CreateStyleChecker(expect)}); \
    }

    RUN_VOID_TESTINFO3(nullset, {.arg2 = nullptr, .exception = ExceptionType::InvalidArgument});

    CORE_TEST(nullset, s2, -1, s1);
    CORE_TEST(nullset, s2, 0, s1);
    CORE_TEST(nullset, s2, 1, s1);
    CORE_TEST(*dfssMockvars.normset_, s2, -1, s1);
    CORE_TEST(*dfssMockvars.normset_, s2, 0, s2);
    CORE_TEST(*dfssMockvars.normset_, s2, 1, s1);
#undef CORE_TEST

    auto style = s1;
    auto initfunc = Init({.typeface_ = nullptr, .fontStyle_ = s2});
    auto checkfunc = CreateStyleChecker(s1);
    RUN_VOID_TESTINFO3(*dfssMockvars.normset_, {.init = initfunc, .arg2 = style, .checkFunc = checkfunc});
}
#undef PARAMFUNC

#define PARAMFUNC CreateTypeface
HWTEST_F(DynamicFontStyleSetTest, CreateTypeface, TestSize.Level1)
{
    DEFINE_TESTINFO1(int);
    Init({})();
    DynamicFontStyleSet nullset(nullptr);
    DynamicFontStyleSet normset(std::make_unique<Typeface>(dfssMockvars.typeface_));
    RUN_TESTINFO1(nullset, {.arg1 = -1, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(nullset, {.arg1 = 0, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(nullset, {.arg1 = 1, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(normset, {.arg1 = -1, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(normset, {.arg1 = 0,
        .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(dfssMockvars.typeface_)});
    RUN_TESTINFO1(normset, {.arg1 = 1, .checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
}
#undef PARAMFUNC

#define PARAMFUNC MatchStyle
HWTEST_F(DynamicFontStyleSetTest, MatchStyle, TestSize.Level1)
{
    DEFINE_TESTINFO1(std::shared_ptr<TexgineFontStyle>);
    Init({})();
    DynamicFontStyleSet nullset(nullptr);
    DynamicFontStyleSet normset(std::make_unique<Typeface>(dfssMockvars.typeface_));
    RUN_TESTINFO1(nullset, {.checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(nullptr)});
    RUN_TESTINFO1(normset, {.checkFunc = CreateChecker<std::shared_ptr<TexgineTypeface>>(dfssMockvars.typeface_)});
}
#undef PARAMFUNC
} // namespace Texgine
