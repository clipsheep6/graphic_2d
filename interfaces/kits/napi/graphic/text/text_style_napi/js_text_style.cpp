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

namespace OHOS::Rosen {
thread_local napi_ref JsTextStyle::constructor_ = nullptr;
const std::string CLASS_NAME = "TextStyle";
napi_value JsTextStyle::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "TextStyle constructor failed");
    }

    JsTextStyle* jsTextStyle = new(std::nothrow) JsTextStyle();
    status = napi_wrap(env, jsThis, jsTextStyle,
                       JsTextStyle::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "TextStyle to wrap native instance");
    }
    return jsThis;
}

napi_value JsTextStyle::Init(napi_env env, napi_value exportObj)
{
    LOGE("ParamTestfromTextStyle | Init");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setColor", JsTextStyle::SetColor),
        DECLARE_NAPI_FUNCTION("setTextDecoration", JsTextStyle::SetTextDecoration),
        DECLARE_NAPI_FUNCTION("setDecorationColor", JsTextStyle::SetDecorationColor),
        DECLARE_NAPI_FUNCTION("setTextDecorationStyle", JsTextStyle::SetTextDecorationStyle),
        DECLARE_NAPI_FUNCTION("setDecorationThicknessScale", JsTextStyle::SetDecorationThicknessScale),
        DECLARE_NAPI_FUNCTION("setFontWeight", JsTextStyle::SetFontWeight),
        DECLARE_NAPI_FUNCTION("setTextBaseline", JsTextStyle::SetTextBaseline),
        DECLARE_NAPI_FUNCTION("setFontFamilies", JsTextStyle::SetFontFamilies),
        DECLARE_NAPI_FUNCTION("setFontSize", JsTextStyle::SetFontSize),
        DECLARE_NAPI_FUNCTION("setLetterSpacing", JsTextStyle::SetLetterSpacing),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Init Failed");
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Invalid Failed to create reference of constructor");
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to set constructor");
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
    LOGE("BeginJsTextStyle | JsTextStyle()");
    m_textStyle = std::make_shared<TextStyle>();
}

std::shared_ptr<TextStyle> JsTextStyle::GetTextStyle()
{
    return m_textStyle;
}

bool JsTextStyle::GetUint32Data(napi_env env, napi_callback_info info, uint32_t& target)
{
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "JsTextStyle GetUint32Data Invalid params");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return false;
    }
    status = napi_get_value_uint32(env, argv[0], &target);
    if (status != napi_ok) {
        return false;
    }
    return true;
}

bool JsTextStyle::GetDoubleData(napi_env env, napi_callback_info info, double& target)
{
    //LOGE("ParamTestfromTextStyle | into GetDoubleData");
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "JsTextStyle GetDoubleData Invalid params");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return false;
    }
    status = napi_get_value_double(env, argv[0], &target);
    if (status != napi_ok) {
        return false;
    }
    return true;
}

napi_value JsTextStyle::SetColor(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetColor(env, info) : nullptr;
}

napi_value JsTextStyle::SetTextDecoration(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetTextDecoration(env, info) : nullptr;
}

napi_value JsTextStyle::SetDecorationColor(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetDecorationColor(env, info) : nullptr;
}

napi_value JsTextStyle::SetTextDecorationStyle(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetTextDecorationStyle(env, info) : nullptr;  
}

napi_value JsTextStyle::SetDecorationThicknessScale(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetDecorationThicknessScale(env, info) : nullptr;  
}

napi_value JsTextStyle::SetFontWeight(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetFontWeight(env, info) : nullptr;  
}

napi_value JsTextStyle::SetTextBaseline(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetTextBaseline(env, info) : nullptr;  
}

napi_value JsTextStyle::SetFontFamilies(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetFontFamilies(env, info) : nullptr;  
}

napi_value JsTextStyle::SetFontSize(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetFontSize(env, info) : nullptr;  
}

napi_value JsTextStyle::SetLetterSpacing(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetLetterSpacing(env, info) : nullptr;  
}

