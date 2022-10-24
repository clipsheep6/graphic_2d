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

#include "mock/mock_any_span.h"
#include "param_test_macros.h"
#include "texgine/typography_types.h"
#include "variant_span.h"

class SkCanvas {};

namespace Texgine {
struct Mockvars {
    double tsWidth_ = 0;
    double tsHeight_ = 0;
    int calledTimesPaint_ = 0;
    int calledTimesPaintShadow_ = 0;
    SkCanvas *catchedPaintCanvas_ = nullptr;
    double catchedPaintOffsetX_ = 0;
    double catchedPaintOffsetY_ = 0;
    SkCanvas *catchedPaintShadowCanvas_ = nullptr;
    double catchedPaintShadowOffsetX_ = 0;
    double catchedPaintShadowOffsetY_ = 0;
} mockvars;

void InitMockvars(struct Mockvars &&vars)
{
    mockvars = std::move(vars);
}

double TextSpan::GetWidth() const
{
    return mockvars.tsWidth_;
}

double TextSpan::GetHeight() const
{
    return mockvars.tsHeight_;
}

void TextSpan::Paint(SkCanvas &canvas, double offsetx, double offsety, const TextStyle &xs)
{
    mockvars.calledTimesPaint_++;
    mockvars.catchedPaintCanvas_ = &canvas;
    mockvars.catchedPaintOffsetX_ = offsetx;
    mockvars.catchedPaintOffsetY_ = offsety;
}

void TextSpan::PaintShadow(SkCanvas &canvas, double offsetx, double offsety, const std::vector<TextShadow> &shadows)
{
    mockvars.calledTimesPaintShadow_++;
    mockvars.catchedPaintShadowCanvas_ = &canvas;
    mockvars.catchedPaintShadowOffsetX_ = offsetx;
    mockvars.catchedPaintShadowOffsetY_ = offsety;
}

class VariantSpanTest : public testing::Test {
public:
    VariantSpanTest()
    {
        struct AttackStruct {
            std::shared_ptr<TextSpan> ts_ = nullptr;
            std::shared_ptr<AnySpan> as_ = nullptr;
        } *p = reinterpret_cast<struct AttackStruct *>(&span0);
        p->as_ = as;
        p->ts_ = ts;
    }

    std::shared_ptr<TextSpan> tsNullptr = nullptr;
    std::shared_ptr<AnySpan> asNullptr = nullptr;
    std::shared_ptr<TextSpan> ts = TextSpan::MakeFromText("");
    std::shared_ptr<MockAnySpan> as = std::make_shared<MockAnySpan>();
    VariantSpan span0;
    VariantSpan span1 = nullptr;
    VariantSpan span2 = tsNullptr;
    VariantSpan span3 = asNullptr;
    VariantSpan span4 = ts;
    VariantSpan span5 = as;
};

TEST_F(VariantSpanTest, TryToTextAndAnySpan)
{
    ASSERT_EQ(span1.TryToTextSpan(), nullptr);
    ASSERT_EQ(span2.TryToTextSpan(), nullptr);
    ASSERT_EQ(span3.TryToTextSpan(), nullptr);
    ASSERT_EQ(span4.TryToTextSpan(), ts);
    ASSERT_EQ(span5.TryToTextSpan(), nullptr);

    ASSERT_EQ(span1.TryToAnySpan(), nullptr);
    ASSERT_EQ(span2.TryToAnySpan(), nullptr);
    ASSERT_EQ(span3.TryToAnySpan(), nullptr);
    ASSERT_EQ(span4.TryToAnySpan(), nullptr);
    ASSERT_EQ(span5.TryToAnySpan(), as);
}

TEST_F(VariantSpanTest, GetWidthAndHeight)
{
    InitMockvars({.tsWidth_ = 8, .tsHeight_ = 4});
    EXPECT_CALL(*as, GetWidth).Times(1).WillOnce(testing::Return(2));
    EXPECT_CALL(*as, GetHeight).Times(1).WillOnce(testing::Return(1));

    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, span0.GetWidth());
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span1.GetWidth());
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span2.GetWidth());
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span3.GetWidth());
    ASSERT_EQ(span4.GetWidth(), 8);
    ASSERT_EQ(span5.GetWidth(), 2);

    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, span0.GetHeight());
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span1.GetHeight());
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span2.GetHeight());
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span3.GetHeight());
    ASSERT_EQ(span4.GetHeight(), 4);
    ASSERT_EQ(span5.GetHeight(), 1);
}

