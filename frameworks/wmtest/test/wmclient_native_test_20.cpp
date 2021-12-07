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

#include "wmclient_native_test_20.h"

#include <cstdio>
#include <sstream>
#include <unistd.h>

#include <gslogger.h>
#include <ipc_object_stub.h>
#include <key_event_handler.h>
#include <multimodal_event_handler.h>
#include <touch_event_handler.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest20 : public INativeTest, public IAdjacentModeChangeListenerClazz {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "split mode";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 20;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    int8_t GetForkNumber() const override
    {
        constexpr uint32_t forkNumber = 2;
        return forkNumber;
    }

    void Run(int32_t argc, const char **argv) override
    {
        argc_ = argc;
        argv_ = argv;
        printf("Run: %d\n", processSequence);
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        auto wmsc = WindowManagerServiceClient::GetInstance();
        wmsc->Init();
        wms = wmsc->GetService();
        wms->OnAdjacentModeChange(this);
        switch (processSequence) {
            case 0:
                Run0(argc, argv);
                break;
            case 1:
                Run1(argc, argv);
                break;
            case 2:
                Run2(argc, argv);
                break;
        }
    }

    void Run0(int32_t argc, const char **argv)
    {
        GSLOG2SO(INFO) << getpid() << " run0";
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            printf("%d ", GetProcessSequence());
            ExitTest();
            return;
        }

        auto onSizeChange = [this](uint32_t w, uint32_t h) {
            printf("onSizeChange %u %u\n", w, h);
            config.width = w;
            config.height = h;
            window->Resize(w, h);
        };
        window->OnSizeChange(onSizeChange);

        auto surface = window->GetSurface();
        config.width = surface->GetDefaultWidth();
        config.height = surface->GetDefaultHeight();
        config.strideAlignment = 0x8;
        config.format = PIXEL_FMT_RGBA_8888;
        config.usage = surface->GetDefaultUsage();

        window->SwitchTop();
        auto draw = std::bind(&WMClientNativeTest20::Draw0, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        windowDrawer = NativeTestDrawer::CreateDrawer(draw, surface, &config);
        windowDrawer->DrawOnce();
    }

    void Draw0(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
    {
        if (adjStatus == ADJACENT_MODE_STATUS_CLEAR) {
            NativeTestDraw::RainbowDraw(vaddr, width, height, 0);
        } else if (adjStatus == ADJACENT_MODE_STATUS_VAGUE) {
            NativeTestDraw::RainbowDraw(vaddr, width, height, 0);
            NativeTestDraw::BlurDraw(vaddr, width, height, count);
        } else {
            NativeTestDraw::RainbowDraw(vaddr, width, height, count);
        }
    }

    void Run1(int32_t argc, const char **argv)
    {
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_SYSTEM_UI);
        if (window == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            printf("%d ", GetProcessSequence());
            ExitTest();
            return;
        }

        auto surface = window->GetSurface();
        window->SwitchTop();
        auto draw = std::bind(&WMClientNativeTest20::Draw1, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        windowDrawer = NativeTestDrawer::CreateDrawer(draw, surface);
        for (auto &icon : icons) {
            icon.x *= window->GetWidth();
            icon.y *= window->GetHeight();
            icon.w *= window->GetWidth();
            icon.h *= window->GetHeight();
        }
        total.x *= window->GetWidth();
        total.y *= window->GetHeight();
        total.w *= window->GetWidth();
        total.h *= window->GetHeight();

        MMIEventHdl.RegisterStandardizedEventHandle(token, window->GetID(), thandler);
        MMIEventHdl.RegisterStandardizedEventHandle(token, window->GetID(), khandler);
    }

    void Draw1(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
    {
        auto addr = reinterpret_cast<uint32_t *>(vaddr);

        // background color: 0x00000000
        for (uint32_t j = 0; j < height; j++) {
            for (uint32_t i = 0; i < width; i++) {
                addr[j * width + i] = 0x00000000;
            }
        }

        auto drawIcon = [addr, width = static_cast<int32_t>(width)](int32_t x,
            int32_t y, int32_t w, int32_t h, uint32_t c) {
            for (int32_t j = y; j < y + h; j++) {
                for (int32_t i = x; i < x + w; i++) {
                    addr[j * width + i] = c;
                }
            }
        };

        int32_t x = total.x;
        int32_t y = total.y;
        width = total.w;
        height = total.h;
        // box background color: 0xffffffff
        drawIcon(x, y, width, height, 0xffffffff);

        // border = 1, color: 0xffaaaaaa
        for (uint32_t j = y; j < y + height; j++) {
            addr[j * width + x] = 0xffaaaaaa;
            addr[j * width + width - 1] = 0xffaaaaaa;
        }
        for (uint32_t i = x; i < x + width; i++) {
            addr[y * width + i] = 0xffaaaaaa;
            addr[(height - 1) * width + i] = 0xffaaaaaa;
        }

        for (auto &icon : icons) {
            drawIcon(icon.x, icon.y, icon.w, icon.h, icon.c);
        }
    }

    bool OnKey(const KeyEvent &event)
    {
        if (event.IsKeyDown() ==  true && event.GetKeyCode() == KeyEventEnum::KEY_BACK) {
            printf("%d\n", GetProcessSequence());
            handler->PostTask(ExitTest);
            return true;
        }
        return false;
    }

    bool OnTouch(const TouchEvent &event)
    {
        int32_t index = event.GetIndex();
        int32_t x = event.GetPointerPosition(index).GetX();
        int32_t y = event.GetPointerPosition(index).GetY();
        if (event.GetAction() == TouchEnum::PRIMARY_POINT_DOWN) {
            for (auto &icon : icons) {
                if (icon.x <= x && x <= icon.x + icon.w && icon.y <= y && y <= icon.y + icon.h) {
                    currentIcon = &icon;
                    if (currentIcon == &icons[0]) {
                        wms->SetAdjacentMode(ADJ_MODE_SINGLE);
                    } else {
                        wms->SetAdjacentMode(ADJ_MODE_UNENABLE);
                    }
                    break;
                }
            }
            downX = x;
            downY = y;
            return true;
        }

        if (event.GetAction() == TouchEnum::POINT_MOVE || event.GetAction() == TouchEnum::PRIMARY_POINT_UP) {
            if (currentIcon) {
                currentIcon->x += x - downX;
                currentIcon->y += y - downY;
            }

            downX = x;
            downY = y;
            handler->PostTask(std::bind(&NativeTestDrawer::DrawOnce, windowDrawer, nullptr));
            if (currentIcon && currentIcon != &icons[0] && event.GetAction() == TouchEnum::POINT_MOVE) {
                if (InSystemUIBox(x, y)) {
                    wms->SetAdjacentMode(ADJ_MODE_SINGLE);
                } else {
                    wms->SetAdjacentMode(ADJ_MODE_SELECT, x, y);
                }
            } else if (currentIcon && currentIcon != &icons[0]) { // UP
                if (InSystemUIBox(x, y)) {
                    wms->SetAdjacentMode(ADJ_MODE_NULL);
                } else {
                    wms->SetAdjacentMode(ADJ_MODE_CONFIRM);
                    StartProcess2AndExit();
                }
            }
            return true;
        }
        return false;
    }

    void StartProcess2AndExit()
    {
        auto pid = fork();
        if (pid == 0) {
            std::vector<const char *> args;
            for (const char **p = argv_; *p != nullptr; p++) {
                args.push_back(*p);
            }
            args.push_back("--process=2");
            args.push_back(nullptr);
            execvp(args[0], args.data());
        }
        ExitTest();
    }

    bool InSystemUIBox(int32_t x, int32_t y)
    {
        return total.x <= x && x <= total.x + total.w && total.y <= y && y <= total.y + total.h;
    }

    void Run2(int32_t argc, const char **argv)
    {
    }

    void OnAdjacentModeChange(AdjacentModeStatus status) override
    {
        if (status == ADJACENT_MODE_STATUS_DESTROY) {
            printf("%d\n", GetProcessSequence());
            handler->PostTask(ExitTest);
        } else if (status == ADJACENT_MODE_STATUS_RETAIN) {
            adjStatus = ADJACENT_MODE_STATUS_CLEAR;
            handler->PostTask(std::bind(&NativeTestDrawer::DrawOnce, windowDrawer, nullptr));
        } else {
            adjStatus = status;
            handler->PostTask(std::bind(&NativeTestDrawer::DrawOnce, windowDrawer, nullptr));
        }
    }

private:
    class KeyEventHandler : public MMI::KeyEventHandler {
    public:
        explicit KeyEventHandler(WMClientNativeTest20 *test) : test_(test)
        {
        }

        virtual bool OnKey(const KeyEvent &event) override
        {
            return test_->OnKey(event);
        }

    private:
        WMClientNativeTest20 *test_ = nullptr;
    };

    class TouchEventHandler : public MMI::TouchEventHandler {
    public:
        explicit TouchEventHandler(WMClientNativeTest20 *test) : test_(test)
        {
        }

        virtual bool OnTouch(const TouchEvent &event) override
        {
            return test_->OnTouch(event);
        }

    private:
        WMClientNativeTest20 *test_ = nullptr;
    };

    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    sptr<IWindowManagerService> wms;
    sptr<Window> window = nullptr;
    uint32_t color_ = 0x00000000;
    sptr<NativeTestDrawer> windowDrawer = nullptr;
    BufferRequestConfig config = {};
    AdjacentModeStatus adjStatus = ADJACENT_MODE_STATUS_CLEAR;

    static constexpr double xx = 0.85;
    static constexpr double ww = 0.15;
    static constexpr double yy = 0.1;
    static constexpr double hh = 0.8;

    struct Position {
        double x;
        double y;
        double w;
        double h;
        uint32_t c;
    };
    struct Position icons[0x4] = {
        { xx + 0.3 * ww, yy + 1 * hh / 9, 0.4 * ww, hh / 9, 0xffff0000 },
        { xx + 0.3 * ww, yy + 3 * hh / 9, 0.4 * ww, hh / 9, 0xff00ff00 },
        { xx + 0.3 * ww, yy + 5 * hh / 9, 0.4 * ww, hh / 9, 0xff0000ff },
        { xx + 0.3 * ww, yy + 7 * hh / 9, 0.4 * ww, hh / 9, 0xffff00ff },
    };
    struct Position *currentIcon = nullptr;
    struct Position total = { xx, yy, ww, hh };
    sptr<IRemoteObject> token = new IPCObjectStub(u"wmclient_native_test_20");
    sptr<KeyEventHandler> khandler = new KeyEventHandler(this);
    sptr<TouchEventHandler> thandler = new TouchEventHandler(this);
    int32_t downX = 0;
    int32_t downY = 0;

    int32_t argc_ = 0;
    const char **argv_ = nullptr;
} g_autoload;
} // namespace
