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
#include "ui_effect/filter/include/filter_fly_out_para.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class FilterFlyOutParaTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: SetGetDegree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FilterFlyOutParaTest, SetGetDegree001, TestSize.Level1)
{
    std::shared_ptr<FlyOutPara> para = std::make_shared<FlyOutPara>();
    para->SetDegree(0.5);
    float rs = para->GetDegree();
    EXPECT_EQ(rs, 0.5);
}

/**
 * @tc.name: SetGetFlyMode001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIShareContextTest, SetGetFlyMode001, TestSize.Level1)
{
    std::shared_ptr<FlyOutPara> para = std::make_shared<FlyOutPara>();
    para->SetFlyMode(1);
    uint32_t rs = para->GetFlyMode();
    EXPECT_EQ(rs, 1);
}
} // namespace OHOS::Rosen
