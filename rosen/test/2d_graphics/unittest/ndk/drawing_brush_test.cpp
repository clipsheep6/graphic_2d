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

#include "c/drawing_brush.h"
#include "c/drawing_color.h"
#include "c/drawing_color_space.h"
#include "c/drawing_color_filter.h"
#include "c/drawing_filter.h"
#include "c/drawing_mask_filter.h"
#include "c/drawing_rect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingBrushTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingBrushTest::SetUpTestCase() {}
void NativeDrawingBrushTest::TearDownTestCase() {}
void NativeDrawingBrushTest::SetUp() {}
void NativeDrawingBrushTest::TearDown() {}

/*
 * @tc.name: NativeDrawingBrushTest_brush001
 * @tc.desc: test for create brush and destroy brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brush001, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_EQ(brush == nullptr, false);
    OH_Drawing_BrushDestroy(brush);
}

/*
 * @tc.name: NativeDrawingBrushTest_brush002
 * @tc.desc: test for the set methods of brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brush002, TestSize.Level1)
{
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush1, false);
    EXPECT_EQ(OH_Drawing_BrushIsAntiAlias(brush1), false);
    OH_Drawing_BrushSetColor(brush1, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush1), 0xFFFF0000);
    constexpr uint8_t alpha = 128;
    OH_Drawing_BrushSetAlpha(brush1, alpha);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(brush1), alpha);
}

/*
 * @tc.name: NativeDrawingBrushTest_SetBlendMode001
 * @tc.desc: test for SetBlendMode.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_SetBlendMode001, TestSize.Level1)
{
    OH_Drawing_BrushSetBlendMode(nullptr, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    OH_Drawing_BrushSetBlendMode(brush, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
}

/*
 * @tc.name: NativeDrawingBrushTest_brushReset
 * @tc.desc: test for the reset methods of brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushReset, TestSize.Level1)
{
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush1, false);
    OH_Drawing_BrushSetColor(brush1, OH_Drawing_ColorSetArgb(0x00, 0xFF, 0x00, 0xFF));
    constexpr uint8_t alpha = 128;
    OH_Drawing_BrushSetAlpha(brush1, alpha);

    OH_Drawing_BrushReset(brush1);
    EXPECT_EQ(OH_Drawing_BrushIsAntiAlias(brush1), false);
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush1), 0xFF000000);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(brush1), 0xFF);

    OH_Drawing_BrushDestroy(brush1);
}

/*
 * @tc.name: NativeDrawingBrushTest_brushSetColor4f
 * @tc.desc: test for the SetColor4f methods of brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushSetColor4f, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSRGB();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_Color4f color4f = {1, 0, 1, 0.5};
    OH_Drawing_BrushSetColor4f(brush, &color4f, colorSpace);

    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

/*
 * @tc.name: NativeDrawingBrushTest_GetFilter009
 * @tc.desc: Gets the filter from a brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_GetFilter009, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    OH_Drawing_Filter *cFilter_ = OH_Drawing_FilterCreate();
    OH_Drawing_Filter *tmpFilter_ = OH_Drawing_FilterCreate();

    OH_Drawing_ColorFilter *cColorFilter_ = OH_Drawing_ColorFilterCreateBlendMode(5, BLEND_MODE_COLOR);
    OH_Drawing_FilterSetColorFilter(cFilter_, cColorFilter_);
    OH_Drawing_BrushSetFilter(brush, cFilter_);
    OH_Drawing_BrushGetFilter(brush, tmpFilter_);
    OH_Drawing_FilterDestroy(cFilter_);
    OH_Drawing_FilterDestroy(tmpFilter_);
    OH_Drawing_ColorFilterDestroy(cColorFilter_);
    OH_Drawing_BrushDestroy(brush);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS