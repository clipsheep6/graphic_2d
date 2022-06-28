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
#include "future.h"
#include "rs_interfaces_test_utils.h"
#include "screen.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "window.h"
#include "window_option.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSInterfacesSystemTest : public testing::Test {
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

sptr<Display> RSInterfacesSystemTest::defaultDisplay_ = nullptr;
DisplayId RSInterfacesSystemTest::defaultDisplayId_ = DISPLAY_ID_INVALID;
ScreenId RSInterfacesSystemTest::defaultScreenId_ = INVALID_SCREEN_ID;
std::string RSInterfacesSystemTest::defaultName_ = "virtualScreen01";
uint32_t RSInterfacesSystemTest::defaultWidth_ = 480;
uint32_t RSInterfacesSystemTest::defaultHeight_ = 320;
float RSInterfacesSystemTest::defaultDensity_ = 2.0;
int32_t RSInterfacesSystemTest::defaultFlags_ = 0;
VirtualScreenOption RSInterfacesSystemTest::defaultOption_ = {
    defaultName_, defaultWidth_, defaultHeight_, defaultDensity_, nullptr, defaultFlags_
};
const std::string defaultCmd_ = "/system/bin/snapshot_display -i ";

void RSInterfacesSystemTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultDisplayId_ = defaultDisplay_->GetId();
    defaultScreenId_ = defaultDisplay_->GetScreenId();
    defaultWidth_ = defaultDisplay_->GetWidth();
    defaultHeight_ = defaultDisplay_->GetHeight();
    defaultOption_.width_ = defaultWidth_;
    defaultOption_.height_ = defaultHeight_;
}

void RSInterfacesSystemTest::TearDownTestCase()
{
}

void RSInterfacesSystemTest::SetUp()
{
}

void RSInterfacesSystemTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ScreenManager01
 * @tc.desc: Modify expand virtualScreen resolution
 * @tc.type: FUNC
 */
HWTEST_F(RSInterfacesSystemTest, ScreenManager01, Function | MediumTest | Level2)
{
    uint32_t resolutionFirstWidth = 1920;
    uint32_t resolutionFirstHeight = 1080;
    uint32_t resolutionLastWidth = 720;
    uint32_t resolutionLastHeight = 1280;
    
    RSInterfacesTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.pSurface_;
    defaultOption_.isForShot_ = false;

    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    sleep(TEST_SLEEP_S);
      
    std::vector<ExpandOption> options = {{defaultScreenId_, 0, 0}, {virtualScreenId, defaultWidth_, 0}};
    ScreenId expansionId = ScreenManager::GetInstance().MakeExpand(options);

    sleep(TEST_SLEEP_S);
    ASSERT_NE(SCREEN_ID_INVALID, expansionId);

    auto ids = RSInterfaces::GetInstance().GetAllScreenIds();
    ScreenId rsId = ids.front();
    auto resolution = RSInterfaces::GetInstance().GetVirtualScreenResolution(rsId);
    ASSERT_EQ(resolution.GetVirtualScreenWidth(), defaultWidth_);
    ASSERT_EQ(resolution.GetVirtualScreenHeight(), defaultHeight_);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(rsId, resolutionFirstWidth, resolutionFirstHeight);
    resolution = RSInterfaces::GetInstance().GetVirtualScreenResolution(rsId);
    ASSERT_EQ(resolution.GetVirtualScreenWidth(), resolutionFirstWidth);
    ASSERT_EQ(resolution.GetVirtualScreenHeight(), resolutionFirstHeight);

    RSInterfaces::GetInstance().SetVirtualScreenResolution(rsId, resolutionLastWidth, resolutionLastHeight);
    resolution = RSInterfaces::GetInstance().GetVirtualScreenResolution(rsId);
    ASSERT_EQ(resolution.GetVirtualScreenWidth(), resolutionLastWidth);
    ASSERT_EQ(resolution.GetVirtualScreenHeight(), resolutionLastHeight);

    auto res = ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, res);
}


