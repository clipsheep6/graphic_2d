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
        DECLARE_NAPI_FUNCTION("conicTo", JsPath::ConicTo),
        DECLARE_NAPI_FUNCTION("cubicTo", JsPath::CubicTo),
        DECLARE_NAPI_FUNCTION("rMoveTo", JsPath::RMoveTo),
        DECLARE_NAPI_FUNCTION("rLineTo", JsPath::RLineTo),
        DECLARE_NAPI_FUNCTION("rQuadTo", JsPath::RQuadTo),
        DECLARE_NAPI_FUNCTION("rConicTo", JsPath::RConicTo),
        DECLARE_NAPI_FUNCTION("rCubicTo", JsPath::RCubicTo),
        DECLARE_NAPI_FUNCTION("addPolygon", JsPath::AddPolygon),
        DECLARE_NAPI_FUNCTION("close", JsPath::Close),
        DECLARE_NAPI_FUNCTION("offset", JsPath::Offset),
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
    if (!jsPath) {
        ROSEN_LOGE("Failed to create JsPath");
        return nullptr;
    }

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
    if (m_path != nullptr) {
        delete m_path;
        m_path = nullptr;
    }
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

napi_value JsPath::ConicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnConicTo(env, info) : nullptr;
}

napi_value JsPath::CubicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnCubicTo(env, info) : nullptr;
}

napi_value JsPath::RMoveTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRMoveTo(env, info) : nullptr;
}

napi_value JsPath::RLineTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRLineTo(env, info) : nullptr;
}

napi_value JsPath::RQuadTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRQuadTo(env, info) : nullptr;
}

napi_value JsPath::RConicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRConicTo(env, info) : nullptr;
}

napi_value JsPath::RCubicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRCubicTo(env, info) : nullptr;
}

napi_value JsPath::AddPolygon(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddPolygon(env, info) : nullptr;
}

napi_value JsPath::Close(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnClose(env, info) : nullptr;
}

napi_value JsPath::Offset(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnOffset(env, info) : nullptr;
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

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);

    JS_CALL_DRAWING_FUNC(m_path->MoveTo(x, y));
    return nullptr;
}

napi_value JsPath::OnLineTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnLineTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);

    JS_CALL_DRAWING_FUNC(m_path->LineTo(x, y));
    return nullptr;
}

napi_value JsPath::OnArcTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnArcTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_SIX] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_SIX);

    double x1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x1);
    double y1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y1);
    double x2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, x2);
    double y2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, y2);
    double startDeg = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, startDeg);
    double sweepDeg = 0.0;
    GET_DOUBLE_PARAM(ARGC_FIVE, sweepDeg);

    JS_CALL_DRAWING_FUNC(m_path->ArcTo(x1, y1, x2, y2, startDeg, sweepDeg));
    return nullptr;
}

napi_value JsPath::OnQuadTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnQuadTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double ctrlPtX = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, ctrlPtX);
    double ctrlPtY = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, ctrlPtY);
    double endPtX = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, endPtX);
    double endPtY = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, endPtY);

    JS_CALL_DRAWING_FUNC(m_path->QuadTo(ctrlPtX, ctrlPtY, endPtX, endPtY));
    return nullptr;
}

napi_value JsPath::OnConicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnConicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FIVE) {
        ROSEN_LOGE("JsPath::OnConicTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double ctrlPtX = 0.0;
    double ctrlPtY = 0.0;
    double endPtX = 0.0;
    double endPtY = 0.0;
    double weight = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], ctrlPtX) && ConvertFromJsValue(env, argv[ARGC_ONE], ctrlPtY) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], endPtX) && ConvertFromJsValue(env, argv[ARGC_THREE], endPtY) &&
        ConvertFromJsValue(env, argv[ARGC_FOUR], weight))) {
        ROSEN_LOGE("JsPath::OnConicTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->ConicTo(ctrlPtX, ctrlPtY, endPtX, endPtY, weight);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnCubicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnCubicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_SIX] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_SIX);

    double px1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, px1);
    double py1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, py1);
    double px2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px2);
    double py2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py2);
    double px3 = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, px3);
    double py3 = 0.0;
    GET_DOUBLE_PARAM(ARGC_FIVE, py3);

    JS_CALL_DRAWING_FUNC(m_path->CubicTo(Point(px1, py1), Point(px2, py2), Point(px3, py3)));
    return nullptr;
}

