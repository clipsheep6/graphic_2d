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

#include "js_pixmap.h"
#include "../js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPixmap::constructor_ = nullptr;
const std::string CLASS_NAME = "Pixmap";
napi_value JsPixmap::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getWidth", JsPixmap::GetWidth),
        DECLARE_NAPI_FUNCTION("getHeight", JsPixmap::GetHeight),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Pixmap class");
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

napi_value JsPixmap::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    Pixmap *pixmap = new Pixmap();
    JsPixmap *jsPixmap = new(std::nothrow) JsPixmap(pixmap);

    status = napi_wrap(env, jsThis, jsPixmap,
                       JsPixmap::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPixmap;
        ROSEN_LOGE("JsPixmap::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsPixmap::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsPixmap *napi = reinterpret_cast<JsPixmap *>(nativeObject);
        delete napi;
    }
}

JsPixmap::~JsPixmap()
{
    if (m_pixmap != nullptr)
        delete m_pixmap;
    m_pixmap = nullptr;
}

napi_value JsPixmap::GetWidth(napi_env env, napi_callback_info info)
{
    JsPixmap* me = CheckParamsAndGetThis<JsPixmap>(env, info);
    return (me != nullptr) ? me->OnGetWidth(env, info) : nullptr;
}

napi_value JsPixmap::GetHeight(napi_env env, napi_callback_info info)
{
    JsPixmap* me = CheckParamsAndGetThis<JsPixmap>(env, info);
    return (me != nullptr) ? me->OnGetHeight(env, info) : nullptr;
}

napi_value JsPixmap::OnGetWidth(napi_env env, napi_callback_info info)
{
    if (m_pixmap == nullptr) {
        ROSEN_LOGE("[NAPI]pixmap is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env,m_pixmap->GetWidth());
}

napi_value JsPixmap::OnGetHeight(napi_env env, napi_callback_info info)
{
    if (m_pixmap == nullptr) {
        ROSEN_LOGE("[NAPI]pixmap is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsNumber(env, m_pixmap->GetHeight());
}

Pixmap* JsPixmap::GetPixmap()
{
    return m_pixmap;
}
} // namespace Drawing
} // namespace OHOS::Rosen