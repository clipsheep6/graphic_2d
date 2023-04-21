/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
#define RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H

#include <surface.h>

#include "platform/drawing/rs_surface.h"

namespace OHOS {
namespace Rosen {
class RenderContext;
class RSSurfaceOhos : public RSSurface {
public:
    RSSurfaceOhos(const sptr<Surface>& producer) : producer_(producer)
    {
    }

    bool IsValid() const override
    {
        return producer_ != nullptr;
    }

    sptr<Surface> GetSurface() const
    {
        return producer_;
    }
    virtual void SetColorSpace(ColorGamut colorSpace) override;
    virtual ColorGamut GetColorSpace() const override;
    virtual uint32_t GetQueueSize() const override;

    virtual void SetSurfaceBufferUsage(uint64_t usage) = 0;
    void ClearAllBuffer() override;
    std::shared_ptr<RSSurface> CreateRSSurface(sptr<Surface> surface);
    virtual SkCanvas* GetCanvas(const std::unique_ptr<RSSurfaceFrame>& frame) override;
    virtual sk_sp<SkSurface> GetSkSurface(const std::unique_ptr<RSSurfaceFrame>& frame) override;
protected:
    sptr<Surface> producer_;
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    uint64_t bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_OHOS_H
