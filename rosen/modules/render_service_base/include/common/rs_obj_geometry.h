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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_GEOMETRY_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_GEOMETRY_H

#include <cmath>

#include "common/rs_common_def.h"
#include "common/rs_vector4.h"
namespace OHOS {
namespace Rosen {
class Transform {
public:
    Transform() = default;
    Transform(const Transform& other) = default;
    ~Transform() = default;
    float pivotX_ { 0.5f };
    float pivotY_ { 0.5f };
    float pivotZ_ { 0.0f };
    float scaleX_ { 1.0f };
    float scaleY_ { 1.0f };
    float skewX_ {0.0f};
    float skewY_ {0.0f};
    float perspX_ {0.0f};
    float perspY_ {0.0f};
    float rotation_ { 0.0f };
    float rotationX_ { 0.0f };
    float rotationY_ { 0.0f };
    float translateX_ { 0.0f };
    float translateY_ { 0.0f };
    float translateZ_ { 0.0f };
    float cameraDistance_ { 0.0f };
    Quaternion quaternion_ { 0.0f, 0.0f, 0.0f, 1.0f };
};

class RSObjGeometry {
public:
    RSObjGeometry() : x_(-INFINITY), y_(-INFINITY), z_(0.0f), width_(-INFINITY), height_(-INFINITY) {}

    virtual ~RSObjGeometry() = default;

    void SetX(float x)
    {
        x_ = x;
    }
    void SetY(float y)
    {
        y_ = y;
    }
    void SetZ(float z)
    {
        z_ = z;
    }
    void SetWidth(float w)
    {
        width_ = w;
    }
    void SetHeight(float h)
    {
        height_ = h;
    }
    void SetPosition(float x, float y)
    {
        SetX(x);
        SetY(y);
    }
    void SetSize(float w, float h)
    {
        SetWidth(w);
        SetHeight(h);
    }
    void SetRect(float x, float y, float w, float h)
    {
        SetPosition(x, y);
        SetSize(w, h);
    }
    void SetPivotX(float x)
    {
        trans_.pivotX_ = x;
    }
    void SetPivotY(float y)
    {
        trans_.pivotY_ = y;
    }
    void SetPivotZ(float z)
    {
        trans_.pivotZ_ = z;
    }
    void SetPivot(float x, float y)
    {
        SetPivotX(x);
        SetPivotY(y);
    }
    void SetScaleX(float x)
    {
        trans_.scaleX_ = x;
    }
    void SetScaleY(float y)
    {
        trans_.scaleY_ = y;
    }
    void SetScale(float x, float y)
    {
        SetScaleX(x);
        SetScaleY(y);
    }
    void SetSkewX(float x)
    {
        trans_.skewX_ = x;
    }
    void SetSkewY(float y)
    {
        trans_.skewY_ = y;
    }
    void SetSkew(float x, float y)
    {
        SetSkewX(x);
        SetSkewY(y);
    }
    void SetPerspX(float x)
    {
        trans_.perspX_ = x;
    }
    void SetPerspY(float y)
    {
        trans_.perspY_ = y;
    }
    void SetPersp(float x, float y)
    {
        SetPerspX(x);
        SetPerspY(y);
    }
    void SetRotation(float rotation)
    {
        trans_.rotation_ = rotation;
    }
    void SetRotationX(float rotationX)
    {
        trans_.rotationX_ = rotationX;
    }
    void SetRotationY(float rotationY)
    {
        trans_.rotationY_ = rotationY;
    }
    void SetTranslateX(float translateX)
    {
        trans_.translateX_ = translateX;
    }
    void SetTranslateY(float translateY)
    {
        trans_.translateY_ = translateY;
    }
    void SetTranslateZ(float translateZ)
    {
        if (!ROSEN_EQ(trans_.translateZ_, translateZ)) {
            trans_.translateZ_ = translateZ;
        }
    }
    void SetCameraDistance(float cameraDistance)
    {
        trans_.cameraDistance_ = cameraDistance;
    }
    void SetQuaternion(const Quaternion& quaternion)
    {
        trans_.quaternion_ = quaternion;
    }

    float GetX() const
    {
        return x_;
    }
    float GetWidth() const
    {
        return width_;
    }
    float GetY() const
    {
        return y_;
    }
    float GetHeight() const
    {
        return height_;
    }
    float GetZ() const
    {
        return z_;
    }
    float GetPivotX() const
    {
        return trans_.pivotX_ ;
    }
    float GetPivotY() const
    {
        return trans_ .pivotY_;
    }
    float GetPivotZ() const
    {
        return trans_ .pivotZ_;
    }
    float GetScaleX() const
    {
        return trans_ .scaleX_;
    }
    float GetScaleY() const
    {
        return trans_ .scaleY_;
    }
    float GetSkewX() const
    {
        return trans_ .skewX_;
    }
    float GetSkewY() const
    {
        return trans_ .skewY_;
    }
    float GetPerspX() const
    {
        return trans_ .perspX_;
    }
    float GetPerspY() const
    {
        return trans_ .perspY_;
    }
    float GetRotation() const
    {
        return trans_ .rotation_;
    }
    float GetRotationX() const
    {
        return trans_ .rotationX_;
    }
    float GetRotationY() const
    {
        return trans_ .rotationY_;
    }
    float GetTranslateX() const
    {
        return trans_ .translateX_;
    }
    float GetTranslateY() const
    {
        return trans_ .translateY_;
    }
    float GetTranslateZ() const
    {
        return trans_ .translateZ_;
    }
    float GetCameraDistance() const
    {
        return trans_ .cameraDistance_;
    }
    Quaternion GetQuaternion() const
    {
        return trans_ .quaternion_;
    }
    bool IsEmpty() const
    {
        return width_ <= 0 && height_ <= 0;
    }
    void Round()
    {
        x_ = std::floor(x_);
        y_ = std::floor(y_);
    }
    RSObjGeometry& operator=(const RSObjGeometry& geo)
    {
        if (&geo != this) {
            SetRect(geo.x_, geo.y_, geo.width_, geo.height_);
            SetZ(geo.z_);
            trans_ = geo.trans_;
        }
        return *this;
    }

protected:
    float x_;
    float y_;
    float z_;
    float width_;
    float height_;
    Transform trans_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_GEOMETRY_H
