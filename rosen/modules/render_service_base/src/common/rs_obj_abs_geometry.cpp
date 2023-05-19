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

#include "common/rs_obj_abs_geometry.h"

#include "include/utils/SkCamera.h"
#ifdef NEW_SKIA
#include "include/core/SkM44.h"
#else
#include "include/core/SkMatrix44.h"
#endif

namespace OHOS {
namespace Rosen {
constexpr unsigned RECT_POINT_NUM = 4;
constexpr unsigned LEFT_TOP_POINT = 0;
constexpr unsigned RIGHT_TOP_POINT = 1;
constexpr unsigned RIGHT_BOTTOM_POINT = 2;
constexpr unsigned LEFT_BOTTOM_POINT = 3;
constexpr float INCH_TO_PIXEL = 72;
constexpr float EPSILON = 1e-4f;

void RSObjAbsGeometry::ConcatMatrix(const SkMatrix& matrix)
{
    if (matrix.isIdentity()) {
        return;
    }
    // we already calculate the matrixes, so we just update them and return
    if (absMatrix_.has_value()) {
        matrix_.preConcat(matrix);
        absMatrix_->preConcat(matrix);
        SetAbsRect();
        return;
    }
    // we haven't calculate the matrixes, so we just record the matrix and calculate it later
    if (!concatMatrix_.has_value()) {
        concatMatrix_ = matrix;
    } else {
        concatMatrix_->preConcat(matrix);
    }
}

/**
 * @brief Updates the matrix of the view with respect to its parent view.
 *
 * @param parent The parent view of the current view.
 * @param offset The offset of the current view with respect to its parent.
 * @param clipRect The optional clipping rectangle of the current view.
 */
void RSObjAbsGeometry::UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent,
    const std::optional<SkPoint>& offset, const std::optional<SkRect>& clipRect)
{
    // Initialize the absolute matrix with the absolute matrix of the parent view if the parent view exists
    if (parent == nullptr) {
        absMatrix_.reset();
    } else {
        absMatrix_ = parent->GetAbsMatrix();
    }
    if (absMatrix_.has_value() && offset.has_value() && !offset.value().isZero()) {
        absMatrix_->preTranslate(offset->x(), offset->y());
    }
    // filter invalid width and height
    if (IsEmpty()) {
        matrix_.reset();
        return;
    }
    // If the view has no transformations or only 2D transformations, update the absolute matrix with 2D
    // transformations
    if (!trans_ || (ROSEN_EQ(trans_->translateZ_, 0.f) && ROSEN_EQ(trans_->rotationX_, 0.f) &&
        ROSEN_EQ(trans_->rotationY_, 0.f) && trans_->quaternion_.IsIdentity())) {
        matrix_ = UpdateAbsMatrix2D(trans_);
    } else {
        // Otherwise, update the absolute matrix with 3D transformations
        matrix_ = UpdateAbsMatrix3D(trans_);
    }
    // We have pending matrix to apply, just preConcat it
    if (concatMatrix_.has_value()) {
        matrix_.preConcat(*concatMatrix_);
    }
    // If the absolute matrix of the current view exists, update it with the context matrix and the current matrix
    if (absMatrix_.has_value()) {
        if (contextMatrix_.has_value()) {
            absMatrix_->preConcat(*contextMatrix_);
        }
        absMatrix_->preConcat(matrix_);
    }
    // Update self relative rect with respect to clipRect
    ApplyContextClipRect(matrix_, clipRect);
    // If the context matrix of the current view exists, update the current matrix with it
    if (contextMatrix_.has_value()) {
        matrix_.preConcat(*contextMatrix_);
    }
    // Update the absolute rectangle of the current view
    SetAbsRect();
}

void RSObjAbsGeometry::ApplyContextClipRect(const SkMatrix& matrix, const std::optional<SkRect>& clipRect)
{
    // if clipRect is valid, update rect with clipRect
    if (!clipRect.has_value() || clipRect.value().isEmpty()) {
        return;
    }
    auto mappedClipRect = clipRect.value();
    // map the clip rect (in parent coordinate system) to the current view coordinate system
    if (!matrix.isIdentity()) {
        SkMatrix invertMatrix;
        if (matrix.invert(&invertMatrix)) {
            mappedClipRect = invertMatrix.mapRect(mappedClipRect);
        }
        // matrix already includes bounds offset, we need to revert it
        mappedClipRect.offset(GetX(), GetY());
    }
    // intersect with current rect
    if (!mappedClipRect.intersect({ x_, y_, x_ + width_, y_ + height_ })) {
        // No visible area
        x_ = y_ = width_ = height_ = 0.0f;
        return;
    }
    x_ = mappedClipRect.left();
    y_ = mappedClipRect.top();
    width_ = mappedClipRect.width();
    height_ = mappedClipRect.height();
}

/**
 * @brief Updates the matrix of the view without its parent view.
 */
void RSObjAbsGeometry::UpdateByMatrixFromSelf()
{
    absMatrix_.reset();
    // If the view has no transformations or only 2D transformations, update the absolute matrix with 2D transformations
    if (!trans_ || (ROSEN_EQ(trans_->translateZ_, 0.f) && ROSEN_EQ(trans_->rotationX_, 0.f) &&
        ROSEN_EQ(trans_->rotationY_, 0.f) && trans_->quaternion_.IsIdentity())) {
        matrix_ = UpdateAbsMatrix2D(trans_);
    } else {
        // Otherwise, update the absolute matrix with 3D transformations
        matrix_ = UpdateAbsMatrix3D(trans_);
    }

    // If the context matrix of the view exists, update the current matrix with it
    if (contextMatrix_.has_value()) {
        matrix_.preConcat(*contextMatrix_);
    }

    // Update the absolute rectangle of the view
    SetAbsRect();
}

bool RSObjAbsGeometry::IsNeedClientCompose() const
{
    if (!trans_) {
        return false;
    }
    // return false if rotation degree is times of 90
    return !ROSEN_EQ(std::remainder(trans_->rotation_, 90.f), 0.f, EPSILON);
}

SkMatrix RSObjAbsGeometry::UpdateAbsMatrix2D(const std::optional<Transform>& trans) const
{
    if (!trans) {
#ifdef NEW_SKIA
        return SkMatrix::Translate(x_, y_);
#else
        return SkMatrix::MakeTrans(x_, y_);
#endif
    }
    auto matrix = SkMatrix::I();
    // Translate
    if ((x_ + trans->translateX_ != 0) || (y_ + trans->translateY_ != 0)) {
        matrix.preTranslate(x_ + trans->translateX_, y_ + trans->translateY_);
    }
    // rotation
    if (!ROSEN_EQ(trans->rotation_, 0.f, EPSILON)) {
        matrix.preRotate(trans->rotation_, trans->pivotX_ * width_, trans->pivotY_ * height_);
    }
    // Scale
    if (!ROSEN_EQ(trans->scaleX_, 1.f) || !ROSEN_EQ(trans->scaleY_, 1.f)) {
        matrix.preScale(trans->scaleX_, trans->scaleY_, trans->pivotX_ * width_, trans->pivotY_ * height_);
    }
    return matrix;
}

/**
 * Update the absolute matrix in 3D space
 */
SkMatrix RSObjAbsGeometry::UpdateAbsMatrix3D(const std::optional<Transform>& trans) const
{
    // If the view has a non-identity quaternion, apply 3D transformations
    if (!trans->quaternion_.IsIdentity()) {
#ifdef NEW_SKIA
        SkM44 matrix3D;
#else
        SkMatrix44 matrix3D;
#endif

        // Translate
        matrix3D.setTranslate(trans->pivotX_ * width_ + x_ + trans->translateX_,
            trans->pivotY_ * height_ + y_ + trans->translateY_, z_ + trans->translateZ_);

        // Rotate
        float x = trans->quaternion_[0];
        float y = trans->quaternion_[1];
        float z = trans->quaternion_[2];
        float w = trans->quaternion_[3];
#ifdef NEW_SKIA
        SkScalar r[16] = {
            1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w), 0,
            2.f * (x * y - z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w), 0,
            2.f * (x * z + y * w), 2.f * (y * z - x * w), 1.f - 2.f * (x * x + y * y), 0,
            0, 0, 0, 1
        };
        SkM44 matrix4 = SkM44::ColMajor(r);
#else
        SkMatrix44 matrix4;
        matrix4.set3x3(
            1.f - 2.f * (y * y + z * z), 2.f * (x * y + z * w), 2.f * (x * z - y * w),
            2.f * (x * y - z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z + x * w),
            2.f * (x * z + y * w), 2.f * (y * z - x * w), 1.f - 2.f * (x * x + y * y));
#endif
        matrix3D = matrix3D * matrix4;

        // Scale
        if (!ROSEN_EQ(trans->scaleX_, 1.f) || !ROSEN_EQ(trans->scaleY_, 1.f)) {
            matrix3D.preScale(trans->scaleX_, trans->scaleY_, 1.f);
        }

        // Translate
        matrix3D.preTranslate(-trans->pivotX_ * width_, -trans->pivotY_ * height_, 0);

        // Concatenate the 3D matrix with the 2D matrix
#ifdef NEW_SKIA
        return matrix3D.asM33();
#else
        return SkMatrix(matrix3D);
#endif
    } else {
        SkMatrix matrix3D;
        Sk3DView camera;

        // Z Position
        camera.translate(0, 0, z_ + trans->translateZ_);

        // Set camera distance
        if (trans->cameraDistance_ == 0) {
            float zOffSet = sqrt(width_ * width_ + height_ * height_) / 2;
            camera.setCameraLocation(0, 0, camera.getCameraLocationZ() - zOffSet / INCH_TO_PIXEL);
        } else {
            camera.setCameraLocation(0, 0, trans->cameraDistance_);
        }

        // Rotate
        camera.rotateX(-trans->rotationX_);
        camera.rotateY(-trans->rotationY_);
        camera.rotateZ(-trans->rotation_);
        camera.getMatrix(&matrix3D);

        // Scale
        if (!ROSEN_EQ(trans->scaleX_, 1.f) || !ROSEN_EQ(trans->scaleY_, 1.f)) {
            matrix3D.preScale(trans->scaleX_, trans->scaleY_);
        }

        // Translate
        matrix3D.preTranslate(-trans->pivotX_ * width_, -trans->pivotY_ * height_);

        // Concatenate the 3D matrix with the 2D matrix
        matrix3D.postTranslate(
            trans->pivotX_ * width_ + x_ + trans->translateX_, trans->pivotY_ * height_ + y_ + trans->translateY_);
        return matrix3D;
    }
}

