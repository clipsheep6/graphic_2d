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

#include "windowmanager_it_test.h"

#include <securec.h>

#include <display_gralloc.h>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <surface.h>
#include <window_manager.h>

using namespace OHOS;
#define LOG GTEST_LOG_(INFO)

class WindowManagerItTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        wm = WindowManager::GetInstance();
        option = WindowOption::Get();
    }
    static void TearDownTestCase()
    {
    }
protected:
    static inline sptr<WindowManager> wm = nullptr;
    static inline sptr<WindowOption> option = nullptr;
};

namespace {
constexpr int64_t SEC_TO_USEC = 1000 * 1000;
constexpr int64_t OneFrameTime = 1 * SEC_TO_USEC / 60; // 1second, 60Hz
constexpr int64_t PERFORMANCE_COUNT = 1000;
int64_t GetNowTime()
{
    struct timeval time;
    gettimeofday(&time, nullptr);
    return SEC_TO_USEC * time.tv_sec + time.tv_usec;
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0100
 * @tc.name    TestGetInstance1
 * @tc.desc    GetInstance
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestGetInstance1, testing::ext::TestSize.Level1)
{
    ASSERT_NE(wm, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0200
 * @tc.name    TestGetInstance2
 * @tc.desc    GetInstance
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestGetInstance2, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    WindowManager::GetInstance();
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0300
 * @tc.name    TestGetInstance3
 * @tc.desc    GetInstance
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestGetInstance3, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        WindowManager* windowManager = nullptr;
        windowManager = WindowManager::GetInstance();
        ASSERT_NE(windowManager, nullptr);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0400
 * @tc.name    TestInit1
 * @tc.desc    Init
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestInit1, testing::ext::TestSize.Level1)
{
    WMError wret = wm->Init();
    ASSERT_EQ(wret, WM_OK);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0500
 * @tc.name    TestInit2
 * @tc.desc    Init
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestInit2, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    WMError wret = wm->Init();
    auto endTime = GetNowTime();

    auto timeSpend = endTime - startTime;
    ASSERT_EQ(wret, WM_OK);
    ASSERT_LT(timeSpend, OneFrameTime);   
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0600
 * @tc.name    TestInit3
 * @tc.desc    Init
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestInit3, testing::ext::TestSize.Level2)
{
    sptr<WindowManager> wm = WindowManager::GetInstance();

    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        WMError wret = wm->Init();
        ASSERT_EQ(wret, WM_OK);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0700
 * @tc.name    TestGetDisplays1
 * @tc.desc    GetDisplays
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestGetDisplays1, testing::ext::TestSize.Level1)
{
    std::vector<struct WMDisplayInfo> displays;
    WMError wret1 = wm->GetDisplays(displays);
    ASSERT_EQ(wret1, WM_OK);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0800
 * @tc.name    TestGetDisplays2
 * @tc.desc    GetDisplays
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestGetDisplays2, testing::ext::TestSize.Level1)
{
    std::vector<struct WMDisplayInfo> displays;
    WMError wret = wm->GetDisplays(displays);
    ASSERT_EQ(wret, WM_OK);
    for (const auto &display : displays) {
        std::cout << "id: " << display.id << ","
        << "width: " << display.width << ","
        << "height: " << display.height << ","
        << "phyWidth: " << display.phyWidth << ","
        << "phyHeight: " << display.phyHeight << std::endl;
    }
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_0900
 * @tc.name    TestGetDisplays3
 * @tc.desc    GetDisplays
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestGetDisplays3, testing::ext::TestSize.Level2)
{
    std::vector<struct WMDisplayInfo> displays;
    auto startTime = GetNowTime();
    WMError wret = wm->GetDisplays(displays);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_EQ(wret, WM_OK);
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1000
 * @tc.name    TestGetDisplays4
 * @tc.desc    GetDisplays
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestGetDisplays4, testing::ext::TestSize.Level2)
{
    std::vector<struct WMDisplayInfo> displays;
    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        WMError wret = wm->GetDisplays(displays);
        ASSERT_EQ(wret, WM_OK);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1100
 * @tc.name    TestCreateWindow1
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1200
 * @tc.name    TestCreateWindow2
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow2, testing::ext::TestSize.Level1)
{
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    option->SetWindowMode(WINDOW_MODE_UNSET);
    sptr<Window> window = nullptr;
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1300
 * @tc.name    TestCreateWindow3
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow3, testing::ext::TestSize.Level1)
{
    option->SetWindowType(WINDOW_TYPE_STATUS_BAR);
    option->SetWindowMode(WINDOW_MODE_FREE);
    sptr<Window> window = nullptr;
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1400
 * @tc.name    TestCreateWindow4
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow4, testing::ext::TestSize.Level1)
{
    option->SetWindowType(WINDOW_TYPE_NAVI_BAR);
    option->SetWindowMode(WINDOW_MODE_MAX);
    sptr<Window> window = nullptr;
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1500
 * @tc.name    TestCreateWindow5
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow5, testing::ext::TestSize.Level1)
{
    option->SetWindowType(WINDOW_TYPE_ALARM_SCREEN);
    sptr<Window> window = nullptr;
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1600
 * @tc.name    TestCreateWindow6
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow6, testing::ext::TestSize.Level1)
{
    option->SetWindowMode(WINDOW_MODE_MAX);
    sptr<Window> window = nullptr;
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1700
 * @tc.name    TestCreateWindow7
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow7, testing::ext::TestSize.Level1)
{
    option->SetWindowType(WINDOW_TYPE_MAX);
    option->SetWindowMode(WINDOW_MODE_MAX);
    sptr<Window> window = nullptr;
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1800
 * @tc.name    TestCreateWindow8
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow8, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
   // GTEST_LOG_(INFO) << option->GetWindowType() << std::endl;
   // GTEST_LOG_(INFO) << option->GetWindowMode() << std::endl;
   // GTEST_LOG_(INFO) << option->GetConsumerSurface() << std::endl;
    option->SetDisplay(option->GetDisplay());
    option->SetWindowType(option->GetWindowType());
    option->SetWindowMode(option->GetWindowMode()); 
    option->SetConsumerSurface(option->GetConsumerSurface()); 
    option->SetX(option->GetX());
    option->SetY(option->GetY());
    option->SetWidth(option->GetWidth());
    option->SetHeight(option->GetHeight());
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_1900
 * @tc.name    TestCreateWindow9
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow9, testing::ext::TestSize.Level1)
{
    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();
    
    sptr<Window> window = nullptr;
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    option->SetWindowMode(WINDOW_MODE_UNSET);
    option->SetDisplay(display->id);
    sptr<Surface> csurface = Surface::CreateSurfaceAsConsumer();
    option->SetConsumerSurface(csurface);
    option->SetX(display->width / 2);
    option->SetY(display->height / 2);
    option->SetWidth(display->phyWidth / 2);
    option->SetHeight(display->phyHeight / 2);
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2000
 * @tc.name    TestCreateWindow10
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow10, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option->SetWindowType(WINDOW_TYPE_MAX);
    option->SetWindowMode(WINDOW_MODE_MAX);
    option->SetDisplay(-1);
    sptr<Surface> csurface = nullptr;
    option->SetConsumerSurface(csurface); 
    option->SetX(-1);
    option->SetY(-1);
    option->SetWidth(0);
    option->SetHeight(0);
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    ASSERT_NE(window, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2100
 * @tc.name    TestCreateWindow11
 * @tc.desc    CreateWindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow11, testing::ext::TestSize.Level1)
{
    sptr<WindowOption> optiontmp = nullptr;
    sptr<Window> window = nullptr;
    WMError wret = wm->CreateWindow(window, optiontmp);
    ASSERT_EQ(wret, WM_ERROR_NULLPTR);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2200
 * @tc.name    TestCreateWindow12
 * @tc.desc    CreateWindow
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow12, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    sptr<WindowOption> optiontmp = WindowOption::Get();
    auto startTime = GetNowTime();
    wm->CreateWindow(window, optiontmp);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend , OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2300
 * @tc.name    TestCreateWindow13
 * @tc.desc    CreateWindow
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateWindow13, testing::ext::TestSize.Level2)
{
    sptr<Window> window = nullptr;
    sptr<WindowOption> optiontmp = WindowOption::Get();
    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        WMError wret = wm->CreateWindow(window, optiontmp);
        ASSERT_EQ(wret, WM_OK);
        window->Destroy();
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2400
 * @tc.name    TestCreateSubwindow1
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow1, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2500
 * @tc.name    TestCreateSubwindow2
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow2, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(subwindowOption->GetWindowType());
    subwindowOption->SetConsumerSurface(subwindowOption->GetConsumerSurface()); 
    subwindowOption->SetX(subwindowOption->GetX());
    subwindowOption->SetY(subwindowOption->GetY());
    subwindowOption->SetWidth(subwindowOption->GetWidth());
    subwindowOption->SetHeight(subwindowOption->GetHeight());
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2600
 * @tc.name    TestCreateSubwindow3
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow3, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_NORMAL);
    sptr<Surface> subSurface = subwindowOption->GetConsumerSurface();
    subwindowOption->SetConsumerSurface(subSurface); 
    subwindowOption->SetX(display->width / 4);
    subwindowOption->SetY(display->height / 4);
    subwindowOption->SetWidth(display->phyWidth / 4);
    subwindowOption->SetHeight(display->phyHeight / 4);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2700
 * @tc.name    TestCreateSubwindow4
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow4, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_MAX);
    sptr<Surface> subSurface = nullptr;
    subwindowOption->SetConsumerSurface(subSurface); 
    subwindowOption->SetX(-1);
    subwindowOption->SetY(-1);
    subwindowOption->SetWidth(-1);
    subwindowOption->SetHeight(-1);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2800
 * @tc.name    TestCreateSubwindow5
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow5, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);
    
    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_MAX);
    sptr<Surface> subSurface = nullptr;
    subwindowOption->SetConsumerSurface(subSurface); 
    subwindowOption->SetX(0);
    subwindowOption->SetY(0);
    subwindowOption->SetWidth(0);
    subwindowOption->SetHeight(0);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_2900
 * @tc.name    TestCreateSubwindow6
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow6, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);
    
    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SubwindowType(100));
    sptr<Surface> subSurface = nullptr;
    subwindowOption->SetConsumerSurface(subSurface); 
    subwindowOption->SetX(-1);
    subwindowOption->SetY(-1);
    subwindowOption->SetWidth(-1);
    subwindowOption->SetHeight(-1);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3000
 * @tc.name    TestCreateSubwindow7
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow7, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_NORMAL);
    sptr<Surface> subSurface = subwindowOption->GetConsumerSurface();
    subwindowOption->SetConsumerSurface(subSurface); 
    subwindowOption->SetX(display->width / 4);
    subwindowOption->SetY(display->height / 4);
    subwindowOption->SetWidth(0);
    subwindowOption->SetHeight(0);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3100
 * @tc.name    TestCreateSubwindow8
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow8, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_NORMAL);
    sptr<Surface> subSurface = subwindowOption->GetConsumerSurface();
    subwindowOption->SetConsumerSurface(subSurface); 
    subwindowOption->SetX(display->width * 4);
    subwindowOption->SetY(display->height * 4);
    subwindowOption->SetWidth(display->width);
    subwindowOption->SetHeight(display->width);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3200
 * @tc.name    TestCreateSubwindow9
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow9, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_NORMAL);
    sptr<Surface> subSurface = subwindowOption->GetConsumerSurface();
    subwindowOption->SetConsumerSurface(subSurface); 
    subwindowOption->SetX(display->width * 4);
    subwindowOption->SetY(display->height * 4);
    subwindowOption->SetWidth(display->width);
    subwindowOption->SetHeight(display->width);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3300
 * @tc.name    TestCreateSubwindow10
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow10, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_NORMAL);
    sptr<Surface> subSurface = subwindowOption->GetConsumerSurface();
    subwindowOption->SetConsumerSurface(subSurface);
    subwindowOption->SetX(display->width / 4);
    subwindowOption->SetY(display->height / 4);
    subwindowOption->SetWidth(display->width);
    subwindowOption->SetHeight(display->width);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3400
 * @tc.name    TestCreateSubwindow11
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow11, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    wm->CreateWindow(window, option);

    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SubwindowType(100));
    sptr<Surface> subSurface = nullptr;
    subwindowOption->SetConsumerSurface(subSurface);
    subwindowOption->SetX(display->width / 4);
    subwindowOption->SetY(display->height / 4);
    subwindowOption->SetWidth(display->width);
    subwindowOption->SetHeight(display->width);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3500
 * @tc.name    TestCreateSubwindow12
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow12, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option->SetWindowType(WINDOW_TYPE_MAX);
    option->SetWindowMode(WINDOW_MODE_MAX);
    option->SetDisplay(-1);
    sptr<Surface> csurface = nullptr;
    option->SetConsumerSurface(csurface); 
    option->SetX(-1);
    option->SetY(-1);
    option->SetWidth(0);
    option->SetHeight(0);
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SubwindowType(100));
    sptr<Surface> subSurface = nullptr;
    subwindowOption->SetConsumerSurface(subSurface); 
    subwindowOption->SetX(-1);
    subwindowOption->SetY(-1);
    subwindowOption->SetWidth(-1);
    subwindowOption->SetHeight(-1);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3600
 * @tc.name    TestCreateSubwindow13
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow13, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option->SetWindowType(WINDOW_TYPE_MAX);
    option->SetWindowMode(WINDOW_MODE_MAX);
    option->SetDisplay(-1);
    sptr<Surface> csurface = nullptr;
    option->SetConsumerSurface(csurface); 
    option->SetX(-1);
    option->SetY(-1);
    option->SetWidth(0);
    option->SetHeight(0);
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);
    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_NORMAL);
    sptr<Surface> subSurface = subwindowOption->GetConsumerSurface();
    subwindowOption->SetConsumerSurface(subSurface);
    subwindowOption->SetX(display->width / 4);
    subwindowOption->SetY(display->height / 4);
    subwindowOption->SetWidth(display->width);
    subwindowOption->SetHeight(display->width);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3700
 * @tc.name    TestCreateSubwindow14
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow14, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option->SetWindowType(WINDOW_TYPE_MAX);
    option->SetWindowMode(WINDOW_MODE_MAX);
    option->SetDisplay(-1);
    sptr<Surface> csurface = nullptr;
    option->SetConsumerSurface(csurface); 
    option->SetX(-1);
    option->SetY(-1);
    option->SetWidth(0);
    option->SetHeight(0);
    WMError wret = wm->CreateWindow(window, option);
    ASSERT_EQ(wret, WM_OK);

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_OK);
    ASSERT_NE(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3800
 * @tc.name    TestCreateSubwindow15
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow15, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    std::vector<struct WMDisplayInfo> displays;
    wm->GetDisplays(displays);
    auto display = displays.begin();

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    subwindowOption->SetWindowType(SUBWINDOW_TYPE_NORMAL);
    sptr<Surface> subSurface = nullptr;
    subwindowOption->SetConsumerSurface(subSurface);
    subwindowOption->SetX(display->width / 4);
    subwindowOption->SetY(display->height / 4);
    subwindowOption->SetWidth(display->width);
    subwindowOption->SetHeight(display->width);
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_ERROR_NULLPTR);
    ASSERT_EQ(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_3900
 * @tc.name    TestCreateSubwindow16
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow16, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    sptr<SubwindowOption> subwindowOption = nullptr;
    sptr<Subwindow> subwindow = nullptr;
    WMError wret_sub = wm->CreateSubwindow(subwindow, window, subwindowOption);
    ASSERT_EQ(wret_sub, WM_ERROR_NULLPTR);
    ASSERT_EQ(subwindow, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_4000
 * @tc.name    TestCreateSubwindow17
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow17, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    auto startTime = GetNowTime();
    wm->CreateSubwindow(subwindow, window, subwindowOption);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_WM_TEST_MODULETEST_WINDOWMANAGER_IT_TEST_4100
 * @tc.name    TestCreateSubwindow18
 * @tc.desc    CreateSubwindow
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(WindowManagerItTest, TestCreateSubwindow18, testing::ext::TestSize.Level1)
{
    sptr<Window> window = nullptr;
    option = WindowOption::Get();
    wm->CreateWindow(window, option);

    sptr<SubwindowOption> subwindowOption = SubwindowOption::Get();
    sptr<Subwindow> subwindow = nullptr;
    auto startTime = GetNowTime();
    for (int i = 0; i < PERFORMANCE_COUNT; i++) {
        wm->CreateSubwindow(subwindow, window, subwindowOption);
        ASSERT_NE(subwindow, nullptr);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

}
