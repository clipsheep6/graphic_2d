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

#include <surface.h>
#include "EGL/egl.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceFrame {
public:
    explicit RSSurfaceFrame() noexcept = default;
    ~RSSurfaceFrame() = default;

    int32_t width_ = 0;
    int32_t height_ = 0;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    std::unique_ptr<uint32_t[]> addr_ = nullptr;
    int32_t bpp_ = 0;
    int32_t releaseFence_ = -1;
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
    int32_t pixelFormat_ = PIXEL_FMT_RGBA_8888;
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    sk_sp<SkSurface> skSurface_ = nullptr;
};
}
}
#endif