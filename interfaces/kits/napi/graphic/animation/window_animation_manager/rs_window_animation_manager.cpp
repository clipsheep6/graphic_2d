/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_window_animation_manager.h"

#include <memory>

#include <js_runtime_utils.h>
#include <rs_window_animation_log.h>
#include <singleton_container.h>
#include <window_adapter.h>

#include "rs_window_animation_utils.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_MAX = 10;
constexpr int32_t ERR_NOT_OK = -1;
constexpr int32_t ERR_OK = 0;
constexpr int32_t ERR_NOT_SYSTEM_APP = 222;

napi_value RSWindowAnimationManager::Init(napi_env env, napi_value exportObj)
{
    WALOGD("Init");
    if (env == nullptr || exportObj == nullptr) {
        WALOGE("Env or exportObj is null!");
        return nullptr;
    }
    napi_value object = nullptr;
    napi_coerce_to_object(env, exportObj, &object);
    if (object == nullptr) {
        WALOGE("Object is null");
        return nullptr;
    }
    auto windowAnimationManager = std::make_unique<RSWindowAnimationManager>();
    napi_wrap(env, object, windowAnimationManager.release(), RSWindowAnimationManager::Finalizer, nullptr, nullptr);
    const char *moduleName = "RSWindowAnimationManager";
    BindNativeFunction(env, exportObj, "setController", moduleName, RSWindowAnimationManager::SetController);
    BindNativeFunction(env, exportObj, "minimizeWindowWithAnimation", moduleName,
        RSWindowAnimationManager::MinimizeWindowWithAnimation);
    BindNativeFunction(env, exportObj, "getWindowAnimationTargets", moduleName,
        RSWindowAnimationManager::GetWindowAnimationTargets);
    return nullptr;
}

void RSWindowAnimationManager::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<RSWindowAnimationManager>(static_cast<RSWindowAnimationManager*>(data));
}

napi_value RSWindowAnimationManager::SetController(napi_env env, napi_callback_info info)
{
    WALOGD("SetController");
    if (!RSWindowAnimationUtils::IsSystemApp()) {
        WALOGE("SetController failed");
         napi_throw(env,CreateJsError(env, ERR_NOT_SYSTEM_APP,
            "WindowAnimationManager setController failed, is not system app"));
        return nullptr;
    }
    auto me = CheckParamsAndGetThis<RSWindowAnimationManager>(env, info);
    return (me != nullptr) ? me->OnSetController(env, info) : nullptr;
}

napi_value RSWindowAnimationManager::MinimizeWindowWithAnimation(napi_env env, napi_callback_info info)
{
    WALOGD("MinimizeWindowWithAnimation");
    if (!RSWindowAnimationUtils::IsSystemApp()) {
        WALOGE("MinimizeWindowWithAnimation failed");
        napi_throw(env,CreateJsError(env, ERR_NOT_SYSTEM_APP,
            "WindowAnimationManager minimizeWindowWithAnimation failed, is not system app"));
        return nullptr;
    }

    auto me = CheckParamsAndGetThis<RSWindowAnimationManager>(env, info);
    return (me != nullptr) ? me->OnMinimizeWindowWithAnimation(env, info) : nullptr;
}

napi_value RSWindowAnimationManager::GetWindowAnimationTargets(napi_env env, napi_callback_info info)
{
    WALOGD("GetWindowAnimationTargets");
    if (!RSWindowAnimationUtils::IsSystemApp()) {
        WALOGE("GetWindowAnimationTargets failed");
        napi_throw(env,CreateJsError(env, ERR_NOT_SYSTEM_APP,
            "WindowAnimationManager getWindowAnimationTargets failed, is not system app"));
        return nullptr;
    }
    auto me = CheckParamsAndGetThis<RSWindowAnimationManager>(env, info);
    return (me != nullptr) ? me->OnGetWindowAnimationTargets(env, info) : nullptr;
}

napi_value RSWindowAnimationManager::OnSetController(napi_env env, napi_callback_info info)
{
    WALOGD("OnSetController");
    // only support one param
    size_t argc = ARGC_MAX;
    napi_value argv[ARGC_MAX] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc != ARGC_ONE) {
        WALOGE("No enough params!");
        return nullptr;
    }
    sptr<RSWindowAnimationController> controller = new RSWindowAnimationController(env);
    controller->SetJsController(argv[0]);
    SingletonContainer::Get<WindowAdapter>().SetWindowAnimationController(controller);
    return nullptr;
}

