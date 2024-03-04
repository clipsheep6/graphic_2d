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

#include "saturation_filter_unittest.h"
#include "saturation_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: SetValue001
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SaturationFilterUnittest, SetValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SaturationFilterUnittest SetValue001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto saturation = std::make_shared<SaturationFilter>();
    bool testResult = saturation != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sSaturation = std::make_shared<float>(0.0f);
    std::weak_ptr<void> vSaturation = sSaturation;
    saturation->SetValue("saturation", vSaturation.lock(), 1);
}

/**
 * @tc.name: SetValue002
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SaturationFilterUnittest, SetValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SaturationFilterUnittest SetValue002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto saturation = std::make_shared<SaturationFilter>();
    bool testResult = saturation != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sSaturation = std::make_shared<float>(0.5f);
    std::weak_ptr<void> vSaturation = sSaturation;
    saturation->SetValue("saturation", vSaturation.lock(), 1);
}

/**
 * @tc.name: SetValue003
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SaturationFilterUnittest, SetValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SaturationFilterUnittest SetValue003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto saturation = std::make_shared<SaturationFilter>();
    bool testResult = saturation != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sSaturation = std::make_shared<float>(1.0f);
    std::weak_ptr<void> vSaturation = sSaturation;
    saturation->SetValue("saturation", vSaturation.lock(), 1);
}

/**
 * @tc.name: GetVertexShader001
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SaturationFilterUnittest, GetVertexShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SaturationFilterUnittest GetVertexShader001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto saturation = std::make_shared<SaturationFilter>();
    bool testResult = saturation != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetVertexShader to get the strings
     */
