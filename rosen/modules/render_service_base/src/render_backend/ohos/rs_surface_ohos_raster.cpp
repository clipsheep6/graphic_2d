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

#include "ohos/rs_surface_ohos_raster.h"

#include <sync_fence.h>

#include "platform/common/rs_log.h"

#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSSurfaceFrame> RSSurfaceOhosRaster::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC)
{
    if (producer_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosRaster::RequestFrame, producer is nullptr");
        return nullptr;
    }

    frame_->width_ = width;
    frame_->height_ = height;

    frame_->requestConfig_.width = width;
    frame_->requestConfig_.height = height;
    frame_->flushConfig_.damage.w = width;
    frame_->flushConfig_.damage.h = height;

    frame_->requestConfig_.usage = bufferUsage_;
    frame_->requestConfig_.format = frame_->pixelFormat_;
    SurfaceError err = producer_->RequestBuffer(frame_->buffer_, frame_->releaseFence_, frame_->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Requestframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }

    err = frame_->buffer_->Map();
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Map Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }


    sptr<SyncFence> tempFence = new SyncFence(frame_->releaseFence_);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        ROSEN_LOGE("RsDebug RSSurfaceOhosRaster::RequestFrame this buffer is not available");
    }
    ROSEN_LOGD("RSSurfaceOhosRaster::RequestFrame");
    return frame_;
}

bool RSSurfaceOhosRaster::FlushFrame(uint64_t uiTimestamp)
{
    // RSSurfaceOhosRaster is the class for platform OHOS, the input pointer should be the pointer to the class
    // RSSurfaceFrameOhos.
    // We use static_cast instead of RTTI and dynamic_cast which are not permitted
    if (frame_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Flushframe Failed, frame is nullptr");
        return false;
    }
    frame_->flushConfig_.timestamp = static_cast<int64_t>(uiTimestamp);
    SurfaceError err = producer_->FlushBuffer(frame_->buffer_, -1, frame_->flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosRaster::Flushframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return false;
    }
    ROSEN_LOGD("RSSurfaceOhosRaster::FlushFrame fence:%d", frame_->releaseFence_);
    return true;
}

void RSSurfaceOhosRaster::SetUiTimeStamp(uint64_t uiTimestamp)
{
    if (frame_ == nullptr || frame_->buffer_ == nullptr) {
        RS_LOGE("RSSurfaceOhosRaster::SetUiTimeStamp: buffer is nullptr");
        return;
    }

    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    GSError ret = frame_->buffer_->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
    if (ret != GSERROR_OK) {
        RS_LOGE("RSSurfaceOhosRaster::SetUiTimeStamp buffer ExtraSet failed");
    }
}

void RSSurfaceOhosRaster::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    frame_->flushConfig_.damage.x = left;
    frame_->flushConfig_.damage.y = top;
    frame_->flushConfig_.damage.w = width;
    frame_->flushConfig_.damage.h = height;
    ROSEN_LOGD("RSSurfaceOhosRaster::SetDamageRegion Set single dirty region");
}

void RSSurfaceOhosRaster::SetDamageRegion(const std::vector<RectI> &rects)
{
    ROSEN_LOGE("RSSurfaceOhosRaster::SetDamageRegion not support many dirty regions");
}

int32_t RSSurfaceOhosRaster::GetBufferAge()
{
    ROSEN_LOGE("RSSurfaceOhosRaster::GetBufferAge failed");
    return -1;
}

void RSSurfaceOhosRaster::ClearBuffer()
{
    if (producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosRaster: Clear surface buffer!");
        producer_->GoBackground();
    }
}

sk_sp<SkSurface> RSSurfaceOhosRaster::AcquireSurface()
{
    sptr<SurfaceBuffer> buffer = frame_->buffer_;
    if ((buffer == nullptr) || (buffer->GetWidth() <= 0) || (buffer->GetHeight() <= 0)) {
        ROSEN_LOGE("Buffer is invalide");
        return nullptr;
    }

    auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
    if (addr == nullptr) {
        ROSEN_LOGE("buffer addr is invalid");
        return nullptr;
    }
    SkImageInfo info =
        SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    sk_sp<SkSurface> skSurface = SkSurface::MakeRasterDirect(info, addr, buffer->GetStride());

    ROSEN_LOGE("RSSurfaceOhosRaster::AcquireSurface success");
    
    return skSurface;
}

}
}