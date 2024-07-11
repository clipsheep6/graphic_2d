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

#include "js_rectf.h"
#include "native_value.h"
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsRectF::constructor_ = nullptr;
const std::string CLASS_NAME = "RectF";
napi_value JsRectF::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("joinRect", JsRectF::JoinRect),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to define JsRectF class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to define static function");
        return nullptr;
    }
    return exportObj;
}

void JsRectF::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsRectF>(static_cast<JsRectF*>(data));
}

JsRectF::~JsRectF()
{
    m_rect = nullptr;
}

napi_value JsRectF::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    JsRectF *jsRectF = new(std::nothrow) JsRectF();
    if (!jsRectF) {
        ROSEN_LOGE("Failed to create JsRectF");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsRectF, JsRectF::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRectF;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsRectF::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRectF *napi = reinterpret_cast<JsRectF *>(nativeObject);
        delete napi;
    }
}

napi_value JsRectF::JoinRect(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter 0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    double ltrb2[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb2, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter 1 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect otherRect = Drawing::Rect(ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
    
    bool joinResult = drawingRect.Join(otherRect);
    if (joinResult) {
        napi_set_named_property(env, argv[ARGC_ZERO], "left", CreateJsNumber(env, drawingRect.GetLeft()));
        napi_set_named_property(env, argv[ARGC_ZERO], "top", CreateJsNumber(env, drawingRect.GetTop()));
        napi_set_named_property(env, argv[ARGC_ZERO], "right", CreateJsNumber(env, drawingRect.GetRight()));
        napi_set_named_property(env, argv[ARGC_ZERO], "bottom", CreateJsNumber(env, drawingRect.GetBottom()));
    }

    return CreateJsValue(env, joinResult);
}

std::shared_ptr<Rect> JsRectF::GetRect()
{
    return m_rect;
}

} // namespace Drawing
} // namespace OHOS::Rosen