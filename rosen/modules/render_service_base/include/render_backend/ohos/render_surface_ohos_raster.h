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

#ifndef RENDER_SERVICE_BASE_RENDER_SURFACE_OHOS_RASTER_H
#define RENDER_SERVICE_BASE_RENDER_SURFACE_OHOS_RASTER_H

#include "render_surface_ohos.h"

namespace OHOS {
namespace Rosen {
class RenderSurfaceOhosRaster : public RenderSurfaceOhos {
public:
    explicit RenderSurfaceOhosRaster(const sptr<Surface>& producer) : RenderSurfaceOhos(producer)
    {
    }

    ~RenderSurfaceOhosRaster()
    {
        frame_ = nullptr;
    }

    std::shared_ptr<RenderSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp = 0, bool useAFBC = true) override;
    
    bool FlushFrame(uint64_t uiTimestamp = 0) override;

    void SetUiTimeStamp(uint64_t uiTimestamp = 0) override;

    void ClearBuffer() override;

    sk_sp<SkSurface> AcquireSurface() override;
};
}
} // namespace Rosen
#endif