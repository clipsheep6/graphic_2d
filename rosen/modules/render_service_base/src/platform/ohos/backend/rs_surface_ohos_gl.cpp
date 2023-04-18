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
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosGl::RequestFrame(int32_t width, int32_t height,
    uint64_t uiTimestamp, bool useAFBC)
{
    std::unique_ptr<RSSurfaceFrameOhosGl> frame = std::make_unique<RSSurfaceFrameOhosGl>(width, height);
    struct NativeWindow* nativeWindow = CreateNativeWindowFromSurface(&producer_);
    renderProxy_->SetUpSurface((EGLNativeWindowType)nativeWindow);

#ifdef RS_ENABLE_AFBC
    int32_t format = 0;
    NativeWindowHandleOpt(nativeWindow, GET_FORMAT, &format);
    if (format == PIXEL_FMT_RGBA_8888 && useAFBC) {
        bufferUsage_ =
            (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
    }
#endif
    NativeWindowHandleOpt(nativeWindow, SET_USAGE, bufferUsage_);
    NativeWindowHandleOpt(nativeWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(nativeWindow, SET_COLOR_GAMUT, colorSpace_);
    NativeWindowHandleOpt(nativeWindow, SET_UI_TIMESTAMP, uiTimestamp);

    renderProxy_->MakeCurrent();

    ROSEN_LOGD("RSSurfaceOhosGl:RequestFrame, width is %d, height is %d",
        frame->GetWidth(), frame->GetHeight());

    frame->SetRenderProxy(renderProxy_);

    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));

    DestoryNativeWindow(nativeWindow);
    
    return ret;
}

void RSSurfaceOhosGl::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    renderProxy_->SetUiTimeStamp(frame, uiTimestamp);
}

bool RSSurfaceOhosGl::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    // gpu render flush
    renderProxy_->RenderFrame();
    renderProxy_->SwapBuffers();
    return true;
}

void RSSurfaceOhosGl::ClearBuffer()
{
    if (renderProxy_ != nullptr && producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosGl: Clear surface buffer!");
        renderProxy_->MakeCurrent();
        renderProxy_->DestorySurface();
        producer_->GoBackground();
    }
}

void RSSurfaceOhosGl::ResetBufferAge()
{
    if (renderProxy_ != nullptr && producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosGl: Reset Buffer Age!");
        renderProxy_->MakeCurrent();
        renderProxy_->DestorySurface();
    }
}
} // namespace Rosen
} // namespace OHOS
