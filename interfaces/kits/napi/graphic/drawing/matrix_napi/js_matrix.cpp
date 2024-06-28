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
        DECLARE_NAPI_FUNCTION("getValue", JsMatrix::GetValue),
        DECLARE_NAPI_FUNCTION("postRotate", JsMatrix::PostRotate),
        DECLARE_NAPI_FUNCTION("postTranslate", JsMatrix::PostTranslate),
        DECLARE_NAPI_FUNCTION("preRotate", JsMatrix::PreRotate),
        DECLARE_NAPI_FUNCTION("preScale", JsMatrix::PreScale),
        DECLARE_NAPI_FUNCTION("preTranslate", JsMatrix::PreTranslate),
        DECLARE_NAPI_FUNCTION("mapPoints", JsMatrix::MapPoints),
        DECLARE_NAPI_FUNCTION("postScale", JsMatrix::PostScale),
        DECLARE_NAPI_FUNCTION("reset", JsMatrix::Reset),
        DECLARE_NAPI_FUNCTION("getAll", JsMatrix::GetAll),
        DECLARE_NAPI_FUNCTION("setPolyToPoly", JsMatrix::SetPolyToPoly),
        DECLARE_NAPI_FUNCTION("setRectToRect", JsMatrix::SetRectToRect),
        DECLARE_NAPI_FUNCTION("mapRect", JsMatrix::MapRect),
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

    JsMatrix* jsMatrix = new(std::nothrow) JsMatrix();
    if (jsMatrix == nullptr) {
        ROSEN_LOGE("JsMatrix::Constructor Failed to create jsMatrix");
        return nullptr;
    }

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

napi_value JsMatrix::GetValue(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnGetValue(env, info) : nullptr;
}

napi_value JsMatrix::OnGetValue(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnGetValue matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t index = -1;
    GET_INT32_PARAM(ARGC_ZERO, index);

    if (index < 0 || index > 8) {
        ROSEN_LOGE("JsMatrix::OnGetValue index is out of range");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double value = m_matrix->Get(index);
    return GetDoubleAndConvertToJsValue(env, value);
}

napi_value JsMatrix::PostRotate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPostRotate(env, info) : nullptr;
}

napi_value JsMatrix::OnPostRotate(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostRotate matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double degree = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, degree);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, py);

    JS_CALL_DRAWING_FUNC(m_matrix->PostRotate(degree, px, py));

    return nullptr;
}

napi_value JsMatrix::PostTranslate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPostTranslate(env, info) : nullptr;
}

napi_value JsMatrix::OnPostTranslate(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostTranslate matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    JS_CALL_DRAWING_FUNC(m_matrix->PostTranslate(dx, dy));

    return nullptr;
}

napi_value JsMatrix::PreRotate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPreRotate(env, info) : nullptr;
}

napi_value JsMatrix::OnPreRotate(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreRotate matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double degree = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, degree);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, py);

    JS_CALL_DRAWING_FUNC(m_matrix->PreRotate(degree, px, py));

    return nullptr;
}

napi_value JsMatrix::PreScale(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPreScale(env, info) : nullptr;
}

napi_value JsMatrix::OnPreScale(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreScale matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double sx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, sx);
    double sy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, sy);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py);

    JS_CALL_DRAWING_FUNC(m_matrix->PreScale(sx, sy, px, py));

    return nullptr;
}

napi_value JsMatrix::PreTranslate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPreTranslate(env, info) : nullptr;
}

napi_value JsMatrix::OnPreTranslate(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreTranslate matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    JS_CALL_DRAWING_FUNC(m_matrix->PreTranslate(dx, dy));

    return nullptr;
}

napi_value JsMatrix::MapPoints(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnMapPoints(env, info) : nullptr;
}

napi_value JsMatrix::OnMapPoints(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnMapPoints matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    uint32_t count = 0;
    napi_value dstArray = argv[ARGC_ZERO];
    napi_value srcArray = argv[ARGC_ONE];
    GET_UINT32_PARAM(ARGC_TWO, count);

    if (count == 0) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect count value.");
    }

    /* Check size */
    uint32_t srcPointsSize = 0;
    if (napi_get_array_length(env, srcArray, &srcPointsSize) != napi_ok || (count > srcPointsSize)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect src array size.");
    }

    std::vector<Point> dstPoints(count);
    std::vector<Point> srcPoints(count);
    /* Fill vector with data from input array */
    if(!ConvertFromJsPointsArray(env, srcArray, srcPoints.data(), count)){
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect src array data.");
    }

    JS_CALL_DRAWING_FUNC(m_matrix->MapPoints(dstPoints, srcPoints, count));
    for (uint32_t idx = 0; idx < dstPoints.size(); idx++) {
        NAPI_CALL(env, napi_set_element(env, dstArray, idx, ConvertPointToJsValue(env, dstPoints.at(idx))));
    }

    return nullptr;
}

napi_value JsMatrix::PostScale(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPostScale(env, info) : nullptr;
}

napi_value JsMatrix::OnPostScale(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostScale matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double sx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, sx);
    double sy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, sy);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py);

    JS_CALL_DRAWING_FUNC(m_matrix->PostScale(sx, sy, px, py));

    return nullptr;
}

napi_value JsMatrix::Reset(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnReset(env, info) : nullptr;
}

