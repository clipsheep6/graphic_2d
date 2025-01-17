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
#include "animation/rs_spring_animation.h"
#include "modifier/rs_property.h"
#include "transaction/rs_transaction_proxy.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSpringAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpringAnimationTest::SetUpTestCase() {}
void RSSpringAnimationTest::TearDownTestCase() {}
void RSSpringAnimationTest::SetUp() {}
void RSSpringAnimationTest::TearDown() {}

/**
 * @tc.name: SetTimingCurveTest001
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetTimingCurveTest001, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    auto byValue = std::make_shared<RSPropertyBase>();
    RSSpringAnimation rsSpringAnimation(property, byValue);
    RSAnimationTimingCurve timingCurve;
    rsSpringAnimation.SetTimingCurve(timingCurve);
}

/**
 * @tc.name: GetTimingCurveTest001
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest001, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    auto byValue = std::make_shared<RSPropertyBase>();
    RSSpringAnimation rsSpringAnimation(property, byValue);;
    rsSpringAnimation.GetTimingCurve();
    ASSERT_NE(property, nullptr);
    ASSERT_NE(byValue, nullptr);
}

/**
 * @tc.name: StartRenderAnimationTest
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, StartRenderAnimationTest, Level1)
{
    auto property = std::make_shared<RSPropertyBase>();
    auto byValue = std::make_shared<RSPropertyBase>();
    RSSpringAnimation rsSpringAnimation(property, byValue);
    auto animation = std::make_shared<RSRenderSpringAnimation>();
    rsSpringAnimation.StartInner(nullptr);
    rsSpringAnimation.StartRenderAnimation(animation);
    rsSpringAnimation.StartUIAnimation(animation);
    ASSERT_NE(property, nullptr);
    ASSERT_NE(byValue, nullptr);
    ASSERT_NE(animation, nullptr);
}
}