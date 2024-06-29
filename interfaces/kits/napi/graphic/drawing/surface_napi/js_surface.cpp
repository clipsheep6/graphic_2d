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

#include "js_surface.h"
#include "js_drawing_utils.h"
#include "native_value.h"

#include "canvas_napi/js_canvas.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsSurface::constructor_ = nullptr;
std::shared_ptr<Surface> g_drawingSurface = nullptr;
const std::string CLASS_NAME = "Surface";
napi_value JsSurface::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getCanvas", JsSurface::GetCanvas)
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSurface::Init Failed to define GPUCanvas class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSurface::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSurface::Init Failed to set constructor");
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
        ROSEN_LOGE("JsSurface::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    if (g_drawingSurface == nullptr) {
        ROSEN_LOGE("JsSurface::Constructor g_drawingSurface is nullptr");
        return nullptr;
    }
    std::shared_ptr<Surface> surface = g_drawingSurface;
    g_drawingSurface = nullptr;

    JsSurface* jsSurface = new(std::nothrow) JsSurface(surface);
    if (jsSurface == nullptr) {
        ROSEN_LOGE("JsSurface::Constructor Failed to create JsSurface");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsSurface, JsSurface::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsSurface;
        ROSEN_LOGE("JsSurface::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsSurface::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsSurface* napi = reinterpret_cast<JsSurface*>(nativeObject);
        delete napi;
    }
}

napi_value JsSurface::GetCanvas(napi_env env, napi_callback_info info)
{
    JsSurface* me = CheckParamsAndGetThis<JsSurface>(env, info);
    return (me != nullptr) ? me->OnGetCanvas(env, info) : nullptr;
}

napi_value JsSurface::OnGetCanvas(napi_env env, napi_callback_info info)
{
    if (surface_ == nullptr) {
        ROSEN_LOGE("JsSurface::OnGetCanvas context is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Canvas* canvas = surface_->GetCanvas().get();
    napi_value jsCanvas = JsCanvas::CreateJsCanvas(env, canvas);
    return jsCanvas;
}

napi_value JsSurface::CreateJsSurface(napi_env env, std::shared_ptr<Surface> surface)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;

    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSurface::CreateJsSurface napi_get_reference_value failed");
        return nullptr;
    }
    g_drawingSurface = surface;
    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        ROSEN_LOGE("JsSurface::CreateJsSurface New instance could not be obtained");
        return nullptr;
    }
    return result;
}


JsSurface::~JsSurface()
{
    surface_ = nullptr;
    g_drawingSurface = nullptr;
}

}
}