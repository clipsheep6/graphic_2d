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

#include "window_it_test.h"

#include <securec.h>

#include <event_handler.h>
#include <display_gralloc.h>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <surface.h>
#include <window_manager.h>

using namespace OHOS;
#define LOG GTEST_LOG_(INFO)

class WindowItTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        wm = WindowManager::GetInstance();
        ASSERT_NE(wm, nullptr);
        wm->Init();

        runner = AppExecFwk::EventRunner::Create(false);
        handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    }
    static void TearDownTestCase()
    {
    }

    void Run(uint32_t lastTime)
    {
        handler->PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner), lastTime);
        runner->Run();
        handler->RemoveTask("WindowItTest::Sync");
    }
protected:
    static inline sptr<WindowManager> wm = nullptr;
    static inline sptr<WindowOption> option = nullptr;
    static inline std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    static inline std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
};

namespace {
constexpr int64_t SEC_TO_USEC = 1000 * 1000;
constexpr int64_t OneFrameTime = 1 * SEC_TO_USEC / 60; // 1second, 60Hz
constexpr int64_t SEC_TO_MSEC = 1000;
constexpr int64_t OneFrameTime_MS = 1 * SEC_TO_MSEC / 60; // 1second, 60Hz
constexpr int64_t PERFORMANCE_COUNT = 1000;
int64_t GetNowTime()
{
    struct timeval time;
    gettimeofday(&time, nullptr);
    return SEC_TO_USEC * time.tv_sec + time.tv_usec;
}

void Draw(uint32_t * const addr, int32_t width, int32_t height, uint32_t colour)
{
    uint32_t addcolour = 0xffffffff; // White
    if (colour == 1)
    {
        addcolour = 0xff000000; // Black
    }
    else if (colour == 2)
    {
        addcolour = 0xff0000ff; // Red
    }
    else if (colour == 3)
    {
        addcolour = 0xff00ffff; // Yellow
    }
    else if (colour == 4)
    {
        addcolour = 0xffffff00; // Blue
    }
    for (int i = 0; i < width * height; i++) {
        addr[i] = addcolour; // alpha red gb
    }
}

void Sync(sptr<Window> window, uint32_t colour)
{
    sptr<Surface> surface = window->GetSurface();
    if (NULL == surface) {
        return;
    }

    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    BufferRequestConfig rconfig = {
        .width = surface->GetDefaultWidth(),
        .height = surface->GetDefaultHeight(),
        .strideAlignment = sizeof(void *),
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surface->GetDefaultUsage(),
        .timeout = 0,
    };

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, rconfig);
    if (ret != SURFACE_ERROR_OK) {
        return;
    }

    Draw(static_cast<uint32_t*>(buffer->GetVirAddr()), rconfig.width, rconfig.height, colour);

    BufferFlushConfig fconfig = {
        .damage = {
        .w = rconfig.width,
        .h = rconfig.height,
        },
    };
    ret = surface->FlushBuffer(buffer, -1, fconfig);

