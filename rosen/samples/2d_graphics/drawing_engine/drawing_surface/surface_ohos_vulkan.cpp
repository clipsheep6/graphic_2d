/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "surface_ohos_vulkan.h"

#include <vulakn_native_surface_ohos.h>
#include <vulkan_window.h>
#include <vulkan_proc_table.h>
#include <hilog/log.h>
#include <display_type.h>
#include "window.h"

namespace OHOS {
namespace Rosen {
SurfaceOhosVulkan::SurfaceOhosVulkan(const sptr<Surface>& producer)
    : SurfaceOhos(producer), frame_(nullptr)
{
}

SurfaceOhosVulkan::~SurfaceOhosVulkan()
{
    frame_ = nullptr;
}

std::unique_ptr<SurfaceFrame> SurfaceOhosVulkan::RequestFrame(int32_t width, int32_t height)
{
    if (mNativeWindow == nullptr) {
        mNativeWindow = CreateNativeWindowBufferFromSurface(&producer_);
    }
    if (mNativeWindow == nullptr) {
        LOGE("SurfaceOhosVulkan nativewindow is null");
        return nullptr;
    }

    if (mVulkanWindow == nullptr) {
        auto vulkan_surface_ohos = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(mNativeWindow);
        mVulkanWindow = new vulkan::VulkanWindow(std::move(vulkan_surface_ohos));
        LOGI("SurfaceOhosVulkan vulkanwindow %p", mVulkanWindow);
    }

    sk_sp<SkSurface> skSurface = mVulkanWindow -> AcquireSurface();

    frame_ = std::make_unique<SurfaceFrameOhosVulkan>(skSurface, width, height);
    frame_->SetColorSpace(ColorGamut::COLOR_GAMUT_SRGB);
    
    NativeWindowHandleOpt(mNativeWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mNativeWindow, SET_COLOR_GAMUT, frame_->GetColorSpace());
    
    std::unique_ptr<SurfaceFrame> ret(std::move(frame_));
    DestoryNativeWindow(nativeWindow);
    return ret;
}

bool SurfaceOhosVulkan::FlushFrame(std::unique_ptr<SurfaceFrame>& frame)
{
    // gpu render flush
    drawingProxy_->RenderFrame();
    if (mVulkanWindow != nullptr) {
        drawingProxy_->SwapBuffers();
    } else {
        LOGE("mVulkanWindow is null");
    }
    return true;
}

SkCanvas* SurfaceOhosVulkan::GetCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    return drawingProxy_->AcquireCanvas(frame);
}
} // namespace Rosen
} // namespace OHOS
