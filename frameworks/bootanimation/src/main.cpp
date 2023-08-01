/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <display_type.h>
#include <display_manager.h>
#include <event_handler.h>
#include "boot_animation.h"
#include "util.h"
#include "core/transaction/rs_interfaces.h"

#include "cxx.h"
#include "lib.rs.h"

using namespace OHOS;

int main(int argc, const char *argv[])
{
    LOGI("main enter");
    int32_t ret = add_function(1, 2);
    LOGE("call add_function，ret: %d", ret);
    WaitRenderServiceInit();

    auto& dms = OHOS::Rosen::DisplayManager::GetInstance();
    auto displayIds = dms.GetAllDisplayIds();

    while (displayIds.empty()) {
        LOGI("displayIds is empty, retry to get displayIds");
        displayIds = dms.GetAllDisplayIds();
        usleep(SLEEP_TIME_US);
    }

    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    Rosen::ScreenId defaultId = interface.GetDefaultScreenId();
    if (defaultId == Rosen::INVALID_SCREEN_ID) {
        LOGE("invalid default screen id, return");
        return 0;
    }

    Rosen::RSScreenModeInfo modeinfo = interface.GetScreenActiveMode(defaultId);
    int screenWidth = modeinfo.GetScreenWidth();
    int screenHeight = modeinfo.GetScreenHeight();

    BootAnimation bootAnimation;
    bootAnimation.Run(defaultId, screenWidth, screenHeight);

    LOGI("main exit");
    return 0;
}
