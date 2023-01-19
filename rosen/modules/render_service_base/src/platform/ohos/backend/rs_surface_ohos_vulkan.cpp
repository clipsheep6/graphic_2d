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

#include "rs_surface_ohos_vulkan.h"

#include <vulkan_window.h>
#include <vulkan_proc_table.h>
#include <vulkan_native_surface_ohos.h>

#include "platform/common/rs_log.h"
#include "window.h"
#include <hilog/log.h>
#include <display_type.h>
#include "pipeline/rs_render_thread.h"

namespace OHOS {
namespace Rosen {

RSSurfaceOhosVulkan::RSSurfaceOhosVulkan(const sptr<Surface>& producer) : RSSurfaceOhos(producer)
{
    bufferUsage_ = HBM_USE_CPU_READ | HBM_USE_MEM_DMA;
}

RSSurfaceOhosVulkan::~RSSurfaceOhosVulkan()
{
    DestoryNativeWindow(mNativeWindow);
    mNativeWindow = nullptr;
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosVulkan::RequestFrame(int32_t width, int32_t height,
    uint64_t uiTimestamp, bool useAFBC)
{
    if (mNativeWindow == nullptr) {
        mNativeWindow = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGI("RSSurfaceOhosVulkan: create native window %p", mNativeWindow);
    }

#ifdef RS_ENABLE_AFBC
    int32_t format = 0;
    NativeWindowHandleOpt(mNativeWindow, GET_FORMAT, &format);
    if (format == PIXEL_FMT_RGBA_8888 && useAFBC) {
        bufferUsage_ =
            (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
    }
#endif

    NativeWindowHandleOpt(mNativeWindow, SET_USAGE, bufferUsage_);
    NativeWindowHandleOpt(mNativeWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mNativeWindow, GET_BUFFER_GEOMETRY, &mHeight, &mWidth);
    NativeWindowHandleOpt(mNativeWindow, SET_COLOR_GAMUT, colorSpace_);
    NativeWindowHandleOpt(mNativeWindow, SET_UI_TIMESTAMP, uiTimestamp);

    if (mVulkanWindow == nullptr) {
        auto vulkanSurface
         = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(mNativeWindow);
        mVulkanWindow = new vulkan::VulkanWindow(std::move(vulkanSurface));
        ROSEN_LOGI("RSSurfaceOhosVulkan: create vulkan window %p", mVulkanWindow);
    }

    if (mVulkanWindow == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: Invalid VulkanWindow, return");
        return nullptr;
    }

    sk_sp<SkSurface> skSurface = mVulkanWindow->AcquireSurface();
    if (!skSurface) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: skSurface is null");
        return nullptr;
    }

    if (skSurface == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: skSurface is nullptr");
    }

    std::unique_ptr<RSSurfaceFrameOhosVulkan> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(skSurface, width, height);

    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));

    return ret;
}

void RSSurfaceOhosVulkan::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (mNativeWindow == nullptr) {
        mNativeWindow = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGI("RSSurfaceOhosVulkan: create native window %p", mNativeWindow);
    }

    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    NativeWindowHandleOpt(mNativeWindow, SET_UI_TIMESTAMP, duration);
}

bool RSSurfaceOhosVulkan::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    mVulkanWindow->SwapBuffers();
    return true;
}

void RSSurfaceOhosVulkan::SetSurfaceBufferUsage(uint64_t usage)
{
    bufferUsage_ = usage;
}

void RSSurfaceOhosVulkan::ClearBuffer()
{
    if (producer_ != nullptr) {
        ROSEN_LOGI("RSSurfaceOhosVulkan: Clear surface buffer!");
        producer_->GoBackground();
    }
}

void RSSurfaceOhosVulkan::ResetBufferAge()
{
    ROSEN_LOGI("RSSurfaceOhosVulkan: Reset Buffer Age!");
}
} // namespace Rosen
} // namespace OHOS
