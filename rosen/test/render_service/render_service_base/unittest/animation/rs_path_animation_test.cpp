/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "animation/rs_path_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_render_path_animation.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_modifier.h"
#include "modifier/rs_property.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSStepsInterpolatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSStepsInterpolatorTest::SetUpTestCase() {}
void RSStepsInterpolatorTest::TearDownTestCase() {}
void RSStepsInterpolatorTest::SetUp() {}
void RSStepsInterpolatorTest::TearDown() {}

/**
 * @tc.name: SetBeginFraction001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetBeginFraction001, Level1)
{
    float Fraction;
    RSPathAnimation::SetBeginFraction(Fraction)
}

/**
 * @tc.name: GetBeginFraction001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, GetBeginFraction001, Level1)
{

    auto BeginFraction_ = RSPathAnimation::GetBeginFraction()
}

/**
 * @tc.name: SetEndFraction001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetEndFraction001, Level1)
{
    float Fraction;
    auto BeginFraction_ = RSPathAnimation::SetEndFraction()
}

/**
 * @tc.name: GetEndFraction001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, GetEndFraction001, Level1)
{

    auto BeginFraction_ = RSPathAnimation::GetEndFraction()
}