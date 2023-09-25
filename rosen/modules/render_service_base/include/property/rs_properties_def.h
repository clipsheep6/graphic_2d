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

class RSGravityWeight {
public:
    RSGravityWeight() = default;
    RSGravityWeight(Vector2f translate, Vector4f scale) :
        translate_(translate), scale_(scale)
    {}

    const Vector2f& GetTranslate() const { return translate_; }
    const Vector4f& GetScale() const { return scale_; }

    static RSGravityWeight FromFrameGravity(Gravity gravity)
    {
        static const Vector4f IDENTITY_SCALE{1, 0, 0, 0};
        static const Vector4f NORMAL_SCALE{0, 1, 0, 0};
        static const Vector4f MIN_SCALE{0, 0, 1, 0};
        static const Vector4f MAX_SCALE{0, 0, 0, 1};

        switch (gravity)
        {
            case Gravity::CENTER: return RSGravityWeight({0.5, 0.5}, IDENTITY_SCALE);
            case Gravity::TOP: return RSGravityWeight({0.5, 0}, IDENTITY_SCALE);
            case Gravity::BOTTOM: return RSGravityWeight({0.5, 1}, IDENTITY_SCALE);
            case Gravity::LEFT: return RSGravityWeight({0, 0.5}, IDENTITY_SCALE);
            case Gravity::RIGHT: return RSGravityWeight({1, 0.5}, IDENTITY_SCALE);
            case Gravity::TOP_LEFT: return RSGravityWeight({0, 0}, IDENTITY_SCALE);
            case Gravity::TOP_RIGHT: return RSGravityWeight({1, 0}, IDENTITY_SCALE);
            case Gravity::BOTTOM_LEFT: return RSGravityWeight({0, 1}, IDENTITY_SCALE);
            case Gravity::BOTTOM_RIGHT: return RSGravityWeight({1, 1}, IDENTITY_SCALE);
            case Gravity::RESIZE: return RSGravityWeight({0, 0}, NORMAL_SCALE);
            case Gravity::RESIZE_ASPECT: return RSGravityWeight({0.5, 0.5}, MIN_SCALE);
            case Gravity::RESIZE_ASPECT_TOP_LEFT: return RSGravityWeight({0, 0}, MIN_SCALE);
            case Gravity::RESIZE_ASPECT_BOTTOM_RIGHT: return RSGravityWeight({1, 1}, MIN_SCALE);
            case Gravity::RESIZE_ASPECT_FILL: return RSGravityWeight({1, 1}, MAX_SCALE);
            case Gravity::RESIZE_ASPECT_FILL_TOP_LEFT: return RSGravityWeight({0, 0}, MAX_SCALE);
            case Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT: return RSGravityWeight({1, 1}, MAX_SCALE);
            default: return RSGravityWeight();
        }
    }

    RSGravityWeight operator+(const RSGravityWeight &b) const
    {
        return RSGravityWeight(translate_ + b.translate_, scale_ + b.scale_);
    }
    
    RSGravityWeight operator-(const RSGravityWeight &b) const
    {
        return RSGravityWeight(translate_ - b.translate_, scale_ - b.scale_);
    }

    RSGravityWeight operator*(const RSGravityWeight &b) const
    {
        return RSGravityWeight(translate_ * b.translate_, scale_ * b.scale_);
    }

    RSGravityWeight operator*(float b) const
    {
        return RSGravityWeight(translate_ * b, scale_ * b);
    }

    bool operator==(const RSGravityWeight &b) const
    {
        return translate_ == b.translate_ && scale_ == b.scale_;
    }

private:
    Vector2f translate_{0, 0};
    Vector4f scale_{1, 0, 0, 0};

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H
