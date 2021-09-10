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
#include <string>
#include <ability.h>
#include <hilog/log.h>

#include "native_display_module.h"
#include "graphic_napi_common.h"

using namespace OHOS;

namespace {
static napi_value DisplayModuleInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    DisplayInit(env, exports);
    return exports;
}

// getDefaultDisplay {{{
namespace getDefaultDisplay {
struct Param {
    std::vector<OHOS::WMDisplayInfo> displayInfos;
};

void Async(napi_env env, std::unique_ptr<Param>& param)
{
    GetDisplayInfos(env, param->displayInfos);
}

napi_value Resolve(napi_env env, std::unique_ptr<Param>& reParam)
{
    napi_value result;
    napi_create_object(env, &result);
    ProcessDisplayInfos(env, result, reParam->displayInfos);
    return result;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    auto param = std::make_unique<Param>();
    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, Resolve, param);
}
} // namespace getDefaultDisplay }}}


napi_value DisplayInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getDefaultDisplay", getDefaultDisplay::MainFunc),
    };
    NAPI_CALL(env, napi_define_properties(
        env, exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
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
    if (displayInfos.empty() || displayInfos.size() <= 0) {
        GNAPI_LOG("-----displayInfos is null-----");
        OHOS::WMDisplayInfo displayInfo;
        displayInfo.id = 0;
        displayInfo.height = 0;
        displayInfo.width = 0;
        displayInfo.phyHeight = 0;
        displayInfo.phyWidth = 0;
        displayInfo.vsync = 0;
        ConvertDisplayInfo(env, result, displayInfo);
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
        ConvertDisplayInfo(env, result, displayInfos[0]);
    }
}

void ConvertDisplayInfo(napi_env env, napi_value result, const OHOS::WMDisplayInfo &displayInfo)
{
    ConvertInfoForInt32(env, result, "id", displayInfo.id);
    ConvertInfoForUndefined(env, result, "name");
    ConvertInfoForUndefined(env, result, "alive");
    ConvertInfoForUndefined(env, result, "state");
    ConvertInfoForUndefined(env, result, "refreshRate");
    ConvertInfoForUndefined(env, result, "rotation");
    ConvertInfoForUint32(env, result, "width", displayInfo.width);
    ConvertInfoForUint32(env, result, "height", displayInfo.height);
    ConvertInfoForUndefined(env, result, "densityDPI");
    ConvertInfoForUndefined(env, result, "densityPixels");
    ConvertInfoForUndefined(env, result, "scaledDensity");
    ConvertInfoForUndefined(env, result, "xDPI");
    ConvertInfoForUndefined(env, result, "yDPI");
}
}

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module module = {
        .nm_version = 1, // NAPI v1
        .nm_flags = 0, // normal
        .nm_filename = nullptr,
        .nm_register_func = DisplayModuleInit,
        .nm_modname = "display",
        .nm_priv = nullptr,
        .reserved = {}
    };
    napi_module_register(&module);
}
