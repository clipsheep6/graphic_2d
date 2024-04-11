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
#include "../drawing/js_drawing_utils.h"

namespace OHOS::Rosen {
std::shared_ptr<TextLineBase> g_TextLineBase = nullptr;
std::shared_ptr<Typography> g_TextLineBaseTypography = nullptr;
thread_local napi_ref JsTextLine::constructor_ = nullptr;
const std::string CLASS_NAME = "TextLine";
napi_value JsTextLine::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getGlyphCount", JsTextLine::GetGlyphCount),
        // DECLARE_NAPI_FUNCTION("getGlyphRuns", JsTextLine::GetGlyphRuns),
        DECLARE_NAPI_FUNCTION("getTextRange", JsTextLine::GetTextRange),
        DECLARE_NAPI_FUNCTION("paint", JsTextLine::Paint),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define TextLine class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set constructor");
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
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    if (!g_TextLineBase || !g_TextLineBaseTypography) {
        ROSEN_LOGE(" clp-ark JsTextLine::Constructor g_TextLineBase or g_TextLineBaseTypography is nullptr");
        return nullptr;
    }

    JsTextLine *jsTextLineBase = new(std::nothrow) JsTextLine(g_TextLineBase, g_TextLineBaseTypography);
    if (!jsTextLineBase) {
        ROSEN_LOGE(" clp-ark JsTextLine::Constructor jsTextLineBase is nullptr");
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsTextLineBase,
                       JsTextLine::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsTextLineBase;
        ROSEN_LOGE("JsTextLine::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsTextLine::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsTextLine *napi = reinterpret_cast<JsTextLine *>(nativeObject);
        delete napi;
    }
}

JsTextLine::JsTextLine(std::shared_ptr<TextLineBase> textLineBase, std::shared_ptr<Typography> paragraph)
    : textLineBase_(textLineBase), paragraphCurrent_(paragraph)
{
}

napi_value JsTextLine::GetGlyphCount(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetGlyphCount(env, info) : nullptr;
}

napi_value JsTextLine::GetGlyphRuns(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetGlyphRuns(env, info) : nullptr;
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
    if (textLineBase_ == nullptr) {
        ROSEN_LOGE("JsFont::OnGetGlyphCount font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    uint32_t textSize = textLineBase_->GetGlyphCount();
    return CreateJsValue(env, textSize);
}

napi_value JsTextLine::OnGetGlyphRuns(napi_env env, napi_callback_info info)
{
    if (textLineBase_ == nullptr) {
        ROSEN_LOGE("JsTextLine::OnGetGlyphRuns TextLine is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::vector<std::unique_ptr<Run>>  runs = textLineBase_->GetGlyphRuns();
    if (runs.empty()) {
        ROSEN_LOGE("JsTextLine::OnGetGlyphRuns runs is empty");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value array = nullptr;
    napi_create_array_with_length(env, runs.size(), &array);
    uint32_t index = 0;
    for (std::unique_ptr<Run>& item : runs) {
        napi_value itemObject = JsRun::CreateJsRun(env, std::move(item), paragraphCurrent_);
        if (itemObject == nullptr) {
            ROSEN_LOGE("OnGetGlyphRuns itemObject is null!");
            continue;
        }
        napi_set_element(env, array, index++, itemObject);
    }
    return array;
}

napi_value JsTextLine::OnGetTextRange(napi_env env, napi_callback_info info)
{
    if (textLineBase_ == nullptr) {
        ROSEN_LOGE("JsTextLine::OnGetTextRange TextLine is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Boundary boundary = textLineBase_->GetTextRange();
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
    if (textLineBase_ == nullptr) {
        ROSEN_LOGE("JsTextLine::OnPaint TextLine is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }    
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE ) {
        ROSEN_LOGE("JsTextLine::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsCanvas));
    if (jsCanvas == nullptr) {
        ROSEN_LOGE("JsTextLine::OnPaint jsCanvas is nullptr");
        return NapiGetUndefined(env);
    }
    if (jsCanvas->GetCanvas() == nullptr) {
        ROSEN_LOGE("JsTextLine::OnPaint canvas is nullptr");
        return NapiGetUndefined(env);
    }
    double x = 0.0;
    double y = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], x) && ConvertFromJsValue(env, argv[ARGC_ONE], y))) {
        ROSEN_LOGE("JsTextLine::OnPaint Argv0 or Argv 1 is invalid");
        return NapiGetUndefined(env);
    }
    textLineBase_->Paint(jsCanvas->GetCanvas(), x, y);
    return NapiGetUndefined(env);
}

napi_value JsTextLine::CreateJsTextLine(napi_env env, std::shared_ptr<TextLineBase> textLineBase,
    std::shared_ptr<Typography> paragraph)
{
    ROSEN_LOGE(" clp-ark JsTextLine::CreateJsTextLine 000 textLineBase.get() = %p constructor_ = %p ",
        textLineBase.get(), constructor_);


    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        g_TextLineBase = textLineBase;
        g_TextLineBaseTypography = paragraph;
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        if (status == napi_ok) {
            ROSEN_LOGE(" clp-ark JsTextLine::CreateJsTextLine OnCreateJsTypography 800   ");
            return result;
        } else {
            ROSEN_LOGE(" clp-ark JsTextLine::CreateJsTextLine: New instance could not be obtained");
        }
    }

    ROSEN_LOGE(" clp-ark JsTextLine::CreateJsTextLine OnCreateJsTypography 900   ");
    return result;
}

} // namespace OHOS::Rosen