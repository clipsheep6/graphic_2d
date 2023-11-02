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
#include "limit_number.h"

#include "touch_screen/touch_screen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TouchScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TouchScreenTest::SetUpTestCase() {}
void TouchScreenTest::TearDownTestCase() {}
void TouchScreenTest::SetUp() {}
void TouchScreenTest::TearDown() {}

/*
 * @tc.name: InitTouchScreen_001
 * @tc.desc: Test InitTouchScreen
 * @tc.type: FUNC
 * @tc.require: issueI8ATD6
 */
HWTEST_F(TouchScreenTest, InitTouchScreen_001, TestSize.Level1)
{
    TOUCH_SCREEN->InitTouchScreen();
    ASSERT_NE(nullptr, TOUCH_SCREEN->tsSetFeatureConfig_);
}

}