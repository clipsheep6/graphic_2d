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

#include "scene_test.h"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional>
#include <thread>

#include <cpudraw.h>
#include <display_type.h>
#include <gslogger.h>
#include <test_header.h>
#include <window_manager_service_client.h>

using namespace std::chrono_literals;

namespace OHOS {
void SceneTest::OnScreenShot(const struct WMImageInfo &info)
{
    screenshotInfo = info;
    if (info.data == nullptr || info.width * info.height == 0) {
        screenshotPromise->Resolve(nullptr);
    }

    Buffer buffer(info.width * info.height);
    uint32_t *image = buffer.Get();
    auto ptr = reinterpret_cast<const uint32_t *>(info.data);
    for (uint32_t i = 0; i < info.width * info.height; i++) {
        image[i] = ptr[i];
    }
    screenshotPromise->Resolve(buffer);
}

void SceneTest::ScreenShot()
{
    screenshotPromise = new Promise<Buffer>();
    WindowManager::GetInstance()->ListenNextScreenShot(0, this);
}

void SceneTest::Init(int32_t initOption)
{
    PART("EnvConditions") {
        if (initOption & InitWM) {
            STEP("WindowManager init success") {
                STEP_ASSERT_EQ(WindowManager::GetInstance()->Init(), GSERROR_OK);
            }
        }

        if (initOption & InitWMS) {
            STEP("WindowManagerService init success") {
                STEP_ASSERT_EQ(WindowManagerServiceClient::GetInstance()->Init(), GSERROR_OK);
            }
        }

        if (initOption & InitDisplay) {
            STEP("Screen size > 0") {
                std::vector<struct WMDisplayInfo> displays;
                WindowManager::GetInstance()->GetDisplays(displays);
                STEP_ASSERT_GT(displays.size(), 0u);

                screenWidth = displays[0].width;
                screenHeight = displays[0].height;
            }
        }
    }
}

void DumpToFile(const std::string &name, SceneTest::Buffer &buffer, int32_t size)
{
    if (access("/data/scene_dump", F_OK) == -1) {
        return;
    }

    std::stringstream ss;
    static int32_t count = 0;
    ss << "/data/scene_" << count++ << "_" << name << ".raw";

    std::ofstream rawDataFile(ss.str(), std::ofstream::binary);
    if (!rawDataFile.good()) {
        return;
    }
    rawDataFile.write(reinterpret_cast<const char *>(buffer.Get()), size);
    rawDataFile.close();
}

SceneTest::Rect GetWindowRect(const sptr<Window> &window)
{
    return SceneTest::Rect(window->GetX(), window->GetY(), window->GetWidth(), window->GetHeight());
}

bool SceneTest::CheckResultAndScreenshot(int32_t result, Rect area)
{
    int32_t x = area.GetX();
    int32_t y = area.GetY();
    int32_t w = area.GetW();
    int32_t h = area.GetH();
    int32_t k = 0;
    auto &buffer = resultImage[result];
    auto screenshot = screenshotPromise->Await();
    uint32_t *screen = screenshot.Get();
    for (int32_t j = 0; j < h; j++) {
        for (int32_t i = 0; i < w; i++) {
            auto &color = screen[(y + j) * screenshotInfo.width + x + i];
            if (buffer[k++] != color) {
                GSLOG2SE(ERROR) << "area: (" << x << ", " << y << ") " << w << "x" << h;
                GSLOG2SE(ERROR) << "i: " << i << ", j: " << j << ", k: " << k - 1;
                GSLOG2SE(ERROR) << "buffer: " << std::showbase << std::hex
                    << buffer[k - 1] << ", screen: " << color << std::noshowbase << std::dec;
                DumpToFile("buffer", buffer, w * h * 0x4);
                DumpToFile("screen", screenshot, screenshotInfo.width * screenshotInfo.height * 0x4);
                return false;
            }
        }
    }

    return true;
}

bool SceneTest::CheckResultAndScreenshot(int32_t result, Rect area, Rect without)
{
    int32_t x = area.GetX();
    int32_t y = area.GetY();
    int32_t w = area.GetW();
    int32_t h = area.GetH();
    int32_t withoutX = without.GetX();
    int32_t withoutY = without.GetY();
    int32_t withoutW = without.GetW();
    int32_t withoutH = without.GetH();
    int32_t k = 0;
    auto &buffer = resultImage[result];
    auto screenshot = screenshotPromise->Await();
    uint32_t *screen = screenshot.Get();
    for (int32_t j = 0; j < h; j++) {
        for (int32_t i = 0; i < w; i++) {
            if (withoutX <= x + i && x + i <= withoutX + withoutW &&
                    withoutY <= y + j && y + j <= withoutY + withoutH) {
                k++;
                continue;
            }

            auto &color = screen[(y + j) * screenshotInfo.width + x + i];
            if (buffer[k++] != color) {
                GSLOG2SE(ERROR) << "area: (" << x << ", " << y << ") " << w << "x" << h;
                GSLOG2SE(ERROR) << "without: (" << withoutX << ", " << withoutY << ") "
                    << withoutW << "x" << withoutH;
                GSLOG2SE(ERROR) << "i: " << i << ", j: " << j << ", k: " << k - 1;
                GSLOG2SE(ERROR) << "buffer: " << std::showbase << std::hex
                    << buffer[k - 1] << ", screen: " << color << std::noshowbase << std::dec;
                DumpToFile("buffer", buffer, w * h * 0x4);
                DumpToFile("screen", screenshot, screenshotInfo.width * screenshotInfo.height * 0x4);
                return false;
            }
        }
    }

    return true;
}

void SceneTest::RainbowDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count)
{
    auto drawOneLine = [addr, width](uint32_t index, uint32_t color) {
        auto lineAddr = addr + index * width;
        for (uint32_t i = 0; i < width; i++) {
            lineAddr[i] = color;
        }
    };
    auto selectColor = [height](int32_t index) {
        auto func = [height](int32_t x) {
            int32_t h = height;

            constexpr double b = 3.0;
            constexpr double k = -1.0;
            auto ret = b + k * (((x % h) + h) % h) / (height / 0x6);
            ret = abs(ret) - 1.0;
            ret = fmax(ret, 0.0);
            ret = fmin(ret, 1.0);
            return uint32_t(ret * 0xff);
        };

        constexpr uint32_t bShift = 0;
        constexpr uint32_t gShift = 8;
        constexpr uint32_t rShift = 16;
        constexpr uint32_t bOffset = 0;
        constexpr uint32_t gOffset = -2;
        constexpr uint32_t rOffset = +2;
        return 0xff000000 +
            (func(index + bOffset * (height / 0x6)) << bShift) +
            (func(index + gOffset * (height / 0x6)) << gShift) +
            (func(index + rOffset * (height / 0x6)) << rShift);
    };

    constexpr int32_t rainbowDrawFramerate = 100;
    uint32_t offset = (count % rainbowDrawFramerate) * height / rainbowDrawFramerate;
    for (uint32_t i = 0; i < height; i++) {
        auto color = selectColor(offset + i);
        drawOneLine(i, color);
    }
}