napi_value JsTextStyle::OnSetLetterSpacing(napi_env env, napi_callback_info info)
{
    double letterSpacing = 0;
    bool result = GetDoubleData(env, info, letterSpacing);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetFontSize");        
    }
    LOGE("ParamTestfromTextStyle | letterSpacing=%f",letterSpacing);
    m_textStyle->letterSpacing = letterSpacing;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetFontSize(napi_env env, napi_callback_info info)
{
    double fontSize = 0;
    bool result = GetDoubleData(env, info, fontSize);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetFontSize");        
    }
    LOGE("ParamTestfromTextStyle | fontSize=%f",fontSize);
    m_textStyle->fontSize = fontSize;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetFontFamilies(napi_env env, napi_callback_info info)
{
    LOGE("ParamTestfromTextStyle | into OnSetFontFamilies");
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetFontFamilies m_textStyle is nullptr");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetFontFamilies napi_get_cb_info faild");
    }
    napi_value fontFamiliesField = argv[0];
    uint32_t arrayLength = 0;
    status = napi_get_array_length(env, fontFamiliesField, &arrayLength);
    if (status != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetFontFamilies Invalid array napi_value");
    }

    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value element;
        status = napi_get_element(env, fontFamiliesField, i, &element);
        if (status != napi_ok) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
                "OnSetFontFamilies napi_get_element Failed");
        }

        size_t bufferSize = 0;
        status = napi_get_value_string_utf8(env, element, nullptr, 0, &bufferSize);
        if (status != napi_ok) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
                "OnSetFontFamilies napi_get_value_string_utf8 Failed");
        }

        size_t strLen = 0;
        auto buffer = std::make_unique<char[]>(bufferSize + 1);
        if (buffer.get() == nullptr) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
                "OnSetFontFamilies Invalid buffer");          
        }
        status = napi_get_value_string_utf8(env, element, buffer.get(), bufferSize + 1, &strLen);
        if (status != napi_ok) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
                "OnSetFontFamilies napi_get_value_string_utf8 Failed");    
        }
        std::string value(buffer.get());
        m_textStyle->fontFamilies.push_back(value);
    }
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetTextBaseline(napi_env env, napi_callback_info info)
{
    uint32_t textBaseline = 0;
    bool result = GetUint32Data(env, info, textBaseline);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed TextBaseline");        
    }
    LOGE("ParamTestfromTextStyle | TextBaseline=%zu",textBaseline);
    m_textStyle->baseline = static_cast<TextBaseline>(textBaseline);
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetFontWeight(napi_env env, napi_callback_info info)
{
    uint32_t fontWeight = 0;
    bool result = GetUint32Data(env, info, fontWeight);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetFontWeight");        
    }
    LOGE("ParamTestfromTextStyle | fontWeight=%zu",fontWeight);
    m_textStyle->fontWeight = static_cast<FontWeight>(fontWeight);
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetDecorationThicknessScale(napi_env env, napi_callback_info info)
{
    double textThickness = 0;
    bool result = GetDoubleData(env, info, textThickness);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetDecorationThicknessScale");        
    }
    LOGE("ParamTestfromTextStyle | textThickness=%f",textThickness);
    m_textStyle->decorationThicknessScale = textThickness;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetTextDecorationStyle(napi_env env, napi_callback_info info)
{
    uint32_t textDecorationStyle = 0;
    bool result = GetUint32Data(env, info, textDecorationStyle);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed setTextDecorationStyle");        
    }
    LOGE("ParamTestfromTextStyle | textDecorationStyle=%zu",textDecorationStyle);
    m_textStyle->decorationStyle = static_cast<TextDecorationStyle>(textDecorationStyle);;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetDecorationColor(napi_env env, napi_callback_info info)
{
    uint32_t textDecorationColor = 0;
    bool result = GetUint32Data(env, info, textDecorationColor);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed textDecorationColor");        
    }
    LOGE("ParamTestfromTextStyle | textDecorationColor=%zu",textDecorationColor);
    m_textStyle->decorationColor = textDecorationColor;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetTextDecoration(napi_env env, napi_callback_info info)
{
    uint32_t textDecoration = 0;
    bool result = GetUint32Data(env, info, textDecoration);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed SetTextDecoration");        
    }
    LOGE("ParamTestfromTextStyle | textDecoration=%zu",textDecoration);
    m_textStyle->decoration = static_cast<TextDecoration>(textDecoration);
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetColor(napi_env env, napi_callback_info info)
{
    LOGE("BeginJsTextStyle | OnSetColor");
    uint32_t colorValue = 0;
    bool result = GetUint32Data(env, info, colorValue);
    LOGE("BeginJsTextStyle | Valut=%zu",colorValue);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed SetColor");        
    }
    m_textStyle->color = colorValue;
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
