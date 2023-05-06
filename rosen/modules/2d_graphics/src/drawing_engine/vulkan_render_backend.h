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
#ifndef VULKAN_RENDER_BACKEND_H
#define VULKAN_RENDER_BACKEND_H

#include <vulkan_native_surface_ohos.h>
#include <vulkan_window.h>

#include "interface_render_backend.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include <memory>

namespace OHOS {
namespace Rosen {
class VulkanRenderBackend : public IRenderBackend {
public:
    VulkanRenderBackend() noexcept = default;
    ~VulkanRenderBackend() override; 
    void Initialize() override;
    void InitializeVulkan(uint32_t num = 0);
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height) override;
    void DamageFrame(const std::vector<RectI> &rects) override;
    int32_t GetBufferAge() override;
    void Destroy() override;
    void RenderFrame() override {};
    void SwapBuffers() override;
    sk_sp<SkSurface> AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame = nullptr) override;
    GrContext* GetGrContext();
    bool SetUpVulkanWindow(std::unique_ptr<vulkan::VulkanNativeSurfaceOHOS> vulkanSurface);
    void WaitForSharedFence();
    void ResetSharedFence();
    void PresentAll();
private:
    std::unique_ptr<vulkan::VulkanWindow> vulkanWindow_;
};
}
}
#endif