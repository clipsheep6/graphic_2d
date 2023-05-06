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

#ifndef RS_RENDER_CONTEXT_H
#define RS_RENDER_CONTEXT_H

#include <EGL/egl.h>
#include <memory>
#include <mutex>
#include "surface.h"
#ifdef RS_ENABLE_VK
#include <vulkan_native_surface_ohos.h>
#endif
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLInterface.h"

#include "interface_render_backend.h"

namespace OHOS {
namespace Rosen {
class RSRenderContext {
public:
    RSRenderContext() = delete;
    explicit RSRenderContext(std::shared_ptr<IRenderBackend> renderBackend);
    virtual ~RSRenderContext();
    void Init();
    static std::unique_ptr<RSRenderContext> Create();
    void RenderFrame();
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height);
    void DamageFrame(const std::vector<RectI> &rects);
    int32_t GetBufferAge();
    void SwapBuffers();
    sk_sp<SkSurface> AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame);
    void Destroy();

    void MakeCurrent(EGLSurface surface, EGLContext context = EGL_NO_CONTEXT);
    EGLSurface CreateEGLSurface(EGLNativeWindowType window);   
    void DestroyEGLSurface(EGLSurface surface);
    GrContext* GetGrContext();
    void SetCacheDir(const std::string& filePath);
    void ClearRedundantResources();
    EGLContext GetEGLContext() const;
    EGLDisplay GetEGLDisplay() const;
    std::string GetShaderCacheSize() const;
    std::string CleanAllShaderCache() const;
    void SetUniRenderMode(bool isUni);
    void SetColorSpace(ColorGamut colorSpace);
    void ShareMakeCurrent(EGLContext shareContext);
    void MakeSelfCurrent();
    EGLContext CreateShareContext();
#ifdef RS_ENABLE_VK
    void InitializeVulkan(uint32_t num);
    bool SetUpVulkanWindow(std::unique_ptr<vulkan::VulkanNativeSurfaceOHOS> vulkanSurface);
    void WaitForSharedFence();
    void ResetSharedFence();
    void PresentAll();
#endif
private:
    std::shared_ptr<IRenderBackend> renderBackend_;
};
}
}
#endif