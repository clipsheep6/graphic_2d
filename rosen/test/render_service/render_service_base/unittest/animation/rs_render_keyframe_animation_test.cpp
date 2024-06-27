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
#include "luminance/rs_luminance_control.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLuminanceControlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLuminanceControlTest::SetUpTestCase() {}
void RSLuminanceControlTest::TearDownTestCase() {}
void RSLuminanceControlTest::SetUp() {}
void RSLuminanceControlTest::TearDown() {}

/**
 * @tc.name: GetTest
 * @tc.desc: test results of GetTest
 * @tc.type: FUNC
 */
HWTEST_F(RSLuminanceControlTest, GetTest001, TestSize.Level1)
{
    auto instance = RSLuminanceControl::Get();
    EXPECT_NE(instance, nullptr);
}

/**
 * @tc.name: InitTest
 * @tc.desc: test results of InitTest
 * @tc.type: FUNC
 */
HWTEST_F(RSLuminanceControlTest, InitTest001, TestSize.Level1)
{
    RSLuminanceControl rsLuminanceControl;
    rsLuminanceControl.Init();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetHdrStatus
 * @tc.desc: test results of SetHdrStatus
 * @tc.type: FUNC
 */
HWTEST_F(RSLuminanceControlTest, SetHdrStatusTest001, TestSize.Level1)
{
    RSLuminanceControl rsLuminanceControl;
    int32_t type = HDR_TYPE::PHOTO;
    rsLuminanceControl.SetHdrStatus(-1, true, type);
    rsLuminanceControl.SetHdrStatus(0, true, type);
    rsLuminanceControl.SetHdrStatus(1, true, type);
    rsLuminanceControl.SetHdrStatus(-1, false, type);
    rsLuminanceControl.SetHdrStatus(0, false, type);
    rsLuminanceControl.SetHdrStatus(1, false, type);
    type = HDR_TYPE::VIDEO;
    rsLuminanceControl.SetHdrStatus(-1, true, type);
    rsLuminanceControl.SetHdrStatus(0, true, type);
    rsLuminanceControl.SetHdrStatus(1, true, type);
    rsLuminanceControl.SetHdrStatus(-1, false, type);
    rsLuminanceControl.SetHdrStatus(0, false, type);
    rsLuminanceControl.SetHdrStatus(1, false, type);

    EXPECT_TRUE(true);
}

/**
 * @tc.name: IsHdrOn
 * @tc.desc: test results of IsHdrOn
 * @tc.type: FUNC
 */
HWTEST_F(RSLuminanceControlTest, IsHdrOnTest001, TestSize.Level1)
{
    RSLuminanceControl rsLuminanceControl;
    rsLuminanceControl.IsHdrOn(-1);
    rsLuminanceControl.IsHdrOn(0);
    rsLuminanceControl.IsHdrOn(1);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: IsDimmingOn
 * @tc.desc: test results of IsDimmingOn
 * @tc.type: FUNC
 */
HWTEST_F(RSLuminanceControlTest, IsDimmingOnTest001, TestSize.Level1)
{
    RSLuminanceControl rsLuminanceControl;
    rsLuminanceControl.IsDimmingOn(-1);
    rsLuminanceControl.IsDimmingOn(0);
    rsLuminanceControl.IsDimmingOn(1);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DimmingIncrease
 * @tc.desc: test results of DimmingIncrease
 * @tc.type: FUNC
 */
HWTEST_F(RSLuminanceControlTest, DimmingIncreaseTest001, TestSize.Level1)
{
    RSLuminanceControl rsLuminanceControl;
    rsLuminanceControl.DimmingIncrease(-1);
    rsLuminanceControl.DimmingIncrease(0);
    rsLuminanceControl.DimmingIncrease(1);
    EXPECT_TRUE(true);
}
}