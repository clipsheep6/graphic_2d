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

constexpr size_t CALLBACK_SIZE = 1;
constexpr size_t ARGS_SIZE_ONE = 1;
constexpr size_t ARGS_SIZE_TWO = 2;
constexpr size_t ARGS_SIZE_THR = 3;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t CODE_SUCCESS = 0;
constexpr int32_t CODE_FAILED = -1;

const std::string WMS_NAPI_EVENT_DISPLAY_ADD = "add";
const std::string WMS_NAPI_EVENT_DISPLAY_REM = "remove";
const std::string WMS_NAPI_EVENT_DISPLAY_CHG = "change";
DisplayCallBack *callback_;
}

napi_value DisplayInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getDefaultDisplay", NAPI_GetDefaultDisplay),
        DECLARE_NAPI_FUNCTION("getAllDisplay", NAPI_GetAllDisplay),
        DECLARE_NAPI_FUNCTION("on", NAPI_RegisterDisplayChange),
        DECLARE_NAPI_FUNCTION("off", NAPI_UnRegisterDisplayChange),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}

/**
 * Promise and async callback
 */
napi_value NAPI_GetDefaultDisplay(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    GNAPI_LOG("%{public}s rgc = [%{public}zu]", __PRETTY_FUNCTION__, argc);
    AsyncDisplayInfoCallbackInfo *asyncDisplayInfoCallbackInfo = new AsyncDisplayInfoCallbackInfo{
        .env = env, .asyncWork = nullptr, .deferred = nullptr};

    if (argc > (ARGS_SIZE_ONE - CALLBACK_SIZE)) {
        GNAPI_LOG("GetDefaultDisplay asyncCallback.");
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, argv[0], &valuetype);
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, argv[0], 1, &asyncDisplayInfoCallbackInfo->callback));

        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetDefaultDisplay", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncDisplayInfoCallbackInfo *asyncDisplayInfoCallbackInfo = (AsyncDisplayInfoCallbackInfo *)data;
                asyncDisplayInfoCallbackInfo->ret = GetDefaultDisplay(asyncDisplayInfoCallbackInfo->displayInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncDisplayInfoCallbackInfo *asyncDisplayInfoCallbackInfo = (AsyncDisplayInfoCallbackInfo *)data;
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                napi_get_undefined(env, &undefined);
                napi_create_object(env, &result[PARAM1]);
                ConvertDisplayInfo(env, result[PARAM1], asyncDisplayInfoCallbackInfo->displayInfo);
                result[PARAM0] = GetCallbackErrorValue(env, asyncDisplayInfoCallbackInfo->ret ? CODE_SUCCESS : CODE_FAILED);
                napi_get_reference_value(env, asyncDisplayInfoCallbackInfo->callback, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, &result[PARAM0], &callResult);

                if (asyncDisplayInfoCallbackInfo->callback != nullptr) {
                    napi_delete_reference(env, asyncDisplayInfoCallbackInfo->callback);
                }
                napi_delete_async_work(env, asyncDisplayInfoCallbackInfo->asyncWork);
                delete asyncDisplayInfoCallbackInfo;
            },
            (void *)asyncDisplayInfoCallbackInfo,
            &asyncDisplayInfoCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncDisplayInfoCallbackInfo->asyncWork));
        napi_value ret = nullptr;
        NAPI_CALL(env, napi_get_null(env, &ret));
        if (ret == nullptr) {
            if (asyncDisplayInfoCallbackInfo != nullptr) {
                delete asyncDisplayInfoCallbackInfo;
                asyncDisplayInfoCallbackInfo = nullptr;
            }
        }
        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));
        return result;
    } else {
        GNAPI_LOG("GetDefaultDisplay promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncDisplayInfoCallbackInfo->deferred = deferred;

        napi_value resourceName;
        napi_create_string_latin1(env, "GetDefaultDisplay", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncDisplayInfoCallbackInfo *asyncDisplayInfoCallbackInfo = (AsyncDisplayInfoCallbackInfo *)data;
                GetDefaultDisplay(asyncDisplayInfoCallbackInfo->displayInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                GNAPI_LOG("=================load=================");
                AsyncDisplayInfoCallbackInfo *asyncDisplayInfoCallbackInfo = (AsyncDisplayInfoCallbackInfo *)data;
                napi_value result;
                napi_create_object(env, &result);
                ConvertDisplayInfo(env, result, asyncDisplayInfoCallbackInfo->displayInfo);
                napi_resolve_deferred(asyncDisplayInfoCallbackInfo->env, asyncDisplayInfoCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncDisplayInfoCallbackInfo->asyncWork);
                delete asyncDisplayInfoCallbackInfo;
            },
            (void *)asyncDisplayInfoCallbackInfo,
            &asyncDisplayInfoCallbackInfo->asyncWork);

        napi_value ret = nullptr;
        NAPI_CALL(env, napi_get_null(env, &ret));
        if (ret == nullptr) {
            if (asyncDisplayInfoCallbackInfo != nullptr) {
                delete asyncDisplayInfoCallbackInfo;
                asyncDisplayInfoCallbackInfo = nullptr;
            }
        }
        return promise;
    }
}

