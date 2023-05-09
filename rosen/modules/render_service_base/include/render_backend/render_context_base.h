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

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLInterface.h"

#include "rs_render_surface_frame.h"

namespace OHOS {
namespace Rosen {
const std::string PLATFORM_OHOS = "ohos";
const std::string PLATFORM_ANDROID = "android";
const std::string PLATFORM_IOS = "ios";
const std::string PLATFORM_WINDOWS = "windows";
const std::string PLATFORM_DARWIN = "darwin";
class RenderContextBase {
public:
    explicit RenderContextBase() noexcept = default;
    virtual ~RenderContextBase() = default;
    virtual void Init() = 0;
    virtual bool IsContextReady() { return false; }
    virtual void MakeCurrent(void* curSurface = nullptr, void* curContext = nullptr) {}
    virtual bool SetUpGrContext() { return false; };
    virtual void* CreateContext(bool share = false) { return nullptr; };
    virtual void* CreateSurface(void* window) { return nullptr; }
    virtual void DestroySurface(void* curSurface) {}
    virtual void DamageFrame(const std::vector<RectI> &rects) {}
    virtual int32_t GetBufferAge() { return 0; }
    virtual void SwapBuffers() {}
    virtual void Destroy() {}

    void SetPlatformName(const std::string& platformName)
    {
        platformName_ = platformName;
    }

    const std::string& GetPlatformName() const
    {
        return platformName_;
    }
    
    GrContext* GetGrContext() const
    {
        return grContext_.get();
    }

protected:
    sk_sp<GrContext> grContext_ = nullptr;
    std::string platformName_ = PLATFORM_OHOS;
};
}
}
#endif