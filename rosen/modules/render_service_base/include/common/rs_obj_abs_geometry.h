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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H

#include <memory>
#include <optional>

#include "include/core/SkMatrix.h"
#include "include/core/SkPoint.h"

#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_obj_geometry.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSObjAbsGeometry : public RSObjGeometry {
public:
    RSObjAbsGeometry() = default;
    ~RSObjAbsGeometry() override = default;
    void ConcatMatrix(const SkMatrix& matrix);

    // Calculate the matrix_ and absMatrix_ with parent's absMatrix
    void UpdateMatrix(const std::shared_ptr<RSObjAbsGeometry>& parent, const std::optional<SkPoint>& offset,
        const std::optional<SkRect>& clipRect);

    // Using by RenderService
    void UpdateByMatrixFromSelf();

    const RectI& GetAbsRect() const
    {
        return absRect_;
    }
    RectI MapAbsRect(const RectF& rect) const;

    // return transform matrix (context + self)
    const SkMatrix& GetMatrix() const;
    // return transform matrix (parent + context + self)
    const SkMatrix& GetAbsMatrix() const;

    bool IsNeedClientCompose() const;

    void SetContextMatrix(const std::optional<SkMatrix>& matrix);

    void Reset() override;

    // Since all the member variables are trivial copyable, the default copy constructor and assignment operator are ok.
    RSObjAbsGeometry(const RSObjAbsGeometry& other) = default;
    RSObjAbsGeometry& operator=(const RSObjAbsGeometry& other) = default;

private:
    SkMatrix UpdateAbsMatrix2D(const std::optional<Transform>& trans) const;
    SkMatrix UpdateAbsMatrix3D(const std::optional<Transform>& trans) const;
    void SetAbsRect();
    void ApplyContextClipRect(const SkMatrix& matrix, const std::optional<SkRect>& clipRect);
    Vector2f GetDataRange(float d0, float d1, float d2, float d3) const;
    RectI absRect_;

    // external matrixes to be concatenated
    std::optional<SkMatrix> concatMatrix_;
    std::optional<SkMatrix> contextMatrix_;

    // only contains the transform of the current node
    SkMatrix matrix_;
    // contains the transform of the current node and all its parents
    std::optional<SkMatrix> absMatrix_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_ABS_GEOMETRY_H
