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
#include "../js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsMatrix::constructor_ = nullptr;
const std::string CLASS_NAME = "Matrix";
napi_value JsMatrix::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setMatrix", JsMatrix::SetMatrix),
        DECLARE_NAPI_FUNCTION("concat", JsMatrix::Concat),
        DECLARE_NAPI_FUNCTION("matrixIsEqual", JsMatrix::MatrixIsEqual),
        DECLARE_NAPI_FUNCTION("invert", JsMatrix::Invert),
        DECLARE_NAPI_FUNCTION("isIdentity", JsMatrix::IsIdentity),
        DECLARE_NAPI_FUNCTION("rotate", JsMatrix::Rotate),
        DECLARE_NAPI_FUNCTION("scale", JsMatrix::Scale),
        DECLARE_NAPI_FUNCTION("translate", JsMatrix::Translate),
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

napi_value JsMatrix::SetMatrix(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    Matrix* mat = me->GetMatrix();
    if (mat == nullptr) {
        ROSEN_LOGE("JsMatrix::SetMatrix mat is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_NINE;
    napi_value argv[ARGC_NINE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_NINE) {
        ROSEN_LOGE("JsMatrix::SetMatrix Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double scaleX = 0;
    if (!ConvertFromJsValue(env, argv[0], scaleX)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    double skewX = 0;
    if (!ConvertFromJsValue(env, argv[1], skewX)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[1] is invalid");
        return NapiGetUndefined(env);
    }

    double transX = 0;
    if (!ConvertFromJsValue(env, argv[2], transX)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[2] is invalid");
        return NapiGetUndefined(env);
    }

    double skewY = 0;
    if (!ConvertFromJsValue(env, argv[3], skewY)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[3] is invalid");
        return NapiGetUndefined(env);
    }

    double scaleY = 0;
    if (!ConvertFromJsValue(env, argv[4], scaleY)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[4] is invalid");
        return NapiGetUndefined(env);
    }

    double transY = 0;
    if (!ConvertFromJsValue(env, argv[5], transY)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[5] is invalid");
        return NapiGetUndefined(env);
    }

    double persp0 = 0;
    if (!ConvertFromJsValue(env, argv[6], persp0)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[6] is invalid");
        return NapiGetUndefined(env);
    }

    double persp1 = 0;
    if (!ConvertFromJsValue(env, argv[7], persp1)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[7] is invalid");
        return NapiGetUndefined(env);
    }

    double persp2 = 0;
    if (!ConvertFromJsValue(env, argv[8], persp2)){
        ROSEN_LOGE("JsMatrix::SetMatrix Argv[8] is invalid");
        return NapiGetUndefined(env);
    }

    mat->SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    return NapiGetUndefined(env);
}

napi_value JsMatrix::Concat(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    Matrix* mat = me->GetMatrix();
    if (mat == nullptr) {
        ROSEN_LOGE("JsMatrix::Concat mat is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsMatrix::Concat Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsMatrix* matrix = nullptr;
    napi_unwrap(env,argv[0], reinterpret_cast<void**>(&matrix));
    if(matrix == nullptr){
        return nullptr;
    }
    mat->PreConcat(*matrix->GetMatrix());
    return NapiGetUndefined(env);
}

napi_value JsMatrix::MatrixIsEqual(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    Matrix* mat = me->GetMatrix();
    if (mat == nullptr) {
        ROSEN_LOGE("JJsMatrix::MatrixIsEqual mat is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsMatrix::MatrixIsEqual Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsMatrix* matrix = nullptr;
    napi_unwrap(env,argv[0], reinterpret_cast<void**>(&matrix));
    if(matrix == nullptr){
        return nullptr;
    }
    return CreateJsValue(env, mat->operator==(*matrix->GetMatrix()));
}

napi_value JsMatrix::Invert(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    Matrix* mat = me->GetMatrix();
    if (mat == nullptr) {
        ROSEN_LOGE("JsMatrix::Invert mat is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsMatrix::Invert Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsMatrix* matrix = nullptr;
    napi_unwrap(env,argv[0], reinterpret_cast<void**>(&matrix));
    if(matrix == nullptr){
        return nullptr;
    }
    return CreateJsValue(env, mat->Invert(*matrix->GetMatrix()));
}

napi_value JsMatrix::IsIdentity(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    Matrix* mat = me->GetMatrix();
    if (mat == nullptr) {
        ROSEN_LOGE("JsMatrix::IsIdentity mat is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsValue(env, mat->IsIdentity());
}

napi_value JsMatrix::Rotate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    Matrix* mat = me->GetMatrix();
    if (mat == nullptr) {
        ROSEN_LOGE("JsMatrix::Rotate mat is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsMatrix::Rotate Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double degree = 0;
    if (!ConvertFromJsValue(env, argv[0], degree)){
        ROSEN_LOGE("JsMatrix::Rotate Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    double px = 0;
    if (!ConvertFromJsValue(env, argv[1], px)){
        ROSEN_LOGE("JsMatrix::Rotate Argv[1] is invalid");
        return NapiGetUndefined(env);
    }

    double py = 0;
    if (!ConvertFromJsValue(env, argv[2], py)){
        ROSEN_LOGE("JsMatrix::Rotate Argv[2] is invalid");
        return NapiGetUndefined(env);
    }
    mat->Rotate(degree, px, py);
    return NapiGetUndefined(env);

}

napi_value JsMatrix::Scale(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    Matrix* mat = me->GetMatrix();
    if (mat == nullptr) {
        ROSEN_LOGE("JsMatrix::Scale mat is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FOUR) {
        ROSEN_LOGE("JsMatrix::Scale Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double sx = 0;
    if (!ConvertFromJsValue(env, argv[0], sx)){
        ROSEN_LOGE("JsMatrix::Scale Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    double sy = 0;
    if (!ConvertFromJsValue(env, argv[1], sy)){
        ROSEN_LOGE("JsMatrix::Scale Argv[1] is invalid");
        return NapiGetUndefined(env);
    }

    double px = 0;
    if (!ConvertFromJsValue(env, argv[2], px)){
        ROSEN_LOGE("JsMatrix::Scale Argv[2] is invalid");
        return NapiGetUndefined(env);
    }

    double py = 0;
    if (!ConvertFromJsValue(env, argv[3], py)){
        ROSEN_LOGE("JsMatrix::Scale Argv[3] is invalid");
        return NapiGetUndefined(env);
    }
    mat->Scale(sx, sy, px, py);
    return NapiGetUndefined(env);
}

napi_value JsMatrix::Translate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    Matrix* mat = me->GetMatrix();
    if (mat == nullptr) {
        ROSEN_LOGE("JsMatrix::Translate mat is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsMatrix::Translate Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double dx = 0;
    if (!ConvertFromJsValue(env, argv[0], dx)){
        ROSEN_LOGE("JsMatrix::Translate Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    double dy = 0;
    if (!ConvertFromJsValue(env, argv[1], dy)){
        ROSEN_LOGE("JsMatrix::Translate Argv[1] is invalid");
        return NapiGetUndefined(env);
    }
    mat->Translate(dx, dy);
    return NapiGetUndefined(env);
}

Matrix *JsMatrix::GetMatrix()
{
    return m_matrix;
}

}
}