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
#include "context/webgl_rendering_context.h"
#include "util/egl_manager.h"
#include "util/log.h"
#include "context/webgl2_rendering_context.h"
#include "napi/n_class.h"
#include "util/object_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
namespace OHOS {
namespace Rosen {

void WebGLRenderingContextBasicBase::SetEglWindow(void *window)
{
    LOGI("WebGLRenderingContextBasicBase::SetEglWindow.\n");
    mEglWindow = reinterpret_cast<NativeWindow *>(window);
}

void WebGLRenderingContextBasicBase::Create(void *context)
{
    LOGI("WebGLRenderingContextBasicBase::Create.\n");
}

void WebGLRenderingContextBasicBase::Init()
{
    LOGI("WebGLRenderingContextBasicBase::Init. %{public}p", this);
    EglManager::GetInstance().Init();
    if (mEGLSurface == nullptr) {
        mEGLSurface = EglManager::GetInstance().CreateSurface(mEglWindow);
    }
}

void WebGLRenderingContextBasicBase::SetBitMapPtr(char *bitMapPtr, int bitMapWidth, int bitMapHeight)
{
    LOGI("WebGLRenderingContextBasicBase::SetBitMapPtr. %{public}p", this);
    LOGI("WebGLRenderingContextBasicBase SetBitMapPtr [%{public}d %{public}d]",
        bitMapWidth, bitMapHeight);
    mBitMapPtr = bitMapPtr;
    mBitMapWidth = bitMapWidth;
    mBitMapHeight = bitMapHeight;
    EglManager::GetInstance().SetPbufferAttributes(bitMapWidth, bitMapHeight);
}

uint64_t WebGLRenderingContextBasicBase::CreateTexture()
{
    return 0;
}

void WebGLRenderingContextBasicBase::SetUpdateCallback(std::function<void()> callback)
{
    mUpdateCallback = callback;
}

void WebGLRenderingContextBasicBase::SetTexture(uint64_t id) {}

void WebGLRenderingContextBasicBase::Attach(uint64_t textureId) {}

void WebGLRenderingContextBasicBase::Update()
{
    if (mEglWindow) {
        LOGI("WebGLRenderingContextBasicBase eglSwapBuffers");
        EGLDisplay eglDisplay = EglManager::GetInstance().GetEGLDisplay();
        eglSwapBuffers(eglDisplay, mEGLSurface);
    } else {
        LOGI("WebGLRenderingContextBasicBase glReadPixels");
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(0, 0, mBitMapWidth, mBitMapHeight, GL_RGBA, GL_UNSIGNED_BYTE, mBitMapPtr);
        LOGI("WebGLRenderingContextBasicBase glReadPixels %{public}u", glGetError());
    }
    if (mUpdateCallback) {
        LOGI("WebGLRenderingContextBasicBase mUpdateCallback");
        mUpdateCallback();
    } else {
        LOGE("WebGLRenderingContextBasicBase mUpdateCallback null");
    }
}

napi_value WebGLRenderingContextBasicBase::GetContextInstance(napi_env env,
    std::string className, napi_callback constructor, napi_finalize finalize_cb)
{
    napi_value instanceValue = nullptr;
    napi_status status;
    if (mContextRef == nullptr) {
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
        status = napi_create_reference(env, instanceValue, 1, &mContextRef);
        if (status != napi_ok) {
            return nullptr;
        }
    } else {
        status = napi_get_reference_value(env, mContextRef, &instanceValue);
        if (status != napi_ok) {
            return nullptr;
        }
    }
    return instanceValue;
}

void WebGLRenderingContextBasicBase::Detach() {}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
