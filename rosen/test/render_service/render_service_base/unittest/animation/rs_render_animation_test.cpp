/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "animation/rs_render_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderAnimationTest::SetUpTestCase() {}
void RSRenderAnimationTest::TearDownTestCase() {}
void RSRenderAnimationTest::SetUp() {}
void RSRenderAnimationTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, Marshalling001, TestSize.Level1)
{
    RSRenderAnimation animation(1);
    Parcel parcel;
    bool res = animation.Marshalling(parcel);
    EXPECT_TRUE(res == false);
}

/**
 * @tc.name: DumpAnimation001
 * @tc.desc: test results of DumpAnimation
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, DumpAnimation001, TestSize.Level1)
{
    RSRenderAnimation animation;
    std::string out = "";
    animation.DumpAnimation(out);
    EXPECT_TRUE(out.empty() == false);
}

/**
 * @tc.name: ParseParam001
 * @tc.desc: test results of ParseParam
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, ParseParam001, TestSize.Level1)
{
    RSRenderAnimation animation;
    Parcel parcel;
    bool res = animation.ParseParam(parcel);
    EXPECT_TRUE(res == false);
}

/**
 * @tc.name: GetAnimationId001
 * @tc.desc: test results of GetAnimationId
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, GetAnimationId001, TestSize.Level1)
{
    RSRenderAnimation animation(1);
    AnimationId res = animation.GetAnimationId();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: IsStarted001
 * @tc.desc: test results of IsStarted
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, IsStarted001, TestSize.Level1)
{
    RSRenderAnimation animation;
    bool res = animation.IsStarted();
    EXPECT_TRUE(res == false);
}

/**
 * @tc.name: IsRunning001
 * @tc.desc: test results of IsRunning
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, IsRunning001, TestSize.Level1)
{
    RSRenderAnimation animation;
    animation.state_ = AnimationState::RUNNING;
    EXPECT_TRUE(animation.IsRunning());
}

/**
 * @tc.name: IsPaused001
 * @tc.desc: test results of IsPaused
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, IsPaused001, TestSize.Level1)
{
    RSRenderAnimation animation;
    animation.state_ = AnimationState::PAUSED;
    EXPECT_TRUE(animation.IsPaused());
}

/**
 * @tc.name: IsFinished001
 * @tc.desc: test results of IsFinished
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, IsFinished001, TestSize.Level1)
{
    RSRenderAnimation animation;
    animation.state_ = AnimationState::FINISHED;
    EXPECT_TRUE(animation.IsFinished());
}

/**
 * @tc.name: GetPropertyId001
 * @tc.desc: test results of GetPropertyId
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSRenderAnimationTest, GetPropertyId001, TestSize.Level1)
{
    RSRenderAnimation animation;
    EXPECT_TRUE(animation.GetPropertyId() == 0);
}
} // namespace OHOS::Rosen