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

#ifndef RENDER_SERVICE_BASE_SRC_PLATFORM_WINDOWS_RS_SURFACE_WINDOWS_H
#define RENDER_SERVICE_BASE_SRC_PLATFORM_WINDOWS_RS_SURFACE_WINDOWS_H

#include <memory>

#include "common/rs_common_def.h"
#include "rs_surface.h"
#include "render_backend/rs_surface_frame.h"

namespace OHOS {
namespace Rosen {
using OnRenderFunc = bool (*)(const void*, const size_t, const int32_t, const int32_t);
class RSSurfaceWindows : public RSSurface {
public:
    RSSurfaceWindows(OnRenderFunc onRender);
    ~RSSurfaceWindows() = default;

    bool IsValid() override;

    uint32_t GetQueueSize() const override;

    std::unique_ptr<RSSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC = true) override;

    bool FlushFrame(uint64_t uiTimestamp) override;

    void ClearBuffer() override;

private:
    void YInvert(void *addr, int32_t width, int32_t height);

    sk_sp<SkSurface> AcquireSurface();

    OnRenderFunc onRender_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_SRC_PLATFORM_WINDOWS_RS_SURFACE_WINDOWS_H
