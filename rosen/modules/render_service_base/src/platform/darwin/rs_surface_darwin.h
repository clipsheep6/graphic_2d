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

#ifndef RENDER_SERVICE_BASE_SRC_PLATFORM_DARWIN_RS_SURFACE_DARWIN_H
#define RENDER_SERVICE_BASE_SRC_PLATFORM_DARWIN_RS_SURFACE_DARWIN_H

#include <memory>

#include <include/gpu/GrContext.h>
#include <surface.h>

#include "platform/drawing/rs_surface.h"
#include "platform/drawing/rs_surface_frame.h"

namespace OHOS {
namespace Rosen {
class RenderContext;
class RSSurfaceDarwin : public RSSurface {
public:
    RSSurfaceDarwin(const sptr<Surface> &producer);
    ~RSSurfaceDarwin() override = default;

    bool IsValid() const override;
    sptr<Surface> GetSurface() const;

    std::unique_ptr<RSSurfaceFrame> RequestFrame(int32_t width, int32_t height, uint64_t uiTimestamp) override;

    bool FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;
    RenderContext* GetRenderContext() override;
    void SetRenderContext(RenderContext* context) override;
    ColorGamut GetColorSpace() const override;
    void SetColorSpace(ColorGamut colorSpace) override;
    uint32_t GetQueueSize() const override;
    void ClearBuffer() override;
    void ClearAllBuffer() override;
    void ResetBufferAge() override;

private:
    void YInvert(void *addr, int32_t width, int32_t height);
    bool SetupGrContext();

    sptr<Surface> producer_ = nullptr;
    RenderContext* renderContext_ = nullptr;
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    sk_sp<GrContext> grContext_ = nullptr;
    sk_sp<SkColorSpace> skColorSpace_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_SRC_PLATFORM_DARWIN_RS_SURFACE_DARWIN_H
