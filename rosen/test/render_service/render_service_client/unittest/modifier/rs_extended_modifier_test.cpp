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
#include "common/rs_common_def.h"
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSExtendedModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSExtendedModifierTest::SetUpTestCase() {}
void RSExtendedModifierTest::TearDownTestCase() {}
void RSExtendedModifierTest::SetUp() {}
void RSExtendedModifierTest::TearDown() {}

/**
 * @tc.name: CreateDrawingContext001
 * @tc.desc: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSExtendedModifierTest, CreateDrawingContext001, TestSize.Level1)
{
    NodeId id = 0;
    RSDrawingContext context = RSExtendedModifierHelper::CreateDrawingContext(id);
    ASSERT_EQ(nullptr, context);
}
    
} // namespace OHOS::Rosen
