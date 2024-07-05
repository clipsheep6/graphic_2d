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

#include "js_shader_effect.h"
#include "matrix_napi/js_matrix.h"

#include <cstdint>
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsShaderEffect::constructor_ = nullptr;
const std::string CLASS_NAME = "ShaderEffect";
napi_value JsShaderEffect::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createColorShader", JsShaderEffect::CreateColorShader),
        DECLARE_NAPI_STATIC_FUNCTION("createLinearGradient", JsShaderEffect::CreateLinearGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createRadialGradient", JsShaderEffect::CreateRadialGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createSweepGradient", JsShaderEffect::CreateSweepGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createTwoPointConical", JsShaderEffect::CreateTwoPointConical),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init failed to define shadowLayer class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsShaderEffect::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsShaderEffect *jsShaderEffect = new(std::nothrow) JsShaderEffect();
    if (jsShaderEffect == nullptr) {
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsShaderEffect, JsShaderEffect::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsShaderEffect;
        ROSEN_LOGE("JsShaderEffect::Constructor failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsShaderEffect::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsShaderEffect* napi = reinterpret_cast<JsShaderEffect*>(nativeObject);
        delete napi;
    }
}

JsShaderEffect::~JsShaderEffect()
{
    m_shaderEffect = nullptr;
}


static const Matrix* CastToMatrix(const JsMatrix* cMatrix)
{
    return reinterpret_cast<const Matrix*>(cMatrix);
}

// static createColorShader(color: number): ShaderEffect;
napi_value JsShaderEffect::CreateColorShader(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    uint32_t color = 0;
    GET_UINT32_PARAM(ARGC_ZERO, color);

    std::shared_ptr<ShaderEffect> colorShader = ShaderEffect::CreateColorShader(color);
    return JsShaderEffect::Create(env, colorShader);
}

napi_value JsShaderEffect::CreateLinearGradient(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_NINE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_NINE);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);
    Drawing::Point startPt = Drawing::Point(x, y);

    double xEnd = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, xEnd);
    double yEnd = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, yEnd);
    Drawing::Point endPt = Drawing::Point(xEnd, yEnd);

    int32_t argb[ARGC_FOUR] = {0};
    if (!ConvertFromJsColor(env, argv[ARGC_FOUR], argb, ARGC_FOUR)) {
        ROSEN_LOGE("CreateLinearGradient::SetColor Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
    }
    ColorQuad color = Color::ColorQuadSetARGB(argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]);
    const std::vector<ColorQuad> colors{color};

    napi_value arrayPos = argv[ARGC_FIVE];
    uint32_t size = 0;
    napi_get_array_length(env, arrayPos, &size);
    std::vector<scalar> pos;
    for (uint32_t i = 0; i < size; i++) {
        napi_value element;
        napi_get_element(env, arrayPos, i, &element);
        double value = 0;
        napi_get_value_double(env, element, &value);
        pos.emplace_back(value);
    }

    uint32_t jsMode = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_SEVEN], jsMode)){
        ROSEN_LOGE("JsShaderEffect::CreateLinearGradient Argv[4] is invalid");
        return NapiGetUndefined(env);
    }

    JsMatrix* matrix = nullptr;
    napi_unwrap(env, argv[ARGC_EIGHT], reinterpret_cast<void**>(&matrix));
    if(matrix == nullptr){
        return nullptr;
    }

    std::shared_ptr<ShaderEffect> linearGradient = ShaderEffect::CreateLinearGradient(startPt, endPt, colors, pos,
        TileMode(jsMode), CastToMatrix(matrix));
    return JsShaderEffect::Create(env, linearGradient);
}

napi_value JsShaderEffect::CreateRadialGradient(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_SEVEN] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_SEVEN);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);
    Drawing::Point centerPt = Drawing::Point(x, y);

    double radius = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, radius);

    int32_t argb[ARGC_FOUR] = {0};
    if (!ConvertFromJsColor(env, argv[ARGC_THREE], argb, ARGC_FOUR)) {
        ROSEN_LOGE("CreateRadialGradient::SetColor Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
    }
    ColorQuad color = Color::ColorQuadSetARGB(argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]);
    const std::vector<ColorQuad> colors{color};

    napi_value arrayPos = argv[ARGC_FOUR];
    uint32_t size = 0;
    napi_get_array_length(env, arrayPos, &size);
    std::vector<scalar> pos;
    for (uint32_t i = 0; i < size; i++) {
        napi_value element;
        napi_get_element(env, arrayPos, i, &element);
        double value = 0;
        napi_get_value_double(env, element, &value);
        pos.emplace_back(value);
    }

    uint32_t jsMode = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_FIVE], jsMode)){
        ROSEN_LOGE("JsShaderEffect::CreateRadialGradient Argv[5] is invalid");
        return NapiGetUndefined(env);
    }

    JsMatrix* matrix = nullptr;
    napi_unwrap(env, argv[ARGC_SIX], reinterpret_cast<void**>(&matrix));
    if(matrix == nullptr){
        return nullptr;
    }

    std::shared_ptr<ShaderEffect> radialGradient = ShaderEffect::CreateRadialGradient(centerPt,
        radius, colors, pos, TileMode(jsMode), CastToMatrix(matrix));
    return JsShaderEffect::Create(env, radialGradient);
}

