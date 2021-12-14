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

#include "wmservice_native_test_7.h"

#include <chrono>
#include <thread>

#include <gslogger.h>
#include <option_parser.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMServiceNativeTest7 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "start page";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmservice";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 7;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            return;
        }

        window->SwitchTop();
        auto surface = window->GetSurface();

        OptionParser oparser;
        std::string filename = "";
        oparser.AddArguments(filename);
        if (oparser.Parse(argc, argv)) {
            ExitTest();
            return;
        }

        auto wmsc = WindowManagerServiceClient::GetInstance();
        wmsc->Init();
        auto wms = wmsc->GetService();

        auto gretCreate = wms->CreateLaunchPage(filename);
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::RainbowDraw, surface);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        auto gretDestroy = wms->CancelLaunchPage();
        GSLOG2SO(INFO) << "return: CreateLaunchPage: " << GSErrorStr(gretCreate) <<
             ", CancelLaunchPage: " << GSErrorStr(gretDestroy);
    }
private:
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
} g_autoload;
} // namespace
