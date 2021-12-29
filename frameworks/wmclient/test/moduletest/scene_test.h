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

#ifndef FRAMEWORKS_WMCLIENT_TEST_MODULETEST_SCENE_TEST_H
#define FRAMEWORKS_WMCLIENT_TEST_MODULETEST_SCENE_TEST_H

#include <gtest/gtest.h>
#include <window_manager.h>

namespace OHOS {
#define DRAW_ARGS std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
using DrawFunc = std::function<void(uint32_t *, uint32_t, uint32_t, uint32_t)>;

class SceneTest : public testing::Test, public IScreenShotCallback {
public:
    // event
    void OnScreenShot(const struct WMImageInfo &info) override;
    void ScreenShot();

    static constexpr int32_t InitWM = 1;
    static constexpr int32_t InitWMS = 2;
    static constexpr int32_t InitDisplay = 4;
    void Init(int32_t initOption);
    int32_t screenWidth = 0;
    int32_t screenHeight = 0;

    class Rect {
    public:
        Rect(int32_t x, int32_t y, int32_t w, int32_t h)
        {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        }

        int32_t GetX() const
        {
            return x;
        }

        int32_t GetY() const
        {
            return y;
        }

        int32_t GetW() const
        {
            return w;
        }

        int32_t GetH() const
        {
            return h;
        }

    private:
        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;
    };
    bool CheckResultAndScreenshot(int32_t result, Rect area);
    bool CheckResultAndScreenshot(int32_t result, Rect area, Rect without);

    static constexpr uint32_t ColorRed = 0xff0000ff;
    static constexpr uint32_t ColorYellow = 0xff00ffff;
    static constexpr uint32_t ColorGreen = 0xff00ff00;
    static constexpr uint32_t ColorCyan = 0xffffff00;
    static constexpr uint32_t ColorBlue = 0xffff0000;
    static constexpr uint32_t ColorPurple = 0xffff00ff;
    static constexpr uint32_t ColorWhite = 0xffffffff;
    static constexpr uint32_t ColorBlack = 0xff000000;
    static constexpr uint32_t ColorTransparent = 0x00000000;
    static void RainbowDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count);
    static void PureColorDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count, uint32_t color);
    sptr<Window> CreateWindow(WindowType type);
    sptr<Window> CreateWindowWH(WindowType type, int32_t w, int32_t h);
    int32_t DrawWindow(const sptr<Window> &window, DrawFunc draw);
    int32_t DrawSurface(const sptr<Surface> &surf, DrawFunc draw, int32_t width, int32_t height);

    class Buffer {
    public:
        Buffer()
        {
        }

        explicit Buffer(std::nullptr_t p)
        {
        }

        explicit Buffer(int32_t size)
        {
            ptr = std::make_shared<std::unique_ptr<uint32_t[]>>();
            *ptr = std::make_unique<uint32_t[]>(size);
        }

        operator uint32_t *()
        {
            return ptr->get();
        }

        uint32_t *Get()
        {
            return ptr->get();
        }

        std::shared_ptr<std::unique_ptr<uint32_t[]>> ptr = nullptr;
    };
    sptr<Promise<Buffer>> screenshotPromise = nullptr;
    struct WMImageInfo screenshotInfo;

    sptr<Window> win1 = nullptr;
    sptr<Window> win2 = nullptr;
    sptr<Window> win3 = nullptr;
    sptr<Window> win4 = nullptr;
    sptr<Subwindow> sub1 = nullptr;
    int32_t resultCount = 1;
    std::map<int32_t, Buffer> resultImage;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMCLIENT_TEST_MODULETEST_SCENE_TEST_H
