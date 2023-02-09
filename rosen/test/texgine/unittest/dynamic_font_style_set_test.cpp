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
#include <src/ports/SkFontMgr_custom.h>

#include <dynamic_font_style_set.h>

#include "param_test_macros.h"

struct Mockvars {
    sk_sp<SkTypeface> typeface_ = sk_make_sp<SkTypeface_Empty>();
    SkFontStyle fontStyle_ = {};
    std::shared_ptr<Texgine::DynamicFontStyleSet> normset_ = nullptr;
} mockvars;

auto Init(struct Mockvars vars)
{
    return [vars = std::move(vars)] {
        mockvars = std::move(vars);
        mockvars.typeface_ = SkTypeface::MakeDefault();
        auto typeface = std::make_unique<Texgine::Typeface>(nullptr);
        mockvars.normset_ = std::make_shared<Texgine::DynamicFontStyleSet>(std::move(typeface));
    };
}

sk_sp<SkTypeface> SkTypeface::MakeDefault()
{
    if (mockvars.typeface_) {
        mockvars.typeface_->fStyle = mockvars.fontStyle_;
    }
    return mockvars.typeface_;
}

namespace Texgine {
Typeface::Typeface(SkTypeface *tf)
{
    typeface_ = mockvars.typeface_;
}

#define PARAMCLASS DynamicFontStyleSet
class DynamicFontStyleSetTest : public testing::Test {
public:
};

#define PARAMFUNC count
TEST_F(DynamicFontStyleSetTest, count)
{
    DEFINE_TESTINFO0();

    DynamicFontStyleSet nullset(nullptr);
    RUN_TESTINFO0(nullset, {.checkFunc = CreateChecker(0)});

    DynamicFontStyleSet normset(std::make_unique<Typeface>(SkTypeface::MakeDefault()));
    RUN_TESTINFO0(normset, {.checkFunc = CreateChecker(1)});
}
#undef PARAMFUNC

#define PARAMFUNC getStyle
auto CreateStyleChecker(SkFontStyle s)
{
    return [s](int, SkFontStyle *style, SkString *, DynamicFontStyleSet &) {
        return s == *style;
    };
}

TEST_F(DynamicFontStyleSetTest, getStyle)
{
    DEFINE_VOID_TESTINFO3(int, SkFontStyle *, SkString *);

    SkFontStyle s1(0, 0, {});
    SkFontStyle s2(1, 1, {});
    DynamicFontStyleSet nullset(nullptr);

#define CORE_TEST(obj, mock, a1, expect)               \
    {                                                  \
        auto style = s1;                               \
        RUN_VOID_TESTINFO3(obj, {                      \
            .init = Init({.fontStyle_ = mock}),        \
            .arg1 = a1, .arg2 = &style,                \
            .checkFunc = CreateStyleChecker(expect)}); \
    }

    RUN_VOID_TESTINFO3(nullset, {.arg2 = nullptr, .exception = ExceptionType::InvalidArgument});

    CORE_TEST(nullset, s2, -1, s1);
    CORE_TEST(nullset, s2, 0, s1);
    CORE_TEST(nullset, s2, 1, s1);
    CORE_TEST(*mockvars.normset_, s2, -1, s1);
    CORE_TEST(*mockvars.normset_, s2, 0, s2);
    CORE_TEST(*mockvars.normset_, s2, 1, s1);
#undef CORE_TEST

    auto style = s1;
    auto initfunc = Init({.typeface_ = nullptr, .fontStyle_ = s2});
    auto checkfunc = CreateStyleChecker(s1);
    RUN_VOID_TESTINFO3(*mockvars.normset_, {.init = initfunc, .arg2 = &style, .checkFunc = checkfunc});
}
#undef PARAMFUNC

#define PARAMFUNC createTypeface
TEST_F(DynamicFontStyleSetTest, createTypeface)
{
    DEFINE_TESTINFO1(int);
    Init({})();
    DynamicFontStyleSet nullset(nullptr);
    DynamicFontStyleSet normset(std::make_unique<Typeface>(SkTypeface::MakeDefault()));
    RUN_TESTINFO1(nullset, {.arg1 = -1, .checkFunc = CreateChecker<SkTypeface *>(nullptr)});
    RUN_TESTINFO1(nullset, {.arg1 = 0, .checkFunc = CreateChecker<SkTypeface *>(nullptr)});
    RUN_TESTINFO1(nullset, {.arg1 = 1, .checkFunc = CreateChecker<SkTypeface *>(nullptr)});
    RUN_TESTINFO1(normset, {.arg1 = -1, .checkFunc = CreateChecker<SkTypeface *>(nullptr)});
    RUN_TESTINFO1(normset, {.arg1 = 0, .checkFunc = CreateChecker<SkTypeface *>(mockvars.typeface_.get())});
    RUN_TESTINFO1(normset, {.arg1 = 1, .checkFunc = CreateChecker<SkTypeface *>(nullptr)});
}
#undef PARAMFUNC

#define PARAMFUNC matchStyle
TEST_F(DynamicFontStyleSetTest, matchStyle)
{
    DEFINE_TESTINFO1(SkFontStyle);
    Init({})();
    DynamicFontStyleSet nullset(nullptr);
    DynamicFontStyleSet normset(std::make_unique<Typeface>(SkTypeface::MakeDefault()));
    RUN_TESTINFO1(nullset, {.checkFunc = CreateChecker<SkTypeface *>(nullptr)});
    RUN_TESTINFO1(normset, {.checkFunc = CreateChecker<SkTypeface *>(mockvars.typeface_.get())});
}
#undef PARAMFUNC
} // namespace Texgine
