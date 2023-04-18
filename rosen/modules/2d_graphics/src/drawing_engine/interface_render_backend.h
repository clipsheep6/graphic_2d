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

#ifndef IRENDER_BACKEND_H
#define IRENDER_BACKEND_H

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLInterface.h"

#include <surface.h>
#include "platform/drawing/rs_surface_frame.h"

namespace OHOS {
namespace Rosen {
class IRenderBackend {
public:
    IRenderBackend() {}
    virtual ~IRenderBackend() {}
    virtual void Initialize() = 0;
    virtual void MakeCurrent() = 0;
    virtual void SwapBuffers() = 0;
    virtual void SetUpSurface(void* window) = 0;
    virtual void DestorySurface() = 0;
    virtual void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height) = 0;
    virtual void DamageFrame(const std::vector<RectI> &rects) = 0;
    virtual void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) = 0;
    virtual int32_t GetBufferAge() = 0;
    virtual void Destroy() = 0;
    virtual void RenderFrame() = 0;
    virtual sk_sp<SkSurface> AcquireSurface(const std::unique_ptr<RSSurfaceFrame>& frame) = 0;
};
}
}
#endif