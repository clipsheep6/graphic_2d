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

#ifndef RENDER_SERVICE_BASE_RENDER_CONTEXT_BASE_H
#define RENDER_SERVICE_BASE_RENDER_CONTEXT_BASE_H

#include "common/rs_rect.h"

#include "rs_render_surface_frame.h"

namespace OHOS {
namespace Rosen {
class RenderContextBase {
public:
    explicit RenderContextBase()
    {
        frame_ = std::make_unique<RSRenderSurfaceFrame>();
    }
    virtual ~RenderContextBase() = default;
    virtual void Init() = 0;
    virtual bool IsContextReady() { return false; }
    virtual void MakeCurrent(void* curSurface = nullptr, void* curContext = nullptr) {}
    virtual void* CreateContext(bool share = false) { return nullptr; };
    virtual bool CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame) { return false; }
    virtual void DestroySurface() {}
    virtual void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height,
        const std::shared_ptr<RSRenderSurfaceFrame>& frame) {};
    virtual void DamageFrame(const std::vector<RectI> &rects) {}
    virtual int32_t GetBufferAge() { return 0; }
    virtual void SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame) {}
    virtual void Destroy() {}
    
    EGLContext GetEGLContext()
    {
        if (frame_ == nullptr) {
            return EGL_NO_CONTEXT;
        }
        return frame_->eglContext_;
    }
    
    EGLDisplay GetEGLDisplay()
    {
        if (frame_ == nullptr) {
            return EGL_NO_DISPLAY;
        }
        return frame_->eglDisplay_;
    }

    void SetPlatformName(const PlatformName& platformName)
    {
        platformName_ = platformName;
    }

    const PlatformName& GetPlatformName() const
    {
        return platformName_;
    }

    void SetRenderType(const RenderType& renderType)
    {
        renderType_ = renderType;
    }

    const RenderType& GetRenderType() const
    {
        return renderType_;
    }

protected:
    std::shared_ptr<RSRenderSurfaceFrame> frame_ = nullptr;
    PlatformName platformName_ = PlatformName::OHOS;
    RenderType renderType_ = RenderType::GLES;
};
}
}
#endif