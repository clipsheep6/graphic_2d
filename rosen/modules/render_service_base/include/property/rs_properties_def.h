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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkMatrix.h"
#else
#include "utils/matrix.h"
#endif

#include "common/rs_color_palette.h"
#include "common/rs_rect.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSObjGeometry;
class RSImage;
class RSShader;

enum class Gravity {
    CENTER = 0,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    RESIZE,
    RESIZE_ASPECT,
    RESIZE_ASPECT_TOP_LEFT,
    RESIZE_ASPECT_BOTTOM_RIGHT,
    RESIZE_ASPECT_FILL,
    RESIZE_ASPECT_FILL_TOP_LEFT,
    RESIZE_ASPECT_FILL_BOTTOM_RIGHT,

    DEFAULT = TOP_LEFT
};

enum class ForegroundColorStrategyType {
    INVALID = 0,
    INVERT_BACKGROUNDCOLOR,
};

class Decoration final {
public:
    Decoration() {}
    ~Decoration() {}
    std::shared_ptr<RSShader> bgShader_ = nullptr;
    std::shared_ptr<RSImage> bgImage_ = nullptr;
    RectF bgImageRect_ = RectF();
    Color backgroundColor_ = RgbPalette::Transparent();
    Color foregroundColor_ = RgbPalette::Transparent();
};

class Sandbox final {
public:
    Sandbox() {}
    ~Sandbox() {}
    std::optional<Vector2f> position_;
#ifndef USE_ROSEN_DRAWING
    std::optional<SkMatrix> matrix_;
#else
    std::optional<Drawing::Matrix> matrix_;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H
