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

#include "ohos/rs_surface_ohos.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
bool RSSurfaceOhos::IsValid()
{
    return producer_ != nullptr;
}

sptr<Surface> RSSurfaceOhos::GetSurface() const
{
    return producer_;
}

uint32_t RSSurfaceOhos::GetQueueSize() const
{
    return producer_->GetQueueSize();
}

#ifndef ROSEN_CROSS_PLATFORM
ColorGamut RSSurfaceOhos::GetColorSpace()
{
    return frame_->colorSpace_;
}

void RSSurfaceOhos::SetColorSpace(ColorGamut colorSpace)
{
    frame_->colorSpace_ = colorSpace;
}
#endif

SkCanvas* RSSurfaceOhos::GetSkCanvas()
{
    if (frame_->skSurface_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhos::GetSkCanvas frame_->skSurface_ is nullptr");
        frame_->skSurface_ = this->AcquireSurface();
    }
    return frame_->skSurface_->getCanvas();
}

sk_sp<SkSurface> RSSurfaceOhos::GetSkSurface()
{
    if (frame_->skSurface_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhos::GetSkSurface frame_->skSurface_ is nullptr");
        frame_->skSurface_ = this->AcquireSurface();
    }
    return frame_->skSurface_;
}

void RSSurfaceOhos::SetSurfaceBufferUsage(uint64_t usage)
{
    bufferUsage_ = usage;
}

void RSSurfaceOhos::SetSurfacePixelFormat(uint64_t pixelFormat)
{
    pixelFormat_ = pixelFormat;
}

void RSSurfaceOhos::SetReleaseFence(const int32_t& fence)
{
    frame_->releaseFence_ = fence;
}

int32_t RSSurfaceOhos::GetReleaseFence() const
{
    return frame_->releaseFence_;
}
}
}