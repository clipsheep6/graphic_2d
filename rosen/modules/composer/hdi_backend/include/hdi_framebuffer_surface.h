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

#ifndef HDI_BACKEND_HDI_FRAMEBUFFER_SURFACE_H
#define HDI_BACKEND_HDI_FRAMEBUFFER_SURFACE_H

#include <refbase.h>

#include <surface.h>
#include <sync_fence.h>

#ifdef RS_ENABLE_GL
#include "egl_surface.h"
#include "render_context/render_context.h"
#endif // RS_ENABLE_GL

namespace OHOS {
namespace Rosen {

class HdiFramebufferSurface : public IBufferConsumerListener {
public:
    static sptr<HdiFramebufferSurface> CreateFramebufferSurface();
    sptr<OHOS::Surface> GetProducerSurface();
    sptr<OHOS::SurfaceBuffer> GetFramebuffer();
    sptr<SyncFence> GetFramebufferFence();
    int32_t ReleaseFramebuffer(const sptr<SyncFence> &releaseFence);

#ifdef RS_ENABLE_GL
    bool SetupRenderContext(const std::shared_ptr<RenderContext> &context);
    std::shared_ptr<RenderContext> GetRenderContext() const
    {
        return renderContext_;
    }
    NativeWindow* GetNativeWindow() const
    {
        return nativeWindow_;
    }
    EGLSurface GetEGLSurface() const
    {
        return eglSurface_;
    }
#endif // RS_ENABLE_GL

private:
    sptr<OHOS::Surface> consumerSurface_ = nullptr;
    sptr<OHOS::Surface> producerSurface_ = nullptr;
    sptr<OHOS::SurfaceBuffer> currentBuffer_ = nullptr;
    sptr<OHOS::SurfaceBuffer> oldBuffer_ = nullptr;
    sptr<SyncFence> fbAcquireFence_ = SyncFence::INVALID_FENCE;
    static constexpr uint32_t MAX_BUFFER_SIZE = 3;

    HdiFramebufferSurface();
    virtual ~HdiFramebufferSurface();

    void OnBufferAvailable() override;
    OHOS::SurfaceError SetBufferQueueSize(uint32_t bufferSize);
    OHOS::SurfaceError CreateSurface(sptr<HdiFramebufferSurface> &fbSurface);

#ifdef RS_ENABLE_GL
    std::shared_ptr<RenderContext> renderContext_;
    NativeWindow* nativeWindow_ = nullptr;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
#endif // RS_ENABLE_GL
};
} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_FRAMEBUFFER_SURFACE_H
