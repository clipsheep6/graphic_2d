/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <test_header.h>

#include "platform/ohos/overdraw/rs_overdraw_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSOverdrawManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOverdrawManagerTest::SetUpTestCase() {}
void RSOverdrawManagerTest::TearDownTestCase() {}
void RSOverdrawManagerTest::SetUp() {}
void RSOverdrawManagerTest::TearDown() {}


HWTEST_F(RSOverdrawManagerTest, CreateListenerDisable, Function | SmallTest | Level2)
{
    auto &overdrawManager = RSOverdrawManager::GetInstance();
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    overdrawManager.StartOverDraw(canvas);
    overdrawManager.FinishOverDraw(canvas);
    ASSERT_EQ(overdrawManager.IsEnabled(), true);
}
} // namespace Rosen
} // namespace OHOS
