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
#include "modifier/rs_modifier_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSModifierManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierManagerTest::SetUpTestCase() {}
void RSModifierManagerTest::TearDownTestCase() {}
void RSModifierManagerTest::SetUp() {}
void RSModifierManagerTest::TearDown() {}

/**
 * @tc.name: Draw001
 * @tc.desc: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSModifierManagerTest, Draw, TestSize.Level1)
{
    std::shared_ptr<RSModifierManager> manager = std::make_shared<RSModifierManager>();
    manager.Draw();
    std::shared_ptr<RSModifierBase> modifier = std::make_shared<RSModifierBase>();
    manager.AddModifier(modifier);
    manager.Draw();
}
    
} // namespace OHOS::Rosen