bool GetDefaultDisplay(OHOS::WMDisplayInfo &displayInfo)
{
    displayInfo.id = 0;
    displayInfo.height = 0;
    displayInfo.width = 0;
    displayInfo.phyHeight = 0;
    displayInfo.phyWidth = 0;
    displayInfo.vsync = 0;
    return true;
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
    // Todo set : name: string;

    /**
     * the display is alive
     */
    // Todo set : alive: boolean;

    /**
     * the state of display
     */
    // Todo set : state: DisplayState;

    /**
     * refresh rate, unit: Hz
     */
    // Todo set : refreshRate: number;

    /**
     * the rotation degrees of the display
     */
    // Todo set : rotation: number;

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
    // Todo set : densityDPI: number;

    /**
     * indicates the display density in pixels. The value of a low-resolution display is 1.0
     */
    // Todo set : densityPixels: number;

    /**
     * indicates the text scale density of a display.
     */
    // Todo set : scaledDensity: number;

    /**
     * the DPI on X-axis.
     */
    // Todo set : xDPI: number;

    /**
     * the DPI on Y-axis.
     */
    // Todo set : yDPI: number;

}

napi_value NAPI_GetAllDisplay(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    AsyncDisplayInfosCallbackInfo *asyncDisplayInfosCallbackInfo = new AsyncDisplayInfosCallbackInfo{
        .env = env, .asyncWork = nullptr, .deferred = nullptr};
    if (argc > (ARGS_SIZE_ONE - CALLBACK_SIZE)) {
        GNAPI_LOG("NAPI_GetAllDisplay asyncCallback.");
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetAllDisplay", NAPI_AUTO_LENGTH, &resourceName);
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, argv[0], &valuetype);
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, argv[0], 1, &asyncDisplayInfosCallbackInfo->callback));

        napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncDisplayInfosCallbackInfo *asyncDisplayInfosCallbackInfo = (AsyncDisplayInfosCallbackInfo *)data;
                asyncDisplayInfosCallbackInfo->ret =
                    GetDisplayInfos(env, asyncDisplayInfosCallbackInfo->displayInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncDisplayInfosCallbackInfo *asyncDisplayInfosCallbackInfo = (AsyncDisplayInfosCallbackInfo *)data;
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                napi_get_undefined(env, &undefined);
                napi_create_array(env, &result[PARAM1]);
                ProcessDisplayInfos(env, result[PARAM1], asyncDisplayInfosCallbackInfo->displayInfos);
                result[PARAM0] = GetCallbackErrorValue(env, asyncDisplayInfosCallbackInfo->ret ? CODE_SUCCESS : CODE_FAILED);
                napi_get_reference_value(env, asyncDisplayInfosCallbackInfo->callback, &callback);
                napi_call_function(env, undefined, callback, ARGS_SIZE_TWO, &result[PARAM0], &callResult);

                if (asyncDisplayInfosCallbackInfo->callback != nullptr) {
                    napi_delete_reference(env, asyncDisplayInfosCallbackInfo->callback);
                }
                napi_delete_async_work(env, asyncDisplayInfosCallbackInfo->asyncWork);
                delete asyncDisplayInfosCallbackInfo;
            },
            (void *)asyncDisplayInfosCallbackInfo,
            &asyncDisplayInfosCallbackInfo->asyncWork);
        NAPI_CALL(env, napi_queue_async_work(env, asyncDisplayInfosCallbackInfo->asyncWork));

        napi_value ret = nullptr;
        NAPI_CALL(env, napi_get_null(env, &ret));
        if (ret == nullptr) {
            if (asyncDisplayInfosCallbackInfo != nullptr) {
                delete asyncDisplayInfosCallbackInfo;
                asyncDisplayInfosCallbackInfo = nullptr;
            }
        }
        napi_value result;
        NAPI_CALL(env, napi_create_int32(env, 1, &result));
        return result;
    } else {
        GNAPI_LOG("GetAllDisplay promise.");
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

    int ret = iWindowManagerService->GetDisplays(displayInfos);

    return ret == CODE_SUCCESS ? true : false;
}

