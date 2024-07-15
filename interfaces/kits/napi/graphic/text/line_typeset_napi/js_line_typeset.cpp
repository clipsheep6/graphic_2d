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
// #include "canvas_napi/js_canvas.h"
// #include "draw/canvas.h"
// #include "recording/recording_canvas.h"
// #include "js_drawing_utils.h"
#include "js_line_typeset.h"
#include "js_text_utils.h"
#include "lineTypeset_builder_napi/js_lineTypeset_builder.h"
// #include "path_napi/js_path.h"
#include "utils/log.h"
#include "text_line_napi/js_text_line.h"

namespace OHOS::Rosen {
std::unique_ptr<LineTypeset> g_LineTypeset = nullptr;
thread_local napi_ref JsLineTypeset::constructor_ = nullptr;
const std::string CLASS_NAME = "LineTypeset";

napi_value JsLineTypeset::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsLineTypeset::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    if (!g_LineTypeset) {
        ROSEN_LOGE("JsLineTypeset::Constructor g_LineTypeset is nullptr");
        return nullptr;
    }

    JsLineTypeset *JsLineTypeset = new(std::nothrow) JsLineTypeset(std::move(g_LineTypeset));
    if (JsLineTypeset == nullptr) {
        return nullptr;
    }

    status = napi_wrap(env, jsThis, JsLineTypeset, JsLineTypeset::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete JsLineTypeset;
        ROSEN_LOGE("JsLineTypeset::Constructor failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsLineTypeset::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsLineTypeset *napi = reinterpret_cast<JsLineTypeset *>(nativeObject);
        delete napi;
    }
}

JsLineTypeset::JsLineTypeset(std::shared_ptr<LineTypeset> lineTypeset) : lineTypeset_(lineTypeset)
{
}

JsLineTypeset::~JsLineTypeset()
{
}

napi_value JsLineTypeset::CreateJsLineTypeset(napi_env env, std::unique_ptr<LineTypeset> LineTypeset)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        g_LineTypeset = std::move(LineTypeset);
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        if (status == napi_ok) {
            return result;
        } else {
            ROSEN_LOGE("CreateJsLineTypeset: New instance could not be obtained");
        }
    }
    return result;
}

napi_value JsLineTypeset::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getLineBreak", JsLineTypeset::GetLineBreak),
        DECLARE_NAPI_FUNCTION("createLine", JsLineTypeset::CreateLine),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Paragraph class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of result");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set result");
        return nullptr;
    }
    return exportObj;
}

napi_value JsLineTypeset::GetLineBreak(napi_env env, napi_callback_info info)
{
    JsLineTypeset* me = CheckParamsAndGetThis<JsLineTypeset>(env, info);
    return (me != nullptr) ? me->OnGetLineBreak(env, info) : nullptr;
}

napi_value JsLineTypeset::OnGetLineBreak(napi_env env, napi_callback_info info)
{
    if (lineTypeset_ == nullptr) {
        ROSEN_LOGE("JsLineTypeset::OnLayout lineTypeset_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsLineTypeset::OnLayout Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double width = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], width))) {
        ROSEN_LOGE("JsLineTypeset::OnLayout Argv is invalid");
        return NapiGetUndefined(env);
    }
    lineTypeset_->Layout(width);
    return NapiGetUndefined(env);
}

napi_value JsLineTypeset::CreateLine(napi_env env, napi_callback_info info)
{
    JsLineTypeset* me = CheckParamsAndGetThis<JsLineTypeset>(env, info);
    return (me != nullptr) ? me->OnCreateLine(env, info) : nullptr;
}

napi_value JsLineTypeset::OnCreateLine(napi_env env, napi_callback_info info)
{
    if (lineTypeset_ == nullptr) {
        ROSEN_LOGE("JsLineTypeset::OnPaint lineTypeset_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsLineTypeset::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    double x = 0.0;
    double y = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsCanvas));
    if (!jsCanvas || !jsCanvas->GetCanvas() ||
        !(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        ROSEN_LOGE("JsLineTypeset::OnPaint Argv is invalid");
        return NapiGetUndefined(env);
    }
    lineTypeset_->Paint(jsCanvas->GetCanvas(), x, y);

    return NapiGetUndefined(env);
}

} // namespace OHOS::Rosen