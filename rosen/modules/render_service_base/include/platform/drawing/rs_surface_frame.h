/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_DRAWING_RS_SURFACE_FRAME_H
#define RENDER_SERVICE_BASE_DRAWING_RS_SURFACE_FRAME_H

#include <memory>

#include "common/rs_rect.h"

namespace OHOS {
namespace Rosen {
class RenderProxy;
class RSSurfaceFrame {
public:
    RSSurfaceFrame() = default;
    explicit RSSurfaceFrame(int32_t width, int32_t height)
        : width_(width), height_(height)
    {
    }
    virtual ~RSSurfaceFrame() = default;
    virtual void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) {};
    virtual void SetDamageRegion(const std::vector<RectI> &rects) {};
    virtual int32_t GetBufferAge() const = 0;
    int32_t GetWidth() const
    {
        return width_;
    }

    int32_t GetHeight() const
    {
        return height_;
    }
protected:
    int32_t width_ = -1;
    int32_t height_ = -1;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DRAWING_RS_SURFACE_FRAME_H
