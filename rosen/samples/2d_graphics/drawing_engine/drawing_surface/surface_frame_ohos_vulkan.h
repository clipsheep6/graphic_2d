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

#ifndef SURFACE_FRAME_OHOS_VULKAN_H
#define SURFACE_FRAME_OHOS_VULKAN_H

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include <display_type.h>
#include <surface.h>
#include "surface_type.h"
#include "surface_frame.h"
#include "surface_frame_ohos.h"
#include "draw/canvas.h"

#ifdef USE_ROSEN_DRAWING
#include "engine_adapter/skia_adapter/skia_surface.h"
#endif

namespace OHOS {
namespace Rosen {
class SurfaceFrameOhosVulkan : public SurfaceFrameOhos {
public:
#ifndef USE_ROSEN_DRAWING
    SurfaceFrameOhosVulkan(sk_sp<SkSurface> surface, int32_t width, int32_t height);
#else
    SurfaceFrameOhosVulkan(std::shared_ptr<Drawing::Surface> surface, int32_t width, int32_t height);
#endif
    ~SurfaceFrameOhosVulkan() override;
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override {};
    void SetColorSpace(GraphicColorGamut colorSpace) override;
    GraphicColorGamut GetColorSpace() const override;
#ifdef USE_ROSEN_DRAWING
    Drawing::Canvas* GetCanvas();
    Drawing::Surface* GetSurface();
#endif
private:
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> surface_;
#else
    std::shared_ptr<Drawing::Surface> surface_;
#endif
    GraphicColorGamut colorSpace_;
};
} // namespace Rosen
} // namespace OHOS
#endif
