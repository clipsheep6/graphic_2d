/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_interpolating_spring_animation.h"
#include "modifier/rs_property.h"
#include "modifier/rs_extended_modifier.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyAnimationTest::SetUpTestCase() {}
void RSPropertyAnimationTest::TearDownTestCase() {}
void RSPropertyAnimationTest::SetUp() {}
void RSPropertyAnimationTest::TearDown() {}

/**
 * @tc.name: SetPropertyValueTest
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyAnimationTest, PropertyValueTest, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    auto value = std::make_shared<RSPropertyBase>();
    RSPropertyAnimation rsPropertyAnimation(property);
    rsPropertyAnimation.SetPropertyValue(value);
    rsPropertyAnimation.GetPropertyValue();
    ASSERT_NE(value, nullptr);
}

/**
 * @tc.name: SetOriginValueTest
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyAnimationTest, SetOriginValueTest, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    auto originValue = std::make_shared<RSPropertyBase>();
    RSPropertyAnimation rsPropertyAnimation(property);
    rsPropertyAnimation.SetOriginValue(originValue);
    ASSERT_NE(originValue, nullptr);
}

/**
 * @tc.name: InitInterpolationValueTest
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyAnimationTest, InitInterpolationValueTest, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    RSPropertyAnimation rsPropertyAnimation(property);
    rsPropertyAnimation.InitInterpolationValue();
    ASSERT_NE(property, nullptr);
}

/**
 * @tc.name: GetPropertyIdTest
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyAnimationTest, GetPropertyIdTest, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    RSPropertyAnimation rsPropertyAnimation(property);
    rsPropertyAnimation.GetPropertyId();
    rsPropertyAnimation.OnStart();
    rsPropertyAnimation.SetPropertyOnAllAnimationFinish();
    rsPropertyAnimation.InitAdditiveMode();
    ASSERT_NE(property, nullptr);
}

/**
 * @tc.name: InitInterpolationValueTest001
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyAnimationTest, OnUpdateStagingValueTest001, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    RSPropertyAnimation rsPropertyAnimation(property);
    rsPropertyAnimation.SetDirection(false);
    rsPropertyAnimation.GetDirection();
    rsPropertyAnimation.SetAutoReverse(true);
    rsPropertyAnimation.SetRepeatCount(2);
    rsPropertyAnimation.OnUpdateStagingValue(true);
    ASSERT_NE(property, nullptr);
}

/**
 * @tc.name: InitInterpolationValueTest002
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyAnimationTest, OnUpdateStagingValueTest002, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    RSPropertyAnimation rsPropertyAnimation(property);
    rsPropertyAnimation.SetDirection(false);
    rsPropertyAnimation.GetDirection();
    rsPropertyAnimation.SetAutoReverse(true);
    rsPropertyAnimation.SetRepeatCount(1);
    rsPropertyAnimation.OnUpdateStagingValue(true);
    ASSERT_NE(property, nullptr);
}

/**
 * @tc.name: InitInterpolationValueTest003
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyAnimationTest, OnUpdateStagingValueTest003, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    RSPropertyAnimation rsPropertyAnimation(property);
    rsPropertyAnimation.SetDirection(false);
    rsPropertyAnimation.GetDirection();
    rsPropertyAnimation.SetAutoReverse(true);
    rsPropertyAnimation.SetRepeatCount(2);
    rsPropertyAnimation.OnUpdateStagingValue(false);
    ASSERT_NE(property, nullptr);
}

/**
 * @tc.name: InitInterpolationValueTest004
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyAnimationTest, OnUpdateStagingValueTest004, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    RSPropertyAnimation rsPropertyAnimation(property);
    rsPropertyAnimation.SetDirection(false);
    rsPropertyAnimation.GetDirection();
    rsPropertyAnimation.SetAutoReverse(true);
    rsPropertyAnimation.SetRepeatCount(1);
    rsPropertyAnimation.OnUpdateStagingValue(false);
    ASSERT_NE(property, nullptr);
}
}