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

#ifndef RS_SURFACE_FRAME_H
#define RS_SURFACE_FRAME_H

#include <memory>
#include "include/core/SkCanvas.h"

#include "surface_type.h"

namespace OHOS {
namespace Rosen {
class RSSurfaceFrame {
public:
    RSSurfaceFrame() = default;
    virtual ~RSSurfaceFrame() = default;
    virtual void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) = 0;
    virtual ColorGamut GetColorSpace() const = 0;
    virtual void SetColorSpace(ColorGamut colorSpace) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif
