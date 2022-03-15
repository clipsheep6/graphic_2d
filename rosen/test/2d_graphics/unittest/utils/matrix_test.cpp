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
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, CreateAndDestory001, TestSize.Level1)
{
    // The best way to create Matrix.
    Matrix matrix;
}

/**
 * @tc.name: CreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, CreateAndDestory002, TestSize.Level1)
{
    // The best way to create Matrix.
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
}

/**
 * @tc.name: Rotate001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Rotate001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Rotate(0.1f, 150.1f, 650.9f);
}

/**
 * @tc.name: Rotate002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Rotate002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Rotate(20.8f, 133.0f, 100);
}

/**
 * @tc.name: Rotate003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Rotate003, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Rotate(300, 250, 55);
}

/**
 * @tc.name: Translate001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Translate001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Translate(20.8f, 100);
}

/**
 * @tc.name: Translate002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Translate002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Translate(77.7f, 190.2f);
}

/**
 * @tc.name: Translate003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Translate003, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Translate(30, 10);
}

/**
 * @tc.name: Scale001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Scale001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Scale(32.1f, 10.6f, 800, 90.1f);
}

/**
 * @tc.name: Scale002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Scale002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Scale(16.5f, 50.6f, 150.8f, 560.9f);
}

/**
 * @tc.name: Scale003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Scale003, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Scale(200, 150, 800, 60);
}

/**
 * @tc.name: OperatorMultiply001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, OperatorMultiply001, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2;
    Matrix matrix3 = matrix1 * matrix2;
}

/**
 * @tc.name: OperatorEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, OperatorEqual001, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2;
    ASSERT_TRUE(matrix1 == matrix2);
}

/**
 * @tc.name: SetMatrix001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, SetMatrix001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->SetMatrix(200, 150, 800, 60, 200, 150, 800, 60, 90);
}

/**
 * @tc.name: SetMatrix002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, SetMatrix002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->SetMatrix(20.9f, 15.8f, 80.8f, 60, 2.4f, 99.9f, 60, 60, 900);
}

/**
 * @tc.name: SetMatrix003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, SetMatrix003, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->SetMatrix(20.9f, 15.8f, 80.8f, 60.6f, 2.4f, 99.9f, 60.5f, 60.1f, 90.5f);
}

/**
 * @tc.name: MapPoints001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, MapPoints001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    std::vector<Point> dst;
    std::vector<Point> src;
    matrixPtr->MapPoints(dst, src, 100);
}

/**
 * @tc.name: MapPoints002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, MapPoints002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    std::vector<Point> dst;
    std::vector<Point> src;
    matrixPtr->MapPoints(dst, src, 191);
}

/**
 * @tc.name: MapPoints003
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, MapPoints003, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    std::vector<Point> dst;
    std::vector<Point> src;
    matrixPtr->MapPoints(dst, src, 500);
}

/**
 * @tc.name: Get001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Get001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Get(101);
}

/**
 * @tc.name: Get002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MatrixTest, Get002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrixPtr = std::make_unique<Matrix>();
    ASSERT_TRUE(matrixPtr != nullptr);
    matrixPtr->Get(264);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
