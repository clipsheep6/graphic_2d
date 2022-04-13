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

#ifndef RENDER_SERVICE_BASE_SRC_PLATFORM_WINDOWS_RS_SURFACE_FRAME_WINDOWS_H
#define RENDER_SERVICE_BASE_SRC_PLATFORM_WINDOWS_RS_SURFACE_FRAME_WINDOWS_H

#include "drawing_engine/drawing_surface/rs_surface_frame.h"

#include <surface_buffer.h>

namespace OHOS {
namespace Rosen {
class RSSurfaceFrameWindows : public RSSurfaceFrame {
    friend class RSSurfaceWindows;

public:
    RSSurfaceFrameWindows(int32_t width, int32_t height);
    ~RSSurfaceFrameWindows() override = default;

    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override;
    ColorGamut GetColorSpace() const override;
    void SetColorSpace(ColorGamut colorSpace) override;

private:
    sptr<SurfaceBuffer> buffer_ = nullptr;
    BufferRequestConfig requestConfig_ = {};
    BufferFlushConfig flushConfig_ = {};
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
    sk_sp<SkColorSpace> skColorSpace_ = nullptr;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_SRC_PLATFORM_WINDOWS_RS_SURFACE_FRAME_WINDOWS_H
