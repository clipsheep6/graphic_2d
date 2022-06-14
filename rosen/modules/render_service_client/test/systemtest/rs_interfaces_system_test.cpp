/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

// gtest
#include <gtest/gtest.h>
#include "rs_interfaces_test_utils.h"
#include "future.h"
#include "screen.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "window.h"
#include "window_option.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSInterfaceSystemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static sptr<Display> defaultDisplay_;
    static DisplayId defaultDisplayId_;
    static ScreenId defaultScreenId_;
    static std::string defaultName_;
    static uint32_t defaultWidth_;
    static uint32_t defaultHeight_;
    static float defaultDensity_;
    static int32_t defaultFlags_;
    static VirtualScreenOption defaultOption_;
    static constexpr uint32_t TEST_SLEEP_S = 1; // test sleep time
};

sptr<Display> RSInterfaceSystemTest::defaultDisplay_ = nullptr;
DisplayId RSInterfaceSystemTest::defaultDisplayId_ = DISPLAY_ID_INVALID;
ScreenId RSInterfaceSystemTest::defaultScreenId_ = INVALID_SCREEN_ID;
std::string RSInterfaceSystemTest::defaultName_ = "virtualScreen01";
uint32_t RSInterfaceSystemTest::defaultWidth_ = 480;
uint32_t RSInterfaceSystemTest::defaultHeight_ = 320;
float RSInterfaceSystemTest::defaultDensity_ = 2.0;
int32_t RSInterfaceSystemTest::defaultFlags_ = 0;
VirtualScreenOption RSInterfaceSystemTest::defaultOption_ = {
    defaultName_, defaultWidth_, defaultHeight_, defaultDensity_, nullptr, defaultFlags_
};
const std::string defaultCmd_ = "/system/bin/snapshot_display -i ";

void RSInterfaceSystemTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultDisplayId_ = defaultDisplay_->GetId();
    defaultScreenId_ = defaultDisplay_->GetScreenId();
    defaultWidth_ = defaultDisplay_->GetWidth();
    defaultHeight_ = defaultDisplay_->GetHeight();
    defaultOption_.width_ = defaultWidth_;
    defaultOption_.height_ = defaultHeight_;
}

void RSInterfaceSystemTest::TearDownTestCase()
{
}

void RSInterfaceSystemTest::SetUp()
{
}

void RSInterfaceSystemTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ScreenManager01
 * @tc.desc: Modify expand virtualScreen resolution
 * @tc.type: FUNC
 */
HWTEST_F(RSInterfaceSystemTest, ScreenManager01, Function | MediumTest | Level2)
{
    RSInterfaceTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;

    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    sleep(TEST_SLEEP_S);
      
    std::vector<ExpandOption> options = {{defaultScreenId_, defaultWidth_, 0}, {virtualScreenId, 0, 0}};
    ScreenId expansionId = ScreenManager::GetInstance().MakeExpand(options);

    sleep(TEST_SLEEP_S);
    ASSERT_NE(SCREEN_ID_INVALID, expansionId);

    DisplayId secDisplayId = DisplayManager::GetInstance().GetDisplayByScreen(virtualScreenId)->GetId();
    ASSERT_NE(DISPLAY_ID_INVALID, secDisplayId);
    sptr<Window> window = utils.CreateWindowByDisplayParam(secDisplayId, "VirtualWindow01", 0, 0, 720, 1280);
    sleep(TEST_SLEEP_S);
    auto surfaceNode = window->GetSurfaceNode();
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(TEST_SLEEP_S);
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    utils.RootNodeInit(rsUiDirector, 720, 1280);
    rsUiDirector->SendMessages();
    sleep(TEST_SLEEP_S);
    window->Show();

    auto tmpCmd = defaultCmd_ + std::to_string(secDisplayId);

    auto ids = RSInterfaces::GetInstance().GetAllScreenIds();
    ScreenId rsId = ids.front();
    auto resolution = RSInterfaces::GetInstance().GetVirtualScreenResolution(rsId);
    ASSERT_EQ(resolution.GetVirtualScreenWidth(), defaultWidth_);
    ASSERT_EQ(resolution.GetVirtualScreenHeight(), defaultHeight_);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(rsId, 1920, 1080);
    sleep(2);
    resolution = RSInterfaces::GetInstance().GetVirtualScreenResolution(rsId);
    ASSERT_EQ(resolution.GetVirtualScreenWidth(), 1920);
    ASSERT_EQ(resolution.GetVirtualScreenHeight(), 1080);
    (void)system(tmpCmd.c_str());

    sleep(2);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(rsId, 720, 1280);
    sleep(2);
    resolution = RSInterfaces::GetInstance().GetVirtualScreenResolution(rsId);
    ASSERT_EQ(resolution.GetVirtualScreenWidth(), 720);
    ASSERT_EQ(resolution.GetVirtualScreenHeight(), 1280);
    (void)system(tmpCmd.c_str());

    sleep(2);

    window->Destroy();
    auto res = ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, res);
    sleep(TEST_SLEEP_S);
}
}
} // namespace Rosen
} // namespace OHOS
