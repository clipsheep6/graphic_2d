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
#include "utils/camera3d.h"
#include "utils/scalar.h"
#include "utils/matrix.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Camera3DTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Camera3DTest::SetUpTestCase() {}
void Camera3DTest::TearDownTestCase() {}
void Camera3DTest::SetUp() {}
void Camera3DTest::TearDown() {}

/**
 * @tc.name: CreateAndDestoryTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, CreateAndDestoryTest001, TestSize.Level1)
{
    // The best way to create Camera3D.
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
}

/**
 * @tc.name: UtilsSaveTestTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsSaveTestTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->Save();
}

/**
 * @tc.name: UtilsRestoreTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRestoreTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->Restore();
}

/**
 * @tc.name: UitlsTranslateTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UitlsTranslateTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->Translate(131.5f, 121, 60);
}

/**
 * @tc.name: UitlsTranslateTest002;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UitlsTranslateTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->Translate(23.6f, 7.9f, 800.9f);
}

/**
 * @tc.name: UitlsTranslateTest003;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UitlsTranslateTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->Translate(200, 250, 601);
}

/**
 * @tc.name: UitlsTranslateTest004;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UitlsTranslateTest004, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->Translate(100, 200, 300);
}

/**
 * @tc.name: UtilsRotateXDegreesTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateXDegreesTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateXDegrees(800.6f);
}

/**
 * @tc.name: UtilsRotateXDegreesTest002;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateXDegreesTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateXDegrees(300);
}

/**
 * @tc.name: UtilsRotateXDegreesTest003;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateXDegreesTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateXDegrees(300.29f);
}

/**
 * @tc.name: UtilsRotateYDegreesTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateYDegreesTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateYDegrees(300.29f);
}

/**
 * @tc.name: UtilsRotateYDegreesTest002;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateYDegreesTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateYDegrees(300);
}

/**
 * @tc.name: UtilsRotateYDegreesTest003;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateYDegreesTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateYDegrees(300.29f);
}

/**
 * @tc.name: UtilsRotateZDegreesTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateZDegreesTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateZDegrees(300.29f);
}

/**
 * @tc.name: UtilsRotateZDegreesTest002;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateZDegreesTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateZDegrees(300);
}

/**
 * @tc.name: UtilsRotateZDegreesTest003;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsRotateZDegreesTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    camera3dPtr->RotateZDegrees(300.29f);
}

/**
 * @tc.name: SetAndGetCameraPosXTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosXTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosX=100;
    camera3dPtr->SetCameraPos(cameraPosX, 300, 709);
    EXPECT_EQ(cameraPosX, camera3dPtr->GetCameraPosX());
}

/**
 * @tc.name: SetAndGetCameraPosXTest002;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosXTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosX=22.1F;
    camera3dPtr->SetCameraPos(cameraPosX, 66.3f, 300.29f);
    EXPECT_EQ(cameraPosX, camera3dPtr->GetCameraPosX());
}

/**
 * @tc.name: SetAndGetCameraPosXTest003;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosXTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosX=300.0F;
    camera3dPtr->SetCameraPos(cameraPosX, 66.3F, 900.2F);
    EXPECT_EQ(cameraPosX, camera3dPtr->GetCameraPosX());
}

/**
 * @tc.name: SetAndGetCameraPosYTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosYTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosY=66.3f;
    camera3dPtr->SetCameraPos(22.1f, cameraPosY, 300.29f);
    EXPECT_EQ(cameraPosY, camera3dPtr->GetCameraPosY());
}

/**
 * @tc.name: SetAndGetCameraPosYTest002;
 * @tc.desc:
 * @tc.type: FUNC FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosYTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosY=300;
    camera3dPtr->SetCameraPos(100, cameraPosY, 709);
    EXPECT_EQ(cameraPosY, camera3dPtr->GetCameraPosY());
}

/**
 * @tc.name: SetAndGetCameraPosYTest003;
 * @tc.desc:
 * @tc.type: FUNC FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosYTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosY=66.3F;
    camera3dPtr->SetCameraPos(300.0F, cameraPosY, 900.2F);
    EXPECT_EQ(cameraPosY, camera3dPtr->GetCameraPosY());
}

/**
 * @tc.name: SetAndGetCameraPosZTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosZTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosZ=300.29f;
    camera3dPtr->SetCameraPos(22.1f, 66.3f, cameraPosZ);
    EXPECT_EQ(cameraPosZ, camera3dPtr->GetCameraPosZ());
}

/**
 * @tc.name: SetAndGetCameraPosZTest002;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosZTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosZ=709;
    camera3dPtr->SetCameraPos(100, 300, cameraPosZ);
    EXPECT_EQ(cameraPosZ, camera3dPtr->GetCameraPosZ());
}

/**
 * @tc.name: SetAndGetCameraPosZTest003;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosZTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    scalar cameraPosZ=900.2F;
    camera3dPtr->SetCameraPos(300.0F, 66.3F, cameraPosZ);
    EXPECT_EQ(cameraPosZ, camera3dPtr->GetCameraPosZ());
}

/**
 * @tc.name: ApplyToMatrixTestTest001;
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, ApplyToMatrixTestTest001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr != nullptr);
    Matrix m;
    camera3dPtr->ApplyToMatrix(m);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS