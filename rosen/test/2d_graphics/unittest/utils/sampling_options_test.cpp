/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "utils/sampling_options.h"
#include "utils/scalar.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SamplingOptionsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SamplingOptionsTest::SetUpTestCase() {}
void SamplingOptionsTest::TearDownTestCase() {}
void SamplingOptionsTest::SetUp() {}
void SamplingOptionsTest::TearDown() {}

/**
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, CreateAndDestory001, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    SamplingOptions samplingOptions_;
}

/**
 * @tc.name: CreateAndDestory002
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, CreateAndDestory002, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    std::unique_ptr<SamplingOptions> samplingOperationPtr_ = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(samplingOperationPtr_ != nullptr);
}

/**
 * @tc.name: CreateAndDestory008
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, CreateAndDestory008, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    CubicResampler c;
    SamplingOptions s(c);
}

/**
 * @tc.name: GetUseCubic001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetUseCubic001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> samplingOperationPtr_ = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(samplingOperationPtr_ != nullptr);
    samplingOperationPtr_->GetUseCubic();
}

/**
 * @tc.name: GetFilterMode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetFilterMode001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> samplingOperationPtr_ = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(samplingOperationPtr_ != nullptr);
    samplingOperationPtr_->GetFilterMode();
}

/**
 * @tc.name: GetMipmapMode001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetMipmapMode001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> samplingOperationPtr_ = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(samplingOperationPtr_ != nullptr);
    samplingOperationPtr_->GetMipmapMode();
}

/**
 * @tc.name: GetCubicCoffB001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetCubicCoffB001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> samplingOperationPtr_ = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(samplingOperationPtr_ != nullptr);
    samplingOperationPtr_->GetCubicCoffB();
}

/**
 * @tc.name: GetCubicCoffC001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetCubicCoffC001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> samplingOperationPtr_ = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(samplingOperationPtr_ != nullptr);
    samplingOperationPtr_->GetCubicCoffC();
}

/**
 * @tc.name: OperatorEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, OperatorEEqual001, TestSize.Level1)
{
    SamplingOptions s1;
    SamplingOptions s2;
    ASSERT_TRUE(s1 == s2);
}

/**
 * @tc.name: OperatorNotEEqual001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, OperatorNotEEqual001, TestSize.Level1)
{
    SamplingOptions s1;
    SamplingOptions s2;
    ASSERT_TRUE(s1 != s2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS