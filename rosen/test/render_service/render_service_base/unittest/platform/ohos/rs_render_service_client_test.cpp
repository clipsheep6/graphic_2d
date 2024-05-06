/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "transaction/rs_render_service_client.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "ui/rs_surface_node.h"
#include "surface_utils.h"
#include <iostream>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
static constexpr uint32_t SET_REFRESHRATE_SLEEP_US = 50000;  // wait for refreshrate change
class RSClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSRenderServiceClient> rsClient = nullptr;
};

void RSClientTest::SetUpTestCase()
{
    rsClient = std::make_shared<RSRenderServiceClient>();
}
void RSClientTest::TearDownTestCase() {}
void RSClientTest::SetUp() {}
void RSClientTest::TearDown()
{
    usleep(SET_REFRESHRATE_SLEEP_US);
}

/**
 * @tc.name: CreateNode Test
 * @tc.desc: CreateNode Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, CreateNode_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceRenderNodeConfig config;
    bool ret = rsClient->CreateNode(config);
    ASSERT_EQ(ret, true);
}

class TestSurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    explicit TestSurfaceCaptureCallback() {}
    ~TestSurfaceCaptureCallback() override {}
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
    }
};

/**
 * @tc.name: TakeSurfaceCapture_Test
 * @tc.desc: Test capture twice with same id
 * @tc.type:FUNC
 * @tc.require: I6X9V1
 */
HWTEST_F(RSClientTest, TakeSurfaceCapture_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    std::shared_ptr<TestSurfaceCaptureCallback> cb = std::make_shared<TestSurfaceCaptureCallback>();
    bool ret = rsClient->TakeSurfaceCapture(123, cb, 1.0f, 1.0f); // test a notfound number: 123
    ASSERT_EQ(ret, true);
    ret = rsClient->TakeSurfaceCapture(123, cb, 1.0f, 1.0f); // test number: 123 twice
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: TakeSurfaceCapture Test nullptr
 * @tc.desc: TakeSurfaceCapture Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, TakeSurfaceCapture_Nullptr, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool ret = rsClient->TakeSurfaceCapture(123, nullptr, 1.0f, 1.0f); // NodeId: 123
    ASSERT_NE(ret, true);
}

/**
 * @tc.name: TakeSurfaceCapture Test nullptr
 * @tc.desc: TakeSurfaceCapture Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, TakeSurfaceCapture01, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool ret = rsClient->TakeSurfaceCapture(123, nullptr, 1.0f, 1.0f, SurfaceCaptureType::DEFAULT_CAPTURE, true);
    ASSERT_NE(ret, true);
}

/**
 * @tc.name: RegisterBufferAvailableListener Test a notfound id True
 * @tc.desc: RegisterBufferAvailableListener Test a notfound id True
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_True, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    rsClient->RegisterBufferAvailableListener(123, cb, true); // test a notfound number: 123
}

/**
 * @tc.name: RegisterBufferAvailableListener Test a notfound id False
 * @tc.desc: RegisterBufferAvailableListener Test a notfound id False
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_False, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    rsClient->RegisterBufferAvailableListener(123, cb, false); // test a notfound number: 123
}

/**
 * @tc.name: RegisterBufferAvailableListener Test nullptr
 * @tc.desc: RegisterBufferAvailableListener Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_Nullptr, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->RegisterBufferAvailableListener(123, nullptr, false); // NodeId: 123
}

/**
 * @tc.name: UnregisterBufferAvailableListener Test a notfound id
 * @tc.desc: UnregisterBufferAvailableListener Test a notfound id
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, UnregisterBufferAvailableListener_False, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    bool ret = rsClient->UnregisterBufferAvailableListener(123); // test a notfound number: 123
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: RegisterApplicationAgent Test nullptr
 * @tc.desc: RegisterApplicationAgent Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterApplicationAgent_Nullptr, TestSize.Level1)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    ASSERT_NE(renderService, nullptr);
    renderService->RegisterApplicationAgent(123, nullptr); // pid: 123
}

/**
 * @tc.name: CreateVirtualScreen Test
 * @tc.desc: CreateVirtualScreen Test
 * @tc.type:FUNC
 * @tc.require: issueI8FSLX
 */
HWTEST_F(RSClientTest, CreateVirtualScreen, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    std::vector<NodeId> filteredAppVector = {1};
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtual0", 320, 180, nullptr, INVALID_SCREEN_ID, -1, filteredAppVector);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
}

