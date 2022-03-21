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
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, CreateAndDestory001, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
}

/**
 * @tc.name: CreateAndDestory003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, CreateAndDestory002, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
}

/**
 * @tc.name: GetUseCubic001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetUseCubic001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(false, sampling->GetUseCubic());
}

/**
 * @tc.name: GetUseCubic002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetUseCubic002, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_FALSE(sampling->GetUseCubic());
}

/**
 * @tc.name: GetFilterMode001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetFilterMode001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(FilterMode::NEAREST, sampling->GetFilterMode());
}

/**
 * @tc.name: GetFilterMode002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetFilterMode002, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(sampling->GetFilterMode(), FilterMode::NEAREST);
}

/**
 * @tc.name: GetMipmapMode001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetMipmapMode001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(MipmapMode::NONE, sampling->GetMipmapMode());
}

/**
 * @tc.name: GetMipmapMode002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetMipmapMode002, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(sampling->GetMipmapMode(), MipmapMode::NONE);
}

/**
 * @tc.name: GetCubicCoffB001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetCubicCoffB001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffB());
}

/**
 * @tc.name: GetCubicCoffB002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetCubicCoffB002, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffB());
}

/**
 * @tc.name: GetCubicCoffC001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetCubicCoffC001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffC());
}

/**
 * @tc.name: GetCubicCoffC002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, GetCubicCoffC002, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffC());
}

/**
 * @tc.name: OperatorEEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, OperatorEEqual001, TestSize.Level1)
{
    SamplingOptions sampling1;
    SamplingOptions sampling2 = sampling1;
    ASSERT_TRUE(sampling2 != sampling1);
}

/**
 * @tc.name: OperatorEEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, OperatorEEqual002, TestSize.Level1)
{
    SamplingOptions sampling1;
    ASSERT_TRUE(sampling1 == sampling1);
}

/**
 * @tc.name: OperatorNotEEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, OperatorNotEEqual001, TestSize.Level1)
{
    SamplingOptions sampling1;
    SamplingOptions sampling2 = sampling1;
    EXPECT_FALSE(sampling2 != sampling1);
}

/**
 * @tc.name: OperatorNotEEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, OperatorNotEEqual002, TestSize.Level1)
{
    SamplingOptions sampling1;
    SamplingOptions sampling2;
    EXPECT_FALSE(sampling1 != sampling2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS