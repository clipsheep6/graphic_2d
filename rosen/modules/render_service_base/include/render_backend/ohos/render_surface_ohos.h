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

#ifndef RENDER_SERVICE_BASE_RENDER_SURFACE_OHOS_H
#define RENDER_SERVICE_BASE_RENDER_SURFACE_OHOS_H

#include <memory>

#include "common/rs_rect.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

#include "rs_render_surface.h"
#include "rs_render_surface_frame.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

namespace OHOS {
namespace Rosen {
class RenderContextBase;
class RenderSurfaceOhos : public RenderSurface {
public:
    RenderSurfaceOhos(const sptr<Surface>& producer) : producer_(producer)
    {
        bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
    }

    ~RenderSurfaceOhos()
    {
        producer_ = nullptr;
    }

    bool IsValid();

    sptr<Surface> GetSurface() const;

    SkCanvas* GetSkCanvas() override;
    
    sk_sp<SkSurface> GetSkSurface() override;

#ifndef ROSEN_CROSS_PLATFORM
    ColorGamut GetColorSpace() override;

    void SetColorSpace(ColorGamut colorSpace) override;
#endif

    virtual sk_sp<SkSurface> AcquireSurface() = 0;
protected:
    void SetSurfaceBufferUsage(uint64_t usage);

    void SetSurfacePixelFormat(uint64_t pixelFormat);

    void SetReleaseFence(const int32_t& fence);

    int32_t GetReleaseFence() const;

    sptr<Surface> producer_ = nullptr;
    int32_t pixelFormat_ = PIXEL_FMT_RGBA_8888;
    uint64_t bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
};
}
} // namespace Rosen
#endif