/**
 * @tc.name: SetVirtualScreenUsingStatus Test
 * @tc.desc: Test SetVirtualScreenUsingStatus while input is true
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetVirtualScreenUsingStatus001, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetVirtualScreenUsingStatus(true);
}

/**
 * @tc.name: SetVirtualScreenUsingStatus Test
 * @tc.desc: Test SetVirtualScreenUsingStatus while input is false
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetVirtualScreenUsingStatus002, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetVirtualScreenUsingStatus(false);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus Test
 * @tc.desc: Test SetCurtainScreenUsingStatus while input is true
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetCurtainScreenUsingStatus001, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetCurtainScreenUsingStatus(true);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus Test
 * @tc.desc: Test SetCurtainScreenUsingStatus while input is false
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetCurtainScreenUsingStatus002, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetCurtainScreenUsingStatus(false);
}

/**
 * @tc.name: ExecuteSynchronousTask Test
 * @tc.desc: Test ExecuteSynchronousTask
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, ExecuteSynchronousTask001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->ExecuteSynchronousTask(nullptr);

    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    rsClient->ExecuteSynchronousTask(task);
}

/**
 * @tc.name: GetUniRenderEnabled Test
 * @tc.desc: GetUniRenderEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetUniRenderEnabled001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ASSERT_EQ(rsClient->GetUniRenderEnabled(), false);
}

/**
 * @tc.name: GetMemoryGraphic Test
 * @tc.desc: GetMemoryGraphic Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetMemoryGraphic001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    int pid = 100; // for test
    rsClient->GetMemoryGraphic(pid);
    auto memGraphics = rsClient->GetMemoryGraphics();
    ASSERT_EQ(memGraphics.size(), 0);
}

/**
 * @tc.name: GetTotalAppMemSize Test
 * @tc.desc: GetTotalAppMemSize Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetTotalAppMemSize001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    float cpuMemSize = 0;
    float gpuMemSize = 0;
    bool ret = rsClient->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CreateNodeAndSurface Test
 * @tc.desc: CreateNodeAndSurface Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, CreateNodeAndSurface001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceRenderNodeConfig config = {.id=0, .name="testSurface"};
    ASSERT_NE(rsClient->CreateNodeAndSurface(config), nullptr);
}

/**
 * @tc.name: CreatePixelMapFromSurfaceId Test
 * @tc.desc: CreatePixelMapFromSurfaceId Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, CreatePixelMapFromSurfaceId001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    SurfaceUtils::GetInstance()->Add(psurface->GetUniqueId(), psurface);
    Rect srcRect = {0, 0, 100, 100};
    rsClient->CreatePixelMapFromSurfaceId(psurface->GetUniqueId(), srcRect);
}

/**
 * @tc.name: SetFocusAppInfo Test
 * @tc.desc: SetFocusAppInfo Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetFocusAppInfo001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto ret = rsClient->SetFocusAppInfo(1, 1, "bundleNameTest", "abilityNameTest", 1);
    ASSERT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetActiveScreenId Test
 * @tc.desc: GetActiveScreenId Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetActiveScreenId001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ASSERT_EQ(rsClient->GetActiveScreenId(), INVALID_SCREEN_ID);
}

/**
 * @tc.name: GetAllScreenIds Test
 * @tc.desc: GetAllScreenIds Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetAllScreenIds001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 480;
    uint32_t defaultHeight = 320;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    auto screenIds = rsClient->GetAllScreenIds();
    ASSERT_NE(screenIds.size(), 0);
}

/**
 * @tc.name: SetVirtualScreenSurface Test
 * @tc.desc: SetVirtualScreenSurface Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetVirtualScreenSurface001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    int32_t ret = rsClient->SetVirtualScreenSurface(123, psurface); // 123 for test
    ASSERT_EQ(ret, 0);
    rsClient->RemoveVirtualScreen(123);
}

/**
 * @tc.name: SetScreenChangeCallback Test
 * @tc.desc: SetScreenChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetScreenChangeCallback001, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    ScreenEvent screenEvent = ScreenEvent::UNKNOWN;
    bool callbacked = false;
    auto callback = [&screenId, &screenEvent, &callbacked](ScreenId id, ScreenEvent event) {
        screenId = id;
        screenEvent = event;
        callbacked = true;
    };
    int32_t status = rsClient->SetScreenChangeCallback(callback);
    EXPECT_EQ(status, StatusCode::SUCCESS);
}

/**
 * @tc.name: SetScreenActiveMode Test
 * @tc.desc: SetScreenActiveMode Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetScreenActiveMode001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    rsClient->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsClient->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);
}

/**
 * @tc.name: SetScreenRefreshRate Test
 * @tc.desc: SetScreenRefreshRate Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetScreenRefreshRate001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    FrameRateLinkerId id = 0;
    FrameRateRange range;
    rsClient->SyncFrameRateRange(id, range);
    uint32_t rateToSet = 990; // 990 for test
    rsClient->SetScreenRefreshRate(screenId, 0, rateToSet);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRate = rsClient->GetScreenCurrentRefreshRate(screenId);
    EXPECT_NE(currentRate, rateToSet);
}

/**
 * @tc.name: SetRefreshRateMode Test
 * @tc.desc: SetRefreshRateMode Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetRefreshRateMode001, TestSize.Level1)
{
    uint32_t rateMode = 100;
    rsClient->SetRefreshRateMode(rateMode);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRateMode = rsClient->GetCurrentRefreshRateMode();
    EXPECT_EQ(currentRateMode, rateMode);
}

/**
 * @tc.name: GetScreenSupportedRefreshRates Test
 * @tc.desc: GetScreenSupportedRefreshRates Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenSupportedRefreshRates001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto supportedRates = rsClient->GetScreenSupportedRefreshRates(screenId);
    EXPECT_EQ(supportedRates.size(), 0);
}

/**
 * @tc.name: SetShowRefreshRateEnabled Test
 * @tc.desc: SetShowRefreshRateEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetShowRefreshRateEnabled001, TestSize.Level1)
{
    rsClient->SetShowRefreshRateEnabled(false);
    EXPECT_EQ(rsClient->GetShowRefreshRateEnabled(), false);

    rsClient->SetShowRefreshRateEnabled(true);
    EXPECT_EQ(rsClient->GetShowRefreshRateEnabled(), true);
}

/**
 * @tc.name: SetVirtualScreenResolution Test
 * @tc.desc: SetVirtualScreenResolution Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetVirtualScreenResolution001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    rsClient->SetVirtualScreenResolution(virtualScreenId, newWidth, newHeight);
    auto curVirtualScreenResolution = rsClient->GetVirtualScreenResolution(virtualScreenId);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenWidth(), newWidth);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenHeight(), newHeight);
}

/**
 * @tc.name: SetScreenPowerStatus Test
 * @tc.desc: SetScreenPowerStatus Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetScreenPowerStatus001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    rsClient->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    usleep(SET_REFRESHRATE_SLEEP_US);
    auto powerStatus = rsClient->GetScreenPowerStatus(screenId);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::POWER_STATUS_ON);
}

/**
 * @tc.name: GetScreenSupportedModes Test
 * @tc.desc: GetScreenSupportedModes Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenSupportedModes001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto supportedScreenModes = rsClient->GetScreenSupportedModes(screenId);
    EXPECT_GT(supportedScreenModes.size(), 0);
}

/**
 * @tc.name: GetScreenCapability Test
 * @tc.desc: GetScreenCapability Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenCapability001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto screenCapability = rsClient->GetScreenCapability(screenId);
    EXPECT_NE(screenCapability.GetType(), DISP_INVALID);
}

/**
 * @tc.name: GetScreenData Test
 * @tc.desc: GetScreenData Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenData001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto screenData = rsClient->GetScreenData(screenId);
    EXPECT_GT(screenData.GetSupportModeInfo().size(), 0);
    EXPECT_NE(screenData.GetCapability().GetType(), DISP_INVALID);
}

/**
 * @tc.name: GetScreenBacklight Test
 * @tc.desc: GetScreenBacklight Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenBacklight001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    rsClient->SetScreenBacklight(screenId, 60); // for test
    usleep(SET_REFRESHRATE_SLEEP_US);
    auto backLight = rsClient->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 60); // for test
}

/**
 * @tc.name: RegisterBufferClearListener Test
 * @tc.desc: RegisterBufferClearListener Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, RegisterBufferClearListener001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferClearCallback cb = [](){};
    bool ret = rsClient->RegisterBufferClearListener(123, cb);
    ASSERT_TRUE(ret);
}

/*
 * @tc.name: GetScreenSupportedColorGamuts Test
 * @tc.desc: GetScreenSupportedColorGamuts Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenSupportedColorGamuts001, TestSize.Level1)
{
    std::vector<ScreenColorGamut> modes;
    int ret = rsClient->GetScreenSupportedColorGamuts(INVALID_SCREEN_ID, modes);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys Test
 * @tc.desc: GetScreenSupportedMetaDataKeys Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenSupportedMetaDataKeys001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    std::vector<ScreenHDRMetadataKey> keys;
    int ret = rsClient->GetScreenSupportedMetaDataKeys(screenId, keys);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_EQ(keys[0], ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X);
}

/*
 * @tc.name: GetScreenColorGamut Test
 * @tc.desc: GetScreenColorGamut Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenColorGamut001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int32_t modeIdx = 0;
    int ret = rsClient->SetScreenColorGamut(virtualScreenId, modeIdx);
    EXPECT_EQ(ret, 0);
    ScreenColorGamut mode = COLOR_GAMUT_NATIVE;
    ret = rsClient->GetScreenColorGamut(virtualScreenId, mode);
    EXPECT_EQ(ret, 0);
}

/*
 * @tc.name: SetScreenGamutMap Test
 * @tc.desc: SetScreenGamutMap Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetScreenGamutMap001, TestSize.Level1)
{
    ScreenId screenId = rsClient->GetDefaultScreenId();
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    uint32_t ret = rsClient->SetScreenGamutMap(screenId, mode);
    ASSERT_EQ(ret, StatusCode::HDI_ERROR);
    ret = rsClient->GetScreenGamutMap(screenId, mode);
    ASSERT_EQ(ret, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenCorrection Test
 * @tc.desc: SetScreenCorrection Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetScreenCorrection001, TestSize.Level1)
{
    ScreenId screenId = rsClient->GetDefaultScreenId();
    uint32_t ret = rsClient->SetScreenCorrection(screenId, ScreenRotation::ROTATION_90);
    ASSERT_EQ(ret, SUCCESS);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation Test
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetVirtualMirrorScreenCanvasRotation001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 1344;
    uint32_t defaultHeight = 2772;
    EXPECT_NE(psurface, nullptr);
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, true), true);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, false), true);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleMode Test
 * @tc.desc: SetVirtualMirrorScreenScaleMode Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetVirtualMirrorScreenScaleMode001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 1344;
    uint32_t defaultHeight = 2772;
    EXPECT_NE(psurface, nullptr);
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenScaleMode(virtualScreenId, ScreenScaleMode::UNISCALE_MODE), true);
}

/*
 * @tc.name: GetScreenHDRCapability Test
 * @tc.desc: GetScreenHDRCapability Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenHDRCapability001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    RSScreenHDRCapability hdrCapability;
    int ret = rsClient->GetScreenHDRCapability(screenId, hdrCapability);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetPixelFormat Test
 * @tc.desc: GetPixelFormat Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetPixelFormat001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);

    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    rsClient->SetPixelFormat(virtualScreenId, GRAPHIC_PIXEL_FMT_BGRA_8888);
    GraphicPixelFormat curPixelFormat;
    auto  ret = rsClient->GetPixelFormat(virtualScreenId, curPixelFormat);
    EXPECT_NE(ret, StatusCode::SCREEN_NOT_FOUND);
    EXPECT_EQ(curPixelFormat, GRAPHIC_PIXEL_FMT_BGRA_8888);
}

/*
 * @tc.name: GetScreenSupportedHDRFormats Test
 * @tc.desc: GetScreenSupportedHDRFormats Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenSupportedHDRFormats001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);

    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<ScreenHDRFormat> hdrFormats;
    int ret = rsClient->GetScreenSupportedHDRFormats(virtualScreenId, hdrFormats);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenHDRFormat Test
 * @tc.desc: GetScreenHDRFormat Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenHDRFormat001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);

    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int ret = rsClient->SetScreenHDRFormat(virtualScreenId, ScreenHDRFormat::NOT_SUPPORT_HDR);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    ScreenHDRFormat hdrFormat = ScreenHDRFormat::NOT_SUPPORT_HDR;
    ret = rsClient->GetScreenHDRFormat(virtualScreenId, hdrFormat);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces Test
 * @tc.desc: GetScreenSupportedColorSpaces Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenSupportedColorSpaces001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 480;
    uint32_t defaultHeight = 320;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    int ret = rsClient->GetScreenSupportedColorSpaces(virtualScreenId, colorSpaces);
    EXPECT_NE(colorSpaces.size(), 0);

    GraphicCM_ColorSpaceType setColorSpace = colorSpaces[0];
    ret = rsClient->SetScreenColorSpace(virtualScreenId, setColorSpace);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    usleep(SET_REFRESHRATE_SLEEP_US);

    GraphicCM_ColorSpaceType curColorSpace;
    ret = rsClient->GetScreenColorSpace(virtualScreenId, curColorSpace);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_EQ(curColorSpace, setColorSpace);
}

/*
 * @tc.name: GetScreenType Test
 * @tc.desc: GetScreenType Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenType001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    RSScreenType type;
    int ret = rsClient->GetScreenType(screenId, type);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetBitmap Test
 * @tc.desc: GetBitmap Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetBitmap001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    bool ret = rsClient->CreateNode(config);
    ASSERT_EQ(ret, true);

    Drawing::Bitmap bitmap;
    ret = rsClient->GetBitmap(config.id, bitmap);
}

/*
 * @tc.name: GetPixelmap Test
 * @tc.desc: GetPixelmap Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetPixelmap001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    bool ret = rsClient->CreateNode(config);
    ASSERT_EQ(ret, true);

    std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
    Drawing::Rect rect = {0, 0, 100, 100};
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    ret = rsClient->GetPixelmap(config.id, pixelmap, &rect, drawCmdList);
}

/*
 * @tc.name: RegisterTypeface Test
 * @tc.desc: RegisterTypeface Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, RegisterTypeface001, TestSize.Level1)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    bool ret = rsClient->RegisterTypeface(typeface);
    EXPECT_EQ(ret, true);
    ret = rsClient->UnRegisterTypeface(typeface);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: SetScreenSkipFrameInterval Test
 * @tc.desc: SetScreenSkipFrameInterval Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetScreenSkipFrameInterval001, TestSize.Level1)
{
    ScreenId screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t skipFrameInterval = 1;  // for test
    int32_t ret = rsClient->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: RegisterOcclusionChangeCallback Test
 * @tc.desc: RegisterOcclusionChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, RegisterOcclusionChangeCallback001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    OcclusionChangeCallback cb = [](std::shared_ptr<RSOcclusionData> data){};
    int32_t ret = rsClient->RegisterOcclusionChangeCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: RegisterSurfaceOcclusionChangeCallback Test
 * @tc.desc: RegisterSurfaceOcclusionChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, RegisterSurfaceOcclusionChangeCallback001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    NodeId id = 0;
    SurfaceOcclusionChangeCallback cb = [](float) {};
    std::vector<float> partitionPoints;
    int32_t ret = rsClient->RegisterSurfaceOcclusionChangeCallback(id, cb, partitionPoints);
    ASSERT_EQ(ret, 0);
    ret = rsClient->UnRegisterSurfaceOcclusionChangeCallback(id);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: RegisterHgmConfigChangeCallback Test
 * @tc.desc: RegisterHgmConfigChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, RegisterHgmConfigChangeCallback001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    HgmConfigChangeCallback cb = [](std::shared_ptr<RSHgmConfigData> data){};
    int32_t ret = rsClient->RegisterHgmConfigChangeCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: RegisterHgmRefreshRateModeChangeCallback Test
 * @tc.desc: RegisterHgmRefreshRateModeChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, RegisterHgmRefreshRateModeChangeCallback001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    HgmRefreshRateModeChangeCallback cb = [](int32_t refreshRateMode){};
    int32_t ret = rsClient->RegisterHgmRefreshRateModeChangeCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: RegisterHgmRefreshRateUpdateCallback Test
 * @tc.desc: RegisterHgmRefreshRateUpdateCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, RegisterHgmRefreshRateUpdateCallback001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    HgmRefreshRateUpdateCallback cb = [](int32_t refreshRate){};
    int32_t ret = rsClient->RegisterHgmRefreshRateUpdateCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: SetAppWindowNum Test
 * @tc.desc: SetAppWindowNum Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetAppWindowNum001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    uint32_t num = 2; // for test
    rsClient->SetAppWindowNum(num);
}

/*
 * @tc.name: SetSystemAnimatedScenes Test
 * @tc.desc: SetSystemAnimatedScenes Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetSystemAnimatedScenes001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool ret = rsClient->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MISSION_CENTER);
    ASSERT_EQ(ret, true);
}

/*
 * @tc.name: ShowWatermark Test
 * @tc.desc: ShowWatermark Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, ShowWatermark001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    std::shared_ptr<Media::PixelMap> watermarkImg = std::make_shared<Media::PixelMap>();
    rsClient->ShowWatermark(watermarkImg, true);
    
}

/*
 * @tc.name: ResizeVirtualScreen Test
 * @tc.desc: ResizeVirtualScreen Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, ResizeVirtualScreen001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);

    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    int32_t ret = rsClient->ResizeVirtualScreen(virtualScreenId, newWidth, newHeight);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: ReportJankStats Test
 * @tc.desc: ReportJankStats Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, ReportJankStats001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->ReportJankStats();
}

/*
 * @tc.name: ReportEventResponse Test
 * @tc.desc: ReportEventResponse Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, ReportEventResponse001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    DataBaseRs info;
    rsClient->ReportEventResponse(info);
}

/*
 * @tc.name: ReportEventComplete Test
 * @tc.desc: ReportEventComplete Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, ReportEventComplete001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    DataBaseRs info;
    rsClient->ReportEventComplete(info);
}

/*
 * @tc.name: ReportEventJankFrame Test
 * @tc.desc: ReportEventJankFrame Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, ReportEventJankFrame001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    DataBaseRs info;
    rsClient->ReportEventJankFrame(info);
}

/*
 * @tc.name: ReportGameStateData Test
 * @tc.desc: ReportGameStateData Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, ReportGameStateData001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    GameStateData info;
    rsClient->ReportGameStateData(info);
}

/*
 * @tc.name: SetHardwareEnabled Test
 * @tc.desc: SetHardwareEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetHardwareEnabled001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetHardwareEnabled(0, true, SelfDrawingNodeType::DEFAULT);
}

/*
 * @tc.name: NotifyLightFactorStatus Test
 * @tc.desc: NotifyLightFactorStatus Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, NotifyLightFactorStatus001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->NotifyLightFactorStatus(true);
}

/*
 * @tc.name: NotifyPackageEvent Test
 * @tc.desc: NotifyPackageEvent Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, NotifyPackageEvent001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    std::vector<std::string> packageList;
    packageList.emplace_back("test");
    rsClient->NotifyPackageEvent(packageList.size(), packageList);
}

/*
 * @tc.name: NotifyRefreshRateEvent Test
 * @tc.desc: NotifyRefreshRateEvent Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, NotifyRefreshRateEvent001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    EventInfo eventInfo;
    rsClient->NotifyRefreshRateEvent(eventInfo);
}

/*
 * @tc.name: NotifyTouchEvent Test
 * @tc.desc: NotifyTouchEvent Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, NotifyTouchEvent001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    int32_t touchStatus = 1;
    rsClient->NotifyTouchEvent(touchStatus);
}

/*
 * @tc.name: SetCacheEnabledForRotation Test
 * @tc.desc: SetCacheEnabledForRotation Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetCacheEnabledForRotation001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool isEnabled = true;
    rsClient->SetCacheEnabledForRotation(isEnabled);
}

/*
 * @tc.name: SetOnRemoteDiedCallback Test
 * @tc.desc: SetOnRemoteDiedCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetOnRemoteDiedCallback001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    OnRemoteDiedCallback callback = [](){};
    rsClient->SetOnRemoteDiedCallback(callback);
}

/*
 * @tc.name: GetCurrentDirtyRegionInfo Test
 * @tc.desc: GetCurrentDirtyRegionInfo Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetCurrentDirtyRegionInfo001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ScreenId screenId = rsClient->GetDefaultScreenId();
    rsClient->GetCurrentDirtyRegionInfo(screenId);
}

/*
 * @tc.name: CreateVSyncReceiver Test
 * @tc.desc: CreateVSyncReceiver Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, CreateVSyncReceiver001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto rsReceiver = rsClient->CreateVSyncReceiver("VsyncReceiverTest");
    ASSERT_NE(rsReceiver, nullptr);
}

/*
 * @tc.name: TriggerSurfaceCaptureCallback Test
 * @tc.desc: TriggerSurfaceCaptureCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, TriggerSurfaceCaptureCallback001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    Media::PixelMap pixelmap;
    rsClient->TriggerSurfaceCaptureCallback(0, &pixelmap);
}

} // namespace Rosen
} // namespace OHOS