void ProcessDisplayInfos(
    napi_env env, napi_value result, const std::vector<OHOS::WMDisplayInfo> &displayInfos)
{
    if (displayInfos.size() > 0) {
        GNAPI_LOG("-----displayInfos is not null-----");
        size_t index = 0;
        for (const auto &item : displayInfos) {
            GNAPI_LOG("id        : %{public}d", item.id);
            GNAPI_LOG("width     : %{public}d", item.width);
            GNAPI_LOG("height    : %{public}d", item.height);
            GNAPI_LOG("phyWidth  : %{public}d", item.phyWidth);
            GNAPI_LOG("phyHeight : %{public}d", item.phyHeight);
            GNAPI_LOG("vsync     : %{public}d", item.vsync);

            napi_value objDisplayInfo = nullptr;
            napi_create_object(env, &objDisplayInfo);
            ConvertDisplayInfo(env, objDisplayInfo, item);
            napi_set_element(env, result, index, objDisplayInfo);
            index++;
        }
    } else {
        GNAPI_LOG("-----displayInfos is null-----");
    }
}

napi_value NAPI_RegisterDisplayChange(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    napi_value _this = nullptr;
    napi_get_undefined(env, &_this);

    size_t argc = ARGS_SIZE_THR;
    napi_value args[ARGS_SIZE_THR] = {0};

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &_this, nullptr));
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    NAPI_ASSERT(env, type == napi_string, "key not string type");

    NAPI_CALL(env, napi_typeof(env, args[1], &type));
    NAPI_ASSERT(env, type == napi_function, "observer not function type");

    NAPI_CALL(env, napi_typeof(env, args[2], &type));
    NAPI_ASSERT(env, type == napi_function, "observer not function type");
    callback_ = new DisplayCallBack(env, args[1], args[2]);

    // save
    const auto &wmsc = WindowManagerServiceClient::GetInstance();
    auto wret = wmsc->Init();
    if (wret != WM_OK) {
        GNAPI_LOG("WindowManagerServiceClient::Init() return %{public}s", WMErrorStr(wret).c_str());
        return _this;
    }

    auto iWindowManagerService = wmsc->GetService();
    if (!iWindowManagerService) {
        GNAPI_LOG("can not get iWindowManagerService");
        return _this;
    }

    iWindowManagerService->AddDisplayChangeListener(callback_);

    GNAPI_LOG("%{public}s end", __PRETTY_FUNCTION__);

    return _this;
}

napi_value NAPI_UnRegisterDisplayChange(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    GNAPI_LOG("%{public}s end", __PRETTY_FUNCTION__);
    return result;
}

DisplayCallBack::DisplayCallBack(napi_env env, napi_value callbackOnScreenPlugin, napi_value callbackOnScreenPlugout)
    : onScreenPlugin(nullptr), onScreenPlugout(nullptr)
{
    this->env_ = env;
    napi_create_reference(env_, callbackOnScreenPlugin, 1, &onScreenPlugin);
    napi_create_reference(env_, callbackOnScreenPlugout, 1, &onScreenPlugout);
}

DisplayCallBack::~DisplayCallBack()
{
    napi_delete_reference(env_, onScreenPlugin);
    napi_delete_reference(env_, onScreenPlugout);
}

void DisplayCallBack::OnScreenPlugin(int32_t did)
{
    GNAPI_LOG("%{public}s called. did:%{public}d", __PRETTY_FUNCTION__, did);
    napi_value callback = nullptr;
    napi_value args[1] = {0};

    napi_create_uint32(env_, did, &args[0]);
    napi_get_reference_value(env_, onScreenPlugin, &callback);

    napi_value global = nullptr;
    napi_get_global(env_, &global);
    napi_call_function(env_, global, callback, 1, args, nullptr);
    GNAPI_LOG("%{public}s end", __PRETTY_FUNCTION__);
}

void DisplayCallBack::OnScreenPlugout(int32_t did)
{
    GNAPI_LOG("%{public}s called. did:%{public}d", __PRETTY_FUNCTION__, did);
    napi_value callback = nullptr;
    napi_value args[1] = {0};

    napi_create_uint32(env_, did, &args[0]);
    napi_get_reference_value(env_, onScreenPlugout, &callback);

    napi_value global = nullptr;
    napi_get_global(env_, &global);
    napi_call_function(env_, global, callback, 1, args, nullptr);
    GNAPI_LOG("%{public}s end", __PRETTY_FUNCTION__);
}