napi_value RSWindowAnimationManager::OnMinimizeWindowWithAnimation(napi_env env, napi_callback_info info)
{
    int32_t errCode = ERR_OK;
    size_t argc = ARGC_MAX;
    napi_value argv[ARGC_MAX] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc < ARGC_ONE || argc > ARGC_TWO) {
        WALOGE("No enough params!");
        errCode = ERR_NOT_OK;
    }

    napi_value targetObj = nullptr;
    napi_coerce_to_object(env, argv[0], &targetObj);
    void* target = nullptr;

    if (targetObj == nullptr) {
        WALOGE("Window animation target object is null!");
        errCode = ERR_NOT_OK;
    } else {
        napi_unwrap(env, targetObj, &target);
    }
    if (target == nullptr) {
        WALOGE("Window animation target is null!");
        errCode = ERR_NOT_OK;
    }
    uint32_t windowId = 0;
    napi_value result = nullptr;
    napi_get_named_property(env, targetObj,"windowId", &result);
    if (errCode == ERR_OK && !ConvertFromJsValue(env, result, windowId)) {
        errCode = ERR_NOT_OK;
    }
    WALOGD("Window animation target windowId is:%{public}u!", windowId);
    NapiAsyncTask::CompleteCallback complete =
        [windowId, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != ERR_OK) {
                task.Reject(env, CreateJsError(env, errCode, "Invalid params."));
                return;
            }
            std::vector<uint32_t> windowIds = {windowId};
            sptr<RSIWindowAnimationFinishedCallback> finishedCallback;
            SingletonContainer::Get<WindowAdapter>().MinimizeWindowsByLauncher(windowIds, true, finishedCallback);
            if (finishedCallback == nullptr) {
                WALOGE("Window animation finished callback is null!");
                task.Reject(env, CreateJsError(env, errCode, "Animation finished callback is null!"));
                return;
            }

            WALOGD("Resolve minimize window with animation!");
            task.Resolve(env,
                RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(env, finishedCallback));
        };

    napi_value lastParam =  nullptr;
    if (argc > 1) {
        napi_valuetype result1;;
        napi_typeof(env, argv[1], &result1);
        if (result1 == (napi_valuetype)(NATIVE_FUNCTION)) {
            lastParam = argv[1];
        }
    }
    NapiAsyncTask::Schedule("RSWindowAnimationManager::OnMinimizeWindowWithAnimation",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value RSWindowAnimationManager::OnGetWindowAnimationTargets(napi_env env, napi_callback_info info)
{
    WALOGE("tanyuhang RSWindowAnimationManager::OnGetWindowAnimationTargets");
    int32_t errCode = ERR_OK;
    size_t argc = ARGC_MAX;
    napi_value argv[ARGC_MAX] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc < ARGC_ONE || argc > ARGC_TWO) {
        WALOGE("No enough params!");
        errCode = ERR_NOT_OK;
    }

    std::vector<uint32_t> missionIds;
    napi_value arr = nullptr;
    napi_coerce_to_object(env, argv[0], &arr);
    if (arr == nullptr) {
        errCode = ERR_NOT_OK;
    } else {
        uint32_t len;
        napi_get_array_length(env, arr, &len);
        for (uint32_t index = 0; index < len; index++) {
            napi_value result;
            napi_get_element(env,arr,index,&result);
            napi_value value = nullptr;
            napi_coerce_to_object(env, result, &value);
            if (value == nullptr) {
                errCode = ERR_NOT_OK;
                WALOGE("element is not number!");
                break;
            }
            uint32_t missionId = reinterpret_cast<uint32_t>(value);
            missionIds.push_back(missionId);
        }
    }
    NapiAsyncTask::CompleteCallback complete =
        [missionIds, errCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            WALOGE("tanyuhang RSWindowAnimationManager::OnGetWindowAnimationTargets");
            if (errCode != ERR_OK) {
                task.Reject(env, CreateJsError(env, errCode, "Invalid params."));
                return;
            }
            std::vector<sptr<RSWindowAnimationTarget>> targets;
            if (!missionIds.empty()) {
                SingletonContainer::Get<WindowAdapter>().GetWindowAnimationTargets(missionIds, targets);
            }

            WALOGD("Resolve get window animation targets!");
            task.Resolve(env,
                RSWindowAnimationUtils::CreateJsWindowAnimationTargetArray(env, targets));
        };

    napi_value lastParam = nullptr;
    if(argc>1){
        napi_valuetype result1;;
        napi_typeof(env, argv[1], &result1);
        if(result1 == (napi_valuetype)(NATIVE_FUNCTION)){
            lastParam = argv[1];
        }
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("RSWindowAnimationManager::OnGetWindowAnimationTargets",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}
} // namespace Rosen
} // namespace OHOS
