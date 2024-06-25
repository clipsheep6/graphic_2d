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

#include "js_shader_effect.h"
#include "../matrix_napi/js_matrix.h"
#include "../js_drawing_utils.h"
#include "native_value.h"
#include "utils/point.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsShaderEffect::constructor_ = nullptr;
const std::string CLASS_NAME = "ShaderEffect";
napi_value JsShaderEffect::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createLinearGradient", JsShaderEffect::CreateLinearGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createRadialGradient", JsShaderEffect::CreateRadialGradient),
        DECLARE_NAPI_STATIC_FUNCTION("createSweepGradient", JsShaderEffect::CreateSweepGradient),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init Failed to define shaderEffect class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Init Failed to set constructor");
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

    JsShaderEffect *jsShaderEffect = new(std::nothrow) JsShaderEffect(std::make_shared<ShaderEffect>());

    status = napi_wrap(env, jsThis, jsShaderEffect,
                       JsShaderEffect::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsShaderEffect;
        ROSEN_LOGE("JsShaderEffect::Constructor Failed to wrap native instance");
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

napi_value JsShaderEffect::CreateLinearGradient(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FIVE) {
        ROSEN_LOGE("JsShaderEffect::CreateLinearGradient Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value startTempValue = nullptr;
    double startX = 0.0;
    double startY = 0.0;

    napi_get_named_property(env, argv[0], "x", &startTempValue);
    napi_get_value_double(env, startTempValue, &startX);
    napi_get_named_property(env, argv[0], "y", &startTempValue);
    napi_get_value_double(env, startTempValue, &startY);

    Drawing::Point startPt = Drawing::Point(startX, startY);

    napi_value endTempValue = nullptr;
    double endX = 0.0f;
    double endY = 0.0f;
    napi_get_named_property(env, argv[ARGC_ONE], "x", &endTempValue);
    napi_get_value_double(env, endTempValue, &endX);
    napi_get_named_property(env, argv[ARGC_ONE], "y", &endTempValue);
    napi_get_value_double(env, endTempValue, &endY);

    Drawing::Point endPt = Drawing::Point(endX, endY);


    napi_value tempValue = nullptr;
    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    napi_get_named_property(env, argv[ARGC_TWO], "alpha", &tempValue);
    bool isAlphaOk = ConvertClampFromJsValue(env, tempValue, alpha, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_TWO], "red", &tempValue);
    bool isRedOk = ConvertClampFromJsValue(env, tempValue, red, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_TWO], "green", &tempValue);
    bool isGreenOk = ConvertClampFromJsValue(env, tempValue, green, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_TWO], "blue", &tempValue);
    bool isBlueOk = ConvertClampFromJsValue(env, tempValue, blue, 0, Color::RGB_MAX);
    if (!(isAlphaOk && isRedOk && isGreenOk && isBlueOk)) {
        ROSEN_LOGE("JsShaderEffect::CreateLinearGradient Argv[2] is invalid");
        return NapiGetUndefined(env);
    }

    ColorQuad color = Color::ColorQuadSetARGB(alpha, red, green, blue);
    const std::vector<ColorQuad> colors{color};

    napi_value arrayPos = argv[ARGC_THREE];
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
    if (!ConvertFromJsValue(env, argv[ARGC_FOUR], jsMode)){
        ROSEN_LOGE("JsShaderEffect::CreateLinearGradient Argv[4] is invalid");
        return NapiGetUndefined(env);
    }

    std::shared_ptr<ShaderEffect> linearGradient = ShaderEffect::CreateLinearGradient(startPt, endPt, colors, pos, TileMode(jsMode));
    return JsShaderEffect::Create(env, linearGradient);

}

