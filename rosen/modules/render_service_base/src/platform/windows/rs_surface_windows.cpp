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

#include "rs_surface_windows.h"

#include <thread>

#include <sync_fence.h>

#include "platform/common/rs_log.h"
#include "rs_surface_frame_windows.h"

namespace OHOS {
namespace Rosen {
RSSurfaceWindows::RSSurfaceWindows(const sptr<Surface>& producer)
    : producer_(producer)
{
}

bool RSSurfaceWindows::IsValid() const
{
    return producer_ != nullptr;
}

sptr<Surface> RSSurfaceWindows::GetSurface() const
{
    return producer_;
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceWindows::RequestFrame(int32_t width, int32_t height)
{
    if (producer_ == nullptr) {
        ROSEN_LOGE("RSSurfaceWindows::RequestFrame, producer is nullptr");
        return nullptr;
    }

    auto frame = std::make_unique<RSSurfaceFrameWindows>(width, height);
    int32_t releaseFence = -1;
    SurfaceError err = producer_->RequestBuffer(frame->buffer_, releaseFence, frame->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceWindows::Requestframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        ROSEN_LOGW("RsDebug RSProcessor::RequestFrame this buffer is not available");
    }

    return frame;
}

bool RSSurfaceWindows::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        ROSEN_LOGE("RSSurfaceFrame::FlushFrame frame is nullptr");
        return false;
    }

    // RSSurfaceWindows is the class for platform Windows,
    // the input pointer should be the pointer to the class RSSurfaceFrameWindows.
    // We use reinterpret_cast instead of RTTI and dynamic_cast which are not permitted.
    auto frameWindows = reinterpret_cast<RSSurfaceFrameWindows *>(frame.get());
    SurfaceError err = producer_->FlushBuffer(frameWindows->buffer_, -1, frameWindows->flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceWindows::Flushframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return false;
    }

    return true;
}

RenderContext* RSSurfaceWindows::GetRenderContext()
{
    return renderContext_;
}

void RSSurfaceWindows::SetRenderContext(RenderContext* context)
{
    renderContext_ = context;
}

ColorGamut RSSurfaceWindows::GetColorSpace()
{
    return colorSpace_;
}

void RSSurfaceWindows::SetColorSpace(ColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}
} // namespace Rosen
} // namespace OHOS
