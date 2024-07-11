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

#include "js_gpu_context.h"
#include "js_drawing_utils.h"
#include "native_value.h"

#include "surface_napi/js_surface.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsGpuContext::constructor_ = nullptr;
const std::string CLASS_NAME = "GpuContext";
napi_value JsGpuContext::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("createSurface", JsGpuContext::CreateSurface)
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsGpuContext::Init Failed to define GPUCanvas class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsGpuContext::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsGpuContext::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsGpuContext::Constructor(napi_env env, napi_callback_info info)
{
    napi_value jsThis = nullptr;
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsGpuContext::Constructor failed to napi_get_cb_info");
        return nullptr;
    }
    if (argc != ARGC_ONE) {
        ROSEN_LOGE("JsGpuContext::Constructor incorrect number of params");
        return nullptr;
    }

    napi_value tempValue = nullptr;
    status = napi_get_named_property(env, argv[ARGC_ZERO], "allowPathMaskCaching", &tempValue);
    if (status != napi_ok) {
        ROSEN_LOGE("JsGpuContext::Constructor Cannot get allowPathMaskCaching");
        return nullptr;
    }
    bool allowPathMaskCaching;
    status = napi_get_value_bool(env, tempValue, &allowPathMaskCaching);
    if (status != napi_ok) {
        ROSEN_LOGE("JsGpuContext::Constructor Cannot get allowPathMaskCaching");
        return nullptr;
    }

    JsGpuContext* jsGpuContext = new(std::nothrow) JsGpuContext();
    if (jsGpuContext == nullptr) {
        ROSEN_LOGE("JsGpuContext::Constructor Failed to create JsGpuContext");
        return nullptr;
    }

    GPUContextOptions contextOps;
    contextOps.SetAllowPathMaskCaching(allowPathMaskCaching);
    bool isSuccess = jsGpuContext->GetGpuContext()->BuildFromGL(contextOps);
    if (!isSuccess) {
        delete jsGpuContext;
        ROSEN_LOGE("JsGpuContext::Constructor BuildFromGL() failed");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsGpuContext, JsGpuContext::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsGpuContext;
        ROSEN_LOGE("JsGpuContext::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsGpuContext::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsGpuContext* napi = reinterpret_cast<JsGpuContext*>(nativeObject);
        delete napi;
    }
}

napi_value JsGpuContext::CreateSurface(napi_env env, napi_callback_info info)
{
    JsGpuContext* me = CheckParamsAndGetThis<JsGpuContext>(env, info);
    return (me != nullptr) ? me->OnCreateSurface(env, info) : nullptr;
}

napi_value JsGpuContext::OnCreateSurface(napi_env env, napi_callback_info info)
{
    if (gpuContext_ == nullptr) {
        ROSEN_LOGE("JsGpuContext::OnCreateSurface context is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    bool budgeted;
    GET_BOOLEAN_PARAM(ARGC_ZERO, budgeted);

    Drawing::ImageInfo imageInfo;
    if (!ConvertFromJsImageInfo(env, argv[ARGC_ONE], imageInfo)) {
        ROSEN_LOGE("JsGpuContext::OnCreateSurface argv[1] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid [1] param.");
    }

    std::shared_ptr<Surface> surface =
        Surface::MakeRenderTarget(gpuContext_, budgeted, imageInfo);
    if (surface == nullptr) {
        ROSEN_LOGE("JsGpuContext::OnCreateSurface Failed to create Surface");
        return nullptr;
    }

    return JsSurface::CreateJsSurface(env, surface);
}

JsGpuContext::JsGpuContext()
{
    gpuContext_ = new GPUContext();
}

JsGpuContext::~JsGpuContext()
{
    delete gpuContext_;
}

GPUContext* JsGpuContext::GetGpuContext()
{
    return gpuContext_;
}
} // namespace Drawing
} // namespace OHOS::Rosen