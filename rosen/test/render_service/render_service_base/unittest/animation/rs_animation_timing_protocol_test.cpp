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

#include "animation/rs_animation_timing_protocol.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationTimingProtocolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationTimingProtocolTest::SetUpTestCase() {}
void RSAnimationTimingProtocolTest::TearDownTestCase() {}
void RSAnimationTimingProtocolTest::SetUp() {}
void RSAnimationTimingProtocolTest::TearDown() {}

/**
 * @tc.name: SetAndGetDuration001
 * @tc.desc: test results of SetAndGetDuration
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetAndGetDuration001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1);
    EXPECT_TRUE(protocol.GetDuration() == 1);
}

/**
 * @tc.name: SetAndGetStartDelay001
 * @tc.desc: test results of SetAndGetStartDelay
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetAndGetStartDelay001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    protocol.SetStartDelay(1);
    EXPECT_TRUE(protocol.GetStartDelay() == 1);
}

/**
 * @tc.name: SetAndGetSpeed001
 * @tc.desc: test results of SetAndGetSpeed
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetAndGetSpeed001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    protocol.SetSpeed(1.f);
    EXPECT_TRUE(protocol.GetSpeed() == 1.f);
}

/**
 * @tc.name: SetAndGetRepeatCount001
 * @tc.desc: test results of SetAndGetRepeatCount
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetAndGetRepeatCount001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    protocol.SetRepeatCount(1);
    EXPECT_TRUE(protocol.GetRepeatCount() == 1);
}

/**
 * @tc.name: SetAndGetAutoReverse001
 * @tc.desc: test results of SetAndGetAutoReverse
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetAndGetAutoReverse001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    protocol.SetAutoReverse(true);
    EXPECT_TRUE(protocol.GetAutoReverse());
}

/**
 * @tc.name: SetAndGetFillMode001
 * @tc.desc: test results of SetAndGetFillMode
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetAndGetFillMode001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    FillMode fillMode = FillMode::BOTH;
    protocol.SetFillMode(fillMode);
    EXPECT_TRUE(protocol.GetFillMode() == FillMode::BOTH);
}

/**
 * @tc.name: SetAndGetDirection001
 * @tc.desc: test results of SetAndGetDirection
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetAndGetDirection001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    protocol.SetDirection(true);
    EXPECT_TRUE(protocol.GetDirection());
}

/**
 * @tc.name: SetFrameRateRangeAndGetFrameRateRange001
 * @tc.desc: test results of SetFrameRateRangeAndGetFrameRateRange
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetFrameRateRangeAndGetFrameRateRange001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    FrameRateRange range;
    protocol.SetFrameRateRange(range);
    EXPECT_TRUE(protocol.GetFrameRateRange() == range);
}

/**
 * @tc.name: SetAndGetFinishCallbackType001
 * @tc.desc: test results of SetAndGetFinishCallbackType
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(RSAnimationTimingProtocolTest, SetAndGetFinishCallbackType001, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    FinishCallbackType finishCallbackType = FinishCallbackType::LOGICALLY;
    protocol.SetFinishCallbackType(finishCallbackType);
    EXPECT_TRUE(protocol.GetFinishCallbackType() == FinishCallbackType::LOGICALLY);
}
} // namespace OHOS::Rosen