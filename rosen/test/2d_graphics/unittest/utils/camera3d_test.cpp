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
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, CreateAndDestory001, TestSize.Level1)
{
    // The best way to create Camera3D.
    Camera3D camera3d_;
}

/**
 * @tc.name: CreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, CreateAndDestory002, TestSize.Level1)
{
    // The best way to create Camera3D.
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
}

/**
 * @tc.name: Save001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Save001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->Save();
}


/**
 * @tc.name: Save002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Save002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->Save();
}

/**
 * @tc.name: Restore001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Restore001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->Restore();
}

/**
 * @tc.name: Restore002;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Restore002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->Restore();
}

/**
 * @tc.name: Translate001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Translate001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->Translate(131.5f, 121, 60);
}

/**
 * @tc.name: Translate002;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Translate002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->Translate(23.6f, 7.9f, 800.9f);
}

/**
 * @tc.name: Translate003;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Translate003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->Translate(200, 250, 601);
}

/**
 * @tc.name: Translate004;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, Translate004, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->Translate(100, 200, 300);
}

/**
 * @tc.name: RotateXDegrees001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateXDegrees001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateXDegrees(800.6f);
}

/**
 * @tc.name: RotateXDegrees002;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateXDegrees002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateXDegrees(300);
}

/**
 * @tc.name: RotateXDegrees003;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateXDegrees003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateXDegrees(300.29f);
}

/**
 * @tc.name: RotateYDegrees001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateYDegrees001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateYDegrees(300.29f);
}

/**
 * @tc.name: RotateYDegrees002;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateYDegrees002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateYDegrees(300);
}

/**
 * @tc.name: RotateYDegrees003;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateYDegrees003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateYDegrees(300.29f);
}

/**
 * @tc.name: RotateZDegrees001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateZDegrees001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateZDegrees(300.29f);
}

/**
 * @tc.name: RotateZDegrees002;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateZDegrees002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateZDegrees(300);
}

/**
 * @tc.name: RotateZDegrees003;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, RotateZDegrees003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    camera3dPtr_->RotateZDegrees(300.29f);
}

/**
 * @tc.name: SetAndGetCameraPosX001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosX001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosX=100;
    camera3dPtr_->SetCameraPos(cameraPosX, 300, 709);
    EXPECT_EQ(cameraPosX, camera3dPtr_->GetCameraPosX());
}

/**
 * @tc.name: SetAndGetCameraPosX002;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosX002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosX=22.1F;
    camera3dPtr_->SetCameraPos(cameraPosX, 66.3f, 300.29f);
    EXPECT_EQ(cameraPosX, camera3dPtr_->GetCameraPosX());
}

/**
 * @tc.name: SetAndGetCameraPosX003;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosX003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosX=300.0F;
    camera3dPtr_->SetCameraPos(cameraPosX, 66.3F, 900.2F);
    EXPECT_EQ(cameraPosX, camera3dPtr_->GetCameraPosX());
}

/**
 * @tc.name: SetAndGetCameraPosY001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosY001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosY=66.3f;
    camera3dPtr_->SetCameraPos(22.1f, cameraPosY, 300.29f);
    EXPECT_EQ(cameraPosY, camera3dPtr_->GetCameraPosY());
}

/**
 * @tc.name: SetAndGetCameraPosY002;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosY002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosY=300;
    camera3dPtr_->SetCameraPos(100, cameraPosY, 709);
    EXPECT_EQ(cameraPosY, camera3dPtr_->GetCameraPosY());
}

/**
 * @tc.name: SetAndGetCameraPosY003;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosY003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosY=66.3F;
    camera3dPtr_->SetCameraPos(300.0F, cameraPosY, 900.2F);
    EXPECT_EQ(cameraPosY, camera3dPtr_->GetCameraPosY());
}

/**
 * @tc.name: SetAndGetCameraPosZ001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosZ001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosZ=300.29f;
    camera3dPtr_->SetCameraPos(22.1f, 66.3f, cameraPosZ);
    EXPECT_EQ(cameraPosZ, camera3dPtr_->GetCameraPosZ());
}

/**
 * @tc.name: SetAndGetCameraPosZ002;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosZ002, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosZ=709;
    camera3dPtr_->SetCameraPos(100, 300, cameraPosZ);
    EXPECT_EQ(cameraPosZ, camera3dPtr_->GetCameraPosZ());
}

/**
 * @tc.name: SetAndGetCameraPosZ003;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, SetAndGetCameraPosZ003, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    scalar cameraPosZ=900.2F;
    camera3dPtr_->SetCameraPos(300.0F, 66.3F, cameraPosZ);
    EXPECT_EQ(cameraPosZ, camera3dPtr_->GetCameraPosZ());
}
/**
 * @tc.name: ApplyToMatrix001;
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(Camera3DTest, ApplyToMatrix001, TestSize.Level1)
{
    std::unique_ptr<Camera3D> camera3dPtr_ = std::make_unique<Camera3D>();
    ASSERT_TRUE(camera3dPtr_ != nullptr);
    Matrix m;
    camera3dPtr_->ApplyToMatrix(m);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS