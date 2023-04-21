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

#include "software_render_backend.h"

#include "drawing_utils.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"

namespace OHOS {
namespace Rosen {
sk_sp<SkSurface> SoftwareRenderBackend::AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame)
{
    RSSurfaceFrameOhosRaster* framePtr = static_cast<RSSurfaceFrameOhosRaster*>(frame.get());
    sptr<SurfaceBuffer> buffer = framePtr->GetBuffer();
    if ((buffer == nullptr) || (buffer->GetWidth() <= 0) || (buffer->GetHeight() <= 0)) {
        LOGE("Buffer is invalide");
        return nullptr;
    }

    auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
    if (addr == nullptr) {
        LOGE("buffer addr is invalid");
        return nullptr;
    }
    SkImageInfo info =
        SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    return SkSurface::MakeRasterDirect(info, addr, buffer->GetStride());
}

void SoftwareRenderBackend::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    auto frameOhosRaster =  static_cast<RSSurfaceFrameOhosRaster *>(frame.get());
    if (frameOhosRaster == nullptr || frameOhosRaster->GetBuffer() == nullptr) {
        LOGE("RSSurfaceOhosRaster::SetUiTimeStamp: buffer is nullptr");
        return;
    }

    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    GSError ret = frameOhosRaster->GetBuffer()->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
    if (ret != GSERROR_OK) {
        LOGE("RSSurfaceOhosRaster::SetUiTimeStamp buffer ExtraSet failed");
    }
}
}
}