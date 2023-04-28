/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef EGL_MANAGER_H
#define EGL_MANAGER_H

#include <mutex>
#include "common/rs_rect.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

#include "platform/drawing/rs_surface_frame.h"

namespace OHOS {
namespace Rosen {
class EGLManager {
public:
    EGLManager() noexcept;
    ~EGLManager();
    void Initialize();
    void CreateContext();
    EGLSurface CreateEGLSurface(EGLNativeWindowType eglNativeWindow);
    void DestroyEGLSurface(EGLSurface surface);
    void MakeCurrent(EGLSurface surface, EGLContext context);
    void SwapBuffers() const;
    EGLint QueryEglBufferAge();
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height);
    void DamageFrame(const std::vector<RectI> &rects);
    void CreatePbufferSurface();
    void ShareMakeCurrent(EGLContext shareContext);
    void ShareMakeCurrentNoSurface(EGLContext shareContext);
    void MakeSelfCurrent();
    
    bool IsEglContextReady() const
    {
        return eglContext_ != EGL_NO_DISPLAY;
    }

    EGLSurface GetEGLSurface() const
    {
        return eglSurface_;
    }

    EGLContext GetEGLContext() const
    {
        return eglContext_;
    }

    EGLDisplay GetEGLDisplay() const
    {
        return eglDisplay_;
    }
    EGLContext CreateShareContext();

private:
    EGLNativeWindowType nativeWindow_ = nullptr;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    EGLSurface pbufferSurface_= EGL_NO_SURFACE;
    EGLConfig config_;
    std::mutex shareContextMutex_;
};
}
}
#endif