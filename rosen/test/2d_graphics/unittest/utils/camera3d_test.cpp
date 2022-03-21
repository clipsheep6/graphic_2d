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
 * @tc.name: UtilsCamera3DTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest001, TestSize.Level1)
{
    // The best way to create Camera3D.
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
}

/**
 * @tc.name: UtilsCamera3DTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->Translate(131.5f, 121.0f, 60.0f);
}

/**
 * @tc.name: UtilsCamera3DTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->Translate(23.6f, 7.9f, 800.9f);
}

/**
 * @tc.name: UtilsCamera3DTest004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest004, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateXDegrees(800.6f);
}

/**
 * @tc.name: UtilsCamera3DTest005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest005, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateXDegrees(300.0f);
}

/**
 * @tc.name: UtilsCamera3DTest006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest006, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateXDegrees(300.29f);
}

/**
 * @tc.name: UtilsCamera3DTest007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest007, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateYDegrees(300.29f);
}

/**
 * @tc.name: UtilsCamera3DTest008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest008, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateYDegrees(300);
}

/**
 * @tc.name: UtilsCamera3DTest009
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest009, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateYDegrees(300.29f);
}

/**
 * @tc.name: UtilsCamera3DTest010
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest010, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateZDegrees(300.29f);
}

/**
 * @tc.name: UtilsCamera3DTest011
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest011, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateZDegrees(300.0f);
}

/**
 * @tc.name: UtilsCamera3DTest012
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest012, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->RotateZDegrees(300.29f);
}

/**
 * @tc.name: UtilsCamera3DTest013
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest013, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->SetCameraPos(8.2f, 9.1f, 8.2f);
    EXPECT_EQ(8.2f, camera3d->GetCameraPosX());
    EXPECT_EQ(9.1f, camera3d->GetCameraPosY());
    EXPECT_EQ(8.2f, camera3d->GetCameraPosZ());
    camera3d->Save();
    camera3d->Restore();
}

/**
 * @tc.name: UtilsCamera3DTest014
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest014, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    camera3d->SetCameraPos(8.0f, 8.1f, 8.2f);
    EXPECT_EQ(8.0f, camera3d->GetCameraPosX());
    EXPECT_EQ(8.1f, camera3d->GetCameraPosY());
    EXPECT_EQ(8.2f, camera3d->GetCameraPosZ());
    camera3d->Save();
    camera3d->Restore();
}

/**
 * @tc.name: UtilsCamera3DTest015
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest015, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    Matrix matrix;
    camera3d->ApplyToMatrix(matrix);
}

/**
 * @tc.name: UtilsCamera3DTest016
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Camera3DTest, UtilsCamera3DTest016, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3d = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3d != nullptr);
    Matrix matrix2;
    camera3d->ApplyToMatrix(matrix2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS