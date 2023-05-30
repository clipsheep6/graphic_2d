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

#include "ohos/render_context_ohos_vk.h"

#include "rs_trace.h"
#include "utils/log.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
const uint32_t PARALLEL_THREAD_NUM = 3;
RenderContextOhosVk::~RenderContextOhosVk()
{
    vulkanWindow_ = nullptr;
}

void RenderContextOhosVk::Init()
{
#ifdef RS_ENABLE_PARALLEL_RENDER
    InitializeVulkan(PARALLEL_THREAD_NUM);
#else
    InitializeVulkan();
#endif
}

bool RenderContextOhosVk::IsContextReady()
{
    return vulkanWindow_ != nullptr;
}

bool RenderContextOhosVk::CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (frame->nativeWindow_ == nullptr) {
        frame->nativeWindow_ = CreateNativeWindowFromSurface(&(frame->producer_));
        LOGD("RSRenderSurfaceOhosVulkan: create native window");
    }

    NativeWindowHandleOpt(frame->nativeWindow_, SET_FORMAT, frame->pixelFormat_);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        NativeWindowHandleOpt(frame->nativeWindow_, GET_FORMAT, &format);
        if (format == PIXEL_FMT_RGBA_8888 && useAFBC) {
            bufferUsage_ =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif
    NativeWindowHandleOpt(frame->nativeWindow_, SET_USAGE, frame->bufferUsage_);
    NativeWindowHandleOpt(frame->nativeWindow_, SET_BUFFER_GEOMETRY, frame->width, frame->height);
    NativeWindowHandleOpt(frame->nativeWindow_, SET_COLOR_GAMUT, frame->colorSpace_);
    NativeWindowHandleOpt(frame->nativeWindow_, SET_UI_TIMESTAMP, frame->uiTimestamp_);

    if (vulkanWindow_ == nullptr) {
        auto vulkanSurface = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(nativeWindow_);
        vulkanWindow_ = std::make_unique<vulkan::VulkanWindow>(std::move(vulkanSurface));
    }

    if (vulkanWindow_ == nullptr) {
        LOGE("Failed to create vulkan window");
        return false;
    }
    frame->vulkanWindow_ = vulkanWindow_;
    return true;
}

void RenderContextOhosVk::DamageFrame(const std::vector<RectI> &rects)
{
    LOGE("VulkanRenderBackend::DamageFrame set multiple dirty rects error");
}

int32_t RenderContextOhosVk::GetBufferAge()
{
    LOGE("VulkanRenderBackend::GetBufferAge error");
    return -1;
}

void RenderContextOhosVk::SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame)
{
    if (IsContextReady()) {
        vulkanWindow_->SwapBuffers();
    } else {
        LOGE("VulkanRenderBackend::SwapBuffers vulkanWindow_ is nullptr");
    }
}

void RenderContextOhosVk::WaitForSharedFence()
{
    vulkan::VulkanWindow::WaitForSharedFence();
}

void RenderContextOhosVk::ResetSharedFence()
{
    vulkan::VulkanWindow::ResetSharedFence();
}

void RenderContextOhosVk::PresentAll()
{
    vulkan::VulkanWindow::PresentAll();
}

void RenderContextOhosVk::InitializeVulkan(uint32_t num)
{
    LOGE("VulkanRenderBackend::InitializeVulkan, num = %d", num);
    vulkan::VulkanWindow::InitializeVulkan(num);
}

}
}