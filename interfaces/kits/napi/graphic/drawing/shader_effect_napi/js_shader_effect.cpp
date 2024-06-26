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
#ifdef ROSEN_OHOS
#include "pixel_map_napi.h"
#endif
#include "js_drawing_utils.h"
#include "matrix_napi/js_matrix.h"
#include "sampling_options_napi/js_sampling_options.h"
#include "native_value.h"

#include <cstdint>

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsShaderEffect::constructor_ = nullptr;
const std::string CLASS_NAME = "ShaderEffect";
napi_value JsShaderEffect::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createImageShader", JsShaderEffect::CreateImageShader),
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

napi_value JsShaderEffect::CreateImageShader(napi_env env, napi_callback_info info)
{
#ifdef ROSEN_OHOS
    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_THREE, ARGC_FIVE);

    Media::PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi);

    if (pixelMapNapi->GetPixelNapiInner() == nullptr) {
        ROSEN_LOGE("JsShaderEffect::CreateImageShader pixelmap GetPixelNapiInner is nullptr");
        return nullptr;
    }

    auto image = ExtractDrawingImage(pixelMapNapi->GetPixelNapiInner());
    if (image == nullptr) {
        ROSEN_LOGE("JsShaderEffect::CreateImageShader image is nullptr");
        return nullptr;
    }

    int32_t tileX = 0;
    GET_ENUM_PARAM(ARGC_ONE, tileX, 0, static_cast<int32_t>(TileMode::DECAL));

    int32_t tileY = 0;
    GET_ENUM_PARAM(ARGC_TWO, tileY, 0, static_cast<int32_t>(TileMode::DECAL));

    auto jsTileX = static_cast<TileMode>(tileX);
    auto jsTileY = static_cast<TileMode>(tileY);

    SamplingOptions samplingOptions;
    if (argc == ARGC_FOUR) {
        JsSamplingOptions* jsSamplingOptions = nullptr;
        napi_status status = napi_unwrap(env, argv[ARGC_THREE], reinterpret_cast<void**>(&jsSamplingOptions));
        if (status == napi_ok && jsSamplingOptions != nullptr) {
            samplingOptions = *jsSamplingOptions->GetSamplingOptions();
            ROSEN_LOGI("JsShaderEffect::CreateImageShader samplingOptions is nullptr, use default value");
        }
    }

    Matrix matrix;
    if (argc == ARGC_FIVE) {
        JsMatrix* jsMatrix = nullptr;
        napi_status status = napi_unwrap(env, argv[ARGC_FOUR], reinterpret_cast<void**>(&jsMatrix));
        if (status == napi_ok && jsMatrix != nullptr) {
            matrix = *jsMatrix->GetMatrix();
        } else {
            ROSEN_LOGI("JsShaderEffect::CreateImageShader matrix is nullptr, use empty matrix");
        }
    }

    auto imageShader = ShaderEffect::CreateImageShader(*image, jsTileX, jsTileY, samplingOptions, matrix);
    return JsShaderEffect::Create(env, imageShader);
#endif
    return nullptr;
}

napi_value JsShaderEffect::Create(napi_env env, const std::shared_ptr<ShaderEffect> effect)
{
    napi_value objValue = nullptr;
    napi_status status = napi_create_object(env, &objValue);
    if (status != napi_ok || objValue == nullptr || effect == nullptr) {
        ROSEN_LOGE("JsShaderEffect::Create napi_create_object failed or effect is null!");
        return NapiGetUndefined(env);
    }

    std::unique_ptr<JsShaderEffect> jsShader = std::make_unique<JsShaderEffect>(effect);
    status = napi_wrap(env, objValue, jsShader.release(), JsShaderEffect::Finalizer, nullptr, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsShaderEffect::Create napi_wrap failed!");
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
