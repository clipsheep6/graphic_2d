/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_FRAME_OHOS_H
#define RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_FRAME_OHOS_H

#include <memory>
#include <surface.h>
#include "drawing_engine/render_proxy.h"

#include "platform/drawing/rs_surface_frame.h"

namespace OHOS {
namespace Rosen {
class RenderProxy;
class RSSurfaceFrameOhos : public RSSurfaceFrame {
public:
    RSSurfaceFrameOhos() = default;
    RSSurfaceFrameOhos(int32_t width, int32_t height) : RSSurfaceFrame(width, height)
    { 
    }
    
    virtual ~RSSurfaceFrameOhos() = default;
    
    void SetRenderProxy(const std::shared_ptr<RenderProxy> renderProxy)
    {
        renderProxy_ = renderProxy;
    }
    
    void SetReleaseFence(const int32_t& fence)
    {
        releaseFence_ = fence;
    }

    int32_t GetReleaseFence() const
    {
        return releaseFence_;
    }

protected:
    std::shared_ptr<RenderProxy> renderProxy_;
    int32_t releaseFence_ = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PLATFORM_RS_SURFACE_FRAME_OHOS_H
