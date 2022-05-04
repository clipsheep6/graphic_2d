/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "rs_surface_frame_windows.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSurfaceFrameWindows::RSSurfaceFrameWindows(int32_t width, int32_t height)
{
    requestConfig_ = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    flushConfig_ = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = width,
            .h = height,
        },
    };
}

void RSSurfaceFrameWindows::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    flushConfig_.damage.x = left;
    flushConfig_.damage.y = top;
    flushConfig_.damage.w = width;
    flushConfig_.damage.h = height;
}

SkCanvas* RSSurfaceFrameWindows::GetCanvas()
{
    if (surface_ != nullptr) {
        return surface_->getCanvas();
    }

    if (canvas_ == nullptr) {
        if (buffer_ == nullptr) {
            ROSEN_LOGW("RSSurfaceFrameWindows::GetCanvas buffer is nullptr");
            return nullptr;
        }

        const auto &addr = reinterpret_cast<uint32_t *>(buffer_->GetVirAddr());
        if (addr == nullptr) {
            ROSEN_LOGW("RSSurfaceFrameWindows::GetCanvas buffer.addr is nullptr");
            return nullptr;
        }

        const auto &width = buffer_->GetWidth();
        const auto &height = buffer_->GetHeight();
        const auto &size = buffer_->GetSize();
        const auto &info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
        canvas_ = SkCanvas::MakeRasterDirect(info, addr, size / height);
    }

    return canvas_.get();
}

void RSSurfaceFrameWindows::SetRenderContext(RenderContext* context)
{
    renderContext_ = context;
}
} // namespace Rosen
} // namespace OHOS