void RSObjAbsGeometry::SetAbsRect()
{
    absRect_ = MapAbsRect(RectF(0.f, 0.f, width_, height_));
}

/**
 * Map the absolute rectangle
 * @param rect the rectangle to map
 * @return the mapped absolute rectangle
 */
RectI RSObjAbsGeometry::MapAbsRect(const RectF& rect) const
{
    RectI absRect;
    auto& matrix = GetAbsMatrix();
    // Check if the matrix has skew or negative scaling
    if (!ROSEN_EQ(matrix.getSkewX(), 0.f) || (matrix.getScaleX() < 0) ||
        !ROSEN_EQ(matrix.getSkewY(), 0.f) || (matrix.getScaleY() < 0)) {
        // Map the rectangle's points to the absolute matrix
        SkPoint p[RECT_POINT_NUM];
        p[LEFT_TOP_POINT].set(rect.left_, rect.top_);
        p[RIGHT_TOP_POINT].set(rect.left_ + rect.width_, rect.top_);
        p[RIGHT_BOTTOM_POINT].set(rect.left_ + rect.width_, rect.top_ + rect.height_);
        p[LEFT_BOTTOM_POINT].set(rect.left_, rect.top_ + rect.height_);
        matrix.mapPoints(p, RECT_POINT_NUM);

        // Get the data range of the mapped points
        Vector2f xRange = GetDataRange(p[LEFT_TOP_POINT].x(), p[RIGHT_TOP_POINT].x(),
            p[RIGHT_BOTTOM_POINT].x(), p[LEFT_BOTTOM_POINT].x());
        Vector2f yRange = GetDataRange(p[LEFT_TOP_POINT].y(), p[RIGHT_TOP_POINT].y(),
            p[RIGHT_BOTTOM_POINT].y(), p[LEFT_BOTTOM_POINT].y());

        // Set the absolute rectangle's properties
        absRect.left_ = static_cast<int>(xRange[0]);
        absRect.top_ = static_cast<int>(yRange[0]);
        absRect.width_ = static_cast<int>(std::ceil(xRange[1] - absRect.left_));
        absRect.height_ = static_cast<int>(std::ceil(yRange[1] - absRect.top_));
    } else {
        // Calculate the absolute rectangle based on the matrix's translation and scaling
        absRect.left_ = static_cast<int>(std::ceil(rect.left_ + matrix.getTranslateX()));
        absRect.top_ = static_cast<int>(std::ceil(rect.top_ + matrix.getTranslateY()));
        int right = static_cast<int>(std::ceil(rect.left_ + matrix.getTranslateX() +
            rect.width_ * matrix.getScaleX()));
        int bottom = static_cast<int>(std::ceil(rect.top_ + matrix.getTranslateY() +
            rect.height_ * matrix.getScaleY()));

        // Assuming matrix is identity,
        // if rect.left_ = 100.5, rect.width_ = 0.5,
        // then absRect.left_ = 101, right = 101, absRect.width_ = 0, the node cannot be drawn.
        // So if right - absRect.left_ == 0 and rect.width_ * matrix.getScaleX() > 0,
        // we should decrease absRect.left_ to make sure absRect.width_ would not be zero
        if (right - absRect.left_ == 0 && rect.width_ * matrix.getScaleX() > 0) {
            absRect.left_ = absRect.left_ - 1;
        }
        if (bottom - absRect.top_ == 0 && rect.height_ * matrix.getScaleY() > 0) {
            absRect.top_ = absRect.top_ - 1;
        }
        absRect.width_ = right - absRect.left_;
        absRect.height_ = bottom - absRect.top_;
    }
    return absRect;
}

Vector2f RSObjAbsGeometry::GetDataRange(float d0, float d1, float d2, float d3) const
{
    float min = d0;
    float max = d0;
    if (d0 > d1) {
        min = d1;
    } else {
        max = d1;
    }
    if (d2 > d3) {
        if (min > d3) {
            min = d3;
        }
        if (max < d2) {
            max = d2;
        }
    } else {
        if (min > d2) {
            min = d2;
        }
        if (max < d3) {
            max = d3;
        }
    }
    return { min, max };
}

void RSObjAbsGeometry::SetContextMatrix(const std::optional<SkMatrix>& matrix)
{
    contextMatrix_ = matrix;
}

const SkMatrix& RSObjAbsGeometry::GetMatrix() const
{
    return matrix_;
}

const SkMatrix& RSObjAbsGeometry::GetAbsMatrix() const
{
    // if absMatrix_ is empty, return matrix_ instead
    return absMatrix_ ? *absMatrix_ : matrix_;
}

void RSObjAbsGeometry::Reset()
{
    RSObjGeometry::Reset();

    matrix_.reset();
    absMatrix_.reset();
    concatMatrix_.reset();
    contextMatrix_.reset();
}
} // namespace Rosen
} // namespace OHOS