napi_value JsPath::OnRMoveTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRMoveTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsPath::OnRMoveTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double dx = 0.0;
    double dy = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], dx) && ConvertFromJsValue(env, argv[ARGC_ONE], dy))) {
        ROSEN_LOGE("JsPath::OnRMoveTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->RMoveTo(dx, dy);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnRLineTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRLineTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsPath::OnRLineTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double dx = 0.0;
    double dy = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], dx) && ConvertFromJsValue(env, argv[ARGC_ONE], dy))) {
        ROSEN_LOGE("JsPath::OnRLineTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->RLineTo(dx, dy);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnRQuadTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRQuadTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FOUR) {
        ROSEN_LOGE("JsPath::OnRQuadTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double dx1 = 0.0;
    double dy1 = 0.0;
    double dx2 = 0.0;
    double dy2 = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], dx1) && ConvertFromJsValue(env, argv[ARGC_ONE], dy1) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], dx2) && ConvertFromJsValue(env, argv[ARGC_THREE], dy2))) {
        ROSEN_LOGE("JsPath::OnRQuadTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->RQuadTo(dx1, dy1, dx2, dy2);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnRConicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRConicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FIVE) {
        ROSEN_LOGE("JsPath::OnRConicTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double ctrlPtX = 0.0;
    double ctrlPtY = 0.0;
    double endPtX = 0.0;
    double endPtY = 0.0;
    double weight = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], ctrlPtX) && ConvertFromJsValue(env, argv[ARGC_ONE], ctrlPtY) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], endPtX) && ConvertFromJsValue(env, argv[ARGC_THREE], endPtY) &&
        ConvertFromJsValue(env, argv[ARGC_FOUR], weight))) {
        ROSEN_LOGE("JsPath::OnRConicTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->RConicTo(ctrlPtX, ctrlPtY, endPtX, endPtY, weight);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnRCubicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRCubicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_SIX;
    napi_value argv[ARGC_SIX] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_SIX) {
        ROSEN_LOGE("JsPath::OnRCubicTo Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double px1 = 0.0;
    double py1 = 0.0;
    double px2 = 0.0;
    double py2 = 0.0;
    double px3 = 0.0;
    double py3 = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], px1) && ConvertFromJsValue(env, argv[ARGC_ONE], py1) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], px2) && ConvertFromJsValue(env, argv[ARGC_THREE], py2) &&
        ConvertFromJsValue(env, argv[ARGC_FOUR], px3) && ConvertFromJsValue(env, argv[ARGC_FIVE], py3))) {
        ROSEN_LOGE("JsPath::OnRCubicTo Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->RCubicTo(px1, py1, px2, py2, px3, py3);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnAddPolygon(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO || argc > ARGC_THREE) {
        ROSEN_LOGE("JsTextBlob::OnAddPolygon Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value array = argv[ARGC_ZERO];
    uint32_t size = 0;
    std::vector<Point> points;
    napi_get_array_length(env, array, &size);
    for (uint32_t i = 0; i < size; i++) {
        napi_value tempRunBuffer = nullptr;
        napi_get_element(env, array, i, &tempRunBuffer);

        napi_value tempValue = nullptr;
        Point point = Point();
        double positionX = 0.0;
        double positionY = 0.0;
        napi_get_named_property(env, tempRunBuffer, "x", &tempValue);
        bool isPositionXOk = ConvertFromJsValue(env, tempValue, positionX);
        napi_get_named_property(env, tempRunBuffer, "y", &tempValue);
        bool isPositionYOk = ConvertFromJsValue(env, tempValue, positionY);
        if (!(isPositionXOk && isPositionYOk)) {
            ROSEN_LOGE("JsPath::OnAddPolygon Argv is invalid");
            return napi_value();
        }

        point.SetX(positionX);
        point.SetY(positionY);
        points.push_back(point);
    }

    uint32_t count = 0;
    bool close = false;
    if (!(!points.empty() && ConvertFromJsValue(env, argv[ARGC_ONE], count) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], close))) {
        ROSEN_LOGE("JsPath::OnAddPolygon Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_path->AddPoly(points, count, close);
    return napi_value();
}

napi_value JsPath::OnClose(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnClose path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JS_CALL_DRAWING_FUNC(m_path->Close());
    return nullptr;
}

napi_value JsPath::OnOffset(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnOffset path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsPath::OnOffset Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double dx = 0.0;
    double dy = 0.0;
    JsPath* jsPath = nullptr;
    napi_unwrap(env, argv[ARGC_ZERO], reinterpret_cast<void**>(&jsPath));
    if (!(jsPath != nullptr && jsPath->GetPath() != nullptr && ConvertFromJsValue(env, argv[ARGC_ONE], dx) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], dy))) {
        ROSEN_LOGE("JsPath::OnOffset Argv is invalid: %{public}zu", argc);
        return NapiGetUndefined(env);
    }

    m_path->Offset(jsPath->GetPath(), dx, dy);
    return NapiGetUndefined(env);
}

napi_value JsPath::OnReset(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnReset path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JS_CALL_DRAWING_FUNC(m_path->Reset());
    return nullptr;
}

Path* JsPath::GetPath()
{
    return m_path;
}
} // namespace Drawing
} // namespace OHOS::Rosen