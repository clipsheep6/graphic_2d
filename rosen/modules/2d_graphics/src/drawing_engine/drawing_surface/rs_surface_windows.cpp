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

#include <GLES3/gl3.h>
#include <glfw_render_context.h>
#include <include/core/SkColorSpace.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <sync_fence.h>

#include "drawing_engine/drawing_utils.h"
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
        LOGE("RSSurfaceWindows::RequestFrame, producer is nullptr");
        return nullptr;
    }

    auto frame = std::make_unique<RSSurfaceFrameWindows>(width, height);
    int32_t releaseFence = -1;
    SurfaceError err = producer_->RequestBuffer(frame->buffer_, releaseFence, frame->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        LOGE("RSSurfaceWindows::Requestframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        LOGW("RsDebug RSProcessor::RequestFrame this buffer is not available");
    }

    return frame;
}

bool RSSurfaceWindows::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        LOGE("RSSurfaceFrame::FlushFrame frame is nullptr");
        return false;
    }

    // RSSurfaceWindows is the class for platform Windows,
    // the input pointer should be the pointer to the class RSSurfaceFrameWindows.
    // We use reinterpret_cast instead of RTTI and dynamic_cast which are not permitted.
    auto frameWindows = reinterpret_cast<RSSurfaceFrameWindows *>(frame.get());
    void *addr = nullptr;
    if (frameWindows->buffer_ == nullptr) {
        LOGW("RSSurfaceWindows::FlushFrame frame.buffer is nullptr");
    } else {
        addr = frameWindows->buffer_->GetVirAddr();
    }

    if (addr == nullptr) {
        LOGW("RSSurfaceWindows::FlushFrame buffer.addr is nullptr");
    } else {
        constexpr auto colorType = kRGBA_8888_SkColorType;
        SkBitmap bitmap;
        bitmap.setInfo(SkImageInfo::Make(frameWindows->buffer_->GetWidth(),
                                         frameWindows->buffer_->GetHeight(),
                                         colorType, kPremul_SkAlphaType));
        bitmap.setPixels(addr);
        if (surface_ != nullptr) {
            surface_->readPixels(bitmap, 0, 0);
        }

#ifdef USE_GLFW_WINDOW
        YInvert(addr, frameWindows->buffer_->GetWidth(), frameWindows->buffer_->GetHeight());
#endif
    }

    SurfaceError err = producer_->FlushBuffer(frameWindows->buffer_, -1, frameWindows->flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        LOGE("RSSurfaceWindows::Flushframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return false;
    }

#ifdef USE_GLFW_WINDOW
    GlfwRenderContext::GetGlobal()->SwapBuffers();
#endif
    return true;
}

SkCanvas* RSSurfaceWindows::GetCanvas(std::unique_ptr<RSSurfaceFrame>& frame)
{
    if (frame == nullptr) {
        LOGE("RSSurfaceWindows::GetCanvas frame is nullptr");
        return nullptr;
    }

    // RSSurfaceWindows is the class for platform Windows,
    // the input pointer should be the pointer to the class RSSurfaceFrameWindows.
    // We use reinterpret_cast instead of RTTI and dynamic_cast which are not permitted.
    auto frameWindows = reinterpret_cast<RSSurfaceFrameWindows *>(frame.get());

    if (surface_ == nullptr && SetupGrContext() != false) {
        constexpr auto colorType = kRGBA_8888_SkColorType;
        SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
        GrBackendRenderTarget backendRenderTarget(
            frameWindows->buffer_->GetWidth(), frameWindows->buffer_->GetHeight(),
            0, 8, {.fFBOID = 0, .fFormat = GL_RGBA8});
        surface_ = SkSurface::MakeFromBackendRenderTarget(grContext_.get(),
                                                                 backendRenderTarget,
                                                                 kTopLeft_GrSurfaceOrigin,
                                                                 colorType,
                                                                 frameWindows->skColorSpace_,
                                                                 &surfaceProps);

#ifdef USE_GLFW_WINDOW
        if (surface_ != nullptr) {
            const auto &canvas = surface_->getCanvas();
            if (canvas != nullptr) {
                canvas->translate(0, frameWindows->buffer_->GetHeight());
                canvas->scale(1, -1);
            }
        }
#endif
    }

    if (surface_ != nullptr) {
        return surface_->getCanvas();
    }

    if (canvas_ == nullptr) {
        if (frameWindows->buffer_ == nullptr) {
            LOGW("RSSurfaceWindows::GetCanvas buffer is nullptr");
            return nullptr;
        }

        const auto &addr = reinterpret_cast<uint32_t *>(frameWindows->buffer_->GetVirAddr());
        if (addr == nullptr) {
            LOGW("RSSurfaceWindows::GetCanvas buffer.addr is nullptr");
            return nullptr;
        }

        const auto &width = frameWindows->buffer_->GetWidth();
        const auto &height = frameWindows->buffer_->GetHeight();
        const auto &size = frameWindows->buffer_->GetSize();
        const auto &info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
        canvas_ = SkCanvas::MakeRasterDirect(info, addr, size / height);
    }

    return canvas_.get();
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

bool RSSurfaceWindows::SetupGrContext()
{
    if (grContext_ != nullptr) {
        return true;
    }

    GlfwRenderContext::GetGlobal()->MakeCurrent();
    sk_sp<const GrGLInterface> glinterface{GrGLCreateNativeInterface()};
    if (glinterface == nullptr) {
        LOGE("glinterface is nullptr");
        return false;
    }

    GrContextOptions options;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    grContext_ = GrContext::MakeGL(glinterface, options);
    if (grContext_ == nullptr) {
        LOGE("grContext is nullptr");
        return false;
    }

    return true;
}
} // namespace Rosen
} // namespace OHOS
