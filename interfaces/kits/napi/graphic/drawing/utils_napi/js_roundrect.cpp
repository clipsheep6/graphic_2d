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

#include "js_roundrect.h"
#include "../js_drawing_utils.h"
#include "native_value.h"
#include "../js_common.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsRoundRect::constructor_ = nullptr;
const std::string CLASS_NAME = "RoundRect";
napi_value JsRoundRect::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setRect", JsRoundRect::SetRect),
        DECLARE_NAPI_FUNCTION("offset", JsRoundRect::Offset),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRoundRect::Init Failed to define RoundRect class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRoundRect::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRoundRect::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsRoundRect::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 3;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRoundRect::Constructor failed to napi_get_cb_info");
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("JsRoundRect::Constructor Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    napi_value tempValue = nullptr;
    double left = 0.0;
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;
    napi_get_named_property(env, argv[0], "left", &tempValue);
    bool isLeftOk = ConvertFromJsValue(env, tempValue, left);
    napi_get_named_property(env, argv[0], "right", &tempValue);
    bool isRightOk = ConvertFromJsValue(env, tempValue, right);
    napi_get_named_property(env, argv[0], "top", &tempValue);
    bool isTopOk = ConvertFromJsValue(env, tempValue, top);
    napi_get_named_property(env, argv[0], "bottom", &tempValue);
    bool isBottomOk = ConvertFromJsValue(env, tempValue, bottom);
    if (!(isLeftOk && isRightOk && isTopOk && isBottomOk)) {
        ROSEN_LOGE("JsRoundRect::Constructor Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    double xRad = 0.0;
    double yRad = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ONE], xRad) && ConvertFromJsValue(env, argv[ARGC_TWO], yRad))) {
        ROSEN_LOGE("JsRoundRect::Constructor Argv is invalid");
        return NapiGetUndefined(env);
    }
    Drawing::Rect rect = Drawing::Rect(left, top, right, bottom);

    RoundRect *roundrect = new RoundRect(rect, xRad, yRad);
    JsRoundRect* jsRoundRect = new(std::nothrow) JsRoundRect(roundrect);
    status = napi_wrap(env, jsThis, jsRoundRect,
                       JsRoundRect::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRoundRect;
        ROSEN_LOGE("JsRoundRect::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsRoundRect::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRoundRect* napi = reinterpret_cast<JsRoundRect*>(nativeObject);
        delete napi;
    }
}

napi_value JsRoundRect::SetRect(napi_env env, napi_callback_info info)
{
    JsRoundRect* me = CheckParamsAndGetThis<JsRoundRect>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    RoundRect *RoundRect = me->GetRoundRect();

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsCanvas::SetRect Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("JsCanvas::SetRect Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    napi_value tempValue = nullptr;
    double left = 0.0;
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;
    napi_get_named_property(env, argv[0], "left", &tempValue);
    bool isLeftOk = ConvertFromJsValue(env, tempValue, left);
    napi_get_named_property(env, argv[0], "right", &tempValue);
    bool isRightOk = ConvertFromJsValue(env, tempValue, right);
    napi_get_named_property(env, argv[0], "top", &tempValue);
    bool isTopOk = ConvertFromJsValue(env, tempValue, top);
    napi_get_named_property(env, argv[0], "bottom", &tempValue);
    bool isBottomOk = ConvertFromJsValue(env, tempValue, bottom);
    if (!(isLeftOk && isRightOk && isTopOk && isBottomOk)) {
        ROSEN_LOGE("JsCanvas::SetRect Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    Drawing::Rect rect = Drawing::Rect(left, top, right, bottom);
    RoundRect->SetRect(rect);
    return NapiGetUndefined(env);
}

napi_value JsRoundRect::Offset(napi_env env, napi_callback_info info)
{
    JsRoundRect* me = CheckParamsAndGetThis<JsRoundRect>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    RoundRect *RoundRect = me->GetRoundRect();

    if (RoundRect == nullptr) {
        ROSEN_LOGE("JsRoundRect::Offset RoundRect is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsRoundRect::Offset Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double dx = 0.0;
    double dy = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], dx) && ConvertFromJsValue(env, argv[ARGC_ONE], dy))) {
        ROSEN_LOGE("JsRoundRect::Offset Argv is invalid");
        return NapiGetUndefined(env);
    }

    RoundRect->Offset(dx, dy);
    return NapiGetUndefined(env);
}

JsRoundRect::~JsRoundRect()
{
    m_roundrect = nullptr;
}

RoundRect* JsRoundRect::GetRoundRect()
{
    return m_roundrect;
}
} // namespace Drawing
} // namespace OHOS::Rosen
