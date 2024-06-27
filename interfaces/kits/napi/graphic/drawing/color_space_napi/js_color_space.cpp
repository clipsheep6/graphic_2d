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

#include "js_color_space.h"

#include "native_value.h"
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
const std::string CLASS_NAME = "ColorSpace";
thread_local napi_ref JsColorSpace::constructor_ = nullptr;
napi_value JsColorSpace::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createSrgb", JsColorSpace::CreateSrgb),
        DECLARE_NAPI_STATIC_FUNCTION("createSrgbLinear", JsColorSpace::CreateSrgbLinear),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsColorSpace::Init failed to define ColorSpace class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsColorSpace::Init failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsColorSpace::Init failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        ROSEN_LOGE("JsColorSpace::Init failed to define static function");
        return nullptr;
    }
    return exportObj;
}

void JsColorSpace::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsColorSpace>(static_cast<JsColorSpace*>(data));
}

JsColorSpace::~JsColorSpace()
{
    m_ColorSpace = nullptr;
}

napi_value JsColorSpace::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsColorSpace::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsColorSpace *jsColorSpace = new(std::nothrow) JsColorSpace();
    if (!jsColorSpace) {
        ROSEN_LOGE("JsColorSpace::Constructor failed to create JsColorSpace");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsColorSpace, JsColorSpace::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsColorSpace;
        ROSEN_LOGE("JsColorSpace::Constructor failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsColorSpace::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsColorSpace *napi = reinterpret_cast<JsColorSpace *>(nativeObject);
        delete napi;
    }
}

napi_value JsColorSpace::CreateSrgb(napi_env env, napi_callback_info info)
{
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    return JsColorSpace::Create(env, colorSpace);
}

napi_value JsColorSpace::CreateSrgbLinear(napi_env env, napi_callback_info info)
{
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGBLinear();
    return JsColorSpace::Create(env, colorSpace);
}

napi_value JsColorSpace::Create(napi_env env, const std::shared_ptr<ColorSpace> colorSpace)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || colorSpace == nullptr) {
        ROSEN_LOGE("JsColorSpace::Create [NAPI]Object or JsColorSpace is null!");
        return nullptr;
    }

    std::unique_ptr<JsColorSpace> jsColorSpace = std::make_unique<JsColorSpace>(colorSpace);
    napi_wrap(env, objValue, jsColorSpace.release(), JsColorSpace::Finalizer, nullptr, nullptr);

    if (objValue == nullptr) {
        ROSEN_LOGE("JsColorSpace::Create [NAPI]objValue is null!");
        return nullptr;
    }
    return objValue;
}

std::shared_ptr<ColorSpace> JsColorSpace::GetColorSpace()
{
    return m_ColorSpace;
}
} // namespace Drawing
} // namespace OHOS::Rosen
