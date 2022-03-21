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
 * @tc.name: UtilsSamplingOptionsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest001, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
}

/**
 * @tc.name: UtilsSamplingOptionsTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest002, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
}

/**
 * @tc.name: UtilsSamplingOptionsTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest003, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(false, sampling->GetUseCubic());
}

/**
 * @tc.name: UtilsSamplingOptionsTest004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest004, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(FilterMode::NEAREST, sampling->GetFilterMode());
}

/**
 * @tc.name: UtilsSamplingOptionsTest005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest005, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(MipmapMode::NONE, sampling->GetMipmapMode());
}

/**
 * @tc.name: UtilsSamplingOptionsTest006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest006, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffB());
}

/**
 * @tc.name: UtilsSamplingOptionsTest007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest007, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffB());
}

/**
 * @tc.name: UtilsSamplingOptionsTest008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest008, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffC());
}

/**
 * @tc.name: UtilsSamplingOptionsTest009
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest009, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffC());
}

/**
 * @tc.name: UtilsSamplingOptionsTest010
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest010, TestSize.Level1)
{
    SamplingOptions sampling1;
    ASSERT_TRUE(sampling1 == sampling1);
}

/**
 * @tc.name: UtilsSamplingOptionsTest011
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest011, TestSize.Level1)
{
    SamplingOptions sampling1;
    SamplingOptions sampling2 = sampling1;
    EXPECT_FALSE(sampling2 != sampling1);
}

/**
 * @tc.name: UtilsSamplingOptionsTest012
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest012, TestSize.Level1)
{
    SamplingOptions sampling1;
    SamplingOptions sampling2 = sampling1;
    EXPECT_FALSE(sampling2 != sampling1);
}

/**
 * @tc.name: UtilsSamplingOptionsTest013
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, UtilsSamplingOptionsTest013, TestSize.Level1)
{
    SamplingOptions sampling1;
    SamplingOptions sampling2;
    EXPECT_FALSE(sampling1 != sampling2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS