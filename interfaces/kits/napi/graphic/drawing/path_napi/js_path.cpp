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

#include "js_path.h"

#include "native_value.h"

#include "js_drawing_utils.h"
#include "roundRect_napi/js_roundrect.h"
#include "matrix_napi/js_matrix.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPath::constructor_ = nullptr;
const std::string CLASS_NAME = "Path";
napi_value JsPath::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("moveTo", JsPath::MoveTo),
        DECLARE_NAPI_FUNCTION("lineTo", JsPath::LineTo),
        DECLARE_NAPI_FUNCTION("arcTo", JsPath::ArcTo),
        DECLARE_NAPI_FUNCTION("quadTo", JsPath::QuadTo),
        DECLARE_NAPI_FUNCTION("cubicTo", JsPath::CubicTo),
        DECLARE_NAPI_FUNCTION("addOval", JsPath::AddOval),
        DECLARE_NAPI_FUNCTION("addCircle", JsPath::AddCircle),
        DECLARE_NAPI_FUNCTION("addArc", JsPath::AddArc),
        DECLARE_NAPI_FUNCTION("addRect", JsPath::AddRect),
        DECLARE_NAPI_FUNCTION("addRoundRect", JsPath::AddRoundRect),
        DECLARE_NAPI_FUNCTION("addPath", JsPath::AddPath),
        DECLARE_NAPI_FUNCTION("transform", JsPath::Transform),
        DECLARE_NAPI_FUNCTION("contains", JsPath::Contains),
        DECLARE_NAPI_FUNCTION("setFillType", JsPath::SetFillType),
        DECLARE_NAPI_FUNCTION("getBounds", JsPath::GetBounds),
        DECLARE_NAPI_FUNCTION("close", JsPath::Close),
        DECLARE_NAPI_FUNCTION("reset", JsPath::Reset),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Path class");
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

napi_value JsPath::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    Path *path = new Path();
    JsPath *jsPath = new(std::nothrow) JsPath(path);

    status = napi_wrap(env, jsThis, jsPath,
                       JsPath::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPath;
        ROSEN_LOGE("JsPath::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsPath::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsPath *napi = reinterpret_cast<JsPath *>(nativeObject);
        delete napi;
    }
}

JsPath::~JsPath()
{
    m_path = nullptr;
}

napi_value JsPath::MoveTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnMoveTo(env, info) : nullptr;
}

napi_value JsPath::LineTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnLineTo(env, info) : nullptr;
}

napi_value JsPath::ArcTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnArcTo(env, info) : nullptr;
}

napi_value JsPath::QuadTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnQuadTo(env, info) : nullptr;
}

napi_value JsPath::CubicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnCubicTo(env, info) : nullptr;
}

napi_value JsPath::AddOval(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddOval(env, info) : nullptr;
}

napi_value JsPath::AddCircle(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddCircle(env, info) : nullptr;
}

napi_value JsPath::AddArc(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddArc(env, info) : nullptr;
}

napi_value JsPath::AddRect(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddRect(env, info) : nullptr;
}

napi_value JsPath::AddRoundRect(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddRoundRect(env, info) : nullptr;
}

napi_value JsPath::AddPath(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddPath(env, info) : nullptr;
}

napi_value JsPath::Transform(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnTransform(env, info) : nullptr;
}

napi_value JsPath::Contains(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnContains(env, info) : nullptr;
}

napi_value JsPath::SetFillType(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnSetFillType(env, info) : nullptr;
}

napi_value JsPath::GetBounds(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnGetBounds(env, info) : nullptr;
}

napi_value JsPath::Close(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnClose(env, info) : nullptr;
}

napi_value JsPath::Reset(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnReset(env, info) : nullptr;
}

