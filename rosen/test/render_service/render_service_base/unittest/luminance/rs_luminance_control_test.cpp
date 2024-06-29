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
 * @tc.name: DimmingIncrease
 * @tc.desc: test results of DimmingIncrease
 * @tc.type: FUNC
 */
HWTEST_F(RSLuminanceControlTest, DimmingIncreaseTest001, TestSize.Level1)
{
    RSLuminanceControl rsLuminanceControl; // 创建实例

    rsLuminanceControl.initStatus_ = true;
    rsLuminanceControl.dimmingIncrease_ = [](int x) ->void {x++;};
    rsLuminanceControl.DimmingIncrease(0);
    EXPECT_TRUE(rsLuminanceControl.initStatus_ && rsLuminanceControl.dimmingIncrease_!=nullptr);

    rsLuminanceControl.initStatus_ = true;
    rsLuminanceControl.dimmingIncrease_ = nullptr;
    rsLuminanceControl.DimmingIncrease(0);
    EXPECT_TRUE(rsLuminanceControl.initStatus_ && rsLuminanceControl.dimmingIncrease_==nullptr);

    rsLuminanceControl.initStatus_ = false;
    rsLuminanceControl.dimmingIncrease_ = [](int x) ->void {x++;};
    rsLuminanceControl.DimmingIncrease(0);
    EXPECT_TRUE(!rsLuminanceControl.initStatus_ && rsLuminanceControl.dimmingIncrease_!=nullptr);

    rsLuminanceControl.initStatus_ = false;
    rsLuminanceControl.dimmingIncrease_ =nullptr;
    rsLuminanceControl.DimmingIncrease(0);
    EXPECT_TRUE(!rsLuminanceControl.initStatus_ && rsLuminanceControl.dimmingIncrease_==nullptr);
}
}