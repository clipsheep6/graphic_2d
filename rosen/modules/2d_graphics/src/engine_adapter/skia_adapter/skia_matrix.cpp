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

#include "skia_matrix.h"

#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
SkiaMatrix::SkiaMatrix()
{
    skMatrix_ = std::make_unique<SkMatrix>();
}

const SkMatrix& SkiaMatrix::ExportSkiaMatrix() const
{
    return *skMatrix_;
}

void SkiaMatrix::Rotate(scalar degree, scalar px, scalar py)
{
    skMatrix_->setRotate(degree, px, py);
}

void SkiaMatrix::Translate(scalar dx, scalar dy)
{
    skMatrix_->setTranslate(dx, dy);
}

void SkiaMatrix::Scale(scalar sx, scalar sy, scalar px, scalar py)
{
    skMatrix_->setScale(sx, sy, px, py);
}

static inline const SkMatrix& GetSkMatrix(const Matrix& matrix)
{
    const SkiaMatrix* m = static_cast<const SkiaMatrix*>(matrix.GetMatrixImpl());
    return m->ExportSkiaMatrix();
}

void SkiaMatrix::Multiply(const Matrix& a, const Matrix& b)
{
    SkMatrix m1 = GetSkMatrix(a);
    SkMatrix m2 = GetSkMatrix(b);

    skMatrix_->setConcat(m1, m2);
}

bool SkiaMatrix::Equals(const Matrix& a, const Matrix& b)
{
    SkMatrix m1 = GetSkMatrix(a);
    SkMatrix m2 = GetSkMatrix(b);

    return (m1 == m2);
}

void SkiaMatrix::SetMatrix(scalar scaleX, scalar skewX, scalar transX,
        scalar skewY,  scalar scaleY, scalar transY,
        scalar persp0, scalar persp1, scalar persp2)
{
    skMatrix_->setAll(scaleX, skewX, transX, skewY, scaleY, transY,
        persp0, persp1, persp2);
}

scalar SkiaMatrix::Get(int index)
{
    return skMatrix_->get(index);
}
}
}
