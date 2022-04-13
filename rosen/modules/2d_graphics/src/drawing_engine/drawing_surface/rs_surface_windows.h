/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <include/core/SkSurface.h>
#include <include/gpu/GrContext.h>
#include <surface.h>

#include "rs_surface.h"
#include "rs_surface_frame.h"

namespace OHOS {
namespace Rosen {
class OHOS_EXPORT RSSurfaceWindows : public RSSurface {
public:
    RSSurfaceWindows(const sptr<Surface> &producer);
    ~RSSurfaceWindows() override = default;

    bool IsValid() const override;
    sptr<Surface> GetSurface() const;

    std::unique_ptr<RSSurfaceFrame> RequestFrame(int32_t width, int32_t height) override;
    bool FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame) override;
    SkCanvas* GetCanvas(std::unique_ptr<RSSurfaceFrame>& frame) override;

private:
    void YInvert(void *addr, int32_t width, int32_t height);
    bool SetupGrContext();

    sptr<Surface> producer_ = nullptr;
    sk_sp<SkSurface> surface_ = nullptr;
    sk_sp<GrContext> grContext_ = nullptr;
    std::shared_ptr<SkCanvas> canvas_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_SRC_PLATFORM_WINDOWS_RS_SURFACE_WINDOWS_H
