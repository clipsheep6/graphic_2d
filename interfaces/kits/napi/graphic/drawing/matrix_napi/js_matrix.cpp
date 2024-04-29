/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "js_matrix.h"
#include "js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsMatrix::constructor_ = nullptr;
const std::string CLASS_NAME = "Matrix";
napi_value JsMatrix::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Init Failed to define Matrix class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsMatrix::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    Matrix *matrix = new Matrix();
    JsMatrix *jsMatrix = new(std::nothrow) JsMatrix(matrix);

    status = napi_wrap(env, jsThis, jsMatrix, JsMatrix::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsMatrix;
        ROSEN_LOGE("JsMatrix::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsMatrix::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsMatrix* napi = reinterpret_cast<JsMatrix*>(nativeObject);
        delete napi;
    }
}

JsMatrix::~JsMatrix()
{
    delete m_matrix;
    m_matrix = nullptr;
}

Matrix *JsMatrix::GetMatrix()
{
    return m_matrix;
}

}
}