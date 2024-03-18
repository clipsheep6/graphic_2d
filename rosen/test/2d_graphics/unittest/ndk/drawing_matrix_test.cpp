/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "c/drawing_matrix.h"
#include "drawing_rect.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingMatrixTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingMatrixTest::SetUpTestCase() {}
void NativeDrawingMatrixTest::TearDownTestCase() {}
void NativeDrawingMatrixTest::SetUp() {}
void NativeDrawingMatrixTest::TearDown() {}

/*
 * @tc.name: NativeDrawingMatrixTest_SetMatrix001
 * @tc.desc: test for SetMatrix.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetMatrix001, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(nullptr);
}

/*
 * @tc.name: NativeDrawingMatrixTest_SetRectToRect002
 * @tc.desc: test for SetRectToRect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetRectToRect002, TestSize.Level1)
{
    OH_Drawing_Rect *rectSrc = OH_Drawing_RectCreate(0, 0, 100, 100);
    OH_Drawing_Rect *rectDst = OH_Drawing_RectCreate(0, 0, 200, 200);
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetRectToRect(matrix, rectSrc, rectDst, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    OH_Drawing_MatrixSetRectToRect(nullptr, nullptr, nullptr, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    OH_Drawing_RectDestroy(rectDst);
    OH_Drawing_RectDestroy(rectSrc);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Reset003
 * @tc.desc: test for Reset.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Reset003, TestSize.Level1)
{
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 2, 0, 0, 0, 1, 2, 0, 0, 1);
    OH_Drawing_MatrixReset(matrix);
    OH_Drawing_MatrixReset(nullptr);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_CreateRotation004
 * @tc.desc: test for CreateRotation.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_CreateRotation004, TestSize.Level1)
{
    // rotate deg: 180 pivot, point (1, 1)
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateRotation(180, 1, 1);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 2));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 2));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_CreateScale005
 * @tc.desc: test for CreateScale.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_CreateScale005, TestSize.Level1)
{
    /* The first 10 is horizontal scale factor.
    The second 10 is vertical scale factor.
    The third 10 is pivot on x-axis.
    The fourth 10 is the pivot on y-axis.
    */
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateScale(10, 10, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -90));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -90));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_CreateTranslation006
 * @tc.desc: test for CreateTranslation.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_CreateTranslation006, TestSize.Level1)
{
    // translate x= 100, y = 200
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateTranslation(100, 200);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 100));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 200));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Concat007
 * @tc.desc: test for Concat.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Concat007, TestSize.Level1)
{
    OH_Drawing_Matrix* matrixA = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixB = OH_Drawing_MatrixCreate();
    bool ret;
    ret = OH_Drawing_MatrixIsEqual(nullptr, matrixB);
    EXPECT_TRUE(!ret);
    ret = OH_Drawing_MatrixIsEqual(matrixA, nullptr);
    EXPECT_TRUE(!ret);
    ret = OH_Drawing_MatrixIsEqual(matrixA, matrixB);
    EXPECT_TRUE(ret);
    OH_Drawing_MatrixSetMatrix(
        matrixA,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_MatrixSetMatrix(
        matrixB,
        1, 0, 100,
        0, -1, 200,
        0, 0, 1);
    ret = OH_Drawing_MatrixIsEqual(matrixA, matrixB);
    EXPECT_TRUE(!ret);
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixConcat(nullptr, matrixA, matrixB);
    OH_Drawing_MatrixConcat(matrix, nullptr, matrixB);
    OH_Drawing_MatrixConcat(matrix, matrixA, nullptr);
    OH_Drawing_MatrixConcat(matrix, matrixA, matrixB);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 100));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -200));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrixA);
    OH_Drawing_MatrixDestroy(matrixB);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Rotate008
 * @tc.desc: test for Rotate.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Rotate008, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    // rotate deg: 180 pivot, point (1, 1)
    OH_Drawing_MatrixRotate(nullptr, 180, 1, 1);
    OH_Drawing_MatrixRotate(matrix, 180, 1, 1);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 2));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 2));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    //GetValue exception
    value = OH_Drawing_MatrixGetValue(nullptr, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, -1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 9);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PreRotate009
 * @tc.desc: test for PreRotate.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PreRotate009, TestSize.Level1)
{
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 2, 0, 0, 0, 1, 2, 0, 0, 1);
    OH_Drawing_MatrixPreRotate(matrix, 5, 10, 20);
    OH_Drawing_MatrixPreRotate(nullptr, 5, 10, 20);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PostRotate010
 * @tc.desc: test for PostRotate.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PostRotate010, TestSize.Level1)
{
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 2, 0, 0, 0, 1, 2, 0, 0, 1);
    OH_Drawing_MatrixPostRotate(matrix, 5, 10, 20);
    OH_Drawing_MatrixPostRotate(nullptr, 5, 10, 20);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Scale011
 * @tc.desc: test for Scale.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Scale011, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    /* The first 10 is horizontal scale factor.
    The second 10 is vertical scale factor.
    The third 10 is pivot on x-axis.
    The fourth 10 is the pivot on y-axis.
    */
    OH_Drawing_MatrixScale(nullptr, 10, 10, 10, 10);
    OH_Drawing_MatrixScale(matrix, 10, 10, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -90));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -90));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PreScale012
 * @tc.desc: test for PreScale.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PreScale012, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixPreScale(nullptr, 10, 10, 10, 10);
    OH_Drawing_MatrixPreScale(matrix, 10, 10, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PostScale013
 * @tc.desc: test for PostScale.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PostScale013, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixPostScale(nullptr, 10, 10, 10, 10);
    OH_Drawing_MatrixPostScale(matrix, 10, 10, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Translate014
 * @tc.desc: test for Translate.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Translate014, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    bool ret;
    ret = OH_Drawing_MatrixIsIdentity(matrix);
    EXPECT_TRUE(ret);
    ret = OH_Drawing_MatrixIsIdentity(nullptr);
    EXPECT_TRUE(!ret);
    // translate x = 100, y = 200
    OH_Drawing_MatrixTranslate(nullptr, 100, 200);
    OH_Drawing_MatrixTranslate(matrix, 100, 200);
    ret = OH_Drawing_MatrixIsIdentity(matrix);
    EXPECT_TRUE(!ret);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 100));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 200));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PreTranslate015
 * @tc.desc: test for PreTranslate.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PreTranslate015, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixPreTranslate(nullptr, 10, 10);
    OH_Drawing_MatrixPreTranslate(matrix, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PostTranslate016
 * @tc.desc: test for PostTranslate.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PostTranslate016, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixPostTranslate(nullptr, 10, 10);
    OH_Drawing_MatrixPostTranslate(matrix, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Invert017
 * @tc.desc: test for Invert.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Invert017, TestSize.Level1)
{
    bool ret;
    ret = OH_Drawing_MatrixInvert(nullptr, nullptr);
    EXPECT_TRUE(!ret);
    OH_Drawing_Matrix* matrixA = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixB = OH_Drawing_MatrixCreate();
    ret = OH_Drawing_MatrixInvert(nullptr, matrixB);
    EXPECT_TRUE(!ret);
    ret = OH_Drawing_MatrixInvert(matrixA, nullptr);
    EXPECT_TRUE(!ret);
    ret = OH_Drawing_MatrixInvert(matrixA, matrixB);
    EXPECT_TRUE(ret);
    OH_Drawing_MatrixSetMatrix(
        matrixA,
        1, 0, 0,
        0, -0.5, 0,
        0, 0, 1);
    ret = OH_Drawing_MatrixInvert(matrixA, matrixB);
    EXPECT_TRUE(ret);

    float value;
    value = OH_Drawing_MatrixGetValue(matrixB, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrixB, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrixB, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));

    value = OH_Drawing_MatrixGetValue(matrixB, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrixB, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -2));
    value = OH_Drawing_MatrixGetValue(matrixB, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));

    value = OH_Drawing_MatrixGetValue(matrixB, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrixB, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrixB, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrixA);
    OH_Drawing_MatrixDestroy(matrixB);
}

/**
 * @tc.name: NativeDrawingMatrixTest_SetPolyToPoly018
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetPolyToPoly018, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    OH_Drawing_Point2D dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_TRUE(OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, 0));
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_SetPolyToPoly019
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetPolyToPoly019, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
    OH_Drawing_Point2D dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}};
    EXPECT_TRUE(OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, 1));
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_SetPolyToPoly020
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetPolyToPoly020, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    OH_Drawing_Point2D dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_TRUE(OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, 4));
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_SetPolyToPoly021
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetPolyToPoly021, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    OH_Drawing_Point2D dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_FALSE(OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, 5));
    OH_Drawing_MatrixDestroy(matrix);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
