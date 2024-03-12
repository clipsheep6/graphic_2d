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
        DECLARE_NAPI_FUNCTION("setWordSpacing", JsTextStyle::SetWordSpacing),
        DECLARE_NAPI_FUNCTION("setHeightScale", JsTextStyle::SetHeightScale),
        DECLARE_NAPI_FUNCTION("setHalfLeading", JsTextStyle::SetHalfLeading),
        DECLARE_NAPI_FUNCTION("setHeightOnly", JsTextStyle::SetHeightOnly),
        DECLARE_NAPI_FUNCTION("setEllipsis", JsTextStyle::SetEllipsis),
        DECLARE_NAPI_FUNCTION("setEllipsisModal", JsTextStyle::SetEllipsisModal),
        DECLARE_NAPI_FUNCTION("setLocale", JsTextStyle::SetLocale),
        DECLARE_NAPI_FUNCTION("setForegroundBrush", JsTextStyle::SetForegroundBrush),
        DECLARE_NAPI_FUNCTION("setBackgroundBrush", JsTextStyle::SetBackgroundBrush),
        DECLARE_NAPI_FUNCTION("setForegroundPen", JsTextStyle::SetForegroundPen),
        DECLARE_NAPI_FUNCTION("setBackgroundPen", JsTextStyle::SetBackgroundPen),
        DECLARE_NAPI_FUNCTION("setTextShadow", JsTextStyle::SetTextShadow),
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

bool JsTextStyle::GetBoolData(napi_env env, napi_callback_info info, bool& target)
{
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "JsTextStyle GetBoolData Invalid params");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return false;
    }
    status = napi_get_value_bool(env, argv[0], &target);
    if (status != napi_ok) {
        return false;
    }
    return true;
}

bool JsTextStyle::GetUint32DataFromName(napi_env env, napi_value element, const char* name, uint32_t& target)
{
    napi_value param = nullptr;
    if (napi_get_named_property(env, element, name, &param) != napi_ok) {
        return false;
    }
    if (napi_get_value_uint32(env, param, &target) != napi_ok) {
        return false;
    }
    return true;
}

bool JsTextStyle::GetDoubleDataFromName(napi_env env, napi_value element, const char* name, double& target)
{
    napi_value param = nullptr;
    if (napi_get_named_property(env, element, name, &param) != napi_ok) {
        return false;
    }
    if (napi_get_value_double(env, param, &target) != napi_ok) {
        return false;
    }
    return true;
}

bool JsTextStyle::GetBoolDataFromName(napi_env env, napi_value element, const char* name, bool& target)
{
    napi_value param = nullptr;
    if (napi_get_named_property(env, element, name, &param) != napi_ok) {
        return false;
    }
    if (napi_get_value_bool(env, param, &target) != napi_ok) {
        return false;
    }
    return true;
}

