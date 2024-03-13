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
#include "js_paragraphstyle.h"

#include <string>
#include <vector>

namespace OHOS::Rosen {
thread_local napi_ref JsParagraphStyle::constructor_ = nullptr;
const std::string CLASS_NAME = "ParagraphStyle";
#define LTR 1
#define RTL 0
napi_value JsParagraphStyle::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        LOGE("JsParagraphStyle failed to napi_get_cb_info");
        return nullptr;
    }

    JsParagraphStyle *jsParagraphStyle = new(std::nothrow) JsParagraphStyle();

    status = napi_wrap(env, jsThis, jsParagraphStyle,
        JsParagraphStyle::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsParagraphStyle;
        LOGE("JsParagraphStyle Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

napi_value JsParagraphStyle::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getStrutStyle", JsParagraphStyle::GetStrutStyle),
        DECLARE_NAPI_FUNCTION("setStrutStyle", JsParagraphStyle::SetStrutStyle),
        DECLARE_NAPI_FUNCTION("setDirection", JsParagraphStyle::SetDirection),
        DECLARE_NAPI_FUNCTION("setTextAlign", JsParagraphStyle::SetTextAlign),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid number param");
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid napi create reference");
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid set name");
    }
    return exportObj;
}

void JsParagraphStyle::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsParagraphStyle *napi = reinterpret_cast<JsParagraphStyle *>(nativeObject);
        delete napi;
    }
}

JsParagraphStyle::JsParagraphStyle()
{
    m_paragraphStyle = std::make_shared<TypographyStyle>();
}

std::shared_ptr<TypographyStyle> JsParagraphStyle::GetParagraphStyle()
{
    return m_paragraphStyle;
}

napi_value JsParagraphStyle::GetStrutStyle(napi_env env, napi_callback_info info)
{
    JsParagraphStyle* me = CheckParamsAndGetThis<JsParagraphStyle>(env, info);
    return (me != nullptr) ? me->OnGetStrutStyle(env, info) : nullptr;
}

napi_value JsParagraphStyle::OnGetStrutStyle(napi_env env, napi_callback_info info)
{
    if (m_paragraphStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (m_paragraphStyle->lineStyleFontFamilies.size() && objValue != nullptr) {
        napi_set_named_property(env, objValue, "fontFamilies", CreateJsArrayString(env,
            m_paragraphStyle->lineStyleFontFamilies));
    }
    return objValue;
}

napi_value JsParagraphStyle::SetStrutStyle(napi_env env, napi_callback_info info)
{
    JsParagraphStyle* me = CheckParamsAndGetThis<JsParagraphStyle>(env, info);
    return (me != nullptr) ? me->OnSetStrutStyle(env, info) : nullptr;
}

void JsParagraphStyle::SetStrutStyleFontFamilies(napi_env env, napi_value fontFamiliesField)
{
    uint32_t arrayLength = 0;
    napi_status status = napi_get_array_length(env, fontFamiliesField, &arrayLength);
    if (status != napi_ok) {
        LOGE("SetStrutStyleFontFamilies Invalid array napi_value");
        return;
    }

    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value element;
        status = napi_get_element(env, fontFamiliesField, i, &element);
        if (status != napi_ok) {
            LOGE("SetStrutStyleFontFamilies napi_get_element faild");
            return;
        }

        size_t bufferSize = 0;
        status = napi_get_value_string_utf8(env, element, nullptr, 0, &bufferSize);
        if (status != napi_ok) {
            LOGE("JsParagraphStyle SetStrutStyleFontFamilies Invalid bufferSize");
            return;
        }

        size_t strLen = 0;
        auto buffer = std::make_unique<char[]>(bufferSize + 1);
        if (buffer.get() == nullptr) {
            LOGE("JsParagraphStyle Invalid buffer");
            return;
        }
        status = napi_get_value_string_utf8(env, element, buffer.get(), bufferSize + 1, &strLen);
        if (status != napi_ok) {
            LOGE("JsParagraphStyle SetStrutStyleFontFamilies Invalid buffer");
            return;
        }
        std::string value(buffer.get());
        m_paragraphStyle->lineStyleFontFamilies.push_back(value);
    }
}

napi_value JsParagraphStyle::OnSetStrutStyle(napi_env env, napi_callback_info info)
{
    if (m_paragraphStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetStrutStyle m_paragraphStyle is nullptr");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetStrutStyle napi_get_cb_info faild");
    }

    napi_value fontFamiliesField = nullptr;
    status = napi_get_named_property(env, argv[0], "fontFamilies", &fontFamiliesField);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetStrutStyle napi_get_named_property faild");
    }
    SetStrutStyleFontFamilies(env, fontFamiliesField);
    return NapiGetUndefined(env);
}

napi_value JsParagraphStyle::SetDirection(napi_env env, napi_callback_info info)
{
    JsParagraphStyle* me = CheckParamsAndGetThis<JsParagraphStyle>(env, info);
    return (me != nullptr) ? me->OnSetTextDirection(env, info) : nullptr;
}

napi_value JsParagraphStyle::OnSetTextDirection(napi_env env, napi_callback_info info)
{
    if (m_paragraphStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextDirection m_paragraphStyle is nullptr");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextDirection napi_get_cb_info faild");
    }
    uint32_t textDir = 0;
    status = napi_get_value_uint32(env, argv[0], &textDir);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextDirection napi_get_value_uint32 faild");
    }

    if (textDir == LTR || textDir == RTL) {
        m_paragraphStyle->textDirection = static_cast<TextDirection>(textDir);
    } else {
        LOGE("JsParagraphStyle::OnSetTextDirection textDir is parameter of anomaly");
    }
    return NapiGetUndefined(env);
}

napi_value JsParagraphStyle::SetTextAlign(napi_env env, napi_callback_info info)
{
    JsParagraphStyle* me = CheckParamsAndGetThis<JsParagraphStyle>(env, info);
    return (me != nullptr) ? me->OnSetTextAlign(env, info) : nullptr;
}

napi_value JsParagraphStyle::OnSetTextAlign(napi_env env, napi_callback_info info)
{
    if (m_paragraphStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextAlign Invalid m_paragraphStyle");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextAlign Invalid napi_get_cb_info");
    }
    uint32_t textAlign = 0;
    status = napi_get_value_uint32(env, argv[0], &textAlign);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextAlign Invalid napi_get_value_int32");
    }
    if (textAlign < 0 ||
        textAlign > static_cast<int32_t>(TextAlign::END)) {
        textAlign = 0;
    }
    m_paragraphStyle->textAlign = static_cast<TextAlign>(textAlign);
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen