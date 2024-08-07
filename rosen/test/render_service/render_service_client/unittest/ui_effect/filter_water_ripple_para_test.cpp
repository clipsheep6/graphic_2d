/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "ui_effect/filter/include/filter_water_ripple_para.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class FilterWaterRippleParaTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: SetGetProgress001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FilterWaterRippleParaTest, SetGetProgress001, TestSize.Level1)
{
    std::shared_ptr<WaterRipplePara> para = std::make_shared<WaterRipplePara>();
    para->SetProgress(0.5);
    float rs = para->GetProgress();
    EXPECT_EQ(rs, 0.5);
}

/**
 * @tc.name: SetGetWaveCount001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FilterWaterRippleParaTest, SetGetWaveCount001, TestSize.Level1)
{
    std::shared_ptr<WaterRipplePara> para = std::make_shared<WaterRipplePara>();
    para->SetWaveCount(1);
    uint32_t rs = para->GetWaveCount();
    EXPECT_EQ(rs, 1);
}

/**
 * @tc.name: SetGetRippleCenterX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FilterWaterRippleParaTest, SetGetRippleCenterX001, TestSize.Level1)
{
    std::shared_ptr<WaterRipplePara> para = std::make_shared<WaterRipplePara>();
    para->SetRippleCenterX(0.5);
    float rs = para->GetRippleCenterX();
    EXPECT_EQ(rs, 0.5);
}

/**
 * @tc.name: SetGetRippleCenterY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FilterWaterRippleParaTest, SetGetRippleCenterY001, TestSize.Level1)
{
    std::shared_ptr<WaterRipplePara> para = std::make_shared<WaterRipplePara>();
    para->SetRippleCenterY(0.5);
    float rs = para->GetRippleCenterY();
    EXPECT_EQ(rs, 0.5);
}

/**
 * @tc.name: SetGetRippleMode001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FilterWaterRippleParaTest, SetGetRippleMode001, TestSize.Level1)
{
    std::shared_ptr<WaterRipplePara> para = std::make_shared<WaterRipplePara>();
    para->SetRippleMode(2);
    uint32_t rs = para->GetRippleMode();
    EXPECT_EQ(rs, 2);
}
} // namespace OHOS::Rosen
