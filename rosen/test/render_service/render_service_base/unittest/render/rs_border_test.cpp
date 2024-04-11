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

#include "gtest/gtest.h"
#include "include/render/rs_border.h"

#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBorderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBorderTest::SetUpTestCase() {}
void RSBorderTest::TearDownTestCase() {}
void RSBorderTest::SetUp() {}
void RSBorderTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    RSColor color(1, 1, 1);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    ASSERT_TRUE(border->GetColor(0) == RgbPalette::Transparent());
    ASSERT_TRUE(border->GetStyle(0) == BorderStyle::NONE);
    border->SetColor(color);
    border->GetColor(0);
    RSColor color2(1, 1, 1);
    border->SetColor(color2);
    border->GetColor(1);
    border->SetWidth(1.f);
    border->SetWidth(2.f);
    ASSERT_TRUE(border->GetWidth(1) == 2.f);
    border->SetStyle(BorderStyle::DOTTED);
    border->SetStyle(BorderStyle::DOTTED);
    ASSERT_TRUE(border->GetStyle(1) == BorderStyle::DOTTED);
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Color color1(1, 1, 1);
    Color color2(2, 2, 2);
    Color color3(3, 3, 3);
    Color color4(4, 4, 4);
    Vector4<Color> vectorColor(color1, color2, color3, color4);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->GetColorFour();
    border->SetColorFour(vectorColor);
    border->GetColorFour();
    Vector4f width(1, 1, 1, 1);
    border->SetWidthFour(width);
    Vector4f width2(1, 2, 3, 4);
    border->SetWidthFour(width2);
    border->GetWidthFour();
    Vector4<uint32_t> vectorStyle(1, 2, 3, 4);
    border->GetStyleFour();
    border->SetStyleFour(vectorStyle);
    border->GetStyleFour();
    border->SetStyle(BorderStyle::DOTTED);
    border->ToString();
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, LifeCycle003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Color color1(1, 1, 1);
    Color color2(1, 1, 1);
    Color color3(1, 1, 1);
    Color color4(1, 1, 1);
    Vector4<Color> vectorColor(color1, color2, color3, color4);
    RSProperties properties;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    properties.SetBorderColor(vectorColor);
    Vector4<uint32_t> style(0, 0, 0, 0);
    properties.SetBorderStyle(style);
    Vector4f cornerRadius(0.f, 0.f, 0.f, 0.f);
    properties.SetCornerRadius(cornerRadius);
    Vector4f width(1.f, 1.f, 1.f, 1.f);
    properties.SetBorderWidth(width);
    ASSERT_TRUE(properties.GetCornerRadius().IsZero());
    Drawing::Canvas canvas;
    RSPropertiesPainter::DrawBorder(properties, canvas);
    Vector4f cornerRadius2(1.f, 2.f, 0.f, 0.f);
    properties.SetCornerRadius(cornerRadius2);
    RSPropertiesPainter::DrawBorder(properties, canvas);
    Vector4<uint32_t> style2(1, 1, 1, 1);
    properties.SetBorderStyle(style2);
    RSPropertiesPainter::DrawBorder(properties, canvas);
    Color color5(2, 1, 1);
    Vector4<Color> Color(color1, color2, color3, color5);
    properties.SetBorderColor(Color);
    RSPropertiesPainter::DrawBorder(properties, canvas);
}

