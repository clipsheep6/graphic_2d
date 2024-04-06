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

#include "js_text_utils.h"

namespace OHOS::Rosen {
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

napi_value NapiThrowError(napi_env env, DrawingErrorCode err, const std::string& message)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(err), message));
    return NapiGetUndefined(env);
}

inline bool GetPlaceholderSpanFromJS(napi_env env, napi_value argValue, PlaceholderSpan& placeholderSpan)
{
    if(argValue == nullptr) {
        return false;
    }
    
    napi_value tempValue = nullptr;

    napi_get_named_property(env, argValue, "width", &tempValue);
    double width = 0;
    if (ConvertFromJsNumber(env, argValue, width)) {
        placeholderSpan.width = width;
    }

    napi_get_named_property(env, argValue, "height", &tempValue);
    double height = 0;
    if (ConvertFromJsNumber(env, argValue, height)) {
        placeholderSpan.height = height;
    }

    napi_get_named_property(env, argValue, "align", &tempValue);
    uint32_t align = 0;
    if (ConvertFromJsNumber(env, tempValue, align)== napi_ok) {
        placeholderSpan.alignment = PlaceholderVerticalAlignment(align);
    }

    napi_get_named_property(env, argValue, "baseline", &tempValue);
    uint32_t baseline = 0;
    if (ConvertFromJsNumber(env, argValue, height)) {
        placeholderSpan.baseline = TextBaseline(baseline);
    }

    napi_get_named_property(env, argValue, "baselineOffset", &tempValue);
    double baselineOffset = 0;
    if (ConvertFromJsNumber(env, argValue, baselineOffset)) {
        placeholderSpan.baselineOffset = baselineOffset;
    }
    return true;
}
} // namespace OHOS::Rosen
