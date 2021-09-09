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

#include "display.h"

#include <string>

#include <ability.h>
#include <hilog/log.h>

using namespace OHOS;

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "DisplayNAPILayer" };
#define GNAPI_LOG(fmt, ...) HiviewDFX::HiLog::Info(LABEL, \
    "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)

constexpr size_t ARGS_SIZE_ONE = 1;
constexpr int32_t CODE_SUCCESS = 0;
}

napi_value DisplayInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getDefaultDisplay", NAPI_GetDefaultDisplay),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}

napi_value GetCallbackErrorValue(napi_env env, int errCode)
{
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &eCode));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", eCode));
    return result;
}

napi_value NAPI_GetDefaultDisplay(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    AsyncDisplayInfosCallbackInfo *asyncDisplayInfosCallbackInfo = new AsyncDisplayInfosCallbackInfo{
        .env = env, .asyncWork = nullptr, .deferred = nullptr};

    GNAPI_LOG("NAPI_GetDefaultDisplay promise.");
    napi_deferred deferred;
    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asyncDisplayInfosCallbackInfo->deferred = deferred;

    napi_value resourceName;
    napi_create_string_latin1(env, "GetDefaultDisplay", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncDisplayInfosCallbackInfo *asyncDisplayInfosCallbackInfo = (AsyncDisplayInfosCallbackInfo *)data;
            GetDisplayInfos(env, asyncDisplayInfosCallbackInfo->displayInfos);
        },
        [](napi_env env, napi_status status, void *data) {
            GNAPI_LOG("=================load=================");
            AsyncDisplayInfosCallbackInfo *asyncDisplayInfosCallbackInfo = (AsyncDisplayInfosCallbackInfo *)data;
            napi_value result;
            napi_create_array(env, &result);
            ProcessDisplayInfos(env, result, asyncDisplayInfosCallbackInfo->displayInfos);
            napi_resolve_deferred(asyncDisplayInfosCallbackInfo->env, asyncDisplayInfosCallbackInfo->deferred, result);
            napi_delete_async_work(env, asyncDisplayInfosCallbackInfo->asyncWork);
            delete asyncDisplayInfosCallbackInfo;
        },
        (void *)asyncDisplayInfosCallbackInfo,
        &asyncDisplayInfosCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncDisplayInfosCallbackInfo->asyncWork);

    napi_value ret = nullptr;
    NAPI_CALL(env, napi_get_null(env, &ret));
    if (ret == nullptr) {
        if (asyncDisplayInfosCallbackInfo != nullptr) {
            delete asyncDisplayInfosCallbackInfo;
            asyncDisplayInfosCallbackInfo = nullptr;
        }
    }
    return promise;
}

bool GetDisplayInfos(
    napi_env env, std::vector<OHOS::WMDisplayInfo> &displayInfos)
{
    const auto &wmsc = WindowManagerServiceClient::GetInstance();
    auto wret = wmsc->Init();
    if (wret != WM_OK) {
        GNAPI_LOG("WindowManagerServiceClient::Init() return %{public}s", WMErrorStr(wret).c_str());
        return false;
    }

    auto iWindowManagerService = wmsc->GetService();
    if (!iWindowManagerService) {
        GNAPI_LOG("can not get iWindowManagerService");
        return false;
    }

    return iWindowManagerService->GetDisplays(displayInfos) == CODE_SUCCESS ? true : false;
}

void ProcessDisplayInfos(
    napi_env env, napi_value result, const std::vector<OHOS::WMDisplayInfo> &displayInfos)
{
    napi_value objDisplayInfo = nullptr;
    napi_create_object(env, &objDisplayInfo);

    if (displayInfos.empty() || displayInfos.size() <= 0) {
        GNAPI_LOG("-----displayInfos is null-----");
        OHOS::WMDisplayInfo displayInfo;
        displayInfo.id = 0;
        displayInfo.height = 0;
        displayInfo.width = 0;
        displayInfo.phyHeight = 0;
        displayInfo.phyWidth = 0;
        displayInfo.vsync = 0;
        ConvertDisplayInfo(env, objDisplayInfo, displayInfo);
    }
    else
    {
        GNAPI_LOG("-----displayInfos is not null-----");
        GNAPI_LOG("id        : %{public}d", displayInfos[0].id);
        GNAPI_LOG("width     : %{public}d", displayInfos[0].width);
        GNAPI_LOG("height    : %{public}d", displayInfos[0].height);
        GNAPI_LOG("phyWidth  : %{public}d", displayInfos[0].phyWidth);
        GNAPI_LOG("phyHeight : %{public}d", displayInfos[0].phyHeight);
        GNAPI_LOG("vsync     : %{public}d", displayInfos[0].vsync);
        ConvertDisplayInfo(env, objDisplayInfo, displayInfos[0]);
    }
    napi_set_element(env, result, 0, objDisplayInfo);
}


void ConvertDisplayInfo(napi_env env, napi_value objDisplayInfo, const OHOS::WMDisplayInfo &displayInfo)
{

    /**
     * display id
     */
    napi_value nId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, displayInfo.id, &nId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "id", nId));

    /**
     * display name
     */
    napi_value name;
    napi_get_undefined(env, &name);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "name", name));

    /**
     * the display is alive
     */
    napi_value alive;
    napi_get_undefined(env, &alive);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "alive", alive));

    /**
     * the state of display
     */
    napi_value state;
    napi_get_undefined(env, &state);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "state", state));

    /**
     * refresh rate, unit: Hz
     */
    napi_value refreshRate;
    napi_get_undefined(env, &refreshRate);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "refreshRate", refreshRate));

    /**
     * the rotation degrees of the display
     */
    napi_value rotation;
    napi_get_undefined(env, &rotation);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "rotation", rotation));

    /**
     * the width of display, unit: pixel
     */
    napi_value width;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, displayInfo.width, &width));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "width", width));
    
    /**
     * the height of display, unit: pixel
     */
    napi_value height;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, displayInfo.height, &height));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "height", height));

    /**
     * indicates the display resolution.
     */
    napi_value densityDPI;
    napi_get_undefined(env, &densityDPI);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "densityDPI", densityDPI));

    /**
     * indicates the display density in pixels. The value of a low-resolution display is 1.0
     */
    napi_value densityPixels;
    napi_get_undefined(env, &densityPixels);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "densityPixels", densityPixels));

    /**
     * indicates the text scale density of a display.
     */
    napi_value scaledDensity;
    napi_get_undefined(env, &scaledDensity);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "scaledDensity", state));

    /**
     * the DPI on X-axis.
     */
    napi_value xDPI;
    napi_get_undefined(env, &xDPI);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "xDPI", xDPI));

    /**
     * the DPI on Y-axis.
     */
    napi_value yDPI;
    napi_get_undefined(env, &yDPI);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objDisplayInfo, "yDPI", yDPI));

}