    auto handler = AppExecFwk::EventHandler::Current();
    handler->PostTask(std::bind(&Sync, window, colour), "WindowItTest::Sync", OneFrameTime_MS);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0100
 * @tc.name    TestGetSurface1
 * @tc.desc    GetSurface
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestGetSurface1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    sptr<Surface> surface = window->GetSurface();
    ASSERT_NE(surface, nullptr);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0200
 * @tc.name    TestGetSurface2
 * @tc.desc    GetSurface
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestGetSurface2, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    auto startTime = GetNowTime();
    sptr<Surface> surface = window->GetSurface();
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0300
 * @tc.name    TestGetSurface3
 * @tc.desc    GetSurface
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestGetSurface3, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        sptr<Surface> surface = window->GetSurface();
        ASSERT_NE(surface, nullptr);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0400
 * @tc.name    TestGetID1
 * @tc.desc    GetID
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestGetID1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    int32_t windowid = -1;
    windowid = window->GetID();
    ASSERT_NE(windowid, -1);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0500
 * @tc.name    TestGetID2
 * @tc.desc    GetID
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestGetID2, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    auto startTime = GetNowTime();
    window->GetID();
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0600
 * @tc.name    TestGetID3
 * @tc.desc    GetID
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestGetID3, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    auto startTime = GetNowTime();
    int32_t windowid = 1;
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        windowid = window->GetID();
        ASSERT_NE(windowid, 1);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0700
 * @tc.name    TestShow1
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0800
 * @tc.name    TestShow2
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow2, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_STATUS_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_0900
 * @tc.name    TestShow3
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow3, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NAVI_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1000
 * @tc.name    TestShow4
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow4, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1100
 * @tc.name    TestShow5
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow5, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_LAUNCHER);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1200
 * @tc.name    TestShow6
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow6, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_INPUT_METHOD);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1300
 * @tc.name    TestShow7
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow7, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_INPUT_METHOD_SELECTOR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1400
 * @tc.name    TestShow8
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow8, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_VOLUME_OVERLAY);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1500
 * @tc.name    TestShow9
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow9, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NOTIFICATION_SHADE);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1600
 * @tc.name    TestShow10
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow10, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_FLOAT);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    auto testFunc = [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        WMError wret = promise->Await();
        ASSERT_EQ(wret, WM_OK);
    };
    handler->PostTask(testFunc);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1700
 * @tc.name    TestShow11
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow11, testing::ext::TestSize.Level1)
{
    sptr<Window> windowBlack = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(windowBlack, option);
    handler->PostTask(std::bind(&Sync, windowBlack, 1), "WindowItTest::Sync");
    auto testFuncBlack = [&]() {
        sptr<Promise<WMError>> promise = windowBlack->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    };
    handler->PostTask(testFuncBlack);
    Run(1000);
    windowBlack->Destroy();

    sptr<Window> windowRed = nullptr;
    wm->CreateWindow(windowRed, option);
    handler->PostTask(std::bind(&Sync, windowRed, 2), "WindowItTest::Sync");
    auto testFuncRed = [&]() {
        sptr<Promise<WMError>> promise = windowRed->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    };
    handler->PostTask(testFuncRed);
    Run(2000);
    windowRed->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1800
 * @tc.name    TestShow12
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow12, testing::ext::TestSize.Level1)
{
    sptr<Window> windowWhite = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(windowWhite, option);
    handler->PostTask(std::bind(&Sync, windowWhite, 0), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowWhite->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(2000);

    sptr<Window> windowBlack = nullptr;
    wm->CreateWindow(windowBlack, option);
    handler->PostTask(std::bind(&Sync, windowBlack, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowBlack->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(2000);

    sptr<Window> windowRed = nullptr;
    wm->CreateWindow(windowRed, option);
    handler->PostTask(std::bind(&Sync, windowRed, 2), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowRed->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(2000);

    sptr<Window> windowYellow = nullptr;
    wm->CreateWindow(windowYellow, option);
    handler->PostTask(std::bind(&Sync, windowYellow, 3), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowYellow->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(2000);

    sptr<Window> windowBlue = nullptr;
    wm->CreateWindow(windowBlue, option);
    handler->PostTask(std::bind(&Sync, windowBlue, 4), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowBlue->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(2000);

    windowBlack->Destroy();
    windowWhite->Destroy();
    windowRed->Destroy();
    windowYellow->Destroy();
    windowBlue->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_1900
 * @tc.name    TestShow13
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow13, testing::ext::TestSize.Level1)
{
    sptr<WindowOption> optiontmp = nullptr;
    optiontmp = WindowOption::Get();
    // GTEST_LOG_(INFO) << "windowRed" << std::endl;
    sptr<Window> windowRed = nullptr;
    optiontmp->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(windowRed, optiontmp);
    handler->PostTask(std::bind(&Sync, windowRed, 2), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowRed->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(3000);

    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(3000);

    window->Destroy();
    windowRed->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2000
 * @tc.name    TestShow14
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow14, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 2000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2100
 * @tc.name    TestShow15
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow15, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();
    
    option->SetX(display->width / 4);
    option->SetY(display->height / 4);
    option->SetWidth(display->phyWidth / 2);
    option->SetHeight(display->phyHeight / 2);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2200
 * @tc.name    TestShow16
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow16, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();
    
    option->SetX(display->width * 4);
    option->SetY(display->height * 4);
    option->SetWidth(display->phyWidth * 2);
    option->SetHeight(display->phyHeight * 2);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2300
 * @tc.name    TestShow17
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow17, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();
    
    option->SetX(0);
    option->SetY(0);
    option->SetWidth(display->phyWidth * 2);
    option->SetHeight(display->phyHeight * 2);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2400
 * @tc.name    TestShow18
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow18, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();
    
    option->SetX(-(display->phyWidth / 2));
    option->SetY(-(display->phyHeight / 2));
    option->SetWidth(display->phyWidth);
    option->SetHeight(display->phyHeight);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
    } );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2500
 * @tc.name    TestShow19
 * @tc.desc    Show
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow19, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        handler->RemoveTask("WindowItTest::Sync");
        window->Destroy();
        sptr<Promise<WMError>> promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_ERROR_DESTROYED_OBJECT);
        runner->Stop();
    } );
    runner->Run();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2600
 * @tc.name    TestShow20
 * @tc.desc    Show
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow20, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        sptr<Promise<WMError>> promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime);
    },100);
    Run(1000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2700
 * @tc.name    TestShow21
 * @tc.desc    Show
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestShow21, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        for (int i = 0; i < PERFORMANCE_COUNT; i++) {
            sptr<Promise<WMError>> promise = window->Show();
            ASSERT_EQ(promise->Await(), WM_OK);
        }
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    } );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2800
 * @tc.name    TestHide1
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_2900
 * @tc.name    TestHide2
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide2, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_STATUS_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3000
 * @tc.name    TestHide3
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide3, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NAVI_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3100
 * @tc.name    TestHide4
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide4, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3200
 * @tc.name    TestHide5
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide5, testing::ext::TestSize.Level1)
{
    sptr<Window> windowBlack = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(windowBlack, option);
    handler->PostTask(std::bind(&Sync, windowBlack, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowBlack->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(2000);
    windowBlack->Destroy();

    sptr<Window> windowRed = nullptr;
    wm->CreateWindow(windowRed, option);
    handler->PostTask(std::bind(&Sync, windowRed, 2), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowRed->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(2000);
    windowRed->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3300
 * @tc.name    TestHide6
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide6, testing::ext::TestSize.Level1)
{
    sptr<Window> windowWhite = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(windowWhite, option);
    handler->PostTask(std::bind(&Sync, windowWhite, 0), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowWhite->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(2000);

    sptr<Window> windowBlack = nullptr;
    wm->CreateWindow(windowBlack, option);
    handler->PostTask(std::bind(&Sync, windowBlack, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowBlack->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(2000);

    sptr<Window> windowRed = nullptr;
    wm->CreateWindow(windowRed, option);
    handler->PostTask(std::bind(&Sync, windowRed, 2), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowRed->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(2000);

    sptr<Window> windowYellow = nullptr;
    wm->CreateWindow(windowYellow, option);
    handler->PostTask(std::bind(&Sync, windowYellow, 3), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowYellow->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(2000);

    sptr<Window> windowBlue = nullptr;
    wm->CreateWindow(windowBlue, option);
    handler->PostTask(std::bind(&Sync, windowBlue, 4), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowBlue->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(2000);

    windowBlack->Destroy();
    windowWhite->Destroy();
    windowRed->Destroy();
    windowYellow->Destroy();
    windowBlue->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3400
 * @tc.name    TestHide7
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide7, testing::ext::TestSize.Level1)
{
    sptr<WindowOption> optiontmp = nullptr;
    optiontmp = WindowOption::Get();
    sptr<Window> windowRed = nullptr;
    optiontmp->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(windowRed, optiontmp);
    handler->PostTask(std::bind(&Sync, windowRed, 2), "WindowItTest::Sync");

    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowRed->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 2000 );
    Run(3000);
    window->Destroy();
    windowRed->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3500
 * @tc.name    TestHide8
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide8, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
        promise = window->Show();
        ASSERT_EQ(promise->Await(), WM_OK);
        promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3600
 * @tc.name    TestHide9
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide9, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();
    
    option->SetX(display->width / 4);
    option->SetY(display->height / 4);
    option->SetWidth(display->phyWidth / 2);
    option->SetHeight(display->phyHeight / 2);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3700
 * @tc.name    TestHide10
 * @tc.desc    Hide
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide10, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        handler->RemoveTask("WindowItTest::Sync");
        window->Destroy();
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_ERROR_DESTROYED_OBJECT);
    } );
    Run(1000);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3800
 * @tc.name    TestHide11
 * @tc.desc    Hide
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide11, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        sptr<Promise<WMError>> promise = window->Hide();
        ASSERT_EQ(promise->Await(), WM_OK);
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime);
    }, 1000 );
    Run(1000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_3900
 * @tc.name    TestHide12
 * @tc.desc    Hide
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestHide12, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        for (int i = 0; i < PERFORMANCE_COUNT; i++) {
            sptr<Promise<WMError>> promise = window->Hide();
            ASSERT_EQ(promise->Await(), WM_OK);
        }
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    },1000);
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4000
 * @tc.name    TestMove1
 * @tc.desc    Move
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        std::vector<struct WMDisplayInfo> displays;
        wm->GetDisplays(displays);
        auto display = displays.begin();
        int32_t x = display->width / 2;
        int32_t y = display->height / 2;

        sptr<Promise<WMError>> promise = window->Move(x, y);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4100
 * @tc.name    TestMove2
 * @tc.desc    Move
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove2, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_STATUS_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        std::vector<struct WMDisplayInfo> displays;
        wm->GetDisplays(displays);
        auto display = displays.begin();
        int32_t x = display->width / 2;
        int32_t y = display->height / 2;

        sptr<Promise<WMError>> promise = window->Move(x, y);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4200
 * @tc.name    TestMove3
 * @tc.desc    Move
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove3, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NAVI_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        std::vector<struct WMDisplayInfo> displays;
        wm->GetDisplays(displays);
        auto display = displays.begin();
        int32_t x = display->width / 2;
        int32_t y = display->height / 2;

        sptr<Promise<WMError>> promise = window->Move(x, y);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4300
 * @tc.name    TestMove4
 * @tc.desc    Move
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove4, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        std::vector<struct WMDisplayInfo> displays;
        wm->GetDisplays(displays);
        auto display = displays.begin();
        int32_t x = display->width / 2;
        int32_t y = display->height / 2;

        sptr<Promise<WMError>> promise = window->Move(x, y);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4400
 * @tc.name    TestMove5
 * @tc.desc    Move
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove5, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Move(0, 0);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4500
 * @tc.name    TestMove6
 * @tc.desc    Move
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove6, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Move(-1, -1);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4600
 * @tc.name    TestMove7
 * @tc.desc    Move
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove7, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Move(10000, 10000);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4700
 * @tc.name    TestMove8
 * @tc.desc    Move
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove8, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        handler->RemoveTask("WindowItTest::Sync");
        window->Destroy();
        sptr<Promise<WMError>> promise = window->Move(0, 0);
        ASSERT_EQ(promise->Await(), WM_ERROR_DESTROYED_OBJECT);
    } );
    Run(1000);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4800
 * @tc.name    TestMove9
 * @tc.desc    Move
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove9, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        sptr<Promise<WMError>> promise = window->Move(0, 0);
        ASSERT_EQ(promise->Await(), WM_OK);
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime);
    }, 1000 );
    Run(1000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_4900
 * @tc.name    TestMove10
 * @tc.desc    Move
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestMove10, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        for (int i = 0; i < PERFORMANCE_COUNT; i++) {
            sptr<Promise<WMError>> promise = window->Move(0, 0);
            ASSERT_EQ(promise->Await(), WM_OK);
        }
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5000
 * @tc.name    TestSwitchTop1
 * @tc.desc    SwitchTop
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSwitchTop1, testing::ext::TestSize.Level1)
{
    sptr<Window> windowBlack = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(windowBlack, option);
    handler->PostTask(std::bind(&Sync, windowBlack, 1), "WindowItTest::Sync");

    sptr<Window> windowRed = nullptr;
    wm->CreateWindow(windowRed, option);
    handler->PostTask(std::bind(&Sync, windowRed, 2), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowBlack->SwitchTop();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    windowBlack->Destroy();
    windowRed->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5100
 * @tc.name    TestSwitchTop2
 * @tc.desc    SwitchTop
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSwitchTop2, testing::ext::TestSize.Level1)
{
    sptr<Window> windowWhite = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(windowWhite, option);
    handler->PostTask(std::bind(&Sync, windowWhite, 0), "WindowItTest::Sync");

    sptr<Window> windowBlack = nullptr;
    wm->CreateWindow(windowBlack, option);
    handler->PostTask(std::bind(&Sync, windowBlack, 1), "WindowItTest::Sync", 200);

    sptr<Window> windowRed = nullptr;
    wm->CreateWindow(windowRed, option);
    handler->PostTask(std::bind(&Sync, windowRed, 2), "WindowItTest::Sync", 400);

    sptr<Window> windowYellow = nullptr;
    wm->CreateWindow(windowYellow, option);
    handler->PostTask(std::bind(&Sync, windowYellow, 3), "WindowItTest::Sync", 600);

    sptr<Window> windowBlue = nullptr;
    wm->CreateWindow(windowBlue, option);
    handler->PostTask(std::bind(&Sync, windowBlue, 4), "WindowItTest::Sync", 800);
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = windowWhite->SwitchTop();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 2000 );
    Run(3000);

    windowWhite->Destroy();
    windowBlack->Destroy();
    windowRed->Destroy();
    windowYellow->Destroy();
    windowBlue->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5200
 * @tc.name    TestSwitchTop3
 * @tc.desc    SwitchTop
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSwitchTop3, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");

    sptr<Window> windowAlarm = nullptr;
    sptr<WindowOption> optiontmp = nullptr;
    optiontmp = WindowOption::Get();
    optiontmp->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(windowAlarm, optiontmp);
    handler->PostTask(std::bind(&Sync, windowAlarm, 2), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->SwitchTop();
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);

    window->Destroy();
    windowAlarm->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5300
 * @tc.name    TestSwitchTop4
 * @tc.desc    SwitchTop
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSwitchTop4, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        handler->RemoveTask("WindowItTest::Sync");
        window->Destroy();
        sptr<Promise<WMError>> promise = window->SwitchTop();
        ASSERT_EQ(promise->Await(), WM_ERROR_DESTROYED_OBJECT);
    } );
    Run(1000);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5400
 * @tc.name    TestSwitchTop5
 * @tc.desc    SwitchTop
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSwitchTop5, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        sptr<Promise<WMError>> promise = window->SwitchTop();
        ASSERT_EQ(promise->Await(), WM_OK);
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        LOG << "spend " << timeSpend << "us";
        ASSERT_LT(timeSpend, OneFrameTime);
    }, 1000 );
    Run(1000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5500
 * @tc.name    TestSwitchTop6
 * @tc.desc    SwitchTop
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSwitchTop6, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        for (int i = 0; i < PERFORMANCE_COUNT; i++) {
            sptr<Promise<WMError>> promise = window->SwitchTop();
            ASSERT_EQ(promise->Await(), WM_OK);
        }
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5600
 * @tc.name    TestSetWindowType1
 * @tc.desc    SetWindowType
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->SetWindowType(WINDOW_TYPE_NORMAL);
        ASSERT_EQ(promise->Await(), WM_ERROR_INVALID_PARAM );
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5700
 * @tc.name    TestSetWindowType2
 * @tc.desc    SetWindowType
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType2, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->SetWindowType(WINDOW_TYPE_STATUS_BAR);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5800
 * @tc.name    TestSetWindowType3
 * @tc.desc    SetWindowType
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType3, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_STATUS_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->SetWindowType(WINDOW_TYPE_NAVI_BAR);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_5900
 * @tc.name    TestSetWindowType4
 * @tc.desc    SetWindowType
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType4, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NAVI_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6000
 * @tc.name    TestSetWindowType5
 * @tc.desc    SwitchTop
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType5, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->SetWindowType(WINDOW_TYPE_NORMAL);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6100
 * @tc.name    TestSetWindowType6
 * @tc.desc    SwitchTop
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType6, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        handler->RemoveTask("WindowItTest::Sync");
        window->Destroy();
        sptr<Promise<WMError>> promise = window->SetWindowType(WINDOW_TYPE_NORMAL);
        ASSERT_EQ(promise->Await(), WM_ERROR_DESTROYED_OBJECT);
    } );
    Run(1000);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6200
 * @tc.name    TestSetWindowType7
 * @tc.desc    SwitchTop
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType7, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->SetWindowType((WindowType)1000);
        ASSERT_EQ(promise->Await(), WM_ERROR_INVALID_PARAM);
    } );
    Run(1000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6300
 * @tc.name    TestSetWindowType8
 * @tc.desc    SwitchTop
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType8, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        sptr<Promise<WMError>> promise = window->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
        ASSERT_EQ(promise->Await(), WM_OK);
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime);
    },1000 );
    Run(1000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6400
 * @tc.name    TestSetWindowType9
 * @tc.desc    SwitchTop
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestSetWindowType9, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
   // option = WindowOption::Get();
   // wm->CreateWindow(window, option);
   // handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        auto startTime = GetNowTime();
        for (int i = 0; i < PERFORMANCE_COUNT; i++) {
            option = WindowOption::Get();
            wm->CreateWindow(window, option);
            handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
            sptr<Promise<WMError>> promise = window->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
            ASSERT_EQ(promise->Await(), WM_OK);
            window->Destroy();
        }
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    }, 1000 );
    Run(3000);
    // window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6500
 * @tc.name    TestResize1
 * @tc.desc    Resize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        int32_t w = window->GetWidth() / 2;
        int32_t h = window->GetHeight() / 2;
        sptr<Promise<WMError>> promise = window->Resize(w, h);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6600
 * @tc.name    TestResize2
 * @tc.desc    Resize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize2, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_STATUS_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        int32_t w = window->GetWidth() / 2;
        int32_t h = window->GetHeight() / 2;
        sptr<Promise<WMError>> promise = window->Resize(w, h);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6700
 * @tc.name    TestResize3
 * @tc.desc    Resize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize3, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NAVI_BAR);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        int32_t w = window->GetWidth() / 2;
        int32_t h = window->GetHeight() / 2;
        sptr<Promise<WMError>> promise = window->Resize(w, h);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6800
 * @tc.name    TestResize4
 * @tc.desc    Resize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize4, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        int32_t w = window->GetWidth() / 2;
        int32_t h = window->GetHeight() / 2;
        sptr<Promise<WMError>> promise = window->Resize(w, h);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_6900
 * @tc.name    TestResize5
 * @tc.desc    Resize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize5, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Resize(0, 0);
        ASSERT_EQ(promise->Await(), WM_ERROR_INVALID_PARAM);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_7000
 * @tc.name    TestResize6
 * @tc.desc    Resize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize6, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        int32_t w = window->GetWidth() * 2;
        int32_t h = window->GetHeight() * 2;
        sptr<Promise<WMError>> promise = window->Resize(w, h);
        ASSERT_EQ(promise->Await(), WM_OK);
    }, 1000 );
    Run(3000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_7100
 * @tc.name    TestResize7
 * @tc.desc    Resize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize7, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        handler->RemoveTask("WindowItTest::Sync");
        window->Destroy();
        int32_t w = window->GetWidth() * 2;
        int32_t h = window->GetHeight() * 2;
        sptr<Promise<WMError>> promise = window->Resize(w, h);
        ASSERT_EQ(promise->Await(), WM_ERROR_DESTROYED_OBJECT);
    } );
    Run(1000);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_7200
 * @tc.name    TestResize8
 * @tc.desc    Resize
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize8, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        sptr<Promise<WMError>> promise = window->Resize(0, 0);
        ASSERT_EQ(promise->Await(), WM_ERROR_INVALID_PARAM);
    } );
    Run(1000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_7300
 * @tc.name    TestResize9
 * @tc.desc    Resize
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize9, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        int32_t w = window->GetWidth() / 2;
        int32_t h = window->GetHeight() / 2;
        auto startTime = GetNowTime();
        sptr<Promise<WMError>> promise = window->Resize(w, h);
        ASSERT_EQ(promise->Await(), WM_OK);
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime);
    },100);
    Run(1000);
    window->Destroy();
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOW_IT_TEST_7400
 * @tc.name    TestResize10
 * @tc.desc    Resize
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowItTest, TestResize10, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    wm->CreateWindow(window, option);
    handler->PostTask(std::bind(&Sync, window, 1), "WindowItTest::Sync");
    handler->PostTask( [&]() {
        int32_t w = window->GetWidth() / 2;
        int32_t h = window->GetHeight() / 2;
        auto startTime = GetNowTime();
        for (int i = 0; i < PERFORMANCE_COUNT; i++) {
            sptr<Promise<WMError>> promise = window->Resize(w, h);
            ASSERT_EQ(promise->Await(), WM_OK);
        }
        auto endTime = GetNowTime();
        auto timeSpend = endTime - startTime;
        ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
    } );
    Run(3000);
    window->Destroy();
}

}
