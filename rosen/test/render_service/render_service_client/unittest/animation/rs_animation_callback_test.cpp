/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "animation/rs_animation_callback.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyTest::SetUpTestCase() {}
void RSPropertyTest::TearDownTestCase() {}
void RSPropertyTest::SetUp() {}
void RSPropertyTest::TearDown() {}

/**
 * @tc.name: AnimationFinishCallback::Execute001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(AnimationFinishCallbackTest, Execute001, Level1)
{

    AnimationFinishCallback::Execute()
}

/**
 * @tc.name: AnimationRepeatCallback::Execute001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(AnimationRepeatCallbackTest, Execute001, Level1)
{
    
    AnimationRepeatCallback::Execute()
}