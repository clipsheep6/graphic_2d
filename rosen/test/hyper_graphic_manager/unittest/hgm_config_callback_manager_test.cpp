/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <test_header.h>

#include "hgm_config_callback_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class HgmConfigCallbackManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HgmConfigCallbackManagerTest::SetUpTestCase() {}
void HgmConfigCallbackManagerTest::TearDownTestCase() {}
void HgmConfigCallbackManagerTest::SetUp() {}
void HgmConfigCallbackManagerTest::TearDown() {}

class TestCallback : public RSIHgmConfigChangeCallback {
public:
    ~TestCallback() = default;
    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) {}
    void OnHgmRefreshRateModeChanged(int32_t refreshRateModeName) {}
    sptr<IRemoteObject> AsObject()
    {
        return nullptr;
    };
};

/**
 * @tc.name: RegisterHgmConfigChangeCallback
 * @tc.desc: Verify the result of RegisterHgmConfigChangeCallback function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmConfigCallbackManagerTest, RegisterHgmConfigChangeCallback, Function | SmallTest | Level1)
{
    pid_t pid1 = 0;
    pid_t pid2 = 1;
    int32_t refreshRateMode = 1;
    auto hgmConfigCallbackManager = HgmConfigCallbackManager::GetInstance();
    sptr<RSIHgmConfigChangeCallback> testCallback1 = new TestCallback();
    sptr<RSIHgmConfigChangeCallback> testCallback2 = new TestCallback();
    hgmConfigCallbackManager->RegisterHgmConfigChangeCallback(pid1, testCallback1);
    hgmConfigCallbackManager->RegisterHgmRefreshRateModeChangeCallback(pid1, testCallback1);
    hgmConfigCallbackManager->SyncHgmConfigChangeCallback();
    hgmConfigCallbackManager->SyncRefreshRateModeChangeCallback(refreshRateMode);
    hgmConfigCallbackManager->RegisterHgmConfigChangeCallback(pid2, testCallback2);
    hgmConfigCallbackManager->RegisterHgmRefreshRateModeChangeCallback(pid2, testCallback2);
    hgmConfigCallbackManager->SyncHgmConfigChangeCallback();
    hgmConfigCallbackManager->SyncRefreshRateModeChangeCallback(refreshRateMode);
    hgmConfigCallbackManager->UnRegisterHgmConfigChangeCallback(0);
    hgmConfigCallbackManager->UnRegisterHgmConfigChangeCallback(1);
}
} // namespace Rosen
} // namespace OHOS