napi_value JsPath::OnMoveTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnMoveTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsPath::OnMoveTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double x = 0.0;
    double y = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], x) && ConvertFromJsValue(env, argv[1], y))) {
        ROSEN_LOGE("JsPath::OnMoveTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->MoveTo(x, y);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnLineTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnLineTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsPath::OnLineTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double x = 0.0;
    double y = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], x) && ConvertFromJsValue(env, argv[1], y))) {
        ROSEN_LOGE("JsPath::OnLineTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->LineTo(x, y);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnArcTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnArcTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_SIX;
    napi_value argv[ARGC_SIX] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_SIX) {
        ROSEN_LOGE("JsPath::OnArcTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double x1 = 0.0;
    double y1 = 0.0;
    double x2 = 0.0;
    double y2 = 0.0;
    double startDeg = 0.0;
    double sweepDeg = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], x1) && ConvertFromJsValue(env, argv[ARGC_ONE], y1) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], x2) && ConvertFromJsValue(env, argv[ARGC_THREE], y2) &&
        ConvertFromJsValue(env, argv[ARGC_FOUR], startDeg) && ConvertFromJsValue(env, argv[ARGC_FIVE], sweepDeg))) {
        ROSEN_LOGE("JsPath::OnArcTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->ArcTo(x1, y1, x2, y2, startDeg, sweepDeg);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnQuadTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnQuadTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FOUR) {
        ROSEN_LOGE("JsPath::OnQuadTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double ctrlPtX = 0.0;
    double ctrlPtY = 0.0;
    double endPtX = 0.0;
    double endPtY = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], ctrlPtX) && ConvertFromJsValue(env, argv[ARGC_ONE], ctrlPtY) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], endPtX) && ConvertFromJsValue(env, argv[ARGC_THREE], endPtY))) {
        ROSEN_LOGE("JsPath::OnQuadTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->QuadTo(ctrlPtX, ctrlPtY, endPtX, endPtY);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnCubicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnCubicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_SIX;
    napi_value argv[ARGC_SIX] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_SIX) {
        ROSEN_LOGE("JsPath::OnCubicTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double px1 = 0.0;
    double py1 = 0.0;
    double px2 = 0.0;
    double py2 = 0.0;
    double px3 = 0.0;
    double py3 = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], px1) && ConvertFromJsValue(env, argv[ARGC_ONE], py1) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], px2) && ConvertFromJsValue(env, argv[ARGC_THREE], py2) &&
        ConvertFromJsValue(env, argv[ARGC_FOUR], px3) && ConvertFromJsValue(env, argv[ARGC_FIVE], py3))) {
        ROSEN_LOGE("JsPath::OnCubicTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->CubicTo(Point(px1, py1), Point(px2, py2), Point(px3, py3));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnAddOval(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddOval path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsPath::OnAddOval Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Drawing::Rect drawingRect;
    if (!ConvertFromJsRect(env, argv[0], drawingRect)) {
        ROSEN_LOGE("JsPath::OnAddOval Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    uint32_t start = 0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ONE], start))) {
        ROSEN_LOGE("JsPath::OnAddOval Argv[1] is invalid");
        return NapiGetUndefined(env);
    }

    uint32_t jsDirection = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_TWO], jsDirection)) {
        ROSEN_LOGE("JsPath::OnAddOval Argv[2] is invalid");
        return NapiGetUndefined(env);
    }

    m_path->AddOval(drawingRect, start, PathDirection(jsDirection));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnAddCircle(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddCircle path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FOUR) {
        ROSEN_LOGE("JsPath::OnAddCircle Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double px1 = 0.0;
    double py1 = 0.0;
    double radius = 0.0;
    uint32_t jsDirection = 0;
    if (!(ConvertFromJsValue(env, argv[0], px1) && ConvertFromJsValue(env, argv[ARGC_ONE], py1) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], radius) && ConvertFromJsValue(env, argv[ARGC_THREE], jsDirection))) {
        ROSEN_LOGE("JsPath::OnAddCircle Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->AddCircle(px1, py1, radius, PathDirection(jsDirection));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnAddArc(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddArc path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsPath::OnAddArc Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Drawing::Rect drawingRect;
    if (!ConvertFromJsRect(env, argv[0], drawingRect)) {
        ROSEN_LOGE("JsPath::OnAddArc Argv[0] is invalid");
        return NapiGetUndefined(env);
    }
    double startDeg = 0.0;
    double sweepDeg = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ONE], startDeg) && ConvertFromJsValue(env, argv[ARGC_TWO], sweepDeg))) {
        ROSEN_LOGE("JsPath::OnAddArc Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->AddArc(drawingRect, startDeg, sweepDeg);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnAddRect(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddRect path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsPath::OnAddRect Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Drawing::Rect drawingRect;
    if (!ConvertFromJsRect(env, argv[0], drawingRect)) {
        ROSEN_LOGE("JsPath::OnAddOval Argv[0] is invalid");
        return NapiGetUndefined(env);
    }
    uint32_t jsDirection = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], jsDirection)) {
        ROSEN_LOGE("JsPath::OnAddRect Argv[1] is invalid");
        return NapiGetUndefined(env);
    }

    m_path->AddRect(drawingRect, PathDirection(jsDirection));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnAddRoundRect(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddRoundRect path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsPath::OnAddRoundRect Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsRoundRect* jsRoundRect = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsRoundRect));
    if (jsRoundRect == nullptr) {
        ROSEN_LOGE("JsPath::OnAddRoundRect jsRoundRect is nullptr");
        return NapiGetUndefined(env);
    }

    if (jsRoundRect->GetRoundRect() == nullptr) {
        ROSEN_LOGE("JsPath::OnAddRoundRect roundRect is nullptr");
        return NapiGetUndefined(env);
    }

    uint32_t jsDirection = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], jsDirection)) {
        ROSEN_LOGE("JsPath::OnAddRoundRect Argv[1] is invalid");
        return NapiGetUndefined(env);
    }
    m_path->AddRoundRect(*jsRoundRect->GetRoundRect(), PathDirection(jsDirection));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnAddPath(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddPath path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsPath::OnAddPath Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsPath* jsPath = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsPath));
    if (jsPath == nullptr) {
        ROSEN_LOGE("JsPath::OnAddPath jsPath is nullptr");
        return NapiGetUndefined(env);
    }

    if (jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsPath::OnAddPath path is nullptr");
        return NapiGetUndefined(env);
    }

    JsMatrix* jsMatrix = nullptr;
    napi_unwrap(env, argv[1], reinterpret_cast<void**>(&jsMatrix));
    if (jsMatrix == nullptr) {
        ROSEN_LOGE("JsPath::OnAddPath jsMatrix is nullptr");
        return NapiGetUndefined(env);
    }

    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsPath::OnAddPath Matrix is nullptr");
        return NapiGetUndefined(env);
    }

    m_path->AddPath(*jsPath->GetPath(), *jsMatrix->GetMatrix());
    return NapiGetUndefined(env);
}

