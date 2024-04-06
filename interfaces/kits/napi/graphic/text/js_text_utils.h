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

#ifndef OHOS_JS_TEXT_UTILS_H
#define OHOS_JS_TEXT_UTILS_H

#include <map>
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "utils/log.h"

namespace OHOS::Rosen {
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
constexpr size_t ARGC_FOUR = 4;
constexpr size_t ARGC_FIVE = 5;
constexpr size_t ARGC_SIX = 6;

enum class TextErrorCode : int32_t {
    OK = 0,
    ERROR_NO_PERMISSION = 201, // the value do not change. It is defined on all system
    ERROR_INVALID_PARAM = 401, // the value do not change. It is defined on all system
    ERROR_DEVICE_NOT_SUPPORT = 801, // the value do not change. It is defined on all system
    ERROR_ABNORMAL_PARAM_VALUE = 18600001, // the value do not change. It is defined on color manager system
};

template<class T>
T* CheckParamsAndGetThis(const napi_env env, napi_callback_info info, const char* name = nullptr)
{
    if (env == nullptr || info == nullptr) {
        return nullptr;
    }
    napi_value object = nullptr;
    napi_value propertyNameValue = nullptr;
    napi_value pointerValue = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &object, nullptr);
    if (object != nullptr && name != nullptr) {
        napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &propertyNameValue);
    }
    napi_value& resObject = propertyNameValue ? propertyNameValue : object;
    if (resObject) {
        return napi_unwrap(env, resObject, (void **)(&pointerValue)) == napi_ok ?
            reinterpret_cast<T*>(pointerValue) : nullptr;
    }
    return nullptr;
}

template<typename T, size_t N>
inline constexpr size_t ArraySize(T (&)[N]) noexcept
{
    return N;
}

inline napi_value CreateJsUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

inline napi_value CreateJsNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

inline bool ConvertClampFromJsValue(napi_env env, napi_value jsValue, int32_t& value, int32_t lo, int32_t hi)
{
    if (jsValue == nullptr) {
        return false;
    }
    bool ret = napi_get_value_int32(env, jsValue, &value) == napi_ok;
    value = std::clamp(value, lo, hi);
    return ret;
}

inline napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

inline bool GetUint32FromJS(napi_env env, napi_callback_info info, size_t& object)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok ||
        argc < ARGC_ONE) {
        return false;
    }
    if (napi_get_value_uint32(env, argv[0], &object) != napi_ok) {
        return false;
    }
    return true;
}

inline bool GetDoubleFromJS(napi_env env, napi_callback_info info, double& object)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok ||
        argc < ARGC_ONE) {
        return false;
    }
    if (napi_get_value_double(env, argv[0], &object) != napi_ok) {
        return false;
    }
    return true;
}

void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func);
} // namespace OHOS::Rosen
#endif // OHOS_JS_TEXT_UTILS_H