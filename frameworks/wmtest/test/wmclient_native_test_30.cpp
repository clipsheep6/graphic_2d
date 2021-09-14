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

#include "wmclient_native_test_30.h"

#include <atomic>
#include <sys/stat.h>
#include <sys/types.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <signal.h>
#include <securec.h>
#include <unistd.h>

#include <display_type.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"
#include "wmclient_native_test_19.h"

using namespace OHOS;
const mode_t MKDIR_MODE = 0777;
namespace {
class WMClientNativeTest30 : public WMClientNativeTest19, public IWindowShotCallback  {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "continuous shot window";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 30;
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
        printf("shot window is over\n");
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
        WMClientNativeTest19::Run(argc, argv);
        printf("Run\n");
        constexpr uint32_t delayTime = 1000;
        PostTask(std::bind(&WMClientNativeTest30::AfterRun, this), delayTime);
    }
    void AfterRun()
    {
        printf("wid == %u \n",windowId);
        do {
            auto wm = WindowManager::GetInstance();
            if(windowId != 0 && Isover != false){
                printf("windowId == %u \n",windowId);
                WMError wmerror = wm->ListenContinueNextWindowShot(windowId, this);
                printf("wmerror == %d \n",wmerror);
                Isover = false;
            }
        } while (running);
    }

    void OnWindowShot(const struct WMImageInfo &info) override
    {
        printf("write completed times: %d\n", ++times);
        std::string defaultPath = "/data/wmtest30";
	    if (0 != access(defaultPath.c_str(), 0)){
		    int a = mkdir(defaultPath.c_str(),MKDIR_MODE);
            if(a == 0){
                printf("mkdir/data/wmtest30 success\n");
            }else{
                printf("mkdir/data/wmtest30 fail\n");
            }
	    }
        std::stringstream wid; 
        wid << windowId;
        std::string rawid = "/data/wmtest30/wmtest" + wid.str() + ".raw";
        printf("rawid = %s \n",rawid.c_str());
        FILE* rawDataFile = fopen(rawid.c_str(), "ab+");
        if(rawDataFile == nullptr){
            printf("rawDataFile is not exist");
            rawDataFile = fopen(rawid.c_str(), "wb+");
        }else{
            printf("rawDataFile is exist");
        }
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
        Isover = true;
        sleep(2);
        if(running == false){
            ExitTest();
        }
    }
    static inline std::atomic<bool> running = true;
    static inline std::atomic<bool> Isover = true;
    uint32_t times = 0;
    sptr<Promise<bool>> promise = nullptr;
    sptr<Window> window = nullptr;
} g_autoload;
} // namespace

