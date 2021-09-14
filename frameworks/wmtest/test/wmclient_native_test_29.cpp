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

#include "wmclient_native_test_29.h"

#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <securec.h>
#include <unistd.h>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest29 : public INativeTest, public IScreenShotCallback {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "continuous shot screen";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 29;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    static void signalHandler(int sig)
    {
        printf("shot screen is over\n");
        running = false;
        
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        std::signal(SIGINT, signalHandler);
        PostTask(std::bind(&WMClientNativeTest29::AfterRun, this));
    }

    void AfterRun()
    {
        do {
            promise = new Promise<bool>();
            auto wm = WindowManager::GetInstance();
            WMError wmerror = wm->ListenNextScreenShot(0, this);
            printf("wmerror == %d \n",wmerror);
        } while (promise->Await());
    }

    void OnScreenShot(const struct WMImageInfo &info) override
    {
        printf("write completed times: %d\n", ++times);
        FILE* rawDataFile = fopen("/data/wmtest27.raw", "ab+");
        if (rawDataFile != nullptr) {
            printf("width: %u, height: %u\n", info.width, info.height);
            printf("format: %u, size: %u, data: %p\n", info.format, info.size, info.data);
            fwrite(static_cast<const char *>(info.data), info.size, 1, rawDataFile);
            fseek(rawDataFile,0, SEEK_END);
            size_t size = ftell(rawDataFile);
            printf("allsize : %u \n",size);
            fclose(rawDataFile);
        }else{
            printf("rawDataFile open fail");
        }
        sleep(2);
        promise->Resolve(running);
        if(running == false){
            ExitTest();
        }
    }

private:
    static inline std::atomic<bool> running = true;
    u_int32_t times = 0;
    sptr<Promise<bool>> promise = nullptr;
} g_autoload;
} // namespace