void SceneTest::PureColorDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count, uint32_t color)
{
    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = color;
    }
}

sptr<Window> SceneTest::CreateWindow(WindowType type)
{
    auto wm = WindowManager::GetInstance();
    if (wm == nullptr) {
        return nullptr;
    }

    auto option = WindowOption::Get();
    if (option == nullptr) {
        return nullptr;
    }

    sptr<Window> window;
    option->SetWindowType(type);
    auto ret = wm->CreateWindow(window, option);
    if (window == nullptr) {
        GSLOG2SE(ERROR) << "CreateWindow return nullptr: " << ret;
        return nullptr;
    }

    return window;
}

sptr<Window> SceneTest::CreateWindowWH(WindowType type, int32_t w, int32_t h)
{
    auto wm = WindowManager::GetInstance();
    if (wm == nullptr) {
        return nullptr;
    }

    auto option = WindowOption::Get();
    if (option == nullptr) {
        return nullptr;
    }

    sptr<Window> window;
    option->SetWindowType(type);
    option->SetWidth(w);
    option->SetHeight(h);
    auto ret = wm->CreateWindow(window, option);
    if (window == nullptr) {
        GSLOG2SE(ERROR) << "CreateWindow return nullptr: " << ret;
        return nullptr;
    }

    return window;
}

int32_t SceneTest::DrawWindow(const sptr<Window> &window, DrawFunc draw)
{
    return DrawSurface(window->GetSurface(), draw, window->GetWidth(), window->GetHeight());
}

int32_t SceneTest::DrawSurface(const sptr<Surface> &surf, DrawFunc draw, int32_t width, int32_t height)
{
    if (surf == nullptr) {
        GSLOG2SE(ERROR) << "surf is nullptr";
        return 0;
    }

    sptr<SurfaceBuffer> buffer;
    BufferRequestConfig rconfig = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surf->GetDefaultUsage(),
        .timeout = 0,
    };

    GSError ret = surf->RequestBufferNoFence(buffer, rconfig);
    if (ret != GSERROR_OK || buffer == nullptr) {
        GSLOG2SE(ERROR) << "Sync surf request buffer failed";
        return 0;
    }

    if (buffer->GetVirAddr() == nullptr) {
        surf->CancelBuffer(buffer);
        return 0;
    }

    auto w = buffer->GetWidth();
    auto h = buffer->GetHeight();
    draw(reinterpret_cast<uint32_t *>(buffer->GetVirAddr()), w, h, 0);

    Buffer buf(w * h);
    resultImage[resultCount] = buf;
    draw(buf, w, h, 0);

    BufferFlushConfig fconfig = {
        .damage = {
            .w = w,
            .h = h,
        },
    };
    surf->FlushBuffer(buffer, -1, fconfig);
    return resultCount++;
}

namespace {
/*
 * Scene:
 *  win1: normal red
 *  win2: normal blue
 *  win3: normal rainbow
 * Result:
 *  can see normal area is rainbow
 */
HWTEST_F(SceneTest, Scene1, testing::ext::TestSize.Level0)
{
    Init(InitWM);
    int32_t result = 0;
    PART("CreateScene") {
        STEP("win1: normal red") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
        }

        STEP("win2: normal blue") {
            win2 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win2, std::bind(PureColorDraw, DRAW_ARGS, ColorYellow));
        }

