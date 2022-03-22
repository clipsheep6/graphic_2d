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
#include "utils/matrix.h"
#include "utils/scalar.h"
#include "utils/point.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class MatrixTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MatrixTest::SetUpTestCase() {}
void MatrixTest::TearDownTestCase() {}
void MatrixTest::SetUp() {}
void MatrixTest::TearDown() {}

/**
 * @tc.name: UtilsMatrixTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest001, TestSize.Level1)
{
    // The best way to create Matrix.
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
}

/**
 * @tc.name: UtilsMatrixTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Rotate(0.1f, 150.1f, 650.9f);
}

/**
 * @tc.name: UtilsMatrixTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest003, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Rotate(20.8f, 133.0f, 100);
}

/**
 * @tc.name: UtilsMatrixTest004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest004, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Translate(20.8f, 100);
}

/**
 * @tc.name: UtilsMatrixTest005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest005, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Translate(77.7f, 190.2f);
}

/**
 * @tc.name: UtilsMatrixTest006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest006, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Scale(32.1f, 10.6f, 800, 90.1f);
}

/**
 * @tc.name: UtilsMatrixTest007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest007, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Scale(16.5f, 50.6f, 150.8f, 560.9f);
}

/**
 * @tc.name: UtilsMatrixTest008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest008, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2;
    matrix1.Rotate(3.0f, 2.0f, 1.0f);
    matrix2.Rotate(1.5f, 2.5f, 3.5f);
    Matrix matrix3 = matrix1 * matrix2;
    EXPECT_TRUE(matrix3 == matrix1);
    EXPECT_FALSE(matrix3 == matrix2);
}

/**
 * @tc.name: UtilsMatrixTest009
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest009, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2;
    matrix1.Rotate(3.7f, 6.0f, 1.9f);
    matrix2.Rotate(5.5f, 12.5f, 30.5f);
    Matrix matrix3 = matrix1 * matrix2;
    EXPECT_TRUE(matrix3 == matrix1);
    EXPECT_FALSE(matrix3 == matrix2);
}

/**
 * @tc.name: UtilsMatrixTest010
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest010, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2 = matrix1;
    EXPECT_TRUE(matrix1 == matrix2);
}

/**
 * @tc.name: UtilsMatrixTest011
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest011, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->SetMatrix(200, 150, 800, 60, 200, 150, 800, 60, 90);
}

/**
 * @tc.name: UtilsMatrixTest012
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest012, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->SetMatrix(20.9f, 15.8f, 80.8f, 60, 2.4f, 99.9f, 60, 60, 900);
}

/**
 * @tc.name: UtilsMatrixTest013
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest013, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->SetMatrix(20.9f, 15.8f, 80.8f, 60.6f, 2.4f, 99.9f, 60.5f, 60.1f, 90.5f);
}

/**
 * @tc.name: UtilsMatrixTest014
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest014, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    std::vector<Point> dst = {{1, 2}};
    std::vector<Point> src = {{2, 3}};
    matrix->MapPoints(dst, src, 100);
}

/**
 * @tc.name: UtilsMatrixTest015
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest015, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    std::vector<Point> dst = {{3, 2}};
    std::vector<Point> src = {{1, 3}};
    matrix->MapPoints(dst, src, 191);
}

/**
 * @tc.name: UtilsMatrixTest016
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, UtilsMatrixTest016, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    ASSERT_EQ(1, matrix->Get(0));
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
