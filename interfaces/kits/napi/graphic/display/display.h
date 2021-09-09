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

#ifndef INTERFACES_KITS_NAPI_GRAPHIC_DISPLAY_DISPLAY_H
#define INTERFACES_KITS_NAPI_GRAPHIC_DISPLAY_DISPLAY_H

#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "window_manager.h"
#include "window_manager_service_client.h"

struct AsyncDisplayInfosCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback = 0;
    std::vector<OHOS::WMDisplayInfo> displayInfos;
    bool ret = false;
};

napi_value DisplayInit(napi_env env, napi_value exports);
napi_value NAPI_GetDefaultDisplay(napi_env env, napi_callback_info info);

napi_value GetCallbackErrorValue(napi_env env, int errCode);
void ConvertDisplayInfo(napi_env env, napi_value objDisplayInfo, const OHOS::WMDisplayInfo &displayInfo);
bool GetDisplayInfos(napi_env env, std::vector<OHOS::WMDisplayInfo> &displayInfos);
void ProcessDisplayInfos(napi_env env, napi_value result, const std::vector<OHOS::WMDisplayInfo> &displayInfos);

#endif // INTERFACES_KITS_NAPI_GRAPHIC_DISPLAY_DISPLAY_H
