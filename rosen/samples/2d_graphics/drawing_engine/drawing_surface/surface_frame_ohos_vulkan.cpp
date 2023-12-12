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

#include "surface_frame_ohos_vulkan.h"

namespace OHOS {
namespace Rosen {
SurfaceFrameOhosVulkan::SurfaceFrameOhosVulkan(sk_sp<SkSurface> surface, int32_t width, int32_t height)
    : SurfaceFrameOhos(width, height), surface_(surface), colorSpace_(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB)
{
}

SurfaceFrameOhosVulkan::~SurfaceFrameOhosVulkan()
{
}

void SurfaceFrameOhosVulkan::SetColorSpace(GraphicColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

GraphicColorGamut SurfaceFrameOhosVulkan::GetColorSpace() const
{
    return colorSpace_;
}

SkCanvas* SurfaceFrameOhosVulkan::GetCanvas()
{
    return surface_ != nullptr ? surface_->GetCanvas() : nullptr;
}

sk_sp<SkSurface> SurfaceFrameOhosVulkan::GetSurface()
{
    return surface_;
}
} // namespace Rosen
} // namespace OHOS