/**
 * @brief 
 * @tc.desc: Virtualmirrorscreen is supported to change the resolution of main screen.
 * @tc.type: FUNC
 */
HWTEST_F(RSInterfacesSystemTest, ScreenManager02, Function | MediumTest | Level2)
{
    uint32_t resolutionFirstWidth = 720;
    uint32_t resolutionFirstHeight = 400;
    
    RSInterfacesTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.pSurface_;
    defaultOption_.isForShot_ = false;

    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    sleep(TEST_SLEEP_S);
      
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);
    sleep(TEST_SLEEP_S);
    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);

    auto ids = RSInterfaces::GetInstance().GetAllScreenIds();
    ScreenId rsId = ids.front();
    auto resolution = RSInterfaces::GetInstance().GetVirtualScreenResolution(rsId);
    ASSERT_EQ(resolution.GetVirtualScreenWidth(), defaultWidth_);
    ASSERT_EQ(resolution.GetVirtualScreenHeight(), defaultHeight_);
    
    ScreenId mainId = RSInterfaces::GetInstance().GetDefaultScreenId();
    RSScreenModeInfo screenModeInfo = RSInterfaces::GetInstance().GetScreenActiveMode(mainId);
    std::cout<<"mainDefaultResolution:"<<screenModeInfo.GetScreenWidth()<<" "<<screenModeInfo.GetScreenHeight()<<std::endl;
    RSInterfaces::GetInstance().SetVirtualScreenResolution(rsId, resolutionFirstWidth, resolutionFirstHeight);
    resolution = RSInterfaces::GetInstance().GetVirtualScreenResolution(rsId);
    ASSERT_EQ(resolution.GetVirtualScreenWidth(), resolutionFirstWidth);
    ASSERT_EQ(resolution.GetVirtualScreenHeight(), resolutionFirstHeight);
    RSInterfaces::GetInstance().SetVirtualScreenResolution(rsId, defaultWidth_, defaultHeight_);

    auto res = ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: ScreenManager03
 * @tc.desc: Physical mirrorscreen is supported to change the resolution of main screen.
 * @tc.type: FUNC
 */
HWTEST_F(RSInterfacesSystemTest, ScreenManager03, Function | MediumTest | Level2)
{
    ScreenId mainId = RSInterfaces::GetInstance().GetDefaultScreenId();
    sleep(TEST_SLEEP_S);

    auto ids = RSInterfaces::GetInstance().GetAllScreenIds();
    ScreenId mirrorId = ids.front();
    for (uint32_t k = 0; k < ids.size(); k++) {
        if (ids[k] != mainId) {
            mirrorId = ids[k];
            break;
        }
    }
    
    RSScreenModeInfo mainDefaultMode = RSInterfaces::GetInstance().GetScreenActiveMode(mainId);
    std::cout<<"MainScreen:"<<mainId<<" "<<mainDefaultMode.GetScreenWidth()<<" "<<mainDefaultMode.GetScreenHeight()<<std::endl;
    RSScreenModeInfo mirrorDefaultMode = RSInterfaces::GetInstance().GetScreenActiveMode(mirrorId);
    std::cout<<"MirrorScreen:"<<mirrorId<<" "<<mirrorDefaultMode.GetScreenWidth()<<" "<<mirrorDefaultMode.GetScreenHeight()<<std::endl;
    auto supportModes = RSInterfaces::GetInstance().GetScreenSupportedModes(mirrorId);
    for(uint32_t i = 0; i < supportModes.size(); i++){
        if (supportModes[i].GetScreenWidth() != mirrorDefaultMode.GetScreenWidth()) {
            std::cout<<"SetResolution:"<<i<<" "<<supportModes[i].GetScreenWidth()<<" "<<supportModes[i].GetScreenHeight()<<std::endl;
            RSInterfaces::GetInstance().SetScreenActiveMode(mirrorId, i);
            break;
        }    
    }
}
}
} // namespace Rosen
} // namespace OHOS
