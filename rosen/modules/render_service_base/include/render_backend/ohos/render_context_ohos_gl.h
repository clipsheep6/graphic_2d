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

#ifndef RENDER_SERVICE_RENDER_CONTEXT_OHOS_GL_H
#define RENDER_SERVICE_RENDER_CONTEXT_OHOS_GL_H

#include <memory>
#include <mutex>

#include "render_context.h"
#include "common/rs_rect.h"

#ifdef ROSEN_IOS
#include "render_context_egl_defines.h"
#else
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"
#endif

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#include "include/gpu/gl/GrGLInterface.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

namespace OHOS {
namespace Rosen {
class RenderContextOhosGl : public RenderContext {
public:
    explicit RenderContextOhosGl() noexcept;
    ~RenderContextOhosGl();
    void Init() override;
    bool IsContextReady() override;
    void MakeCurrent(void* curSurface = nullptr, void* curContext = nullptr) override;
    bool SetUpGrContext() override;
    void* CreateContext(bool share = false) override;
    void* CreateSurface(void* window) override;
    void DestroySurface(void* curSurface) override;
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height) override;
    void DamageFrame(const std::vector<RectI> &rects) override;
    int32_t GetBufferAge() override;
    void SwapBuffers() override;
    void Destroy() override;

    EGLContext GetEGLContext() override
    {
        return eglContext_;
    }

    EGLDisplay GetEGLDisplay() override
    {
        return eglDisplay_;
    }

private:
    void CreatePbufferSurface();

    EGLNativeWindowType nativeWindow_;
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