napi_value JsPath::OnTransform(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnTransform path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPath::OnTransform Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsMatrix* jsMatrix = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsMatrix));
    if (jsMatrix == nullptr) {
        ROSEN_LOGE("JsPath::OnTransform jsMatrix is nullptr");
        return NapiGetUndefined(env);
    }

    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsPath::OnTransform Matrix is nullptr");
        return NapiGetUndefined(env);
    }

    m_path->Transform(*jsMatrix->GetMatrix());
    return NapiGetUndefined(env);
}

napi_value JsPath::OnContains(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnContains path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsPath::OnContains Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double x = 0.0;
    double y = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], x) && ConvertFromJsValue(env, argv[1], y))) {
        ROSEN_LOGE("JsPath::OnContains Argv is invalid");
        return NapiGetUndefined(env);
    }

    return CreateJsValue(env, m_path->Contains(x, y));
}

napi_value JsPath::OnSetFillType(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnSetFillType path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPath::OnSetFillType Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    uint32_t jsFillType = 0;
    if (!ConvertFromJsValue(env, argv[0], jsFillType)) {
        ROSEN_LOGE("JsPath::OnSetFillType Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    m_path->SetFillStyle(PathFillType(jsFillType));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnGetBounds(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnGetBounds path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    auto bounds = m_path->GetBounds();
    std::shared_ptr<Rect> rect = std::make_shared<Rect>(bounds.GetLeft(),
                                                        bounds.GetTop(), bounds.GetRight(), bounds.GetBottom());
    if (!rect) {
        ROSEN_LOGE("JsPath::OnGetBounds return value is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return GetRectAndConvertToJsValue(env, rect);
}

napi_value JsPath::OnClose(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnClose path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    m_path->Close();
    return NapiGetUndefined(env);
}

napi_value JsPath::OnReset(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnReset path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    m_path->Reset();
    return NapiGetUndefined(env);
}

Path* JsPath::GetPath()
{
    return m_path;
}
} // namespace Drawing
} // namespace OHOS::Rosen