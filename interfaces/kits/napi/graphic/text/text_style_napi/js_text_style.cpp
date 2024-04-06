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

#include "js_text_style.h"
#include "utils/log.h"

#include <codecvt>
#include <string>
#include <locale>

namespace OHOS::Rosen {
thread_local napi_ref JsTextStyle::constructor_ = nullptr;
const std::string CLASS_NAME = "TextStyle";
napi_value JsTextStyle::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    if (napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr) != napi_ok) {
        LOGE("napi_get_cb_info failed");
        return nullptr;
    }

    JsTextStyle* jsTextStyle = new(std::nothrow) JsTextStyle();
    if (napi_wrap(env, jsThis, jsTextStyle, JsTextStyle::Destructor, nullptr, nullptr) != napi_ok) {
        delete jsTextStyle;
        LOGE("napi_wrap failed");
        return nullptr;
    }
    return jsThis;
}

napi_value JsTextStyle::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_GETTER_SETTER("fontWeight", JsTextStyle::JsGetFontWeight,
            JsTextStyle::JsSetFontWeight),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        LOGE("napi_define_class failed");
        return nullptr;
    }

    if (napi_create_reference(env, constructor, 1, &constructor_) != napi_ok) {
        LOGE("napi_create_reference failed");
        return nullptr;
    }

    if (napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor) != napi_ok) {
        LOGE("napi_set_named_property failed");
        return nullptr;
    }
    return exportObj;
}

void JsTextStyle::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsTextStyle* napi = reinterpret_cast<JsTextStyle*>(nativeObject);
        delete napi;
    }
}

JsTextStyle::JsTextStyle()
{
    m_textStyle = std::make_shared<TextStyle>();
}

napi_value JsTextStyle::JsGetFontWeight(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnGetFontWeight(env, info) : nullptr;
}

napi_value JsTextStyle::JsSetFontWeight(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetFontWeight(env, info) : nullptr;
}

napi_value JsTextStyle::OnGetFontWeight(napi_env env, napi_callback_info info)
{
    if (m_textStyle == nullptr) {
        LOGE("m_textStyle is nullptr");
        return nullptr;
    }
    napi_value fontWeight = nullptr;
    size_t currentFontWeight = static_cast<size_t>(m_textStyle->fontWeight);
    napi_create_uint32(env, currentFontWeight, &fontWeight);
    return fontWeight;
}

napi_value JsTextStyle::OnSetFontWeight(napi_env env, napi_callback_info info)
{
    if (m_textStyle == nullptr) {
        LOGE("m_textStyle is nullptr");
        return nullptr;
    }

    size_t currentFontWeight = 0;
    if (!GetUint32FromJS(env, info, currentFontWeight)) {
        LOGE("GetUint32FromJS falied");
        return nullptr;
    }
    m_textStyle->fontWeight = static_cast<FontWeight>(currentFontWeight);
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
