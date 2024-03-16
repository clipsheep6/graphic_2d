/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "native_hgm.h"

#include <stdint.h>
#include <vector>
#include <cstdio>
#include "transaction/rs_interfaces.h"
#include "hgm_log.h"
#include "hgm_command.h"

using namespace OHOS;
using namespace Rosen;

void OH_NativeHgm_SetRefreshRateMode(int32_t mode)
{
    auto& interfaces = OHOS::Rosen::RSInterfaces::GetInstance();
    interfaces.SetRefreshRateMode(mode);
}

int32_t OH_NativeHgm_GetScreenCurrentRefreshRate(int32_t screen)
{
    ScreenId id = 0;
    if (screen < 0) {
        HGM_LOGE("Native HGM GetScreenCurrentRefreshRate illegal screenId");
    } else {
        id = static_cast<ScreenId>(screen);
    }
    auto& interfaces = OHOS::Rosen::RSInterfaces::GetInstance();
    uint32_t currentRate = interfaces.GetScreenCurrentRefreshRate(id);
    int32_t returnInt = 0;
    if (currentRate > static_cast<uint32_t>(OLED_MAX_HZ)) {
        HGM_LOGE("Native HGM  GetScreenCurrentRefreshRate got a rate too above maximal value");
    } else {
        returnInt = static_cast<int32_t>(currentRate);
    }
    return returnInt;
}

int32_t OH_NativeHgm_GetCurrentRefreshRateMode()
{
    int32_t rateMode = 0;
    auto& interfaces = OHOS::Rosen::RSInterfaces::GetInstance();
    rateMode = interfaces.GetCurrentRefreshRateMode();
    return rateMode;
}

int32_t* OH_NativeHgm_GetScreenSupportedRefreshRates(int32_t screen)
{
    ScreenId id = 0;
    if (screen < 0) {
        HGM_LOGE("Native HGM GetScreenCurrentRefreshRate illegal screenId");
    } else {
        id = static_cast<ScreenId>(screen);
    }
    auto& interfaces = OHOS::Rosen::RSInterfaces::GetInstance();
    std::vector<int32_t> currentRates = interfaces.GetScreenSupportedRefreshRates(id);
    int numSupportedRates = static_cast<int>(currentRates.size());
    int32_t *arr = new int32_t[numSupportedRates];
    for (int index = 0; index < numSupportedRates; ++index) {
        arr[index] = currentRates[index];
    }
    return arr;
}