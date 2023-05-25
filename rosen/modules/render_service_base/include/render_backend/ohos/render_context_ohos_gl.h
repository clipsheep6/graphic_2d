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

#include "render_context_base.h"
#include "common/rs_rect.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

#include "include/gpu/gl/GrGLInterface.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

namespace OHOS {
namespace Rosen {
class RenderContextOhosGl : public RenderContextBase {
public:
    explicit RenderContextOhosGl() noexcept;
    ~RenderContextOhosGl();
    void Init() override;
    bool IsContextReady() override;
    void MakeCurrent(void* curSurface = nullptr, void* curContext = nullptr) override;
    void* CreateContext(bool share = false) override;
    bool CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    void DestroySurface() override;
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height,
        const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    void DamageFrame(const std::vector<RectI> &rects) override;
    int32_t GetBufferAge() override;
    void SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    void Destroy() override;
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