/**
 * @tc.name: SetColorTest001
 * @tc.desc: Verify function SetColor
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, SetColorTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    border->SetColor(Color(0, 0, 0));
    EXPECT_EQ(border->GetColor(0), Color(0, 0, 0));
}

/**
 * @tc.name: SetWidthTest001
 * @tc.desc: Verify function SetWidth
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, SetWidthTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    border->SetWidth(1.0f);
    EXPECT_EQ(border->GetWidth(0), 1.0f);
}

/**
 * @tc.name: SetStyleTest001
 * @tc.desc: Verify function SetStyle
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, SetStyleTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    border->SetStyle(BorderStyle::SOLID);
    EXPECT_TRUE(border->GetStyle(0) == BorderStyle::SOLID);
}

/**
 * @tc.name: GetColorTest001
 * @tc.desc: Verify function GetColor
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetColorTest001, TestSize.Level1)
{
    Color color1(1, 1, 1);
    Color color2(2, 2, 2);
    Color color3(3, 3, 3);
    Color color4(4, 4, 4);
    auto border = std::make_shared<RSBorder>();
    RSColor val { 0, 0, 0, 0 };
    EXPECT_TRUE(border->GetColor(0) == val);

    border->SetColor(color1);
    EXPECT_TRUE(border->GetColor(0) == color1);

    Vector4<Color> vectorColor(color1, color2, color3, color4);
    border->SetColorFour(vectorColor);
    EXPECT_TRUE(border->GetColor(1) == color2);
}

/**
 * @tc.name: GetWidthTest001
 * @tc.desc: Verify function GetWidth
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetWidthTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    EXPECT_EQ(border->GetWidth(0), 0.f);

    border->SetWidth(1.f);
    EXPECT_EQ(border->GetWidth(0), 1.f);

    Vector4f width3(1.f, 2.f, 3.f, 4.f);
    border->SetWidthFour(width3);
    EXPECT_EQ(border->GetWidth(1), 2.f);
}

/**
 * @tc.name: GetStyleTest001
 * @tc.desc: Verify function GetStyle
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetStyleTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    EXPECT_TRUE(border->GetStyle(0) == BorderStyle::NONE);

    border->SetStyle(BorderStyle::SOLID);
    EXPECT_TRUE(border->GetStyle(0) == BorderStyle::SOLID);

    Vector4<uint32_t> vectorStyle(0, 1, 2, 3);
    border->SetStyleFour(vectorStyle);
    EXPECT_TRUE(border->GetStyle(0) == BorderStyle::SOLID);
}

/**
 * @tc.name: SetColorFourTest001
 * @tc.desc: Verify function SetColorFour
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, SetColorFourTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Color color1(1, 1, 1);
    Color color2(2, 2, 2);
    Color color3(3, 3, 3);
    Color color4(4, 4, 4);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    EXPECT_TRUE(border->GetColorFour() == vectorColor1);

    Color color11(1, 1, 1);
    Color color22(1, 1, 1);
    Color color33(1, 1, 1);
    Color color44(1, 1, 1);
    Vector4<Color> vectorColor2(color11, color22, color33, color44);
    border->SetColorFour(vectorColor2);
    EXPECT_TRUE(border->GetColor() == color11);
}

/**
 * @tc.name: SetWidthFourTest001
 * @tc.desc: Verify function SetWidthFour
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, SetWidthFourTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Vector4f width(1.f, 2.f, 3.f, 4.f);
    border->SetWidthFour(width);
    EXPECT_EQ(border->GetWidthFour(), width);

    Vector4f width2(1.f, 1.f, 1.f, 1.f);
    border->SetWidthFour(width2);
    EXPECT_EQ(border->GetWidthFour(), 1.f);
}

/**
 * @tc.name: SetStyleFourTest001
 * @tc.desc: Verify function SetStyleFour
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, SetStyleFourTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Vector4<uint32_t> vectorStyle(0, 1, 2, 3);
    border->SetStyleFour(vectorStyle);
    EXPECT_TRUE(border->GetStyleFour() == vectorStyle);

    Vector4<uint32_t> vectorStyle1(0, 0, 0, 0);
    border->SetStyleFour(vectorStyle1);
    EXPECT_TRUE(border->GetStyle() == BorderStyle::SOLID);
}

/**
 * @tc.name: SetRadiusFourTest001
 * @tc.desc: Verify function SetRadiusFour
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, SetRadiusFourTest001, TestSize.Level1)
{
    Vector4f radius(1.f, 2.f, 3.f, 4.f);
    auto border = std::make_shared<RSBorder>();
    border->SetRadiusFour(radius);
    EXPECT_TRUE(border->GetRadiusFour() == radius);
}

/**
 * @tc.name: GetColorFourTest001
 * @tc.desc: Verify function GetColorFour
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetColorFourTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Color color1(1, 1, 1);
    Color color2(2, 2, 2);
    Color color3(3, 3, 3);
    Color color4(4, 4, 4);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    EXPECT_TRUE(border->GetColorFour() == vectorColor1);

    Color color11(1, 1, 1);
    Color color22(1, 1, 1);
    Color color33(1, 1, 1);
    Color color44(1, 1, 1);
    Vector4<Color> vectorColor2(color11, color22, color33, color44);
    border->SetColorFour(vectorColor2);
    EXPECT_TRUE(border->GetColor() == color11);
}

/**
 * @tc.name: GetWidthFourTest001
 * @tc.desc: Verify function GetWidthFour
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetWidthFourTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Vector4f width(1.f, 2.f, 3.f, 4.f);
    border->SetWidthFour(width);
    EXPECT_EQ(border->GetWidthFour(), width);

    Vector4f width2(1.f, 1.f, 1.f, 1.f);
    border->SetWidthFour(width2);
    EXPECT_EQ(border->GetWidthFour(), 1.f);
}

/**
 * @tc.name: GetStyleFourTest001
 * @tc.desc: Verify function GetStyleFour
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetStyleFourTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Vector4<uint32_t> vectorStyle(0, 1, 2, 3);
    border->SetStyleFour(vectorStyle);
    EXPECT_TRUE(border->GetStyleFour() == vectorStyle);

    Vector4<uint32_t> vectorStyle1(0, 0, 0, 0);
    border->SetStyleFour(vectorStyle1);
    EXPECT_TRUE(border->GetStyle() == BorderStyle::SOLID);
}

/**
 * @tc.name: GetRadiusFourTest001
 * @tc.desc: Verify function GetRadiusFour
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetRadiusFourTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    auto radius = border->GetRadiusFour();
    EXPECT_TRUE(radius[0] == 0.0f);
}

/**
 * @tc.name: ApplyFillStyleTest001
 * @tc.desc: Verify function ApplyFillStyle
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, ApplyFillStyleTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Brush brush;
    EXPECT_FALSE(border->ApplyFillStyle(brush));
    border->SetColor(Color(0, 0, 0));
    border->SetStyle(BorderStyle::NONE);
    EXPECT_FALSE(border->ApplyFillStyle(brush));
    Vector4<uint32_t> vectorStyle(0, 1, 2, 3);
    border->SetStyleFour(vectorStyle);
    EXPECT_FALSE(border->ApplyFillStyle(brush));
    border->SetColor(Color(0, 0, 0));
    border->SetStyle(BorderStyle::SOLID);
    border->SetWidth(0.f);
    EXPECT_FALSE(border->ApplyFillStyle(brush));
    border->SetWidth(1.f);
    EXPECT_TRUE(border->ApplyFillStyle(brush));
}

/**
 * @tc.name: ApplyPathStyleTest001
 * @tc.desc: Verify function ApplyPathStyle
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, ApplyPathStyleTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Pen pen;
    EXPECT_FALSE(border->ApplyPathStyle(pen));
    border->SetColor(Color(0, 0, 0));
    EXPECT_FALSE(border->ApplyPathStyle(pen));
    border->SetWidth(0.f);
    EXPECT_FALSE(border->ApplyPathStyle(pen));
    border->SetStyle(BorderStyle::SOLID);
    EXPECT_TRUE(border->ApplyPathStyle(pen));
}

/**
 * @tc.name: ApplyFourLineTest001
 * @tc.desc: Verify function ApplyFourLine
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, ApplyFourLineTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Pen pen;
    EXPECT_FALSE(border->ApplyFourLine(pen));
    border->SetColor(Color(0, 0, 0));
    EXPECT_FALSE(border->ApplyFourLine(pen));
    border->SetStyle(BorderStyle::SOLID);
    EXPECT_TRUE(border->ApplyFourLine(pen));
}

/**
 * @tc.name: ApplyLineStyleTest001
 * @tc.desc: Verify function ApplyLineStyle
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, ApplyLineStyleTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Pen pen;
    EXPECT_FALSE(border->ApplyLineStyle(pen, 0, 1.f));
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::DOTTED);
    EXPECT_FALSE(border->ApplyLineStyle(pen, 0, 1.f));
    border->SetStyle(BorderStyle::SOLID);
    EXPECT_TRUE(border->ApplyLineStyle(pen, 0, 1.f));
}

/**
 * @tc.name: PaintFourLineTest001
 * @tc.desc: Verify function PaintFourLine
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, PaintFourLineTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    RectF rect;
    border->PaintFourLine(canvas, pen, rect);
    EXPECT_TRUE(border->colors_.empty());
}

/**
 * @tc.name: PaintTopPathTest001
 * @tc.desc: Verify function PaintTopPath
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, PaintTopPathTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::SOLID);
    border->PaintTopPath(canvas, pen, rrect, innerRectCenter);
    border->SetStyle(BorderStyle::NONE);
    border->PaintTopPath(canvas, pen, rrect, innerRectCenter);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: PaintRightPathTest001
 * @tc.desc: Verify function PaintRightPath
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, PaintRightPathTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::SOLID);
    border->PaintRightPath(canvas, pen, rrect, innerRectCenter);
    border->SetStyle(BorderStyle::NONE);
    border->PaintRightPath(canvas, pen, rrect, innerRectCenter);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: PaintBottomPathTest001
 * @tc.desc: Verify function PaintBottomPath
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, PaintBottomPathTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::SOLID);
    border->PaintBottomPath(canvas, pen, rrect, innerRectCenter);
    border->SetStyle(BorderStyle::NONE);
    border->PaintBottomPath(canvas, pen, rrect, innerRectCenter);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: PaintLeftPathTest001
 * @tc.desc: Verify function PaintLeftPath
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, PaintLeftPathTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::SOLID);
    border->PaintLeftPath(canvas, pen, rrect, innerRectCenter);
    border->SetStyle(BorderStyle::NONE);
    border->PaintLeftPath(canvas, pen, rrect, innerRectCenter);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: ToStringTest001
 * @tc.desc: Verify function ToString
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, ToStringTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    EXPECT_TRUE(border->ToString().empty());
    border->SetColor(Color(1, 1, 1));
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::NONE);
    EXPECT_FALSE(border->ToString().empty());
}

/**
 * @tc.name: HasBorderTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, HasBorderTest, TestSize.Level1)
{
    RSColor color(1, 1, 1);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->SetColor(color);
    border->GetColor(1);
    border->SetStyle(BorderStyle::DOTTED);
    border->SetWidth(2.f);
    bool hasBorder = border->HasBorder();
    ASSERT_TRUE(hasBorder);
}
} // namespace OHOS::Rosen
