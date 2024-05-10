/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "modifier/rs_modifier_manager.h"
#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "modifier/rs_property_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSModifierManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierManagerTest::SetUpTestCase() {}
void RSModifierManagerTest::TearDownTestCase() {}
void RSModifierManagerTest::SetUp() {}
void RSModifierManagerTest::TearDown() {}

/**
 * @tc.name: CreateDrawingContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, CreateDrawingContextTest, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    rsModifierManager.Draw();
}

/**
 * @tc.name: RemoveAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, RemoveAnimationTest, TestSize.Level1)
{
    uint64_t ANIMATION_ID = 100;
    uint64_t PROPERTY_ID = 101;
    uint64_t PREFERRED = 120;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    AnimationId key = renderCurveAnimation->GetAnimationId();

    RSModifierManager rsModifierManager;
    rsModifierManager.SetFrameRateGetFunc(
        [PREFERRED](const RSPropertyUnit unit, float velocity) -> int32_t {return PREFERRED;});
    rsModifierManager.AddAnimation(renderCurveAnimation);
    ASSERT_EQ(rsModifierManager.HasUIAnimation(), true);
    rsModifierManager.RemoveAnimation(key);
    ASSERT_EQ(rsModifierManager.HasUIAnimation(), false);
}

/**
 * @tc.name: IsDisplaySyncEnabled
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, IsDisplaySyncEnabled, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    ASSERT_EQ(rsModifierManager.IsDisplaySyncEnabled(), false);
    rsModifierManager.SetDisplaySyncEnable(true);
    ASSERT_EQ(rsModifierManager.IsDisplaySyncEnabled(), true);
}

/**
 * @tc.name: JudgeAnimateWhetherSkip
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierManagerTest, JudgeAnimateWhetherSkip, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    AnimationId animId = 0;
    int64_t time = 1111111111111;
    int64_t vsyncPeriod = 11718750;
    auto isSkip = rsModifierManager.JudgeAnimateWhetherSkip(animId, time, vsyncPeriod);
    ASSERT_EQ(isSkip, false);
}

/**
 * @tc.name: AddModifier
 * @tc.desc: Test AddModifier and Draw and Animate
 * @tc.type: FUNC
 * @tc.require: AAA
 */
HWTEST_F(RSModifierManagerTest, AddModifier, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    auto prop = std::make_shared<RSPropertyBase>();
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSAlphaModifier>(prop);
    rsModifierManager.AddModifier(modifier);
    rsModifierManager.Draw();
    ASSERT_TRUE(rsModifierManager.modifiers_.empty());
    rsModifierManager.Animate(1, 1);
    ASSERT_FALSE(rsModifierManager.isDisplaySyncEnabled_);
}

/**
 * @tc.name: FlushStartAnimation
 * @tc.desc: Test FlushStartAnimation and JudgeAnimateWhetherSkip and GetFrameRateRange
 * @tc.type: FUNC
 * @tc.require: AAA
 */
HWTEST_F(RSModifierManagerTest, FlushStartAnimation, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    AnimationId id = 1;
    std::shared_ptr<RSRenderAnimation> animation = std::make_shared<RSRenderAnimation>(id);
    rsModifierManager.animations_.emplace(animation->GetAnimationId(), animation);
    rsModifierManager.FlushStartAnimation(1);
    ASSERT_TRUE(rsModifierManager.modifiers_.empty());

    uint64_t ANIMATION_ID = 100;
    uint64_t PROPERTY_ID = 101;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    AnimationId key = renderCurveAnimation->GetAnimationId();
    rsModifierManager.AddAnimation(renderCurveAnimation);
    rsModifierManager.JudgeAnimateWhetherSkip(key, 1, 1);
    ASSERT_TRUE(rsModifierManager.displaySyncs_.count(renderCurveAnimation->GetAnimationId()));

    ASSERT_EQ(rsModifierManager.GetFrameRateRange().min_, 0);
}

/**
 * @tc.name: OnAnimationFinished
 * @tc.desc: Test OnAnimationFinished and RegisterSpringAnimation and UnregisterSpringAnimation
 * @tc.type: FUNC
 * @tc.require: AAA
 */
HWTEST_F(RSModifierManagerTest, OnAnimationFinished, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    AnimationId id = 1;
    std::shared_ptr<RSRenderAnimation> animation = std::make_shared<RSRenderAnimation>(id);
    rsModifierManager.OnAnimationFinished(animation);

    PropertyId propertyId = 1;
    rsModifierManager.RegisterSpringAnimation(propertyId, id);
    ASSERT_FALSE(rsModifierManager.springAnimations_.empty());

    rsModifierManager.UnregisterSpringAnimation(propertyId, id);
    ASSERT_TRUE(rsModifierManager.springAnimations_.empty());
}


/**
 * @tc.name: QuerySpringAnimation
 * @tc.desc: Test QuerySpringAnimation and GetAnimation
 * @tc.type: FUNC
 * @tc.require: AAA
 */
HWTEST_F(RSModifierManagerTest, QuerySpringAnimation, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    PropertyId propertyId = 1;
    rsModifierManager.QuerySpringAnimation(propertyId);
    AnimationId id = 1;
    rsModifierManager.RegisterSpringAnimation(propertyId, id);
    ASSERT_EQ(rsModifierManager.QuerySpringAnimation(propertyId), nullptr);

    id = 0;
    rsModifierManager.RegisterSpringAnimation(propertyId, id);
    ASSERT_EQ(rsModifierManager.QuerySpringAnimation(propertyId), nullptr);

    std::shared_ptr<RSRenderAnimation> animation = std::make_shared<RSRenderAnimation>(id);
    rsModifierManager.animations_.emplace(animation->GetAnimationId(), animation);
    ASSERT_NE(rsModifierManager.GetAnimation(animation->GetAnimationId()), nullptr);
 * @tc.name: UnregisterSpringAnimationTest001
 * @tc.desc: test results of UnregisterSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierManagerTest, UnregisterSpringAnimationTest001, TestSize.Level1)
{
    RSModifierManager rsModifierManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsModifierManager.UnregisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}
} // namespace OHOS::Rosen