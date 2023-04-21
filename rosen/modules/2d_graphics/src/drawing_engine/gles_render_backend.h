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
#include "include/gpu/GrContext.h"
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
    void MakeCurrent() override;
    void SwapBuffers() override;
    void SetUpSurface(void* window) override;
    void DestorySurface() override;
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height) override;
    void DamageFrame(const std::vector<RectI> &rects) override;
    void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;
    int32_t GetBufferAge() override;
    GrContext* GetGrContext() const
    {
        return grContext_.get();
    }
    bool SetUpGrContext();
    void Destroy() override;
    void RenderFrame() override;
    sk_sp<SkSurface> AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame) override;

#ifndef ROSEN_CROSS_PLATFORM
    void SetColorSpace(ColorGamut colorSpace)
    {
        colorSpace_ = colorSpace;
    }

    ColorGamut GetColorSpace() const
    {
        return colorSpace_;
    }
#endif
    void SetCacheDir(const std::string& filePath)
    {
        cacheDir_ = filePath;
    }
    
    void SetUniRenderMode(bool isUni)
    {
        isUniRenderMode_ = isUni;
    }
#ifdef RS_ENABLE_GL
    std::string GetShaderCacheSize() const
    {
        return mHandler_->QuerryShader();
    }

    std::string CleanAllShaderCache() const
    {
        return mHandler_->ClearShader();
    }
#endif

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
    sk_sp<GrContext> grContext_ = nullptr;
    sk_sp<SkSurface> skSurface_ = nullptr;
    bool isUniRenderMode_ = false;
    const std::string UNIRENDER_CACHE_DIR = "/data/service/el0/render_service";
    std::string cacheDir_;
    std::shared_ptr<MemoryHandler> mHandler_;
#ifndef ROSEN_CROSS_PLATFORM
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
#endif
};
}
}
#endif