        STEP("win3: normal rainbow") {
            win3 = CreateWindow(WINDOW_TYPE_NORMAL);
            result = DrawWindow(win3, std::bind(RainbowDraw, DRAW_ARGS));
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("see normal area is rainbow") {
            auto ret = CheckResultAndScreenshot(result, GetWindowRect(win3));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: status red
 *  win2: normal rainbow
 *  win3: navigation yellow
 * Result:
 *  1. can see status area is red
 *  2. can see normal area is rainbow
 *  3. can see navigation area is yellow
 */
HWTEST_F(SceneTest, Scene2, testing::ext::TestSize.Level0)
{
    Init(InitWM);
    int32_t result1 = 0;
    int32_t result2 = 0;
    int32_t result3 = 0;
    PART("CreateScene") {
        STEP("win1: status red") {
            win1 = CreateWindow(WINDOW_TYPE_STATUS_BAR);
            result1 = DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
        }

        STEP("win2: normal rainbow") {
            win2 = CreateWindow(WINDOW_TYPE_NORMAL);
            result2 = DrawWindow(win2, std::bind(RainbowDraw, DRAW_ARGS));
        }

        STEP("win3: navigation yellow") {
            win3 = CreateWindow(WINDOW_TYPE_NAVI_BAR);
            result3 = DrawWindow(win3, std::bind(PureColorDraw, DRAW_ARGS, ColorYellow));
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("1. see status area is red") {
            auto ret = CheckResultAndScreenshot(result1, GetWindowRect(win1));
            STEP_ASSERT_EQ(ret, true);
        }

        STEP("2. see normal area is rainbow") {
            auto ret = CheckResultAndScreenshot(result2, GetWindowRect(win2));
            STEP_ASSERT_EQ(ret, true);
        }

        STEP("3. see navigation area is yellow") {
            auto ret = CheckResultAndScreenshot(result3, GetWindowRect(win3));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: normal red
 *  win2: normal yellow
 *  win3: normal blue
 *  win1/win2/win3 Hide
 *  win2 Show
 * Result:
 *  can see normal area is yellow
 */
HWTEST_F(SceneTest, Scene3, testing::ext::TestSize.Level0)
{
    Init(InitWM);
    int32_t result = 0;
    PART("CreateScene") {
        STEP("win1: normal red") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
        }

        STEP("win2: normal yellow") {
            win2 = CreateWindow(WINDOW_TYPE_NORMAL);
            result = DrawWindow(win2, std::bind(PureColorDraw, DRAW_ARGS, ColorYellow));
        }

        STEP("win3: normal blue") {
            win3 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win3, std::bind(PureColorDraw, DRAW_ARGS, ColorBlue));
        }

        STEP("win1/win2/win3 Hide") {
            auto promise1 = win1->Hide();
            auto promise2 = win2->Hide();
            auto promise3 = win3->Hide();
            promise1->Await();
            promise2->Await();
            promise3->Await();
        }

        STEP("win2 Show") {
            win2->Show()->Await();
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("see normal area is yellow") {
            auto ret = CheckResultAndScreenshot(result, GetWindowRect(win2));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: normal red
 *  win1 resize to screen's half
 *  win1 move to screen's left top 1/4 position
 * Result:
 *  can see the window area is red
 */
HWTEST_F(SceneTest, Scene4, testing::ext::TestSize.Level0)
{
    Init(InitWM | InitDisplay);
    int32_t result = 0;
    PART("CreateScene") {
        STEP("win1: normal") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
        }

        STEP("win1 resize to screen's half") {
            win1->Resize(screenWidth / 0x2, screenHeight / 0x2)->Await();
        }

        STEP("win1 move to screen's left top 1/4 position") {
            win1->Move(screenWidth / 0x4, screenHeight / 0x4)->Await();
        }

        STEP("win1: normal red") {
            result = DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
        }
        std::this_thread::sleep_for(600ms);
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("can see the window area is red") {
            auto ret = CheckResultAndScreenshot(result, GetWindowRect(win1));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: normal red
 *  win2: normal yellow
 *  win3: normal blue
 *  win1 switchtop
 * Result:
 *  can see normal area is red
 */
HWTEST_F(SceneTest, Scene5, testing::ext::TestSize.Level0)
{
    Init(InitWM);
    int32_t result = 0;
    PART("CreateScene") {
        STEP("win1: normal red") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
            result = DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
        }

        STEP("win2: normal yellow") {
            win2 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win2, std::bind(PureColorDraw, DRAW_ARGS, ColorYellow));
        }

        STEP("win3: normal blue") {
            win3 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win3, std::bind(PureColorDraw, DRAW_ARGS, ColorBlue));
        }

        STEP("win1 Show") {
            win1->SwitchTop()->Await();
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("see normal area is red") {
            auto ret = CheckResultAndScreenshot(result, GetWindowRect(win1));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: normal red
 *  win2: alarm yellow
 *  win3: systemui blue
 *  win4: popup rainbow
 *  win1 switchtop
 * Result:
 *  can see win4 area is rainbow
 */
HWTEST_F(SceneTest, Scene6, testing::ext::TestSize.Level0)
{
    Init(InitWM);
    int32_t result = 0;
    PART("CreateScene") {
        STEP("win1: normal red") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
        }

        STEP("win2: alarm yellow") {
            win2 = CreateWindow(WINDOW_TYPE_ALARM_SCREEN);
            DrawWindow(win2, std::bind(PureColorDraw, DRAW_ARGS, ColorYellow));
        }

        STEP("win3: systemui blue") {
            win3 = CreateWindow(WINDOW_TYPE_SYSTEM_UI);
            DrawWindow(win3, std::bind(PureColorDraw, DRAW_ARGS, ColorBlue));
        }

        STEP("win4: popup rainbow") {
            constexpr int32_t wh = 256; // for align
            win4 = CreateWindowWH(WINDOW_TYPE_POPUP, wh, wh);
            result = DrawWindow(win4, std::bind(RainbowDraw, DRAW_ARGS));
        }

        STEP("win1 Show") {
            win1->SwitchTop()->Await();
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("can see win4 area is rainbow") {
            auto ret = CheckResultAndScreenshot(result, GetWindowRect(win4));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: normal red
 *  win1 enter PIP mode(size: screen's half, position: screen's left top 1/4 position)
 *  win2: normal blue
 * Result:
 *  can see the win1 area is red
 *  can see the win2 but not win1 area is blue
 */
HWTEST_F(SceneTest, Scene7, testing::ext::TestSize.Level0)
{
    Init(InitWM | InitDisplay);
    int32_t result1 = 0;
    int32_t result2 = 0;
    PART("CreateScene") {
        STEP("win1: normal") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
        }

        STEP("win1 enter PIP mode(size: screen's half, position: screen's left top 1/4 position)") {
            win1->EnterPIPMode(screenWidth / 0x4, screenHeight / 0x4, screenWidth / 0x2, screenHeight / 0x2);
        }
        std::this_thread::sleep_for(600ms);

        STEP("win1: normal red") {
            result1 = DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
        }

        STEP("win2: normal blue") {
            win2 = CreateWindow(WINDOW_TYPE_NORMAL);
            result2 = DrawWindow(win2, std::bind(PureColorDraw, DRAW_ARGS, ColorBlue));
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("can see the win1 area is red") {
            auto ret = CheckResultAndScreenshot(result1, GetWindowRect(win1));
            STEP_ASSERT_EQ(ret, true);
        }

        STEP("can see the win2 but not win1 area is blue") {
            auto ret = CheckResultAndScreenshot(result2, GetWindowRect(win2), GetWindowRect(win1));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: normal red
 *  win2: normal yellow
 *  enter split mode
 * Result:
 *  can see normal area is yellow
 */
HWTEST_F(SceneTest, Scene8, testing::ext::TestSize.Level0)
{
    Init(InitWM | InitWMS);
    int32_t result1 = 0;
    int32_t result2 = 0;
    PART("CreateScene") {
        auto wms = WindowManagerServiceClient::GetInstance()->GetService();
        STEP("win1: normal red") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
            wms->SetSplitMode(SPLIT_MODE_SINGLE);
        }
        std::this_thread::sleep_for(50ms);

        wms->SetSplitMode(SPLIT_MODE_SELECT, 0, 0);

        STEP("win2: normal yellow") {
            win2 = CreateWindow(WINDOW_TYPE_NORMAL);
            DrawWindow(win2, std::bind(PureColorDraw, DRAW_ARGS, ColorYellow));
            wms->SetSplitMode(SPLIT_MODE_CONFIRM);
        }
        std::this_thread::sleep_for(600ms);

        STEP("enter split mode") {
            result1 = DrawWindow(win1, std::bind(PureColorDraw, DRAW_ARGS, ColorRed));
            result2 = DrawWindow(win2, std::bind(PureColorDraw, DRAW_ARGS, ColorYellow));
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("see the win1 area is red") {
            auto ret = CheckResultAndScreenshot(result1, GetWindowRect(win1));
            STEP_ASSERT_EQ(ret, true);
        }

        STEP("see the win2 area is yellow") {
            auto ret = CheckResultAndScreenshot(result2, GetWindowRect(win2));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: normal red but not subwindow
 *  sub1: normal yellow
 * Result:
 *  can see subwindow area is yellow
 *  can see window but not subwindow area is red
 */
HWTEST_F(SceneTest, Scene9, testing::ext::TestSize.Level0)
{
    Init(InitWM | InitDisplay);
    int32_t result1 = 0;
    int32_t result2 = 0;
    PART("CreateScene") {
        STEP("win1: normal red but not subwindow") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
            auto func = [=](uint32_t *addr, uint32_t width, uint32_t height, uint32_t count) {
                PureColorDraw(addr, width, height, count, ColorRed);
                Cpudraw draw(addr, width, height);
                draw.SetColor(ColorTransparent);
                draw.DrawRect(screenWidth / 0x4 - win1->GetX(),
                    screenHeight / 0x4 - win1->GetY(), screenWidth / 0x2, screenHeight / 0x2);
            };
            result1 = DrawWindow(win1, func);
        }

        STEP("sub1: normal yellow") {
            auto so = SubwindowOption::Get();
            so->SetX(screenWidth / 0x4 - win1->GetX());
            so->SetY(screenHeight / 0x4 - win1->GetY());
            so->SetWidth(screenWidth / 0x2);
            so->SetHeight(screenHeight / 0x2);
            WindowManager::GetInstance()->CreateSubwindow(sub1, win1, so);
            auto func = std::bind(PureColorDraw, DRAW_ARGS, ColorYellow);
            result2 = DrawSurface(sub1->GetSurface(), func, screenWidth / 0x2, screenHeight / 0x2);
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("can see subwindow area is yellow") {
            auto ret = CheckResultAndScreenshot(result2,
                Rect(screenWidth / 0x4, screenHeight / 0x4, screenWidth / 0x2, screenHeight / 0x2));
            STEP_ASSERT_EQ(ret, true);
        }

        STEP("can see window but not subwindow area is red") {
            auto ret = CheckResultAndScreenshot(result1, GetWindowRect(win1),
                Rect(screenWidth / 0x4, screenHeight / 0x4, screenWidth / 0x2, screenHeight / 0x2));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}

/*
 * Scene:
 *  win1: normal red but not subwindow
 *  sub1: normal yellow
 *  move sub1
 * Result:
 *  can see subwindow area is yellow
 *  can see window but not subwindow area is red
 */
HWTEST_F(SceneTest, Scene10, testing::ext::TestSize.Level0)
{
    Init(InitWM | InitDisplay);
    int32_t result1 = 0;
    int32_t result2 = 0;
    PART("CreateScene") {
        STEP("win1: normal red but not subwindow") {
            win1 = CreateWindow(WINDOW_TYPE_NORMAL);
            auto func = [=](uint32_t *addr, uint32_t width, uint32_t height, uint32_t count) {
                PureColorDraw(addr, width, height, count, ColorRed);
                Cpudraw draw(addr, width, height);
                draw.SetColor(ColorTransparent);
                draw.DrawRect(0, 0, screenWidth / 0x4, screenHeight / 0x4);
            };
            result1 = DrawWindow(win1, func);
        }

        STEP("sub1: normal yellow") {
            auto so = SubwindowOption::Get();
            so->SetX(screenWidth / 0x4 - win1->GetX());
            so->SetY(screenHeight / 0x4 - win1->GetY());
            so->SetWidth(screenWidth / 0x2);
            so->SetHeight(screenHeight / 0x2);
            WindowManager::GetInstance()->CreateSubwindow(sub1, win1, so);
            auto func = std::bind(PureColorDraw, DRAW_ARGS, ColorYellow);
            DrawSurface(sub1->GetSurface(), func, screenWidth / 0x2, screenHeight / 0x2);
        }

        STEP("move sub1") {
            sub1->Move(0, 0);
            sub1->Resize(screenWidth / 0x4, screenHeight / 0x4);
            auto func = std::bind(PureColorDraw, DRAW_ARGS, ColorYellow);
            result2 = DrawSurface(sub1->GetSurface(), func, screenWidth / 0x4, screenHeight / 0x4);
        }
    }

    PART("CheckResult") {
        ScreenShot();

        STEP("can see subwindow area is yellow") {
            auto ret = CheckResultAndScreenshot(result2,
                Rect(win1->GetX(), win1->GetY(), screenWidth / 0x4, screenHeight / 0x4));
            STEP_ASSERT_EQ(ret, true);
        }

        STEP("can see window but not subwindow area is red") {
            auto ret = CheckResultAndScreenshot(result1, GetWindowRect(win1),
                Rect(win1->GetX(), win1->GetY(), screenWidth / 0x4, screenHeight / 0x4));
            STEP_ASSERT_EQ(ret, true);
        }
    }
}
} // namespace
} // namespace OHOS
