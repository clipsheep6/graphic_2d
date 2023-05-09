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

#include "ohos/render_surface_ohos.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
bool RenderSurfaceOhos::IsValid()
{
    return producer_ != nullptr;
}

sptr<Surface> RenderSurfaceOhos::GetSurface() const
{
    return producer_;
}

#ifndef ROSEN_CROSS_PLATFORM
ColorGamut RenderSurfaceOhos::GetColorSpace()
{
    return frame_->colorSpace_;
}

void RenderSurfaceOhos::SetColorSpace(ColorGamut colorSpace)
{
    frame_->colorSpace_ = colorSpace;
}
#endif

SkCanvas* RenderSurfaceOhos::GetSkCanvas()
{
    if (frame_->skSurface_ == nullptr) {
        ROSEN_LOGE("ZJ RenderSurfaceOhos::GetSkCanvas frame_->skSurface_ is nullptr");
        frame_->skSurface_ = this->AcquireSurface();
    }
    return frame_->skSurface_->getCanvas();
}

sk_sp<SkSurface> RenderSurfaceOhos::GetSkSurface()
{
    if (frame_->skSurface_ == nullptr) {
        ROSEN_LOGE("ZJ RenderSurfaceOhos::GetSkSurface frame_->skSurface_ is nullptr");
        frame_->skSurface_ = this->AcquireSurface();
    }
    return frame_->skSurface_;
}

void RenderSurfaceOhos::SetSurfaceBufferUsage(uint64_t usage)
{
    bufferUsage_ = usage;
}

void RenderSurfaceOhos::SetSurfacePixelFormat(uint64_t pixelFormat)
{
    pixelFormat_ = pixelFormat;
}

void RenderSurfaceOhos::SetReleaseFence(const int32_t& fence)
{
    frame_->releaseFence_ = fence;
}

int32_t RenderSurfaceOhos::GetReleaseFence() const
{
    return frame_->releaseFence_;
}
}
}