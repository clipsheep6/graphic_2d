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

#include "layer_context.h"

#include <securec.h>
#include <sync_fence.h>
#include "hdi_log.h"


using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

namespace {
#define LOGI(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(            \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloComposer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define LOGE(fmt, ...) ::OHOS::HiviewDFX::HiLog::Error(           \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloComposer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
}

LayerContext::LayerContext(IRect dst, IRect src, uint32_t zorder, LayerType layerType)
    : dst_(dst), src_(src), zorder_(zorder), layerType_(layerType)
{
    cSurface_ = Surface::CreateSurfaceAsConsumer();
    cSurface_->SetDefaultWidthAndHeight(src.w, src.h);
    cSurface_->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);

    OHOS::sptr<IBufferProducer> producer = cSurface_->GetProducer();
    pSurface_ = Surface::CreateSurfaceAsProducer(producer);
    cSurface_->RegisterConsumerListener(this);

    hdiLayer_ = HdiLayerInfo::CreateHdiLayerInfo();
    LOGI("%{public}s: create surface w:%{public}d, h:%{public}d", __func__, src.w, src.h);
}

LayerContext::~LayerContext()
{
    cSurface_ = nullptr;
    pSurface_ = nullptr;
    prevBuffer_ = nullptr;
    hdiLayer_ = nullptr;
}

void LayerContext::OnBufferAvailable()
{
}

const std::shared_ptr<HdiLayerInfo> LayerContext::GetHdiLayer()
{
    return hdiLayer_;
}

SurfaceError LayerContext::InitContext()
{
    sEglManager_ = EglManager::GetInstance();
    if (sEglManager_ == nullptr) {
        LOGE("EglManager::GetInstance Failed.");
        return SURFACE_ERROR_INIT;
    }

    if (sEglManager_->Init(EGL_NO_CONTEXT,pSurface_) != SURFACE_ERROR_OK) {
        LOGE("EglManager init failed.");
        return SURFACE_ERROR_INIT;
    }
/*
    if (GetEglSurface() == EGL_NO_SURFACE) {
        LOGE("use offscreen render first resqueset the buffer");
        if (RequestBufferProc() != SURFACE_ERROR_OK) {
            LOGE("RequestBufferProc failed.");
            return SURFACE_ERROR_INIT;
        }
    }*/
    return SURFACE_ERROR_OK;
}

SurfaceError LayerContext::InitEGL()
{
    if (isEglInit){
        LOGE("EGL INITED");
        return SURFACE_ERROR_OK;
    }
    if (InitContext() != SURFACE_ERROR_OK){
        LOGE("failed to init Contex");
        return SURFACE_ERROR_INIT;
    }
    isEglInit = true;
    return SURFACE_ERROR_OK;

}

SurfaceError LayerContext::SwapBuffers()
{
    if (!isEglInit) {
        LOGE("egl init is not init.");
        return SURFACE_ERROR_INIT;
    }
    LOGE("use onscreen swap");
    sEglManager_->SwapBuffer();
    LOGE("SwapBuffer end");

    return SURFACE_ERROR_OK;
}

SurfaceError LayerContext::DrawColorWithEGL()
{
    LOGE("DrawColorWithEGL begain");
    SurfaceError ret = InitEGL();
    if (ret != SURFACE_ERROR_OK) {
        LOGE("failed to InitEGL");
        return SURFACE_ERROR_INIT;
    }
    glClearColor(1.0,1.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    LOGE("swap buffer begain");
    SwapBuffers();
    LOGE("swap buffer end");
    return SURFACE_ERROR_OK;

}

SurfaceError LayerContext::DrawBufferColor()
{
    LOGE("DrawBufferColor start");
    return DrawColorWithEGL();
    OHOS::sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    BufferRequestConfig config = {
        .width = src_.w,
        .height = src_.h,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = pSurface_->GetDefaultUsage(),
    };

    SurfaceError ret = pSurface_->RequestBuffer(buffer, releaseFence, config);
    if (ret != 0) {
        LOGE("RequestBuffer failed: %{public}s", SurfaceErrorStr(ret).c_str());
        return ret;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    tempFence->Wait(100); // 100 ms

    if (buffer == nullptr) {
        LOGE("%s: buffer is nullptr", __func__);
        return SURFACE_ERROR_NULLPTR;
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    LOGI("buffer w:%{public}d h:%{public}d stride:%{public}d", buffer->GetWidth(),
          buffer->GetHeight(), buffer->GetBufferHandle()->stride);
    DrawColor(addr, buffer->GetWidth(), buffer->GetHeight());

    BufferFlushConfig flushConfig = {
        .damage = {
        .w = src_.w,
        .h = src_.h,
        },
    };

    ret = pSurface_->FlushBuffer(buffer, -1, flushConfig);
    if (ret != SURFACE_ERROR_OK) {
        LOGE("FlushBuffer failed");
    }

    return ret;
}

SurfaceError LayerContext::FillHDILayer()
{
    LOGE("FillHDILayer start");
    OHOS::sptr<SurfaceBuffer> buffer = nullptr;
    int32_t acquireFence = -1;
    int64_t timestamp;
    Rect damage;
    SurfaceError ret = cSurface_->AcquireBuffer(buffer, acquireFence, timestamp, damage);
    if (ret != SURFACE_ERROR_OK) {
        LOGE("Acquire buffer failed");
        return ret;
    }

    LayerAlpha alpha = { .enPixelAlpha = true };

    hdiLayer_->SetSurface(cSurface_);
    hdiLayer_->SetBuffer(buffer, acquireFence, prevBuffer_, prevFence_);
    hdiLayer_->SetZorder(zorder_);
    hdiLayer_->SetAlpha(alpha);
    hdiLayer_->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    hdiLayer_->SetVisibleRegion(1, src_);
    hdiLayer_->SetDirtyRegion(src_);
    hdiLayer_->SetLayerSize(dst_);
    hdiLayer_->SetBlendType(BlendType::BLEND_SRCOVER);
    hdiLayer_->SetCropRect(src_);
    hdiLayer_->SetPreMulti(false);

    prevBuffer_ = buffer;
    prevFence_ = acquireFence;

    return ret;
}

void LayerContext::DrawColor(void *image, int width, int height)
{
    if (layerType_ >= LayerType::LAYER_EXTRA) {
        DrawExtraColor(image, width, height);
    } else {
        DrawBaseColor(image, width, height);
    }
}

void LayerContext::DrawExtraColor(void *image, uint32_t width, uint32_t height)
{
    frameCounter_ = frameCounter_ % 60; // 60 is cycle size
    if (frameCounter_ == 0) {
        colorIndex_ = colorIndex_ % colors_.size();
        color_ = colors_[colorIndex_];
        colorIndex_++;
    }

    uint32_t *pixel = static_cast<uint32_t *>(image);
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0;  y < height; y++) {
            *pixel++ = color_;
        }
    }

    frameCounter_++;
}

void LayerContext::DrawBaseColor(void *image, uint32_t width, uint32_t height)
{
    static uint32_t value = 0x00;
    if (layerType_ == LayerType::LAYER_STATUS) {
        value = 0xfff0000f;
    } else if (layerType_ == LayerType::LAYER_LAUNCHER) {
        value = 0xffffffff;
    } else {
        value = 0xff00ffff;
    }

    uint32_t *pixel = static_cast<uint32_t *>(image);
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0;  y < height; y++) {
            *pixel++ = value;
        }
    }
}
