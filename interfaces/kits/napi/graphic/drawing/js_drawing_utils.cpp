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

#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
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

bool ConvertFromJsTextEncoding(napi_env env, TextEncoding& textEncoding, napi_value nativeType)
{
    napi_value type = nativeType;
    if (type == nullptr) {
        return false;
    }
    uint32_t resultValue = 0;
    napi_get_value_uint32(env, type, &resultValue);

    switch (resultValue) {
        case 0: // 0: TextEncoding::UTF8
            textEncoding = TextEncoding::UTF8;
            break;
        case 1: // 1: TextEncoding::UTF16
            textEncoding = TextEncoding::UTF16;
            break;
        case 2: // 2: TextEncoding::UTF32
            textEncoding = TextEncoding::UTF32;
            break;
        case 3: // 3: TextEncoding::GLYPH_ID
            textEncoding = TextEncoding::GLYPH_ID;
            break;
        default: // default: TextEncoding::UTF8
            textEncoding = TextEncoding::UTF8;
            break;
    }
    return true;
}

bool ConvertFromJsRect(napi_env env, napi_value src, Rect& drawingRect)
{
    if (src == nullptr) {
        return false;
    }
    double left = 0.0;
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;
    napi_value tempValue = nullptr;
    napi_get_named_property(env, src, "left", &tempValue);
    bool isLeftOk = ConvertFromJsValue(env, tempValue, left);
    napi_get_named_property(env, src, "right", &tempValue);
    bool isRightOk = ConvertFromJsValue(env, tempValue, right);
    napi_get_named_property(env, src, "top", &tempValue);
    bool isTopOk = ConvertFromJsValue(env, tempValue, top);
    napi_get_named_property(env, src, "bottom", &tempValue);
    bool isBottomOk = ConvertFromJsValue(env, tempValue, bottom);
    if (!(isLeftOk && isRightOk && isTopOk && isBottomOk)) {
        return false;
    }
    drawingRect = Drawing::Rect(left, top, right, bottom);
    return true;
}

bool ConvertFromJsFontEdging(napi_env env, FontEdging& fontEdging, napi_value nativeType)
{
    napi_value type = nativeType;
    if (type == nullptr) {
        return false;
    }
    uint32_t resultValue = 0;
    napi_get_value_uint32(env, type, &resultValue);

    switch (resultValue) {
        case 0:
            fontEdging = FontEdging::ALIAS;
            break;
        case 1:
            fontEdging = FontEdging::ANTI_ALIAS;
            break;
        case 2:
            fontEdging = FontEdging::SUBPIXEL_ANTI_ALIAS;
            break;
        default:
            fontEdging = FontEdging::ALIAS;
            break;
    }
    return true;
}

bool ConvertFromJsFontHinting(napi_env env, FontHinting& fontHinting, napi_value nativeType)
{
    napi_value type = nativeType;
    if (type == nullptr) {
        return false;
    }
    uint32_t resultValue = 0;
    napi_get_value_uint32(env, type, &resultValue);

    switch (resultValue) {
        case 0: 
            fontHinting = FontHinting::NONE;
            break;
        case 1:
            fontHinting = FontHinting::SLIGHT;
            break;
        case 2:
            fontHinting = FontHinting::NORMAL;
            break;
        case 3:
            fontHinting = FontHinting::FULL;
            break;
        default:
            fontHinting = FontHinting::NORMAL;
            break;
    }
    return true;
}

napi_value NapiThrowError(napi_env env, DrawingErrorCode err, const std::string& message)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(err), message));
    return NapiGetUndefined(env);
}
} // namespace Drawing
} // namespace OHOS::Rosen
