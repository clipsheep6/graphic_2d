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

#include "pipeline/rs_render_display_sync.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderDisplaySyncTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderDisplaySyncTest::SetUpTestCase() {}
void RSRenderDisplaySyncTest::TearDownTestCase() {}
void RSRenderDisplaySyncTest::SetUp() {}
void RSRenderDisplaySyncTest::TearDown() {}

/**
 * @tc.name: GetId
 * @tc.desc: Test GetId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderDisplaySyncTest, GetId, TestSize.Level1)
{
    uint64_t id = 1;
    auto displaySync = std::make_shared<RSRenderDisplaySync>(id);
    ASSERT_NE(displaySync, nullptr);
    EXPECT_EQ(displaySync->GetId(), 1);
}

/**
 * @tc.name: SetExpectedFrameRateRange
 * @tc.desc: Test SetExpectedFrameRateRange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderDisplaySyncTest, SetExpectedFrameRateRange, TestSize.Level1)
{
    uint64_t id = 1;
    auto displaySync = std::make_shared<RSRenderDisplaySync>(id);
    ASSERT_NE(displaySync, nullptr);
    FrameRateRange range = {0, 120, 60};
    displaySync->SetExpectedFrameRateRange(range);
    EXPECT_EQ(displaySync->GetExpectedFrameRange(), range);
}

/**
 * @tc.name: SetAnimateResult
 * @tc.desc: Test SetAnimateResult
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderDisplaySyncTest, SetAnimateResult, TestSize.Level1)
{
    uint64_t id = 1;
    auto displaySync = std::make_shared<RSRenderDisplaySync>(id);
    ASSERT_NE(displaySync, nullptr);
    std::tuple<bool, bool, bool> result = {true, false, true};
    displaySync->SetAnimateResult(result);
    EXPECT_EQ(displaySync->GetAnimateResult(), result);
}

/**
 * @tc.name: OnFrameSkip
 * @tc.desc: Test OnFrameSkip
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderDisplaySyncTest, OnFrameSkip, TestSize.Level1)
{
    uint64_t id = 1;
    auto displaySync = std::make_shared<RSRenderDisplaySync>(id);
    ASSERT_NE(displaySync, nullptr);

    FrameRateRange range = {0, 120, 60};
    displaySync->SetExpectedFrameRateRange(range);

    uint64_t timestamp = 107265354180;
    int64_t period = 8333333;
    bool isDisplaySyncEnabled = true;
    bool isFrameSkip = displaySync->OnFrameSkip(timestamp, period, isDisplaySyncEnabled);
    EXPECT_EQ(isFrameSkip, false);

    timestamp = 107273687513;
    isFrameSkip = displaySync->OnFrameSkip(timestamp, period, isDisplaySyncEnabled);
    EXPECT_EQ(isFrameSkip, true);

    timestamp = 107293131957;
    period = 11111111;
    isFrameSkip = displaySync->OnFrameSkip(timestamp, period, isDisplaySyncEnabled);
    EXPECT_EQ(isFrameSkip, false);

    timestamp = 107304243068;
    isDisplaySyncEnabled = false;
    isFrameSkip = displaySync->OnFrameSkip(timestamp, period, isDisplaySyncEnabled);
    EXPECT_EQ(isFrameSkip, false);

    timestamp = 107304243068;
    isDisplaySyncEnabled = true;
    isFrameSkip = displaySync->OnFrameSkip(timestamp, period, isDisplaySyncEnabled);
    EXPECT_EQ(isFrameSkip, false);
}
} // namespace OHOS::Rosen