void JsTextStyle::ScanNapiValue(napi_env env, napi_value argv, uint32_t content)
{
    for (uint32_t i = 0; i < content; i++) {
        napi_value element;
        uint32_t colorNumber = 0;
        double offsetX = 0;
        double offsetY = 0;
        double radius = false;
        if (napi_get_element(env, argv, i, &element) != napi_ok) {
            LOGE("OnSetFontFamilies napi_get_element Failed");
            return;
        }
        if (!GetUint32DataFromName(env, element, "color", colorNumber)) {
            LOGE("OnSetStrutStyle GetUint32DataFromName faild");
            return;
        }
        LOGE("ParamTestfromTextStyle Color =%zu",colorNumber);
        if (!GetDoubleDataFromName(env, element, "offsetPointX", offsetX)) {
            LOGE("OnSetStrutStyle GetUint32DataFromName faild");
            return;
        }
        LOGE("ParamTestfromTextStyle offsetPointX =%f", offsetX);
        if (!GetDoubleDataFromName(env, element, "offsetPointY", offsetY)) {
            LOGE("OnSetStrutStyle GetUint32DataFromName faild");
            return;
        }
        LOGE("ParamTestfromTextStyle offsetPointY =%f", offsetY);
        if (!GetDoubleDataFromName(env, element, "blurRadius", radius)) {
            LOGE("OnSetStrutStyle GetUint32DataFromName faild");
            return;
        }
        LOGE("ParamTestfromTextStyle GetBoolDataFromName =%f",radius);
        Drawing::Color color(colorNumber);
        Drawing::Point offset(offsetX, offsetY);
        TextShadow tempTextShadow(color, offset, radius);
        LOGE("ParamTestfromTextStyle m_textStyle->shadows.size=%zu",m_textStyle->shadows.size());
        m_textStyle->shadows.push_back(tempTextShadow);
    }
    return;
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

napi_value JsTextStyle::SetHeightScale(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetLetterSpacing(env, info) : nullptr;
}

napi_value JsTextStyle::SetHalfLeading(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetHalfLeading(env, info) : nullptr;
}

napi_value JsTextStyle::SetHeightOnly(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetHeightOnly(env, info) : nullptr;
}

napi_value JsTextStyle::SetEllipsis(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetEllipsis(env, info) : nullptr;
}

napi_value JsTextStyle::SetEllipsisModal(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetEllipsisModal(env, info) : nullptr;
}

napi_value JsTextStyle::SetLocale(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetLocale(env, info) : nullptr;
}

napi_value JsTextStyle::SetForegroundBrush(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetForegroundBrush(env, info) : nullptr;
}

napi_value JsTextStyle::SetBackgroundBrush(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetBackgroundBrush(env, info) : nullptr;
}

napi_value JsTextStyle::SetForegroundPen(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetForegroundPen(env, info) : nullptr;
}

napi_value JsTextStyle::SetBackgroundPen(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetBackgroundPen(env, info) : nullptr;
}

napi_value JsTextStyle::SetTextShadow(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetTextShadow(env, info) : nullptr;
}

napi_value JsTextStyle::OnSetTextShadow(napi_env env, napi_callback_info info)
{
    LOGE("ParamTestfromTextStyle | into OnSetTextShadow");
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextShadow Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        LOGE("ParamTestfromTextStyle Out1");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextShadow napi_get_cb_info faild");
    }

    uint32_t arrayLength = 0;
    status = napi_get_array_length(env, argv[0], &arrayLength);
    if (status != napi_ok) {
        LOGE("ParamTestfromTextStyle Out2 arrayLength=%zu",arrayLength);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetTextShadow Invalid array");
    }
    ScanNapiValue(env, argv[0], arrayLength);
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetBackgroundPen(napi_env env, napi_callback_info info)
{
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetBackgroundPen Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetBackgroundPen napi_get_cb_info Faild");
    }

    Drawing::JsPen* jsPen = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsPen));
    if (jsPen == nullptr) {
        LOGE("JsTextStyle::OnSetBackgroundPen jsBrush is nullptr");
        return NapiGetUndefined(env);
    }
    LOGE("ParamTestfromTextStyle | BackgroundPen Address = %p",jsPen->GetPen());
    m_textStyle->foregroundPen = *jsPen->GetPen();
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetForegroundPen(napi_env env, napi_callback_info info)
{
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetForegroundPen Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetForegroundPen napi_get_cb_info Faild");
    }

    Drawing::JsPen* jsPen = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsPen));
    if (jsPen == nullptr) {
        LOGE("JsTextStyle::OnSetForegroundPen jsBrush is nullptr");
        return NapiGetUndefined(env);
    }
    LOGE("ParamTestfromTextStyle | ForegroundPen Address = %p",jsPen->GetPen());
    m_textStyle->foregroundPen = *jsPen->GetPen();
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetBackgroundBrush(napi_env env, napi_callback_info info)
{
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetBackgroundBrush Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetBackgroundBrush napi_get_cb_info Faild");
    }

    Drawing::JsBrush* jsBrush = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsBrush));
    if (jsBrush == nullptr) {
        LOGE("JsTextStyle::OnSetBackgroundBrush jsBrush is nullptr");
        return NapiGetUndefined(env);
    }
    LOGE("ParamTestfromTextStyle | BackgroundBrush Address = %p",jsBrush->GetBrush());
    m_textStyle->backgroundBrush = *jsBrush->GetBrush();
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetForegroundBrush(napi_env env, napi_callback_info info)
{
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetForegroundBrush Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetForegroundBrush napi_get_cb_info Faild");
    }

    Drawing::JsBrush* jsBrush = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsBrush));
    if (jsBrush == nullptr) {
        LOGE("JsTextStyle::jsBrush is nullptr");
        return NapiGetUndefined(env);
    }
    LOGE("ParamTestfromTextStyle | ForegroundBrush Address = %p",jsBrush->GetBrush());
    m_textStyle->foregroundBrush = *jsBrush->GetBrush();
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetLocale(napi_env env, napi_callback_info info)
{
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetLocale m_textStyle is nullptr");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetEllipsis napi_get_cb_info Faild");
    }
    size_t length = 0;
    if (napi_get_value_string_utf8(env, argv[0], NULL, 0, &length) != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetEllipsis napi_get_value_string_utf8 Faild");
    }
    auto buffer = std::make_unique<char[]>(length + 1);
    size_t strLength = 0;
    if (napi_get_value_string_utf8(env, argv[0], buffer.get(), length + 1, &strLength) != napi_ok) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetEllipsis buffer Faild");
    }
    m_textStyle->locale.assign(buffer.get());
    LOGE("ParamTestfromTextStyle | locale= %s",m_textStyle->locale.c_str());
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetEllipsisModal(napi_env env, napi_callback_info info)
{
    uint32_t ellipsisMode = 0;
    bool result = GetUint32Data(env, info, ellipsisMode);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetEllipsisModal");
    }
    LOGE("ParamTestfromTextStyle | ellipsisMode=%zu",ellipsisMode);
    m_textStyle->ellipsisModal = static_cast<EllipsisModal>(ellipsisMode);
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetEllipsis(napi_env env, napi_callback_info info)
{
    if (m_textStyle == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "OnSetEllipsis m_textStyle is nullptr");
    }

    m_textStyle->ellipsis = u"\u2026";
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    std::string utf8Str = converter.to_bytes(m_textStyle->ellipsis);
    LOGE("ParamTestfromTextStyle | ellispsis = %s",utf8Str.c_str());
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetHeightOnly(napi_env env, napi_callback_info info)
{
    bool heightOnly = 0;
    bool result = GetBoolData(env, info, heightOnly);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetHeightOnly");        
    }
    LOGE("ParamTestfromTextStyle | heightOnly=%d",heightOnly);
    m_textStyle->heightOnly = heightOnly;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetHalfLeading(napi_env env, napi_callback_info info)
{
    bool halfLeading = 0;
    bool result = GetBoolData(env, info, halfLeading);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetHalfLeading");
    }
    LOGE("ParamTestfromTextStyle | halfLeading=%d",halfLeading);
    m_textStyle->halfLeading = halfLeading;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetHeightScale(napi_env env, napi_callback_info info)
{
    double heightScale = 0;
    bool result = GetDoubleData(env, info, heightScale);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetHeightScale");
    }
    LOGE("ParamTestfromTextStyle | heightScale=%f",heightScale);
    m_textStyle->heightScale = heightScale;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::SetWordSpacing(napi_env env, napi_callback_info info)
{
    JsTextStyle* me = CheckParamsAndGetThis<JsTextStyle>(env, info);
    return (me != nullptr) ? me->OnSetLetterSpacing(env, info) : nullptr;
}

