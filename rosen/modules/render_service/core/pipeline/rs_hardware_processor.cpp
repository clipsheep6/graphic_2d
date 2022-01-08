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

#include "pipeline/rs_hardware_processor.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_util.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSHardwareProcessor::RSHardwareProcessor() {}

RSHardwareProcessor::~RSHardwareProcessor() {}

void RSHardwareProcessor::Init(ScreenId id)
{
    backend_ = HdiBackend::GetInstance();
    backend_->RegPrepareComplete(std::bind(&RSHardwareProcessor::Redraw, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3), nullptr);
    screenManager_ = CreateOrGetScreenManager();
    if (!screenManager_) {
        ROSEN_LOGE("RSHardwareProcessor::Init ScreenManager is nullptr");
        return;
    }
    output_ = screenManager_->GetOutput(id);
}

void RSHardwareProcessor::PostProcess()
{
    if (output_ == nullptr) {
        ROSEN_LOGE("RSHardwareProcessor::PostProcess output is nullptr");
        return;
    }

    output_->SetLayerInfo(layers_);
    std::vector<std::shared_ptr<HdiOutput>> outputs{output_};
    if (backend_) {
        backend_->Repaint(outputs);
    }
}

void RSHardwareProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    ROSEN_LOGI("RsDebug RSHardwareProcessor::ProcessSurface start node id:%llu available buffer:%d", node.GetId(),
        node.GetAvailableBufferCount());
    if (!output_) {
        ROSEN_LOGE("RSHardwareProcessor::ProcessSurface output is nullptr");
        return;
    }
    OHOS::sptr<SurfaceBuffer> cbuffer;
    RSProcessor::SpecialTask task = [] () -> void{};
    bool ret = ConsumeAndUpdateBuffer(node, task, cbuffer);
    if (!ret) {
        ROSEN_LOGE("RsDebug RSCompatibleProcessor::ProcessSurface consume buffer fail");
        return;
    }
    ComposeInfo info = {
        .srcRect = {
            .x = 0,
            .y = 0,
            .w = node.GetBuffer()->GetWidth(),
            .h = node.GetBuffer()->GetHeight(),
        },
        .dstRect = {
            .x = node.GetRenderProperties().GetBoundsPositionX(),
            .y = node.GetRenderProperties().GetBoundsPositionY(),
            .w = node.GetRenderProperties().GetBoundsWidth(),
            .h = node.GetRenderProperties().GetBoundsHeight(),
        },
        .zOrder = node.GetRenderProperties().GetPositionZ(),
        .alpha = alpha_,
        .buffer = node.GetBuffer(),
        .fence = node.GetFence(),
    };
    std::shared_ptr<HdiLayerInfo> layer = HdiLayerInfo::CreateHdiLayerInfo();
    ROSEN_LOGE("RsDebug RSHardwareProcessor::ProcessSurface surfaceNode id:%llu [%d %d %d %d]"\
        "buffer [%d %d] buffaddr:%p, z:%d", node.GetId(), info.dstRect.x, info.dstRect.y, info.dstRect.w,
        info.dstRect.h, info.srcRect.w, info.srcRect.h, node.GetBuffer().GetRefPtr(), info.zOrder);
    RsRenderServiceUtil::ComposeSurface(layer, node.GetConsumer(), layers_, info);
}

void RSHardwareProcessor::Redraw(sptr<Surface>& surface, const struct PrepareCompleteParam& param, void* data)
{
    if (!param.needFlushFramebuffer) {
        return;
    }
    //TODO config size update
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    auto canvas = CreateCanvas(surface, requestConfig);
    for (auto layer : param.layers) {
        SkMatrix matrix;
        matrix.reset();
        matrix.setTranslate(layer->GetLayerSize().x, layer->GetLayerSize().y);
        RsRenderServiceUtil::DrawBuffer(canvas.get(), matrix, layer->GetBuffer(), 0, 0, 2800, 1600);
    }
    BufferFlushConfig flushConfig = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = 0x100,
            .h = 0x100,
        },
    };
    FlushBuffer(surface, flushConfig);
}

} // namespace Rosen
} // namespace OHOS