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

#include "rs_surface_darwin.h"

#include <thread>

#include <GLES3/gl3.h>
#include <glfw_render_context.h>
#include <include/core/SkColorSpace.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <sync_fence.h>

#include "platform/common/rs_log.h"
#include "rs_surface_frame_darwin.h"

namespace OHOS {
namespace Rosen {
RSSurfaceDarwin::RSSurfaceDarwin(const sptr<Surface>& producer)
    : producer_(producer)
{
}

bool RSSurfaceDarwin::IsValid() const
{
    return producer_ != nullptr;
}

sptr<Surface> RSSurfaceDarwin::GetSurface() const
{
    return producer_;
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceDarwin::RequestFrame(int32_t width, int32_t height, uint64_t uiTimestamp)
{
    if (producer_ == nullptr) {
        ROSEN_LOGE("RSSurfaceDarwin::RequestFrame, producer is nullptr");
        return nullptr;
    }

    auto frame = std::make_unique<RSSurfaceFrameDarwin>(width, height);
    int32_t releaseFence = -1;
    SurfaceError err = producer_->RequestBuffer(frame->buffer_, releaseFence, frame->requestConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceDarwin::Requestframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return nullptr;
    }

    sptr<SyncFence> tempFence = new SyncFence(releaseFence);
    int res = tempFence->Wait(3000);
    if (res < 0) {
        ROSEN_LOGW("RsDebug RSProcessor::RequestFrame this buffer is not available");
    }

    if (SetupGrContext() == false) {
        return frame;
    }

    constexpr auto colorType = kRGBA_8888_SkColorType;
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
    GrBackendRenderTarget backendRenderTarget(
        frame->buffer_->GetWidth(), frame->buffer_->GetHeight(),
        0, 8, {.fFBOID = 0, .fFormat = GL_RGBA8});
    frame->surface_ = SkSurface::MakeFromBackendRenderTarget(grContext_.get(),
                                                             backendRenderTarget,
                                                             kTopLeft_GrSurfaceOrigin,
                                                             colorType, skColorSpace_, &surfaceProps);
#ifdef USE_GLFW_WINDOW
    if (frame->surface_ != nullptr) {
        const auto &canvas = frame->surface_->getCanvas();
        if (canvas != nullptr) {
            canvas->translate(0, frame->buffer_->GetHeight());
            canvas->scale(1, -1);
        }
    }
#endif

    return frame;
}

bool RSSurfaceDarwin::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (frame == nullptr) {
        ROSEN_LOGE("RSSurfaceFrame::FlushFrame frame is nullptr");
        return false;
    }

    // RSSurfaceDarwin is the class for platform Darwin,
    // the input pointer should be the pointer to the class RSSurfaceFrameDarwin.
    // We use reinterpret_cast instead of RTTI and dynamic_cast which are not permitted.
    auto frameDarwin = reinterpret_cast<RSSurfaceFrameDarwin *>(frame.get());
    void *addr = nullptr;
    if (frameDarwin->buffer_ == nullptr) {
        ROSEN_LOGW("RSSurfaceDarwin::FlushFrame frame.buffer is nullptr");
    } else {
        addr = frameDarwin->buffer_->GetVirAddr();
    }

    if (addr == nullptr) {
        ROSEN_LOGW("RSSurfaceDarwin::FlushFrame buffer.addr is nullptr");
    } else {
        constexpr auto colorType = kRGBA_8888_SkColorType;
        SkBitmap bitmap;
        bitmap.setInfo(SkImageInfo::Make(frameDarwin->buffer_->GetWidth(),
                                         frameDarwin->buffer_->GetHeight(),
                                         colorType, kPremul_SkAlphaType));
        bitmap.setPixels(addr);
        if (frameDarwin->surface_ != nullptr) {
            frameDarwin->surface_->readPixels(bitmap, 0, 0);
        }

#ifdef USE_GLFW_WINDOW
        if (frameDarwin->surface_ != nullptr) {
            YInvert(addr, frameDarwin->buffer_->GetWidth(), frameDarwin->buffer_->GetHeight());
        }
#endif
    }

    SurfaceError err = producer_->FlushBuffer(frameDarwin->buffer_, -1, frameDarwin->flushConfig_);
    if (err != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSSurfaceDarwin::Flushframe Failed, error is : %s", SurfaceErrorStr(err).c_str());
        return false;
    }

#ifdef USE_GLFW_WINDOW
    GlfwRenderContext::GetGlobal()->SwapBuffers();
#endif
    return true;
}

RenderContext* RSSurfaceDarwin::GetRenderContext()
{
    return renderContext_;
}

void RSSurfaceDarwin::SetRenderContext(RenderContext* context)
{
    renderContext_ = context;
}

void RSSurfaceDarwin::YInvert(void *addr, int32_t width, int32_t height)
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

bool RSSurfaceDarwin::SetupGrContext()
{
    if (grContext_ != nullptr) {
        return true;
    }

    GlfwRenderContext::GetGlobal()->MakeCurrent();
    sk_sp<const GrGLInterface> glinterface{GrGLCreateNativeInterface()};
    if (glinterface == nullptr) {
        ROSEN_LOGE("glinterface is nullptr");
        return false;
    }

    GrContextOptions options;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    const auto &grContext = GrContext::MakeGL(glinterface, options);
    if (grContext == nullptr) {
        ROSEN_LOGE("grContext is nullptr");
        return false;
    }

    grContext_ = grContext;
    return true;
}

uint32_t RSSurfaceDarwin::GetQueueSize() const
{
    return producer_->GetQueueSize();
}

void RSSurfaceDarwin::ClearBuffer()
{
    if (producer_ != nullptr) {
        ROSEN_LOGI("RSSurfaceDarwin: Clear surface buffer!");
        producer_->GoBackground();
    }
}

void RSSurfaceDarwin::ClearAllBuffer()
{
    if (producer_ != nullptr) {
        producer_->Disconnect();
    }
}

void RSSurfaceDarwin::ResetBufferAge()
{
    ROSEN_LOGD("RSSurfaceDarwin: Reset Buffer Age!");
}
} // namespace Rosen
} // namespace OHOS
