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
#include "utils/log.h"
#include "js_text_utils.h"

namespace OHOS::Rosen {
#define ERROR_DATA -1
void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func)
{
    std::string fullName;
    if (moduleName) {
        fullName = moduleName;
        fullName += '.';
    }
    fullName += name;
    napi_value funcValue = nullptr;
    napi_create_function(env, fullName.c_str(), fullName.size(), func, nullptr, &funcValue);
    napi_set_named_property(env, object, fullName.c_str(), funcValue);
}

napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, errCode), CreateJsValue(env, message), &result);
    return result;
}

napi_value NapiThrowError(napi_env env, TextErrorCode err, const std::string& message)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(err), message));
    return NapiGetUndefined(env);
}

int32_t GetInt32ByName(napi_env env, napi_callback_info info, const char* dataName)
{
    LOGE("TextStyleTEST| into GetInt32ByName dataName=%s",dataName);
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return ERROR_DATA;
    }

    napi_value jSValue = nullptr;
    if (napi_get_named_property(env, argv[0], dataName, &jSValue) != napi_ok) {
        return ERROR_DATA;
    }

    size_t target = 0;
    if (napi_get_value_uint32(env, argv[0], &target) != napi_ok) {
        return ERROR_DATA;
    }
    LOGE("TextStyleTEST| iover GetInt32ByName dataName=%s target=%d",dataName,target);
    return target;
}
} // namespace OHOS::Rosen