napi_value JsTextStyle::OnSetWordSpacing(napi_env env, napi_callback_info info)
{
    double wordSpace = 0;
    bool result = GetDoubleData(env, info, wordSpace);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetWordSpacing");
    }
    LOGE("ParamTestfromTextStyle | wordSpace=%f",wordSpace);
    m_textStyle->wordSpacing = wordSpace;
    return NapiGetUndefined(env);
}

napi_value JsTextStyle::OnSetLetterSpacing(napi_env env, napi_callback_info info)
{
    double letterSpacing = 0;
    bool result = GetDoubleData(env, info, letterSpacing);
    if (!result) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
            "Failed OnSetLetterSpacing");
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
        if (napi_get_value_string_utf8(env, element, nullptr, 0, &bufferSize) != napi_ok) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
                "OnSetFontFamilies napi_get_value_string_utf8 Failed");
        }

        size_t strLen = 0;
        auto buffer = std::make_unique<char[]>(bufferSize + 1);
        status = napi_get_value_string_utf8(env, element, buffer.get(), bufferSize + 1, &strLen);
        if (status != napi_ok) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM,
                "OnSetFontFamilies napi_get_value_string_utf8 Failed");
        }
        std::string value(buffer.get());
        m_textStyle->fontFamilies.push_back(value);
        LOGE("ParamTestfromTextStyle | fontFamilies=%s",value.c_str());
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
