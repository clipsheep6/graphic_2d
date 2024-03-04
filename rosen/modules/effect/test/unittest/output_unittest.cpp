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

#include "image_chain.h"
#include "output.h"
#include "filter_factory.h"
#include "output_unittest.h"
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
HWTEST_F(OutputUnittest, SetValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest SetValue001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto outputFormat = std::make_shared<std::string>("jpg");
    std::weak_ptr<void> vOutputFormat = outputFormat;
    output->SetValue("format", vOutputFormat.lock(), 1);
}

/**
 * @tc.name: SetValue002
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, SetValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest SetValue002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto outputFormat = std::make_shared<std::string>("png");
    std::weak_ptr<void> vOutputFormat = outputFormat;
    output->SetValue("format", vOutputFormat.lock(), 1);
}

/**
 * @tc.name: SetValue003
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, SetValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest SetValue003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto outputDST = std::make_shared<std::string>("test");
    std::weak_ptr<void> vOutputDST = outputDST;
    output->SetValue("dst", vOutputDST.lock(), 1);
}

/**
 * @tc.name: GetVertexShader001
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetVertexShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetVertexShader001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetVertexShader to get the strings
     */
