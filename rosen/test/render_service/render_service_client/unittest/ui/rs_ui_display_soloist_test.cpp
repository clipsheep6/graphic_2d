/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "ui/rs_ui_display_soloist.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplaySoloistTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplaySoloistTest::SetUpTestCase() {}
void RSDisplaySoloistTest::TearDownTestCase() {}
void RSDisplaySoloistTest::SetUp() {}
void RSDisplaySoloistTest::TearDown() {}

/**
 * @tc.name: InsertAndRemoveSoloistTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplaySoloistTest, InsertAndRemoveSoloistTest001, TestSize.Level1)
{
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    ASSERT_FALSE(soloistManager.InitVsyncReceiver());

    std::shared_ptr<SoloistId> soloistIdObj = OHOS::Rosen::SoloistId::Create();
    int32_t soloistId = soloistIdObj->GetId();
    ASSERT_EQ(soloistManager.GetIdToSoloistMap().size(), 0);
    soloistManager.InsertUseExclusiveThreadFlag(soloistId, true);
    ASSERT_EQ(soloistManager.GetIdToSoloistMap().size(), 1);

    FrameRateRange frameRateRange(0, 120, 60);
    ASSERT_TRUE(frameRateRange.IsValid());
    soloistManager.RemoveSoloist(soloistId);
}

} // namespace OHOS::Rosen
