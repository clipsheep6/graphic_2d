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

#include "hdi_layer.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiLayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiLayer> hdiLayer_;
};

void HdiLayerTest::SetUpTestCase()
{
    hdiLayer_ = HdiLayer::CreateHdiLayer(0);
    hdiLayer_->Init();
    LayerInfoPtr layerInfo;
    hdiLayer_->UpdateLayerInfo(layerInfo);
}

void HdiLayerTest::TearDownTestCase()
{

}

namespace {
/**
 * @tc.name: GetLayerStatus001
 * @tc.desc: Verify the GetLayerStatus of hdilayer
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerTest, GetLayerStatus001, Function | MediumTest| Level0)
{
    bool isUsing = true;
    HdiLayerTest::hdiLayer_->SetLayerStatus(isUsing);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->GetLayerStatus(), true);

    isUsing = false;
    HdiLayerTest::hdiLayer_->SetLayerStatus(isUsing);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->GetLayerStatus(), false);
}

/**
 * @tc.name: GetLayerId001
 * @tc.desc: Verify the GetLayerId of hdilayer
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerTest, GetLayerId001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiLayerTest::hdiLayer_->GetLayerId(), 0u);
}
} // namespace

} // namespace Rosen
} // namespace OHOS