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

#include "js_surface.h"
#include "../js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsSurface::constructor_ = nullptr;
const std::string CLASS_NAME = "Surface";
napi_value JsSurface::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getImageSnapshot", JsSurface::GetImageSnapshot),
        DECLARE_NAPI_FUNCTION("build", JsSurface::Build),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Surface class");
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

napi_value JsSurface::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    Surface *surface = new Surface();
    JsSurface *jsSurface = new(std::nothrow) JsSurface(surface);

    status = napi_wrap(env, jsThis, jsSurface,
                       JsSurface::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsSurface;
        ROSEN_LOGE("JsSurface::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsSurface::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsSurface *napi = reinterpret_cast<JsSurface *>(nativeObject);
        delete napi;
    }
}

JsSurface::~JsSurface()
{
    m_surface = nullptr;
}

napi_value JsSurface::GetImageSnapshot(napi_env env, napi_callback_info info)
{
    JsSurface* me = CheckParamsAndGetThis<JsSurface>(env, info);
    return (me != nullptr) ? me->OnGetImageSnapshot(env, info) : nullptr;
}

napi_value JsSurface::Build(napi_env env, napi_callback_info info)
{
    JsSurface* me = CheckParamsAndGetThis<JsSurface>(env, info);
    return (me != nullptr) ? me->OnBuild(env, info) : nullptr;
}

napi_value JsSurface::OnGetImageSnapshot(napi_env env, napi_callback_info info)
{
    if (m_surface == nullptr) {
        ROSEN_LOGE("[NAPI]surface is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("Drawing_napi: OnDrawRect Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("Drawing_napi: OnDrawRect Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    m_surface->GetImageSnapshot();

    return NapiGetUndefined(env);
}

napi_value JsSurface::OnBuild(napi_env env, napi_callback_info info)
{
    if (m_surface == nullptr) {
        ROSEN_LOGE("[NAPI]surface is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    //..........................


    return NapiGetUndefined(env);
}


Surface* JsSurface::GetSurface()
{
    return m_surface;
}
} // namespace Drawing
} // namespace OHOS::Rosen