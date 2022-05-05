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


#include "pipeline/rs_software_processor.h"

#include <cinttypes>
#include "sync_fence.h"

#include "include/core/SkMatrix.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_util.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"

namespace OHOS {
namespace Rosen {

RSSoftwareProcessor::RSSoftwareProcessor() {}

RSSoftwareProcessor::~RSSoftwareProcessor() {}

void RSSoftwareProcessor::Init(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    offsetX_ = offsetX;
    offsetY_ = offsetY;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSSoftwareProcessor::Init: failed to get screen manager!");
        return;
    }

    producerSurface_ = screenManager->GetProducerSurface(id);
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSSoftwareProcessor::Init for Screen(id %{public}" PRIu64 "): ProducerSurface is null!", id);
        return;
    }
    currScreenInfo_ = screenManager->QueryScreenInfo(id);
    BufferRequestConfig requestConfig = {
        .width = currScreenInfo_.width,
        .height = currScreenInfo_.height,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    rsSurface_ = std::make_shared<RSSurfaceOhosRaster>(producerSurface_);
    auto skCanvas = CreateCanvas(rsSurface_, requestConfig);
    if (skCanvas == nullptr) {
        RS_LOGE("RSSoftwareProcessor canvas is null.");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas);
}

void RSSoftwareProcessor::PostProcess()
{
    SetBufferTimeStamp();
    if (!rsSurface_) {
        RS_LOGE("RSSoftwareProcessor::PostProcess rsSurface_ is null.");
        return;
    }
    rsSurface_->FlushFrame(currFrame_);
}

void RSSoftwareProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        RS_LOGE("RSSoftwareProcessor::ProcessSurface: Canvas is null!");
        return;
    }
    auto consumerSurface = node.GetConsumer();
    if (!consumerSurface) {
        RS_LOGE("RSSoftwareProcessor::ProcessSurface: node's consumerSurface is null!");
        return;
    }

    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    if (node.GetAvailableBufferCount() > 0) {
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        auto sret = consumerSurface->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
        if (sret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGE("RSSoftwareProcessor::ProcessSurface: AcquireBuffer failed!");
            return;
        }

        if (cbuffer != node.GetBuffer() && node.GetBuffer() != nullptr) {
            SurfaceError ret = consumerSurface->ReleaseBuffer(node.GetBuffer(), SyncFence::INVALID_FENCE);
            if (ret != SURFACE_ERROR_OK) {
                RS_LOGE("RSSoftwareProcessor::ProcessSurface: ReleaseBuffer buffer error! error: %{public}d.", ret);
                return;
            }
        }

        node.SetBuffer(cbuffer);
        node.SetFence(acquireFence);

        if (node.ReduceAvailableBuffer() > 0) {
            if (auto mainThread = RSMainThread::Instance()) {
                mainThread->RequestNextVSync();
            }
        }
    } else {
        cbuffer = node.GetBuffer();
    }

    if (cbuffer == nullptr) {
        RS_LOGE("RSSoftwareProcessor::ProcessSurface: surface buffer is null!");
        return;
    }

    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RsDebug RSSoftwareProcessor::ProcessSurface geoPtr == nullptr");
        return;
    }
    node.SetDstRect({geoPtr->GetAbsRect().left_ - offsetX_, geoPtr->GetAbsRect().top_ - offsetY_,
        geoPtr->GetAbsRect().width_, geoPtr->GetAbsRect().height_});
    auto params = RsRenderServiceUtil::CreateBufferDrawParam(node);
    RsRenderServiceUtil::DrawBuffer(*canvas_, params);
}
} // namespace Rosen
} // namespace OHOS