napi_value JsShaderEffect::CreateSweepGradient(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_NINE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_NINE);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);
    Drawing::Point startPt = Drawing::Point(x, y);

    double radius = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, radius);

    int32_t argb[ARGC_FOUR] = {0};
    if (!ConvertFromJsColor(env, argv[ARGC_THREE], argb, ARGC_FOUR)) {
        ROSEN_LOGE("createSweepGradient::SetColor Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
    }
    ColorQuad color = Color::ColorQuadSetARGB(argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]);
    const std::vector<ColorQuad> colors{color};

    napi_value arrayPos = argv[ARGC_FOUR];
    uint32_t size = 0;
    napi_get_array_length(env, arrayPos, &size);
    std::vector<scalar> pos;
    for (uint32_t i = 0; i < size; i++) {
        napi_value element;
        napi_get_element(env, arrayPos, i, &element);
        double value = 0;
        napi_get_value_double(env, element, &value);
        pos.push_back(value);
    }

    uint32_t jsMode = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_FIVE], jsMode)){
        ROSEN_LOGE("JsShaderEffect::CreateLinearGradient Argv[5] is invalid");
        return NapiGetUndefined(env);
    }

    double startAngle = 0.0;
    GET_DOUBLE_PARAM(ARGC_SIX, startAngle);

    double endAngle = 0.0;
    GET_DOUBLE_PARAM(ARGC_SEVEN, endAngle);

    JsMatrix* matrix = nullptr;
    napi_unwrap(env, argv[ARGC_EIGHT], reinterpret_cast<void**>(&matrix));
    if(matrix == nullptr){
        return nullptr;
    }

    std::shared_ptr<ShaderEffect> SweepGradient = ShaderEffect::CreateSweepGradient(startPt, colors, pos,
        TileMode(jsMode), startAngle, endAngle, CastToMatrix(matrix));
    return JsShaderEffect::Create(env, SweepGradient);

}

napi_value JsShaderEffect::CreateTwoPointConical(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TEN] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TEN);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);
    Drawing::Point startPt = Drawing::Point(x, y);

    double startRadius = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, startRadius);

    double xEnd = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, xEnd);
    double yEnd = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, yEnd);
    Drawing::Point endPt = Drawing::Point(xEnd, yEnd);

    double endRadius = 0.0;
    GET_DOUBLE_PARAM(ARGC_FIVE, endRadius);

    int32_t argb[ARGC_FOUR] = {0};
    if (!ConvertFromJsColor(env, argv[ARGC_SIX], argb, ARGC_FOUR)) {
        ROSEN_LOGE("CreateTwoPointConical::SetColor Argv[6] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
    }
    ColorQuad color = Color::ColorQuadSetARGB(argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]);
    const std::vector<ColorQuad> colors{color};

    napi_value arrayPos = argv[ARGC_SEVEN];
    uint32_t size = 0;
    napi_get_array_length(env, arrayPos, &size);
    std::vector<scalar> pos;
    for (uint32_t i = 0; i < size; i++) {
        napi_value element;
        napi_get_element(env, arrayPos, i, &element);
        double value = 0;
        napi_get_value_double(env, element, &value);
        pos.push_back(value);
    }

    uint32_t jsMode = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_EIGHT], jsMode)){
        ROSEN_LOGE("JsShaderEffect::CreateLinearGradient Argv[8] is invalid");
        return NapiGetUndefined(env);
    }

    JsMatrix* matrix = nullptr;
    napi_unwrap(env, argv[ARGC_NINE], reinterpret_cast<void**>(&matrix));
    if(matrix == nullptr){
        return nullptr;
    }

    std::shared_ptr<ShaderEffect> twoPointConical = ShaderEffect::CreateTwoPointConical(startPt,
        startRadius, endPt, endRadius, colors, pos, TileMode(jsMode), CastToMatrix(matrix));
    return JsShaderEffect::Create(env, twoPointConical);
}

napi_value JsShaderEffect::Create(napi_env env, const std::shared_ptr<ShaderEffect> effect)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || effect == nullptr) {
        ROSEN_LOGE("[NAPI]Object or JsShaderEffect is null!");
        return NapiGetUndefined(env);
    }

    std::unique_ptr<JsShaderEffect> jsShader = std::make_unique<JsShaderEffect>(effect);
    napi_wrap(env, objValue, jsShader.release(), JsShaderEffect::Finalizer, nullptr, nullptr);

    if (objValue == nullptr) {
        ROSEN_LOGE("[NAPI]objValue is null!");
        return NapiGetUndefined(env);
    }
    return objValue;

}

void JsShaderEffect::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsShaderEffect>(static_cast<JsShaderEffect*>(data));
}

std::shared_ptr<ShaderEffect> JsShaderEffect::GetShaderEffect()
{
    return m_shaderEffect;
}

} // namespace Drawing
} // namespace OHOS::Rosen
