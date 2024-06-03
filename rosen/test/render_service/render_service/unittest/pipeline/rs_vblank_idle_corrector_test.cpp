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

#include "pipeline/rs_vblank_idle_corrector.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSVBlankIdleCorrectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSVBlankIdleCorrectorTest::SetUpTestCase() {}
void RSVBlankIdleCorrectorTest::TearDownTestCase() {}
void RSVBlankIdleCorrectorTest::SetUp() {}
void RSVBlankIdleCorrectorTest::TearDown() {}

/**
 * @tc.name: SetScreenVBlankIdle001
 * @tc.desc: test RSVBlankIdleCorrectorTest.SetScreenVBlankIdle001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVBlankIdleCorrectorTest, SetScreenVBlankIdle001, TestSize.Level1)
{
    auto rsVBlankIdleCorrector = std::make_shared<RSVBlankIdleCorrector>();
    ASSERT_NE(nullptr, rsVBlankIdleCorrector);
    // device id is 0
    rsVBlankIdleCorrector->SetScreenVBlankIdle(0);
}
} // namespace OHOS::Rosen