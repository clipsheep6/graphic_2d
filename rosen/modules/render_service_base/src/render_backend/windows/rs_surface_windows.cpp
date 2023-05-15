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

#include "rs_skSurface_windows.h"

#include <include/core/SkColorSpace.h>
#include <include/gpu/gl/GrGLInterface.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSurfaceWindows::RSSurfaceWindows(OnRenderFunc onRender)
    : onRender_(onRender)
{
}

bool RSSurfaceWindows::IsValid()
{
    return onRender_ != nullptr;
}

uint32_t RSSurfaceWindows::GetQueueSize()
{
    return 0x03;
}

std::shared_ptr<RSSurfaceFrame> RSSurfaceWindows::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC)
{
    if (onRender_ == nullptr) {
        ROSEN_LOGE("RSSurfaceWindows::RequestFrame, producer is nullptr");
        return nullptr;
    }

    frame_->width_ = width;
    frame_->height_ = height_;

    frame_->skSurface_ = AcquireSurface();
    return frame_;
}

bool RSSurfaceWindows::FlushFrame(uint64_t uiTimestamp)
{
    if (frame_ == nullptr) {
        ROSEN_LOGE("RSSurfaceFrame::FlushFrame frame is nullptr");
        return false;
    }

    // RSSurfaceWindows is the class for platform Windows,
    // the input pointer should be the pointer to the class RSSurfaceFrameWindows.
    // We use reinterpret_cast instead of RTTI and dynamic_cast which are not permitted.
    void *addr = nullptr;
    if (frame_->addr_ == nullptr) {
        ROSEN_LOGW("RSSurfaceWindows::FlushFrame frame.addr is nullptr");
    } else {
        addr = frame_->addr_.get();
    }

    constexpr auto colorType = kRGBA_8888_SkColorType;
    SkBitmap bitmap;
    bitmap.setInfo(SkImageInfo::Make(frame_->width_,
                                     frame_->height_,
                                     colorType, kPremul_SkAlphaType));
    bitmap.setPixels(addr);
    if (frame_->skSurface_ != nullptr) {
        frame_->skSurface_->readPixels(bitmap, 0, 0);
    }

#ifdef USE_GLFW_WINDOW
    if (frame_->skSurface_ != nullptr) {
        YInvert(addr, frame_->width_, frame_->height_);
    }
#endif

    int32_t width = frame_->width_;
    int32_t height = frame_->height_;
    int32_t size = width * height * 0x4;
    onRender_(addr, size, width, height);

    renderContext_->SwapBuffers();
    return true;
}

void RSSurfaceWindows::YInvert(void *addr, int32_t width, int32_t height)
{
    const auto &pixels = reinterpret_cast<uint32_t *>(addr);
    const auto &halfHeight = height / 0x2;
    const auto &tmpPixels = std::make_unique<uint32_t[]>(width * halfHeight);
    for (int32_t i = 0; i < halfHeight; i++) {
        for (int32_t j = 0; j < width; j++) {
            tmpPixels[i * width + j] = pixels[i * width + j];
        }
    }

    for (int32_t i = 0; i < halfHeight; i++) {
        const auto &r = height - 1 - i;
        for (int32_t j = 0; j < width; j++) {
            pixels[i * width + j] = pixels[r * width + j];
        }
    }

    for (int32_t i = 0; i < halfHeight; i++) {
        const auto &r = height - 1 - i;
        for (int32_t j = 0; j < width; j++) {
            pixels[r * width + j] = tmpPixels[i * width + j];
        }
    }
}

sk_sp<SkSurface> RSSurfaceWindows::AcquireSurface()
{
    if (!renderContext_->SetUpGrContext()) {
        ROSEN_LOGE("GrContext is not ready!!!");
        return nullptr;
    }
    GrContext* grContext = renderContext_->GetGrContext();
    constexpr auto colorType = kRGBA_8888_SkColorType;

    sk_sp<SkColorSpace> skColorSpace = nullptr;
#if defined(NEW_SKIA)
    SkSurfaceProps surfaceProps(0, kRGB_H_SkPixelGeometry);
#else
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
#endif
    constexpr uint32_t format = 0x8058; // GL_RGBA8
    GrBackendRenderTarget backendRenderTarget(
        frame_->width_, frame_->height_, 0, 8, {.fFBOID = 0, .fFormat = format});
    frame->skSurface_ = SkSurface::MakeFromBackendRenderTarget(grContext,
                                                             backendRenderTarget,
                                                             kTopLeft_GrSurfaceOrigin,
                                                             colorType, skColorSpace, &surfaceProps);
#ifdef USE_GLFW_WINDOW
    if (frame_->skSurface_ != nullptr) {
        const auto &canvas = frame_->skSurface_->getCanvas();
        if (canvas != nullptr) {
            canvas->translate(0, frame_->height_);
            canvas->scale(1, -1);
        }
    }
#endif
}
} // namespace Rosen
} // namespace OHOS
