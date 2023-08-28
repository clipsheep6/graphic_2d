/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "context/webgl_rendering_context_basic_base.h"

#include "context/webgl2_rendering_context.h"
#include "context/webgl_rendering_context.h"
#include "napi/n_class.h"
#include "util/egl_manager.h"
#include "util/log.h"
#include "util/object_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
namespace OHOS {
namespace Rosen {

WebGLRenderingContextBasicBase::~WebGLRenderingContextBasicBase()
{
    if (webGlContextAttributes_ != nullptr) {
        delete webGlContextAttributes_;
    }
    if (eglSurface_ != nullptr) {
        eglDestroySurface(EglManager::GetInstance().GetEGLDisplay(), eglSurface_);
        eglSurface_ = nullptr;
    }
}

void WebGLRenderingContextBasicBase::SetEglWindow(void* window)
{
    LOGI("WebGLRenderingContextBasicBase::SetEglWindow.\n");
    eglWindow_ = reinterpret_cast<NativeWindow*>(window);
}

void WebGLRenderingContextBasicBase::Create(void* context)
{
    LOGI("WebGLRenderingContextBasicBase::Create.\n");
}

void WebGLRenderingContextBasicBase::Init()
{
    LOGI("WebGLRenderingContextBasicBase::Init. %{public}p", this);
    EglManager::GetInstance().Init();
    if (eglSurface_ == nullptr) {
        eglSurface_ = EglManager::GetInstance().CreateSurface(eglWindow_);
    }
}

void WebGLRenderingContextBasicBase::SetBitMapPtr(char* bitMapPtr, int bitMapWidth, int bitMapHeight)
{
    LOGI("WebGLRenderingContextBasicBase::SetBitMapPtr. %{public}p", this);
    LOGI("WebGLRenderingContextBasicBase SetBitMapPtr [%{public}d %{public}d]", bitMapWidth, bitMapHeight);
    bitMapPtr_ = bitMapPtr;
    bitMapWidth_ = bitMapWidth;
    bitMapHeight_ = bitMapHeight;
    EglManager::GetInstance().SetPbufferAttributes(bitMapWidth, bitMapHeight);
}

uint64_t WebGLRenderingContextBasicBase::CreateTexture()
{
    return 0;
}

void WebGLRenderingContextBasicBase::SetUpdateCallback(std::function<void()> callback)
{
    updateCallback_ = callback;
}

void WebGLRenderingContextBasicBase::SetTexture(uint64_t id) {}

void WebGLRenderingContextBasicBase::Attach(uint64_t textureId) {}

void WebGLRenderingContextBasicBase::Update()
{
    if (eglWindow_) {
        LOGI("WebGLRenderingContextBasicBase eglSwapBuffers");
        EGLDisplay eglDisplay = EglManager::GetInstance().GetEGLDisplay();
        eglSwapBuffers(eglDisplay, eglSurface_);
    } else {
        LOGI("WebGLRenderingContextBasicBase glReadPixels");
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(0, 0, bitMapWidth_, bitMapHeight_, GL_RGBA, GL_UNSIGNED_BYTE, bitMapPtr_);
        LOGI("WebGLRenderingContextBasicBase glReadPixels %{public}u", glGetError());
    }
    if (updateCallback_) {
        LOGI("WebGLRenderingContextBasicBase mUpdateCallback");
        updateCallback_();
    } else {
        LOGE("WebGLRenderingContextBasicBase mUpdateCallback null");
    }
}

void WebGLRenderingContextBasicBase::Detach() {}

bool WebGLRenderingContextBasicBase::CreateSurface()
{
    if (eglSurface_ == nullptr) {
        eglSurface_ = EglManager::GetInstance().CreateSurface(eglWindow_);
    }
    return true;
}

string WebGLRenderingContextBasicBase::GetContextAttr(
    const std::string& str, const std::string& key, int keyLength, int value)
{
    size_t item = str.find(key);
    if (item != string::npos) {
        string itemVar = str.substr(item + keyLength, value);
        return itemVar;
    }
    return "false";
}

bool WebGLRenderingContextBasicBase::SetWebGLContextAttributes(const std::vector<std::string>& vec)
{
    if (vec.size() <= 1) {
        return true;
    }
    if (webGlContextAttributes_ == nullptr) {
        webGlContextAttributes_ = new WebGLContextAttributes();
        if (webGlContextAttributes_ == nullptr) {
            return false;
        }
    }

    size_t i;
    for (i = 1; i < vec.size(); i++) {
        string alpha = GetContextAttr(vec[i], "alpha", 7, 4);
        if (alpha == "true") {
            webGlContextAttributes_->alpha_ = true;
        }
        string depth = GetContextAttr(vec[i], "depth", 7, 4);
        if (depth == "true") {
            webGlContextAttributes_->depth_ = true;
        }
        string stencil = GetContextAttr(vec[i], "stencil", 9, 4);
        if (stencil == "true") {
            webGlContextAttributes_->stencil_ = true;
        }
        string premultipliedAlpha = GetContextAttr(vec[i], "premultipliedAlpha", 23, 4);
        if (premultipliedAlpha == "true") {
            webGlContextAttributes_->premultipliedAlpha_ = true;
        }
        string preserveDrawingBuffer = GetContextAttr(vec[i], "preserveDrawingBuffer", 18, 4);
        if (preserveDrawingBuffer == "true") {
            webGlContextAttributes_->preserveDrawingBuffer_ = true;
        }
        string failIfMajorPerformanceCaveat = GetContextAttr(vec[i], "failIfMajorPerformanceCaveat", 30, 4);
        if (failIfMajorPerformanceCaveat == "true") {
            webGlContextAttributes_->failIfMajorPerformanceCaveat_ = true;
        }
        string desynchronized = GetContextAttr(vec[i], "desynchronized", 16, 4);
        if (desynchronized == "true") {
            webGlContextAttributes_->desynchronized_ = true;
        }
        string highPerformance = GetContextAttr(vec[i], "powerPreference", 18, 16);
        if (highPerformance == "high-performance") {
            webGlContextAttributes_->powerPreference_ = highPerformance;
        } else {
            string lowPower = GetContextAttr(vec[i], "powerPreference", 18, 9);
            if (lowPower == "low-power") {
                webGlContextAttributes_->powerPreference_ = lowPower;
            } else {
                string defaultVar = GetContextAttr(vec[i], "powerPreference", 18, 7);
                if (defaultVar == "default") {
                    webGlContextAttributes_->powerPreference_ = defaultVar;
                }
            }
        }
    }
    return true;
}

napi_value WebGLRenderingContextBasicBase::GetContextInstance(napi_env env,
    std::string className, napi_callback constructor, napi_finalize finalize_cb)
{
    napi_value instanceValue = nullptr;
    napi_status status;
    if (contextRef_ == nullptr) {
        napi_value contextClass = nullptr;
        napi_define_class(env, className.c_str(), NAPI_AUTO_LENGTH, constructor, nullptr, 0, nullptr, &contextClass);
        status = napi_new_instance(env, contextClass, 0, nullptr, &instanceValue);
        if (status != napi_ok) {
            return nullptr;
        }
        status = napi_wrap(env, instanceValue, static_cast<void*>(this), finalize_cb, nullptr, nullptr);
        if (status != napi_ok) {
            return nullptr;
        }
        status = napi_create_reference(env, instanceValue, 1, &contextRef_);
        if (status != napi_ok) {
            return nullptr;
        }
    } else {
        status = napi_get_reference_value(env, contextRef_, &instanceValue);
        if (status != napi_ok) {
            return nullptr;
        }
    }
    return instanceValue;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
