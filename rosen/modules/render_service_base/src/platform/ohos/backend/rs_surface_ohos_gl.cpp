/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_surface_ohos_gl.h"
#include "platform/common/rs_log.h"
#include "window.h"
#include <hilog/log.h>
#include "pipeline/rs_render_thread.h"

namespace OHOS {
namespace Rosen {

RSSurfaceOhosGl::RSSurfaceOhosGl(const sptr<Surface>& producer) : RSSurfaceOhos(producer)
{
    bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
}

void RSSurfaceOhosGl::SetSurfaceBufferUsage(uint64_t usage)
{
    bufferUsage_ = usage;
}

RSSurfaceOhosGl::~RSSurfaceOhosGl()
{
    DestoryNativeWindow(mWindow);
    if (renderProxy_ != nullptr) {
        renderProxy_->DestroyEGLSurface(mEglSurface);
    }
    mWindow = nullptr;
    mEglSurface = EGL_NO_SURFACE;
    ROSEN_LOGD("RSSurfaceOhosGl::~RSSurfaceOhosGl");
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosGl::RequestFrame(int32_t width, int32_t height,
    uint64_t uiTimestamp, bool useAFBC)
{
    if (renderProxy_ == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosGl::RequestFrame, renderProxy_ is nullptr!");
        return nullptr;
    }
    renderProxy_->SetColorSpace(colorSpace_);
    if (mWindow == nullptr) {
        mWindow = CreateNativeWindowFromSurface(&producer_);
        mEglSurface = renderProxy_->CreateEGLSurface((EGLNativeWindowType)mWindow);
        ROSEN_LOGD("RSSurfaceOhosGl: create and Init EglSurface");
    }

    if (mEglSurface == EGL_NO_SURFACE) {
        ROSEN_LOGE("RSSurfaceOhosGl: Invalid eglSurface, return");
        return nullptr;
    }

    std::unique_ptr<RSSurfaceFrameOhosGl> frame = std::make_unique<RSSurfaceFrameOhosGl>(width, height);

#ifdef RS_ENABLE_AFBC
    int32_t format = 0;
    NativeWindowHandleOpt(mWindow, GET_FORMAT, &format);
    if (format == PIXEL_FMT_RGBA_8888 && useAFBC) {
        bufferUsage_ =
            (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
    }
#endif
    NativeWindowHandleOpt(mWindow, SET_USAGE, bufferUsage_);
    NativeWindowHandleOpt(mWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mWindow, SET_COLOR_GAMUT, colorSpace_);
    NativeWindowHandleOpt(mWindow, SET_UI_TIMESTAMP, uiTimestamp);

    renderProxy_->MakeCurrent(mEglSurface);

    ROSEN_LOGD("RSSurfaceOhosGl:RequestFrame, width is %d, height is %d",
        width, height);

    frame->SetRenderProxy(renderProxy_);

    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));

    return ret;
}

void RSSurfaceOhosGl::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    NativeWindowHandleOpt(mWindow, SET_UI_TIMESTAMP, duration);
}

bool RSSurfaceOhosGl::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (renderProxy_ == nullptr) {
        ROSEN_LOGD("RSSurfaceOhosGl::FlushFrame, GetrenderProxy_ failed!");
        return false;
    }

    // gpu render flush
    renderProxy_->RenderFrame();
    renderProxy_->SwapBuffers();
    ROSEN_LOGD("RSSurfaceOhosGl: FlushFrame, SwapBuffers eglsurface");
    return true;
}

void RSSurfaceOhosGl::ClearBuffer()
{
    if (renderProxy_ != nullptr && mEglSurface != EGL_NO_SURFACE && producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosGl: Clear surface buffer!");
        DestoryNativeWindow(mWindow);
        renderProxy_->MakeCurrent(EGL_NO_SURFACE);
        renderProxy_->DestroyEGLSurface(mEglSurface);
        mEglSurface = EGL_NO_SURFACE;
        mWindow = nullptr;
        producer_->GoBackground();
    }
}

void RSSurfaceOhosGl::ResetBufferAge()
{
    if (renderProxy_ != nullptr && mEglSurface != EGL_NO_SURFACE && producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosGl: Reset Buffer Age!");
        DestoryNativeWindow(mWindow);
        renderProxy_->MakeCurrent(EGL_NO_SURFACE, renderProxy_->GetEGLContext());
        renderProxy_->DestroyEGLSurface(mEglSurface);
        mEglSurface = EGL_NO_SURFACE;
        mWindow = nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS
