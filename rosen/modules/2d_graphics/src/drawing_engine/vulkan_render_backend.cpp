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

#include "drawing_utils.h"
#include "vulkan_render_backend.h"

namespace OHOS {
namespace Rosen {
const uint32_t PARALLEL_THREAD_NUM = 3;
VulkanRenderBackend::~VulkanRenderBackend()
{
    vulkanWindow_ = nullptr;
}

void VulkanRenderBackend::Initialize()
{
#ifdef RS_ENABLE_PARALLEL_RENDER
    InitializeVulkan(PARALLEL_THREAD_NUM);
#else
    InitializeVulkan();
#endif
}

void VulkanRenderBackend::InitializeVulkan(uint32_t num)
{
    LOGE("VulkanRenderBackend::InitializeVulkan, num = %d", num);
    vulkan::VulkanWindow::InitializeVulkan(num);
}

void VulkanRenderBackend::DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height)
{
    LOGE("VulkanRenderBackend::DamageFrame set single dirty rect error");
}

void VulkanRenderBackend::DamageFrame(const std::vector<RectI> &rects)
{
    LOGE("VulkanRenderBackend::DamageFrame set multiple dirty rects error");
}

int32_t VulkanRenderBackend::GetBufferAge()
{
    LOGE("VulkanRenderBackend::GetBufferAge error");
    return 0;
}

void VulkanRenderBackend::Destroy()
{
    vulkanWindow_ = nullptr;
}

void VulkanRenderBackend::SwapBuffers()
{
    if (vulkanWindow_ != nullptr) {
        vulkanWindow_->SwapBuffers();
    } else {
        LOGE("VulkanRenderBackend::SwapBuffers vulkanWindow_ is nullptr");
    }
}

sk_sp<SkSurface> VulkanRenderBackend::AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    if (vulkanWindow_ != nullptr) {
        return vulkanWindow_->AcquireSurface();
    }
    LOGE("VulkanRenderBackend::AcquireSurface vulkanWindow_ is nullptr");
    return nullptr;
}

bool VulkanRenderBackend::SetUpVulkanWindow(std::unique_ptr<vulkan::VulkanNativeSurfaceOHOS> vulkanSurface)
{
    if (vulkanWindow_ != nullptr) {
        LOGI("VulkanRenderBackend::SetUpVulkanWindow vulkan window has created");
        return true;
    }
    vulkanWindow_ = std::make_unique<vulkan::VulkanWindow>(std::move(vulkanSurface));
    if (vulkanWindow_ == nullptr) {
        LOGE("VulkanRenderBackend::SetUpVulkanWindow create vulkan window failed");
        return false;
    }
    return true;
}

void VulkanRenderBackend::WaitForSharedFence()
{
    vulkan::VulkanWindow::WaitForSharedFence();
}

void VulkanRenderBackend::ResetSharedFence()
{
    vulkan::VulkanWindow::ResetSharedFence();
}

void VulkanRenderBackend::PresentAll()
{
    vulkan::VulkanWindow::PresentAll();
}

}
}