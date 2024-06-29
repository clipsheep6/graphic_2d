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
#include <gtest/gtest.h>
#include "hgm_ltpo_power_policy.h"
#include "common/rs_common_hook.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int DEFAULT_MAX_FPS = 120;
constexpr int IDLE_FPS = 60;

class HgmLtpoPowerPolicyTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    void SetConfigEnable(std::string isEnable);
    void SetIdleStateEnable(bool isIdle);
};

void HgmLtpoPowerPolicyTest::SetConfigEnable(std::string isEnable)
{
    std::unordered_map<std::string, std::string> animationPowerConfig = { { "animation_ltpo_power_enable", isEnable },
        { "animation_idle_fps", "60" }, { "animation_idle_duration", "2000" } };
    HgmLtpoPowerPolicy::Instance().SetConfig(animationPowerConfig);
}

void HgmLtpoPowerPolicyTest::SetIdleStateEnable(bool isIdle)
{
    HgmLtpoPowerPolicy::Instance().SetIdleState(isIdle);
}

/**
 * @tc.name: SetConfigTest1
 * @tc.desc: test results of SetConfigTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, SetConfigTest1, TestSize.Level1)
{
    SetConfigEnable("true");
}

/**
 * @tc.name: SetConfigTest2
 * @tc.desc: test results of SetConfigTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, SetConfigTest2, TestSize.Level1)
{
    SetConfigEnable("false");
}

/**
 * @tc.name: SetIdleStateTest1
 * @tc.desc: test results of SetIdleStateTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, SetIdleStateTest1, TestSize.Level1)
{
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    SetIdleStateEnable(false);
}

/**
 * @tc.name: SetIdleStateTest2
 * @tc.desc: test results of SetIdleStateTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, SetIdleStateTest2, TestSize.Level1)
{
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    SetIdleStateEnable(true);
}

/**
 * @tc.name: SetIdleStateTest3
 * @tc.desc: test results of SetIdleStateTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, SetIdleStateTest3, TestSize.Level1)
{
    SetConfigEnable("false");
    SetIdleStateEnable(false);
    SetIdleStateEnable(true);
}

/**
 * @tc.name: StatisticAnimationTimeTest1
 * @tc.desc: test results of StatisticAnimationTimeTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, StatisticAnimationTimeTest1, TestSize.Level1)
{
    SetConfigEnable("false");
    SetIdleStateEnable(false);
    uint64_t currentTime = 1719544264071;
    HgmLtpoPowerPolicy::Instance().StatisticAnimationTime(currentTime);
}

/**
 * @tc.name: StatisticAnimationTimeTest2
 * @tc.desc: test results of StatisticAnimationTimeTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, StatisticAnimationTimeTest2, TestSize.Level1)
{
    SetConfigEnable("true");
    SetIdleStateEnable(false);
    uint64_t currentTime = 1719544264071;
    HgmLtpoPowerPolicy::Instance().StatisticAnimationTime(currentTime);
}

/**
 * @tc.name: StatisticAnimationTimeTest3
 * @tc.desc: test results of StatisticAnimationTimeTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, StatisticAnimationTimeTest3, TestSize.Level1)
{
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    uint64_t currentTime = 1719544264071;
    HgmLtpoPowerPolicy::Instance().StatisticAnimationTime(currentTime);
}

/**
 * @tc.name: StartNewAnimationTest1
 * @tc.desc: test results of StartNewAnimationTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, StartNewAnimationTest1, TestSize.Level1)
{
    SetConfigEnable("true");
    HgmLtpoPowerPolicy::Instance().StartNewAnimation();
}

/**
 * @tc.name: StartNewAnimationTest2
 * @tc.desc: test results of StartNewAnimationTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, StartNewAnimationTest2, TestSize.Level1)
{
    SetConfigEnable("true");
    RsCommonHook::Instance().OnStartNewAnimation();
}

/**
 * @tc.name: GetAnimationIdleFpsTest1
 * @tc.desc: test results of GetAnimationIdleFpsTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, GetAnimationIdleFpsTest1, TestSize.Level1)
{
    SetConfigEnable("false");
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmLtpoPowerPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);
}

/**
 * @tc.name: GetAnimationIdleFpsTest2
 * @tc.desc: test results of GetAnimationIdleFpsTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, GetAnimationIdleFpsTest2, TestSize.Level1)
{
    SetConfigEnable("true");
    HgmLtpoPowerPolicy::Instance().lastAnimationTimestamp_ = 1719544264071;
    HgmLtpoPowerPolicy::Instance().firstAnimationTimestamp_ = 1719544263071;
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmLtpoPowerPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);
}

/**
 * @tc.name: GetAnimationIdleFpsTest3
 * @tc.desc: test results of GetAnimationIdleFpsTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmLtpoPowerPolicyTest, GetAnimationIdleFpsTest3, TestSize.Level1)
{
    SetConfigEnable("true");
    HgmLtpoPowerPolicy::Instance().lastAnimationTimestamp_ = 1719544264071;
    HgmLtpoPowerPolicy::Instance().firstAnimationTimestamp_ = 1719544261071;
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmLtpoPowerPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, IDLE_FPS);
    ASSERT_EQ(rsRange.min_, IDLE_FPS);
    ASSERT_EQ(rsRange.preferred_, IDLE_FPS);
}

} // namespace Rosen
} // namespace OHOS