/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawing_error_code.h"
#include "drawing_rect.h"
#include "drawing_region.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingRegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingRegionTest::SetUpTestCase() {}
void NativeDrawingRegionTest::TearDownTestCase() {}
void NativeDrawingRegionTest::SetUp() {}
void NativeDrawingRegionTest::TearDown() {}

/*
 * @tc.name: NativeDrawingRegionTest_region001
 * @tc.desc: test for create drawing_region.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_region001, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    EXPECT_NE(region, nullptr);
    OH_Drawing_RegionDestroy(region);
}

/*
 * @tc.name: NativeDrawingRegionSetRectTest_region002
 * @tc.desc: test for constructs a rectangular Region matching the bounds of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionSetRectTest_region002, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect=OH_Drawing_RectCreate(0.0f, 0.0f, 256.0f, 256.0f);
    EXPECT_TRUE(OH_Drawing_RegionSetRect(region, rect));
    OH_Drawing_RegionSetRect(nullptr, rect);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_RegionSetRect(region, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRegionIsRegionContained_region003
 * @tc.desc: test the other region is in the region.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionIsRegionContained_region003, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect=OH_Drawing_RectCreate(10.0f, 10.0f, 200.0f, 200.0f);
    OH_Drawing_RegionSetRect(region, rect);

    OH_Drawing_Region* otherOne = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* otherRectOne=OH_Drawing_RectCreate(0.0f, 0.0f, 20.0f, 20.0f);
    EXPECT_EQ(OH_Drawing_RegionIsRegionContained(region, otherOne),false);
    OH_Drawing_RegionSetRect(otherOne, otherRectOne);
    EXPECT_EQ(OH_Drawing_RegionIsRegionContained(region, otherOne),false);

    OH_Drawing_Region* otherTwo = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* otherRectTwo=OH_Drawing_RectCreate(10.0f, 10.0f, 20.0f, 20.0f);
    OH_Drawing_RegionSetRect(otherTwo, otherRectTwo);
    EXPECT_EQ(OH_Drawing_RegionIsRegionContained(region, otherTwo),true);

    OH_Drawing_Region* otherThree = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* otherRectThree=OH_Drawing_RectCreate(10.0f, 10.0f, 201.0f, 201.0f);
    OH_Drawing_RegionSetRect(otherThree, otherRectThree);
    EXPECT_EQ(OH_Drawing_RegionIsRegionContained(region, otherThree),false);

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RegionDestroy(otherOne);
    OH_Drawing_RegionDestroy(otherTwo);
    OH_Drawing_RegionDestroy(otherThree);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(otherRectOne);
    OH_Drawing_RectDestroy(otherRectTwo);
    OH_Drawing_RectDestroy(otherRectThree);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS