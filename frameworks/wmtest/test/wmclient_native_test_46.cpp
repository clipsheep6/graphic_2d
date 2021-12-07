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

#include "wmclient_native_test_46.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <thread>
#include <iservice_registry.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest46 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "input method window";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 46;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    // void OnAdjacentModeChange(int32_t wid, int32_t x, int32_t y, int32_t width, int32_t height, AdjacentModeStatus status) override
    // {
    //     printf("OnAdjacentModeChange46");
    //     if(status ==  ADJACENT_MODE_STATUS_DESTROY)
    //     {
    //         wms->DestroyWindow(wid);
    //     }
    // }

    // *********************************************************************************************** //
    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        auto wmsc = WindowManagerServiceClient::GetInstance();
        wmsc->Init();
        wms = wmsc->GetService();

        // wms->OnAdjacentModeChange(this);
        int count = 0;

        constexpr uint32_t delayTime = 2000;
        PostTask(std::bind(&WMClientNativeTest46::TouchDown, this), delayTime * count++);
        PostTask(std::bind(&WMClientNativeTest46::TouchMove1, this), delayTime * count++);
        PostTask(std::bind(&WMClientNativeTest46::TouchMove2, this), delayTime * count++);
        PostTask(std::bind(&WMClientNativeTest46::TouchMove3, this), delayTime * count++);
        PostTask(std::bind(&WMClientNativeTest46::TouchUp, this), delayTime * count++);
    }

    void TouchDown()
    {
        auto wretPromiseFinal = wms->SetAdjacentMode(ADJ_MODE_DIVIDER_TOUCH_DOWN);
        auto wretFinal = wretPromiseFinal->Await();
        if (wretFinal != WM_OK) {
            printf("Register Window Change Listener failed\n");
            ExitTest();
            return;
        }
    }

    void TouchMove1()
    {
        auto wretPromiseFinal = wms->SetAdjacentMode(ADJ_MODE_DIVIDER_TOUCH_MOVE, 0, 100);
        auto wretFinal = wretPromiseFinal->Await();
        if (wretFinal != WM_OK) {
            printf("Register Window Change Listener failed\n");
            ExitTest();
            return;
        }
    }

    void TouchMove2()
    {
        auto wretPromiseFinal = wms->SetAdjacentMode(ADJ_MODE_DIVIDER_TOUCH_MOVE, 0, 400);
        auto wretFinal = wretPromiseFinal->Await();
        if (wretFinal != WM_OK) {
            printf("Register Window Change Listener failed\n");
            ExitTest();
            return;
        }
    }

    void TouchMove3()
    {
        auto wretPromiseFinal = wms->SetAdjacentMode(ADJ_MODE_DIVIDER_TOUCH_MOVE, 0, 800);
        auto wretFinal = wretPromiseFinal->Await();
        if (wretFinal != WM_OK) {
            printf("Register Window Change Listener failed\n");
            ExitTest();
            return;
        }
    }

    void TouchUp()
    {
        auto wretPromiseFinal = wms->SetAdjacentMode(ADJ_MODE_DIVIDER_TOUCH_UP);
        auto wretFinal = wretPromiseFinal->Await();
        if (wretFinal != WM_OK) {
            printf("Register Window Change Listener failed\n");
            ExitTest();
            return;
        }
    }

private:
    sptr<IWindowManagerService> wms = nullptr;
} g_autoload;
} // namespace
