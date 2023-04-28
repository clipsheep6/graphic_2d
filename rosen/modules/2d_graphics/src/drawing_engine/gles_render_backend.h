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

#ifndef GLES_RENDER_BACKEND_H
#define GLES_RENDER_BACKEND_H
#include "interface_render_backend.h"
#include "egl_manager.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#include "include/gpu/gl/GrGLInterface.h"
#include "render_context/memory_handler.h"
#include <EGL/egl.h>
#include <memory>

namespace OHOS {
namespace Rosen {
const int STENCIL_BUFFER_SIZE = 8;
class GLESRenderBackend : public IRenderBackend {
public:
    GLESRenderBackend() noexcept;
    ~GLESRenderBackend() override;
    void Initialize() override;
    void MakeCurrent(EGLSurface surface, EGLContext context = EGL_NO_CONTEXT);
    void SwapBuffers() override;
    EGLSurface CreateEGLSurface(EGLNativeWindowType eglNativeWindow);
    void DestroyEGLSurface(EGLSurface surface);
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height) override;
    void DamageFrame(const std::vector<RectI> &rects) override;
    int32_t GetBufferAge() override;
#ifdef NEW_SKIA
    GrDirectContext* GetGrContext() const
    {
        return grContext_.get();
    }
#else
    GrContext* GetGrContext() const
    {
        return grContext_.get();
    }
#endif
    void SetUpGrContext();
    void Destroy() override;
    void RenderFrame() override;
    sk_sp<SkSurface> AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame = nullptr) override;
    void ClearRedundantResources();
    void ShareMakeCurrent(EGLContext shareContext);
    void MakeSelfCurrent();
    EGLContext CreateShareContext();
    void SetColorSpace(ColorGamut colorSpace)
    {
        colorSpace_ = colorSpace;
    }

    ColorGamut GetColorSpace() const
    {
        return colorSpace_;
    }
    void SetCacheDir(const std::string& filePath)
    {
        cacheDir_ = filePath;
    }
    
    void SetUniRenderMode(bool isUni)
    {
        isUniRenderMode_ = isUni;
    }

    std::string GetShaderCacheSize() const
    {
        return mHandler_->QuerryShader();
    }

    std::string CleanAllShaderCache() const
    {
        return mHandler_->ClearShader();
    }

    EGLContext GetEGLContext() const
    {
        if (eglManager_ != nullptr) {
            return eglManager_->GetEGLContext();
        }
        return EGL_NO_CONTEXT;
    }

    EGLDisplay GetEGLDisplay() const
    {
        if (eglManager_ != nullptr) {
            return eglManager_->GetEGLDisplay();
        }
        return EGL_NO_DISPLAY;
    }
private:
    std::unique_ptr<EGLManager> eglManager_;
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
    sk_sp<SkSurface> skSurface_ = nullptr;
    bool isUniRenderMode_ = false;
    const std::string UNIRENDER_CACHE_DIR = "/data/service/el0/render_service";
    std::string cacheDir_;
    std::shared_ptr<MemoryHandler> mHandler_;
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
};
}
}
#endif