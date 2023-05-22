/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include "include/core/SkSurface.h"
#include "include/gpu/GrContext.h"
#ifdef IOS_PLATFORM
#include "render_context_egl_defines.h"
#else
#include "EGL/egl.h"
#endif
namespace OHOS {
namespace Rosen {
class RenderContext {
public:
    virtual ~RenderContext() = default;
    virtual void InitializeEglContext() = 0;
    virtual sk_sp<SkSurface> AcquireSurface(int width, int height) = 0;
    virtual sk_sp<SkSurface> GetSurface() const = 0;
    virtual bool SetUpGrContext() = 0;
    virtual void MakeCurrent(EGLSurface surface = nullptr, EGLContext context = EGL_NO_CONTEXT) = 0;
    virtual void RenderFrame() = 0;
    virtual GrContext* GetGrContext() const = 0;
    virtual void SetCacheDir(const std::string& filePath) {}
    virtual EGLContext GetEGLContext() const { return nullptr; }
    virtual EGLSurface CreateEGLSurface(EGLNativeWindowType) = 0;
    virtual void SwapBuffers(EGLSurface surface = nullptr) const = 0;
    virtual EGLint QueryEglBufferAge() { return 0;}
};

class RenderContextFactory {
public:
    static RenderContextFactory& GetInstance();

    ~RenderContextFactory()
    {
        if (context_ != nullptr) {
            delete context_;
        }
        context_ = nullptr;
    }

    RenderContext* CreateEngine();

    RenderContext* CreateNewEngine()
    {
        return context_;
    }

private:
    RenderContextFactory() : context_(nullptr) {}
    RenderContextFactory(const RenderContextFactory&) = delete;
    RenderContextFactory& operator=(const RenderContextFactory&) = delete;

    RenderContext* context_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif
