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

#include "js_text_line.h"

#include "canvas_napi/js_canvas.h"
#include "run_napi/js_run.h"
#include "utils/log.h"


namespace OHOS::Rosen {
thread_local napi_ref JsTextLine::constructor_ = nullptr;
const std::string CLASS_NAME = "TextLine";
napi_value JsTextLine::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getGlyphCount", JsTextLine::GetGlyphCount),
        DECLARE_NAPI_FUNCTION("getTextRange", JsTextLine::GetTextRange),
        DECLARE_NAPI_FUNCTION("paint", JsTextLine::Paint),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        LOGE("Failed to define TextLine class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        LOGE("Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        LOGE("Failed to set constructor");
        return nullptr;
    }

    napi_property_descriptor staticProperty[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createTextLine", JsTextLine::CreateTextLine),
    };
    status = napi_define_properties(env, exportObj, 1, staticProperty);
    if (status != napi_ok) {
        LOGE("Failed to define static function");
        return nullptr;
    }

    return exportObj;
}

napi_value JsTextLine::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    JsTextLine *jsTextLineBase = new(std::nothrow) JsTextLine();

    status = napi_wrap(env, jsThis, jsTextLineBase,
                       JsTextLine::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsTextLineBase;
        LOGE("JsTextLine::Constructor Failed to wrap native instance");
        return nullptr;
    }
    LOGE("JsTextLine::Constructor success");
    return jsThis;
}

void JsTextLine::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    LOGE("JsTextLine::Destructor excute");
    (void)finalize;
    if (nativeObject != nullptr) {
        JsTextLine *napi = reinterpret_cast<JsTextLine *>(nativeObject);
        delete napi;
    }
}

napi_value JsTextLine::CreateTextLine(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        LOGE("Failed to get the representation of constructor object");
        return nullptr;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        LOGE("Failed to instantiate JavaScript font instance");
        return nullptr;
    }
    return result;
}

JsTextLine::JsTextLine()
{
}

void JsTextLine::SetTextLine(std::unique_ptr<TextLineBase> textLine)
{
    textLine_ = std::move(textLine);
}

napi_value JsTextLine::GetGlyphCount(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetGlyphCount(env, info) : nullptr;
}

napi_value JsTextLine::GetTextRange(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetTextRange(env, info) : nullptr;
}

napi_value JsTextLine::Paint(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnPaint(env, info) : nullptr;
}

napi_value JsTextLine::OnGetGlyphCount(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        LOGE("JsTextLine::OnGetGlyphCount textLine_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    uint32_t textSize = textLine_->GetGlyphCount();

    return CreateJsValue(env, textSize);
}

napi_value JsTextLine::OnGetTextRange(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        LOGE("JsTextLine::OnGetTextRange TextLine is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Boundary boundary = textLine_->GetTextRange();
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "start", CreateJsNumber(env, boundary.leftIndex));
        napi_set_named_property(env, objValue, "end", CreateJsNumber(env, boundary.rightIndex));
    }
    return objValue;
}

napi_value JsTextLine::OnPaint(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        LOGE("JsTextLine::OnPaint TextLine is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        LOGE("JsTextLine::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsCanvas));
    if (jsCanvas == nullptr) {
        LOGE("JsTextLine::OnPaint jsCanvas is nullptr");
        return NapiGetUndefined(env);
    }
    if (jsCanvas->GetCanvas() == nullptr) {
        LOGE("JsTextLine::OnPaint canvas is nullptr");
        return NapiGetUndefined(env);
    }
    double x = 0.0;
    double y = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        LOGE("JsTextLine::OnPaint Argv0 or Argv 1 is invalid");
        return NapiGetUndefined(env);
    }
    textLine_->Paint(jsCanvas->GetCanvas(), x, y);
    return NapiGetUndefined(env);
}

std::unique_ptr<TextLineBase> JsTextLine::GetTextLineBase()
{
    return std::move(textLine_);
}

} // namespace OHOS::Rosen