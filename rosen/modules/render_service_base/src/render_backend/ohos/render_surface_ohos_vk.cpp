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

#include "ohos/render_surface_ohos_vk.h"

#include <vulkan_proc_table.h>
#include <vulkan_native_surface_ohos.h>

#include "platform/common/rs_log.h"
#include "window.h"

namespace OHOS {
namespace Rosen {

RenderSurfaceOhosVk::~RenderSurfaceOhosVk()
{
    frame_ = nullptr;
    DestoryNativeWindow(nativeWindow_);
    nativeWindow_ = nullptr;
    vulkanWindow_ = nullptr;
}

std::shared_ptr<RenderSurfaceFrame> RenderSurfaceOhosVk::RequestFrame(int32_t width, int32_t height,
    uint64_t uiTimestamp, bool useAFBC)
{
    if (nativeWindow_ == nullptr) {
        nativeWindow_ = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGD("RenderSurfaceOhosVk: create native window");
    }

    NativeWindowHandleOpt(nativeWindow_, SET_FORMAT, pixelFormat_);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        NativeWindowHandleOpt(nativeWindow_, GET_FORMAT, &format);
        if (format == PIXEL_FMT_RGBA_8888 && useAFBC) {
            bufferUsage_ =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif

    NativeWindowHandleOpt(nativeWindow_, SET_USAGE, bufferUsage_);
    NativeWindowHandleOpt(nativeWindow_, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(nativeWindow_, SET_COLOR_GAMUT, frame_->colorSpace_);
    NativeWindowHandleOpt(nativeWindow_, SET_UI_TIMESTAMP, uiTimestamp);

    if (vulkanWindow_ == nullptr) {
        auto vulkanSurface = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(nativeWindow_);
        vulkanWindow_ = std::make_shared<vulkan::VulkanWindow>(std::move(vulkanSurface));
        renderContext_->MakeCurrent((void*) vulkanWindow_.get());
        ROSEN_LOGD("RenderSurfaceOhosVk: create vulkan window");
    }

    if (vulkanWindow_ == nullptr) {
        ROSEN_LOGE("RenderSurfaceOhosVk: Invalid VulkanWindow, return");
        return nullptr;
    }

    frame_->width_ = width;
    frame_->height_ = height;

    return frame_;
}

void RenderSurfaceOhosVk::SetUiTimeStamp(uint64_t uiTimestamp)
{
    if (nativeWindow_ == nullptr) {
        nativeWindow_ = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGD("RenderSurfaceOhosVk: create native window");
    }

    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    NativeWindowHandleOpt(nativeWindow_, SET_UI_TIMESTAMP, duration);
}

bool RenderSurfaceOhosVk::FlushFrame(uint64_t uiTimestamp)
{
    if (renderContext_ == nullptr) {
        ROSEN_LOGE("RenderSurfaceOhosVk::FlushFrame, renderContext_ failed!");
        return false;
    }

    renderContext_->SwapBuffers();
    return true;
}

void RenderSurfaceOhosVk::ClearBuffer()
{
    if (producer_ != nullptr) {
        ROSEN_LOGD("RenderSurfaceOhosVk: Clear surface buffer!");
        producer_->GoBackground();
    }
}

sk_sp<SkSurface> RenderSurfaceOhosVk::AcquireSurface()
{
    frame_->skSurface_ = vulkanWindow_->AcquireSurface();
    return frame_->skSurface_;
}

} // namespace Rosen
} // namespace OHOS