napi_value JsShaderEffect::CreateRadialGradient(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FIVE) {
        ROSEN_LOGE("JsShaderEffect::CreateRadialGradient Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value tempValue = nullptr;
    double startX = 0.0f;
    double startY = 0.0f;
    napi_get_named_property(env, argv[0], "x", &tempValue);
    napi_get_value_double(env, tempValue, &startX);
    napi_get_named_property(env, argv[0], "y", &tempValue);
    napi_get_value_double(env, tempValue, &startY);
    Drawing::Point centerPt = Drawing::Point(startX,startY);

    double radius = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], radius)) {
        ROSEN_LOGE("JsShaderEffect::CreateRadialGradient Argv[1] is invalid");
        return NapiGetUndefined(env);
    }

    napi_value temp = nullptr;
    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    napi_get_named_property(env, argv[ARGC_TWO], "alpha", &temp);
    bool isAlphaOk = ConvertClampFromJsValue(env, temp, alpha, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_TWO], "red", &temp);
    bool isRedOk = ConvertClampFromJsValue(env, temp, red, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_TWO], "green", &temp);
    bool isGreenOk = ConvertClampFromJsValue(env, temp, green, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_TWO], "blue", &temp);
    bool isBlueOk = ConvertClampFromJsValue(env, temp, blue, 0, Color::RGB_MAX);
    if (!(isAlphaOk && isRedOk && isGreenOk && isBlueOk)) {
        ROSEN_LOGE("JsShaderEffect::CreateRadialGradient Argv[2] is invalid");
        return NapiGetUndefined(env);
    }

    ColorQuad color = Color::ColorQuadSetARGB(alpha, red, green, blue);
    const std::vector<ColorQuad> colors{color};

    napi_value arrayPos = argv[ARGC_THREE];
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
    if (!ConvertFromJsValue(env, argv[ARGC_FOUR], jsMode)){
        ROSEN_LOGE("JsShaderEffect::CreateRadialGradient Argv[4] is invalid");
        return NapiGetUndefined(env);
    }

    std::shared_ptr<ShaderEffect> radialGradient = ShaderEffect::CreateRadialGradient(centerPt, radius, colors, pos, TileMode(jsMode));
    return JsShaderEffect::Create(env, radialGradient);

}

napi_value JsShaderEffect::CreateSweepGradient(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_SEVEN;
    napi_value argv[ARGC_SEVEN] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_SEVEN) {
        ROSEN_LOGE("JsShaderEffect::CreateSweepGradient Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value tempValue = nullptr;
    double centerX = 0.0f;
    double centerY = 0.0f;
    napi_get_named_property(env, argv[0], "x", &tempValue);
    napi_get_value_double(env, tempValue, &centerX);
    napi_get_named_property(env, argv[0], "y", &tempValue);
    napi_get_value_double(env, tempValue, &centerY);
    Drawing::Point centerPt = Drawing::Point(centerX, centerY);

    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    napi_get_named_property(env, argv[ARGC_ONE], "alpha", &tempValue);
    bool isAlphaOk = ConvertClampFromJsValue(env, tempValue, alpha, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_ONE], "red", &tempValue);
    bool isRedOk = ConvertClampFromJsValue(env, tempValue, red, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_ONE], "green", &tempValue);
    bool isGreenOk = ConvertClampFromJsValue(env, tempValue, green, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[ARGC_ONE], "blue", &tempValue);
    bool isBlueOk = ConvertClampFromJsValue(env, tempValue, blue, 0, Color::RGB_MAX);
    if (!(isAlphaOk && isRedOk && isGreenOk && isBlueOk)) {
        ROSEN_LOGE("JsShaderEffect::CreateSweepGradient Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    ColorQuad color = Color::ColorQuadSetARGB(alpha, red, green, blue);
    const std::vector<ColorQuad> colors{color};

    napi_value arrayPos = argv[ARGC_TWO];
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
    if (!ConvertFromJsValue(env, argv[ARGC_THREE], jsMode)){
        ROSEN_LOGE("JsShaderEffect::CreateLinearGradient Argv[3] is invalid");
        return NapiGetUndefined(env);
    }

    double startAngle;
    if (!ConvertFromJsValue(env, argv[ARGC_FOUR], startAngle)) {
        ROSEN_LOGE("JsShaderEffect::CreateSweepGradient Argv[4] is invalid");
        return NapiGetUndefined(env);
    }

    double endAngle;
    if (!ConvertFromJsValue(env, argv[ARGC_FIVE], endAngle)) {
        ROSEN_LOGE("JsShaderEffect::CreateSweepGradient Argv[5] is invalid");
        return NapiGetUndefined(env);
    }

    JsMatrix* matrix = nullptr;
    napi_unwrap(env,argv[ARGC_SIX], reinterpret_cast<void**>(&matrix));
    if(matrix == nullptr){
        return nullptr;
    }

    std::shared_ptr<ShaderEffect> SweepGradient = ShaderEffect::CreateSweepGradient(centerPt, colors, pos, TileMode(jsMode), startAngle, endAngle, matrix->GetMatrix());
    return JsShaderEffect::Create(env, SweepGradient);

}

napi_value JsShaderEffect::Create(napi_env env, const std::shared_ptr<ShaderEffect> gradient)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || gradient == nullptr) {
        ROSEN_LOGE("[NAPI]Object or JsShaderEffect is null!");
        return NapiGetUndefined(env);
    }

    std::unique_ptr<JsShaderEffect> jsGradient = std::make_unique<JsShaderEffect>(gradient);
    napi_wrap(env, objValue, jsGradient.release(), JsShaderEffect::Finalizer, nullptr, nullptr);

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
