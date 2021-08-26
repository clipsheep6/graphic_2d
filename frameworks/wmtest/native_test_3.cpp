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

#include "native_test_3.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest3 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "navi bar window";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 3;
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

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NAVI_BAR);
        if (window == nullptr) {
            printf("NativeTestFactory::CreateWindow return nullptr\n");
            return;
        }

        window->SwitchTop();
        auto surface = window->GetSurface();
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, surface);
    }

private:
    sptr<Window> window;
    sptr<NativeTestSync> windowSync;
} g_autoload;
} // namespace
