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
    Transform()
        : pivotX_(0.5f), pivotY_(0.5f), scaleX_(1.f), scaleY_(1.f), rotation_(0.f), rotationX_(0.f), rotationY_(0.f),
          translateX_(0), translateY_(0), translateZ_(0), cameraDistance_(0.f)
    {}
    ~Transform() = default;
    float pivotX_;
    float pivotY_;
    float scaleX_;
    float scaleY_;
    float rotation_;
    float rotationX_;
    float rotationY_;
    float translateX_;
    float translateY_;
    float translateZ_;
    float cameraDistance_;
    Quaternion quaternion_;
};

class RSObjGeometry {
public:
    RSObjGeometry() : x_(-INFINITY), y_(-INFINITY), z_(0), width_(-INFINITY), height_(-INFINITY) {}

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
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->pivotX_ = x;
    }
    void SetPivotY(float y)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->pivotY_ = y;
    }
    void SetPivot(float x, float y)
    {
        SetPivotX(x);
        SetPivotY(y);
    }
    void SetScaleX(float x)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->scaleX_ = x;
    }
    void SetScaleY(float y)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->scaleY_ = y;
    }
    void SetScale(float x, float y)
    {
        SetScaleX(x);
        SetScaleY(y);
    }
    void SetRotation(float rotation)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->rotation_ = rotation;
    }
    void SetRotationX(float rotationX)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->rotationX_ = rotationX;
    }
    void SetRotationY(float rotationY)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->rotationY_ = rotationY;
    }
    void SetTranslateX(float translateX)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->translateX_ = translateX;
    }
    void SetTranslateY(float translateY)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->translateY_ = translateY;
    }
    void SetTranslateZ(float translateZ)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->translateZ_ = translateZ;
    }
    void SetCameraDistance(float cameraDistance)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->cameraDistance_ = cameraDistance;
    }
    void SetQuaternion(const Quaternion& quaternion)
    {
        if (!trans_.has_value()) {
            trans_ = Transform();
        }
        trans_->quaternion_ = quaternion;
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
        return trans_ ? trans_->pivotX_ : 0.5f;
    }
    float GetPivotY() const
    {
        return trans_ ? trans_->pivotY_ : 0.5f;
    }
    float GetScaleX() const
    {
        return trans_ ? trans_->scaleX_ : 1.f;
    }
    float GetScaleY() const
    {
        return trans_ ? trans_->scaleY_ : 1.f;
    }
    float GetRotation() const
    {
        return trans_ ? trans_->rotation_ : 0.f;
    }
    float GetRotationX() const
    {
        return trans_ ? trans_->rotationX_ : 0.f;
    }
    float GetRotationY() const
    {
        return trans_ ? trans_->rotationY_ : 0.f;
    }
    float GetTranslateX() const
    {
        return trans_ ? trans_->translateX_ : 0.f;
    }
    float GetTranslateY() const
    {
        return trans_ ? trans_->translateY_ : 0.f;
    }
    float GetTranslateZ() const
    {
        return trans_ ? trans_->translateZ_ : 0.f;
    }
    float GetCameraDistance() const
    {
        return trans_ ? trans_->cameraDistance_ : 0.f;
    }
    Quaternion GetQuaternion() const
    {
        return trans_ ? trans_->quaternion_ : Quaternion();
    }
    bool IsEmpty() const
    {
        return width_ <= 0 && height_ <= 0;
    }
    RSObjGeometry& operator=(const RSObjGeometry& other)
    {
        if (&other != this) {
            SetRect(other.x_, other.y_, other.width_, other.height_);
            SetZ(other.z_);
            trans_ = other.trans_;
        }
        return *this;
    }

    virtual void Reset()
    {
        x_ = -INFINITY;
        y_ = -INFINITY;
        z_ = 0;
        width_ = -INFINITY;
        height_ = -INFINITY;
        trans_.reset();
    }

protected:
    float x_;
    float y_;
    float z_;
    float width_;
    float height_;
    std::optional<Transform> trans_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_GEOMETRY_H
