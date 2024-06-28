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

#include "js_rect.h"
#include "../js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsRect::constructor_ = nullptr;
const std::string CLASS_NAME = "Rect";
napi_value JsRect::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getWidth", JsRect::GetWidth),
        DECLARE_NAPI_FUNCTION("getHeight", JsRect::GetHeight),
        DECLARE_NAPI_FUNCTION("intersect", JsRect::Intersect),
        DECLARE_NAPI_FUNCTION("setLeft", JsRect::SetLeft),
        DECLARE_NAPI_FUNCTION("setTop", JsRect::SetTop),
        DECLARE_NAPI_FUNCTION("setRight", JsRect::SetRight),
        DECLARE_NAPI_FUNCTION("setBottom", JsRect::SetBottom),
        DECLARE_NAPI_FUNCTION("getLeft", JsRect::GetLeft),
        DECLARE_NAPI_FUNCTION("getTop", JsRect::GetTop),
        DECLARE_NAPI_FUNCTION("getRight", JsRect::GetRight),
        DECLARE_NAPI_FUNCTION("getBottom", JsRect::GetBottom),
        DECLARE_NAPI_FUNCTION("copy", JsRect::Copy),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Rect class");
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

napi_value JsRect::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    Rect *rect = new Rect();
    JsRect *jsRect = new(std::nothrow) JsRect(rect);

    status = napi_wrap(env, jsThis, jsRect,
                       JsRect::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRect;
        ROSEN_LOGE("JsRect::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsRect::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRect *napi = reinterpret_cast<JsRect *>(nativeObject);
        delete napi;
    }
}

JsRect::~JsRect()
{
    if (m_rect != nullptr)
        delete m_rect;
    m_rect = nullptr;
}

napi_value JsRect::GetWidth(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnGetWidth(env, info) : nullptr;
}

napi_value JsRect::GetHeight(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnGetHeight(env, info) : nullptr;
}

napi_value JsRect::Intersect(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnIntersect(env, info) : nullptr;
}

napi_value JsRect::SetLeft(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnSetLeft(env, info) : nullptr;
}

napi_value JsRect::SetTop(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnSetTop(env, info) : nullptr;
}

napi_value JsRect::SetRight(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnSetRight(env, info) : nullptr;
}

napi_value JsRect::SetBottom(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnSetBottom(env, info) : nullptr;
}

napi_value JsRect::GetLeft(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnGetLeft(env, info) : nullptr;
}

napi_value JsRect::GetTop(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnGetTop(env, info) : nullptr;
}

napi_value JsRect::GetRight(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnGetRight(env, info) : nullptr;
}

napi_value JsRect::GetBottom(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnGetBottom(env, info) : nullptr;
}

napi_value JsRect::Copy(napi_env env, napi_callback_info info)
{
    JsRect* me = CheckParamsAndGetThis<JsRect>(env, info);
    return (me != nullptr) ? me->OnCopy(env, info) : nullptr;
}

napi_value JsRect::OnGetWidth(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env,m_rect->GetWidth());
}

napi_value JsRect::OnGetHeight(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env, m_rect->GetHeight());
}

napi_value JsRect::OnIntersect(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("Drawing_napi: OnIntersect Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("Drawing_napi: OnIntersect Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value tempValue = nullptr;
    double left = 0.0f;
    double top = 0.0f;
    double right = 0.0f;
    double bottom = 0.0f;
    napi_get_named_property(env, argv[0], "left", &tempValue);
    napi_get_value_double(env, tempValue, &left);
    napi_get_named_property(env, argv[0], "top", &tempValue);
    napi_get_value_double(env, tempValue, &top);    
    napi_get_named_property(env, argv[0], "right", &tempValue);
    napi_get_value_double(env, tempValue, &right);
    napi_get_named_property(env, argv[0], "bottom", &tempValue);
    napi_get_value_double(env, tempValue, &bottom);

    Drawing::Rect rect = Drawing::Rect(left, top, right, bottom);
    m_rect->Intersect(rect);
    //return NapiGetUndefined(env);
    return CreateJsNumber(env, m_rect->Intersect(rect));
}

napi_value JsRect::OnSetLeft(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double left = 0;
    ConvertFromJsNumber(env, argv[0], left);
    m_rect->SetLeft((float)left);
    return NapiGetUndefined(env);
}

napi_value JsRect::OnSetTop(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double top = 0;
    ConvertFromJsNumber(env, argv[0], top);
    m_rect->SetTop((float)top);
    return NapiGetUndefined(env);
}

napi_value JsRect::OnSetRight(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double right = 0;
    ConvertFromJsNumber(env, argv[0], right);
    m_rect->SetRight((float)right);
    return NapiGetUndefined(env);
}

napi_value JsRect::OnSetBottom(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double bottom = 0;
    ConvertFromJsNumber(env, argv[0], bottom);
    m_rect->SetBottom((float)bottom);
    return NapiGetUndefined(env);
}

napi_value JsRect::OnGetLeft(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env,m_rect->GetLeft());
}

napi_value JsRect::OnGetTop(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env,m_rect->GetTop());
}

napi_value JsRect::OnGetRight(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env,m_rect->GetRight());
}

napi_value JsRect::OnGetBottom(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env,m_rect->GetBottom());
}

napi_value JsRect::OnCopy(napi_env env, napi_callback_info info)
{
    if (m_rect == nullptr) {
        ROSEN_LOGE("[NAPI]rect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return GetRectAndConvertToJsValue(env, std::shared_ptr<Rect>(m_rect));
}

Rect* JsRect::GetRect()
{
    return m_rect;
}
} // namespace Drawing
} // namespace OHOS::Rosen