TEST_F(VariantSpanTest, PaintAndPaintShadow)
{
    SkCanvas canvas, canvas1, canvas2, canvas3, canvas4;
    InitMockvars({});
    SkCanvas *pcanvas = nullptr;
    double asX = 0;
    double asY = 0;
    EXPECT_CALL(*as, Paint).Times(1).WillOnce([&](SkCanvas &canvas, double offsetx, double offsety) {
        pcanvas = &canvas;
        asX = offsetx;
        asY = offsety;
    });

    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, span0.Paint(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span1.Paint(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span2.Paint(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span3.Paint(canvas, 0, 0));
    EXPECT_NO_THROW({
        ASSERT_EQ(mockvars.calledTimesPaint_, 0);
        span4.Paint(canvas2, 2048, 4096);
        ASSERT_EQ(mockvars.calledTimesPaint_, 1);
        ASSERT_EQ(mockvars.catchedPaintCanvas_, &canvas2);
        ASSERT_EQ(mockvars.catchedPaintOffsetX_, 2048);
        ASSERT_EQ(mockvars.catchedPaintOffsetY_, 4096);
    });
    EXPECT_NO_THROW({
        span5.Paint(canvas1, 512, 1024);
        ASSERT_EQ(pcanvas, &canvas1);
        ASSERT_EQ(asX, 512);
        ASSERT_EQ(asY, 1024);
    });

    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, span0.PaintShadow(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span1.PaintShadow(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span2.PaintShadow(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::Nullptr, span3.PaintShadow(canvas, 0, 0));
    EXPECT_NO_THROW({
        ASSERT_EQ(mockvars.calledTimesPaintShadow_, 0);
        span4.PaintShadow(canvas4, 32768, 65536);
        ASSERT_EQ(mockvars.calledTimesPaintShadow_, 1);
        ASSERT_EQ(mockvars.catchedPaintShadowCanvas_, &canvas4);
        ASSERT_EQ(mockvars.catchedPaintShadowOffsetX_, 32768);
        ASSERT_EQ(mockvars.catchedPaintShadowOffsetY_, 65536);
    });
    EXPECT_NO_THROW({span5.PaintShadow(canvas3, 8192, 16384);});
}

TEST_F(VariantSpanTest, Operator)
{
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, (void)(bool)span0);
    ASSERT_FALSE(span1);
    ASSERT_FALSE(span2);
    ASSERT_FALSE(span3);
    ASSERT_TRUE(span4);
    ASSERT_TRUE(span5);

    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, (void)(span0 == nullptr));
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, (void)(span0 != nullptr));
    ASSERT_EQ(span1, nullptr);
    ASSERT_EQ(span2, nullptr);
    ASSERT_EQ(span3, nullptr);
    ASSERT_NE(span4, nullptr);
    ASSERT_NE(span5, nullptr);

    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, (void)(span0 == span1));
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, (void)(span0 != span1));
    ASSERT_EQ(span1, span1);
    ASSERT_EQ(span1, span2);
    ASSERT_EQ(span1, span3);
    ASSERT_NE(span1, span4);
    ASSERT_NE(span1, span5);

    ASSERT_NE(span4, span5);

    auto span6 = span4;
    ASSERT_EQ(span6, span4);

    auto span7 = span5;
    ASSERT_EQ(span7, span5);
}
} // namespace Texgine
