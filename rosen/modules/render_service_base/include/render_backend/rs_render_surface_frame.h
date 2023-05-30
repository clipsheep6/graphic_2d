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

#ifndef RENDER_SERVICE_BASE_RENDER_SURFACE_FRAME_H
#define RENDER_SERVICE_BASE_RENDER_SURFACE_FRAME_H

#include <EGL/egl.h>
#include <surface.h>

#include "include/core/SkSurface.h"

#ifdef RS_ENABLE_VK
#include <vulkan_window.h>
#endif

namespace OHOS {
namespace Rosen {
enum PlatformName {
    OHOS,
    ANDROID,
    IOS,
    WINDOWS,
    DARWIN,
};
enum RenderType {
    RASTER,
    GLES,
    VULKAN,
};    
class RSRenderSurfaceFrame {
public:
    explicit RSRenderSurfaceFrame() noexcept = default;
    ~RSRenderSurfaceFrame() = default;

    int32_t width_ = 0;
    int32_t height_ = 0;
    uint64_t uiTimestamp_ = 0;
    int32_t bpp_ = 0;
    int32_t releaseFence_ = -1;
    int32_t pixelFormat_ = PIXEL_FMT_RGBA_8888;
    uint64_t bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    std::unique_ptr<uint32_t[]> addr_ = nullptr;
    BufferRequestConfig requestConfig_ = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    BufferFlushConfig flushConfig_ = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = 0x100,
            .h = 0x100,
        },
    };
    sk_sp<SkSurface> skSurface_ = nullptr;
    struct NativeWindow* nativeWindow_ = nullptr;
#ifdef RS_ENABLE_VK
    std::shared_ptr<vulkan::VulkanWindow> vulkanWindow_ = nullptr;
#endif
    sptr<Surface> producer_ = nullptr;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
};
}
}
#endif