napi_value JsMatrix::OnReset(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnReset matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    m_matrix->Reset();
    return nullptr;
}

napi_value JsMatrix::GetAll(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnGetAll(env, info) : nullptr;
}

napi_value JsMatrix::OnGetAll(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnGetAll matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    // get target array from ARGV, check it's size
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    napi_value targetArray = argv[ARGC_ZERO];

    uint32_t arrayLength = 0;
    if (napi_get_array_length(env, targetArray, &arrayLength) != napi_ok) {
        ROSEN_LOGE("JsMatrix::OnGetAll buffer is not an array");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    if (arrayLength != Matrix::MATRIX_SIZE) {
        // let's resize incoming array to required 9 elements
        if (napi_set_named_property(env, targetArray, "length", CreateJsValue(env, Matrix::MATRIX_SIZE)) != napi_ok) {
            ROSEN_LOGE("JsMatrix::OnGetAll unable to resize input array");
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        }
    }

    Matrix::Buffer matrData;
    m_matrix->GetAll(matrData);

    for (int i = 0; i < Matrix::MATRIX_SIZE; ++i) {
        if (napi_set_element(env, targetArray, i, CreateJsValue(env, matrData[i])) != napi_ok) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Failed to set data to param.");
        }
    }

    return nullptr;
}

napi_value JsMatrix::SetPolyToPoly(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetPolyToPoly(env, info) : nullptr;
}

napi_value JsMatrix::OnSetPolyToPoly(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetPolyToPoly matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    uint32_t count = 0;
    GET_UINT32_PARAM(ARGC_TWO, count);

    if (count == 0) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Invalid param 'count' == 0");
    }
    uint32_t srcPointsSize = 0;
    if (napi_get_array_length(env, argv[ARGC_ZERO], &srcPointsSize) != napi_ok || (count > srcPointsSize)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect src array size.");
    }
    Drawing::Point srcPoints[count];
    if (!ConvertFromJsPointsArray(env, argv[ARGC_ZERO], srcPoints, count)){
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Invalid 'src' Array<Point> type.");
    }
    Drawing::Point dstPoints[count];
    bool result = m_matrix->SetPolyToPoly(srcPoints, dstPoints, count);

    if (result) {
        for (uint32_t i = 0; i < count; i++) {
            if (napi_set_element(env, argv[ARGC_ONE], i,
                GetPointAndConvertToJsValue(env, dstPoints[i])) != napi_ok) {
                return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                       "Cannot fill 'dst' Array<Point> type.");
            }
        }
    }

    return CreateJsValue(env, result);
}

napi_value JsMatrix::SetRectToRect(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetRectToRect(env, info) : nullptr;
}

napi_value JsMatrix::OnSetRectToRect(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetRectToRect matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double ltrb[ARGC_FOUR];
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect 'src' Rect type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect srcRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    Drawing::Rect dstRect;

    int32_t stf;
    GET_ENUM_PARAM(ARGC_TWO, stf, 0, static_cast<int32_t>(ScaleToFit::END_SCALETOFIT));

    bool result = m_matrix->SetRectToRect(srcRect, dstRect, static_cast<ScaleToFit>(stf));

    if (result) {
        napi_value objValue = argv[ARGC_ONE];
        if (napi_set_named_property(env, objValue, "left",
                CreateJsNumber(env, dstRect.GetLeft())) != napi_ok ||
            napi_set_named_property(env, objValue, "top",
                CreateJsNumber(env, dstRect.GetTop())) != napi_ok ||
            napi_set_named_property(env, objValue, "right",
                CreateJsNumber(env, dstRect.GetRight())) != napi_ok ||
            napi_set_named_property(env, objValue, "bottom",
                CreateJsNumber(env, dstRect.GetBottom())) != napi_ok ) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                    "Cannot fill 'dst' Rect type.");
        }
    }

    return CreateJsValue(env, result);
}

napi_value JsMatrix::MapRect(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnMapRect(env, info) : nullptr;
}

napi_value JsMatrix::OnMapRect(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnMapRect matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double src[ARGC_FOUR];
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], src, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsMatrix::OnMapRect Incorrect parameter type for src rect. The types must be numbers.");
    }
    Rect dstRect;
    Rect srcRect{ src[ARGC_ZERO], src[ARGC_ONE], src[ARGC_TWO], src[ARGC_THREE]};
    auto res = CreateJsValue(env, m_matrix->MapRect(dstRect, srcRect));
    auto objValue = argv[ARGC_ZERO];
    if (napi_set_named_property(env, objValue, "left",
            CreateJsNumber(env, dstRect.GetLeft())) != napi_ok ||
        napi_set_named_property(env, objValue, "top",
            CreateJsNumber(env, dstRect.GetTop())) != napi_ok ||
        napi_set_named_property(env, objValue, "right",
            CreateJsNumber(env, dstRect.GetRight())) != napi_ok ||
        napi_set_named_property(env, objValue, "bottom",
            CreateJsNumber(env, dstRect.GetBottom())) != napi_ok ) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "JsMatrix::OnMapRect Cannot fill 'dst' Rect type.");
    }
    return res;
}

JsMatrix::~JsMatrix()
{
    m_matrix = nullptr;
}

std::shared_ptr<Matrix> JsMatrix::GetMatrix()
{
    return m_matrix;
}
}
}