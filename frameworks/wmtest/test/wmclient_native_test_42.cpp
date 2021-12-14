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

#include "wmclient_native_test_42.h"

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
class WMClientNativeTest42 : public INativeTest {
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
        constexpr int32_t id = 42;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    // *********************************************************************************************** //
    void Run(int32_t argc, const char **argv) override
    {
        pipe(pipeFd);
        pid_t pid = fork();
        if (pid < 0) {
            printf("%s fork failed", __func__);
            ExitTest();
            return;
        }

        if (pid == 0) {
            ChildProcess();
        } else {
            MainProcess();
        }
    }

    void ChildProcess()
    {
        printf("ChildProcess start \n");
        char buf[10];
        read(pipeFd[0], &buf, sizeof(buf));

        constexpr uint32_t delayTime = 1000;
        PostTask(std::bind(&WMClientNativeTest42::ChildProcessAfter, this), delayTime);
    }

    void ChildProcessAfter()
    {
        printf("ChildProcess createwindow start \n");

        char buf[10] = "end";
        write(pipeFd[1], buf, sizeof(buf));
        printf("ChildProcess end \n");
    }

    void MainProcess()
    {
        printf("MainProcess start \n");
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        auto wmsc = WindowManagerServiceClient::GetInstance();
        wmsc->Init();
        wms = wmsc->GetService();

        auto wretPromise = wms->SetAdjacentMode(ADJ_MODE_SINGLE);
        auto wret = wretPromise->Await();
        if (wret != WM_OK) {
            printf("Register Window Change Listener failed\n");
            ExitTest();
            return;
        }

        uint32_t msg = 0;
        write(pipeFd[1], &msg, sizeof(msg));
        constexpr uint32_t delayTime = 1000;
        PostTask(std::bind(&WMClientNativeTest42::MAinProcessAfter, this), delayTime);
    }

    void MAinProcessAfter()
    {
        std::this_thread::yield();
        char buf[10];
        read(pipeFd[0], buf, sizeof(buf));
        printf("MainProcess end \n");
    }
private:

    sptr<IWindowManagerService> wms = nullptr;
    sptr<Window> winBack= nullptr;
    sptr<NativeTestSync> winBackSync = nullptr;
    static inline int pipeFd[2];

} g_autoload;
} // namespace
