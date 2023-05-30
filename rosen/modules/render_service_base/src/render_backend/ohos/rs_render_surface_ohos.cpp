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

#include "ohos/rs_render_surface_ohos.h"

#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include <hilog/log.h>
#include "window.h"
#include "include/core/SkSurface.h"

#if !defined(NEW_SKIA)
#include "memory/rs_tag_tracker.h"
#endif

namespace OHOS {
namespace Rosen {
RSRenderSurfaceOhos::RSRenderSurfaceOhos(const sptr<Surface>& producer, const std::shared_ptr<DrawingContext>& drawingContext)
{
    frame_ = std::make_shared<RSRenderSurfaceFrame>();
    frame_->producer_ = producer;
    drawingContext_ = drawingContext;
}

RSRenderSurfaceOhos::~RSRenderSurfaceOhos()
{
    frame_ = nullptr;
    drawingContext_ = nullptr;
    renderContext_ = nullptr;
}
bool RSRenderSurfaceOhos::IsValid()
{
    return frame_->producer_ != nullptr;
}

sptr<Surface> RSRenderSurfaceOhos::GetSurface() const
{
    return frame_->producer_;
}

uint32_t RSRenderSurfaceOhos::GetQueueSize() const
{
    return frame_->producer_->GetQueueSize();
}

std::shared_ptr<RSRenderSurfaceFrame> RSRenderSurfaceOhos::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC)
{
    if (renderContext_ == nullptr) {
        ROSEN_LOGE("RSRenderSurfaceOhos::RequestFrame, renderContext_ is nullptr");
        return nullptr;
    }
    
    frame_->width_ = width;
    frame_->height_ = height;
    frame_->uiTimestamp_ = uiTimestamp;
    
    bool isSuccess = renderContext_->CreateSurface(frame_);
    if (!isSuccess) {
        ROSEN_LOGE("RSRenderSurfaceOhos::RequestFrame, failed to create surface");
        return nullptr;
    }

    ROSEN_LOGE("ZJ RSRenderSurfaceOhos::RequestFrame, width is %d, height is %d",
        width, height);

    return frame_;
}

bool RSRenderSurfaceOhos::FlushFrame(uint64_t uiTimestamp)
{
    if (renderContext_ == nullptr) {
        ROSEN_LOGE("RSRenderSurfaceOhos::FlushFrame, GetRenderContext failed!");
        return false;
    }

    // gles render flush
    RenderType renderType = renderContext_->GetRenderType();
    if (renderType == RenderType::GLES) {
        RenderFrame();
    }
    renderContext_->SwapBuffers(frame_);
    ROSEN_LOGE("ZJ RSRenderSurfaceOhos:: FlushFrame, SwapBuffers eglsurface");
    return true;
}

void RSRenderSurfaceOhos::SetUiTimeStamp(uint64_t uiTimestamp)
{
    RenderType renderType = renderContext_->GetRenderType();
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    if (renderType == RenderType::GLES || renderType == RenderType::VULKAN) {
        NativeWindowHandleOpt(frame_->nativeWindow_, SET_UI_TIMESTAMP, duration);
    } else if (renderType == RenderType::RASTER) {
        if (frame_->buffer_ == nullptr) {
            RS_LOGE("RSRenderSurfaceOhos::SetUiTimeStamp buffer is nullptr");
            return;
        }
        GSError ret = frame_->buffer_->GetExtraData()->ExtraSet("timeStamp", static_cast<int64_t>(duration));
        if (ret != GSERROR_OK) {
            RS_LOGE("RSRenderSurfaceOhos::SetUiTimeStamp buffer ExtraSet failed");
        }
    } else {
        RS_LOGE("RSRenderSurfaceOhos::SetUiTimeStamp unknown render type");
    }
}

void RSRenderSurfaceOhos::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    renderContext_->DamageFrame(left, top, width, height, frame_);
}

void RSRenderSurfaceOhos::SetDamageRegion(const std::vector<RectI> &rects)
{
    renderContext_->DamageFrame(rects);
}

int32_t RSRenderSurfaceOhos::GetBufferAge()
{
    return renderContext_->GetBufferAge();
}

void RSRenderSurfaceOhos::ClearBuffer()
{
    RenderType renderType = renderContext_->GetRenderType();
    if (renderType == RenderType::GLES) {
        DestoryNativeWindow(frame_->nativeWindow_);
        renderContext_->MakeCurrent(EGL_NO_SURFACE);
        renderContext_->DestroySurface();
    }
    if (frame_->producer_ != nullptr) {
        ROSEN_LOGD("RSRenderSurfaceOhos:: Clear surface buffer!");
        frame_->producer_->GoBackground();
    }
}

SkCanvas* RSRenderSurfaceOhos::GetSkCanvas()
{
    if (frame_->skSurface_ == nullptr) {
        ROSEN_LOGE("RSRenderSurfaceOhos::GetSkCanvas frame_->skSurface_ is nullptr");
        RenderType renderType = renderContext_->GetRenderType();
        frame_->skSurface_ = drawingContext_->AcquireSurface(frame_, renderType);
    }
    return frame_->skSurface_->getCanvas();
}

sk_sp<SkSurface> RSRenderSurfaceOhos::GetSkSurface()
{
    if (frame_->skSurface_ == nullptr) {
        ROSEN_LOGE("RSRenderSurfaceOhos::GetSkSurface frame_->skSurface_ is nullptr");
        RenderType renderType = renderContext_->GetRenderType();
        frame_->skSurface_ = drawingContext_->AcquireSurface(frame_, renderType);
    }
    return frame_->skSurface_;
}
#ifndef ROSEN_CROSS_PLATFORM
ColorGamut RSRenderSurfaceOhos::GetColorSpace()
{
    return frame_->colorSpace_;
}
void RSRenderSurfaceOhos::SetColorSpace(ColorGamut colorSpace)
{
    frame_->colorSpace_ = colorSpace;
}
#endif
void RSRenderSurfaceOhos::SetSurfaceBufferUsage(uint64_t usage)
{
    frame_->bufferUsage_ = usage;
}

void RSRenderSurfaceOhos::SetSurfacePixelFormat(uint64_t pixelFormat)
{
    frame_->pixelFormat_ = pixelFormat;
}

void RSRenderSurfaceOhos::SetReleaseFence(const int32_t& fence)
{
    frame_->releaseFence_ = fence;
}

int32_t RSRenderSurfaceOhos::GetReleaseFence() const
{
    return frame_->releaseFence_;
}

void RSRenderSurfaceOhos::RenderFrame()
{
#ifdef RS_ENABLE_GL
    RS_TRACE_FUNC();
    // flush commands
    if (frame_->skSurface_->getCanvas() != nullptr) {
        ROSEN_LOGD("RenderFrame: Canvas");
        frame_->skSurface_->getCanvas()->flush();
    } else {
        ROSEN_LOGE("canvas is nullptr!!!");
    }
#endif
}
}
}