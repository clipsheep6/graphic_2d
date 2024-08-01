/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "drawing_path_effect.h"
#include "drawing_pen.h"
#include "drawing_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPathEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPathEffectTest::SetUpTestCase() {}
void NativeDrawingPathEffectTest::TearDownTestCase() {}
void NativeDrawingPathEffectTest::SetUp() {}
void NativeDrawingPathEffectTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPathEffectTest_PathEffect001
 * @tc.desc: test for PathEffect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_PathEffect001, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    float intervals[] = {1, 1, 1};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 3, 0.0);
    OH_Drawing_PenSetPathEffect(nullptr, pathEffect);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    // 3 is the number of elements of the intervals array
    pathEffect = OH_Drawing_CreateDashPathEffect(nullptr, 3, 0.0);
    EXPECT_EQ(pathEffect, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 0, 0.0);
    EXPECT_EQ(pathEffect, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    pathEffect = OH_Drawing_CreateDashPathEffect(intervals, -1, 0.0);
    EXPECT_EQ(pathEffect, nullptr);
}

/*
 * @tc.name: NativeDrawingPathEffectTest_PathEffect002
 * @tc.desc: test for PathEffect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */ //OH_Drawing_CreateDashPathEffect
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_PathEffect002, TestSize.Level1)
{
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_PathEffect *pathEffect =  OH_Drawing_CreatePathDashEffect(path, 500, 20, PATH_DASH_STYLE_MORPH);
    EXPECT_NE(pathEffect, nullptr);
    OH_Drawing_PathEffect *pathEffectTwo = OH_Drawing_CreateCornerPathEffect(10);
    EXPECT_NE(pathEffectTwo, nullptr);
    OH_Drawing_PathEffect *pathEffectThree = OH_Drawing_CreateDiscretePathEffect(100, -50, 10);
    EXPECT_NE(pathEffectThree, nullptr);
    float vals[2] = {10, 50};
    OH_Drawing_PathEffect *pathEffectOne = OH_Drawing_CreateDashPathEffect(vals, 2, 100);
    float valf[2] = {100, 50};
    OH_Drawing_PathEffect *pathEffectSix = OH_Drawing_CreateDashPathEffect(valf, 2, 0);
    OH_Drawing_PathEffect *pathEffectFour = OH_Drawing_CreateSumPathEffect(pathEffectOne, pathEffectSix);
    EXPECT_NE(pathEffectFour, nullptr);
    OH_Drawing_PathEffect *pathEffectFive = OH_Drawing_CreateComposePathEffect(pathEffect, pathEffectSix);
    EXPECT_NE(pathEffectFive, nullptr);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_PathEffectDestroy(pathEffectOne);
    OH_Drawing_PathEffectDestroy(pathEffectTwo);
    OH_Drawing_PathEffectDestroy(pathEffectThree);
    OH_Drawing_PathEffectDestroy(pathEffectFour);
    OH_Drawing_PathEffectDestroy(pathEffectFive);
    OH_Drawing_PathEffectDestroy(pathEffectSix);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS