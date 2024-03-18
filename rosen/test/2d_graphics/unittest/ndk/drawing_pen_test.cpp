/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "c/drawing_color.h"
#include "c/drawing_color_space.h"
#include "c/drawing_color_filter.h"
#include "c/drawing_filter.h"
#include "c/drawing_pen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPenTest::SetUpTestCase() {}
void NativeDrawingPenTest::TearDownTestCase() {}
void NativeDrawingPenTest::SetUp() {}
void NativeDrawingPenTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPenTest_pen001
 * @tc.desc: test for create drawing_pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen001, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_EQ(pen == nullptr, false);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_pen002
 * @tc.desc: test for the get and set methods about AntiAlias for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen002, TestSize.Level1)
{
    OH_Drawing_Pen* pen1 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen1, true);
    EXPECT_EQ(OH_Drawing_PenIsAntiAlias(pen1), true);
    OH_Drawing_PenSetAntiAlias(pen1, false);
    EXPECT_EQ(OH_Drawing_PenIsAntiAlias(pen1), false);
    OH_Drawing_PenDestroy(pen1);
}

/*
 * @tc.name: NativeDrawingPenTest_pen003
 * @tc.desc: test for the get and set methods about the color for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen003, TestSize.Level1)
{
    OH_Drawing_Pen* pen2 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen2, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_PenGetColor(pen2), 0xFFFF0000);
    OH_Drawing_PenDestroy(pen2);
}

/*
 * @tc.name: NativeDrawingPenTest_pen004
 * @tc.desc: test for the get and set methods about the width for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen004, TestSize.Level1)
{
    OH_Drawing_Pen* pen3 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen3, 10);
    EXPECT_EQ(OH_Drawing_PenGetWidth(pen3), 10);
    OH_Drawing_PenDestroy(pen3);
}

/*
 * @tc.name: NativeDrawingPenTest_pen005
 * @tc.desc: test for the get and set methods about the miterLimit for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen005, TestSize.Level1)
{
    OH_Drawing_Pen* pen4 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetMiterLimit(pen4, 5);
    EXPECT_EQ(OH_Drawing_PenGetMiterLimit(pen4), 5);
    OH_Drawing_PenDestroy(pen4);
}

/*
 * @tc.name: NativeDrawingPenTest_pen006
 * @tc.desc: test for the get and set methods about the line cap style for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen006, TestSize.Level1)
{
    OH_Drawing_Pen* pen5 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP);
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP);
    OH_Drawing_PenDestroy(pen5);
}

/*
 * @tc.name: NativeDrawingPenTest_pen007
 * @tc.desc: test for the get and set methods about the line join style for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen007, TestSize.Level1)
{
    OH_Drawing_Pen* pen6 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);
    OH_Drawing_PenDestroy(pen6);
}

/*
 * @tc.name: NativeDrawingPenTest_SetBlendMode008
 * @tc.desc: test for the get and set methods about the line join style for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_SetBlendMode008, TestSize.Level1)
{
    OH_Drawing_Pen* pen8 = OH_Drawing_PenCreate();
    EXPECT_NE(pen8, nullptr);
    OH_Drawing_PenSetBlendMode(pen8, OH_Drawing_BlendMode::BLEND_MODE_SRC);
    OH_Drawing_PenSetBlendMode(nullptr, OH_Drawing_BlendMode::BLEND_MODE_SRC);
    OH_Drawing_PenDestroy(pen8);
}

/*
 * @tc.name: NativeDrawingPenTest_penReset
 * @tc.desc: test for the reset method for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penReset, TestSize.Level1)
{
    OH_Drawing_Pen* pen9 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen9, true);
    OH_Drawing_PenSetColor(pen9, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetWidth(pen9, 10);
    OH_Drawing_PenSetMiterLimit(pen9, 5);
    OH_Drawing_PenSetCap(pen9, OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP);
    OH_Drawing_PenSetJoin(pen9, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);

    OH_Drawing_PenReset(pen9);
    EXPECT_EQ(OH_Drawing_PenIsAntiAlias(pen9), false);
    EXPECT_EQ(OH_Drawing_PenGetColor(pen9), 0xFF000000);
    EXPECT_EQ(OH_Drawing_PenGetWidth(pen9), 0);
    EXPECT_EQ(OH_Drawing_PenGetMiterLimit(pen9), -1);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen9), OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen9), OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);

    OH_Drawing_PenDestroy(pen9);
}

/*
 * @tc.name: NativeDrawingPenTest_penSetColor4f
 * @tc.desc: test for the SetColor4f methods of pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penSetColor4f, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSRGB();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_Color4f color4f = {1, 0, 1, 0.5};
    OH_Drawing_PenSetColor4f(pen, &color4f, colorSpace);

    OH_Drawing_PenDestroy(pen);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

/*
 * @tc.name: NativeDrawingPenTest_GetFilter009
 * @tc.desc: Gets the filter from a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_GetFilter009, TestSize.Level1)
{
    OH_Drawing_Pen* pen9 = OH_Drawing_PenCreate();
    EXPECT_NE(pen9, nullptr);
    OH_Drawing_Filter *cFilter_ = OH_Drawing_FilterCreate();
    OH_Drawing_Filter *tmpFilter_ = OH_Drawing_FilterCreate();

    OH_Drawing_ColorFilter *cColorFilter_ = OH_Drawing_ColorFilterCreateBlendMode(5, BLEND_MODE_COLOR);
    OH_Drawing_FilterSetColorFilter(cFilter_, cColorFilter_);
    OH_Drawing_PenSetFilter(pen9, cFilter_);
    OH_Drawing_PenGetFilter(pen9, tmpFilter_);
    OH_Drawing_FilterDestroy(cFilter_);
    OH_Drawing_FilterDestroy(tmpFilter_);
    OH_Drawing_ColorFilterDestroy(cColorFilter_);
    OH_Drawing_PenDestroy(pen9);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS