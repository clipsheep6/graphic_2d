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

#ifndef RENDER_SERVICE_BASE_RS_SURFACE_H
#define RENDER_SERVICE_BASE_RS_SURFACE_H

#include <memory>

#include "common/rs_rect.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

#include "rs_surface_frame.h"
#include "render_context.h"

namespace OHOS {
namespace Rosen {
class RSSurface {
public:
    explicit RSSurface()
    {
        frame_ = std::make_shared<RSSurfaceFrame>();
    }

    virtual ~RSSurface() = default;

    virtual bool IsValid() = 0;

    virtual uint32_t GetQueueSize() const = 0;

    virtual std::shared_ptr<RSSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp = 0, bool useAFBC = true) = 0;

    virtual bool FlushFrame(uint64_t uiTimestamp = 0) = 0;
    
    virtual void SetUiTimeStamp(uint64_t uiTimestamp = 0) = 0;

    virtual void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) = 0;

    virtual void SetDamageRegion(const std::vector<RectI> &rects) = 0;
    
    virtual int32_t GetBufferAge() = 0;

#ifndef ROSEN_CROSS_PLATFORM
    virtual ColorGamut GetColorSpace() = 0;
    virtual void SetColorSpace(ColorGamut colorSpace) = 0;
#endif
    virtual void ClearBuffer() = 0;

    virtual SkCanvas* GetSkCanvas() = 0;
    
    virtual sk_sp<SkSurface> GetSkSurface() = 0;

    void SetRenderContext(const std::shared_ptr<RenderContext>& renderContext)
    {
        renderContext_ = renderContext;
    }

    const std::shared_ptr<RenderContext>& GetRenderContext() const
    {
        return renderContext_;
    }
protected:
    std::shared_ptr<RSSurfaceFrame> frame_;
    std::shared_ptr<RenderContext> renderContext_;
};
}
} // namespace Rosen
#endif