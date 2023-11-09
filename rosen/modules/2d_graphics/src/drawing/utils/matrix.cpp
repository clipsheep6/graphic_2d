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

#include "utils/matrix.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Matrix::Matrix() : matrixImplPtr(ImplFactory::CreateMatrixImpl()) {}

void Matrix::Rotate(scalar degree, scalar px, scalar py)
{
    matrixImplPtr->Rotate(degree, px, py);
}

void Matrix::Translate(scalar dx, scalar dy)
{
    matrixImplPtr->Translate(dx, dy);
}

void Matrix::Scale(scalar sx, scalar sy, scalar px, scalar py)
{
    matrixImplPtr->Scale(sx, sy, px, py);
}

void Matrix::PreRotate(scalar degree)
{
    matrixImplPtr->PreRotate(degree);
}

void Matrix::PreTranslate(scalar dx, scalar dy)
{
    matrixImplPtr->PreTranslate(dx, dy);
}

void Matrix::PreScale(scalar sx, scalar sy)
{
    matrixImplPtr->PreScale(sx, sy);
}

void Matrix::PreConcat(const Matrix& other)
{
    matrixImplPtr->PreConcat(other);
}

bool Matrix::Invert(Matrix& inverse) const
{
    return matrixImplPtr->Invert(inverse);
}

Matrix Matrix::operator*(const Matrix& m)
{
    matrixImplPtr->Multiply(*this, m);
    return *this;
}

bool Matrix::operator==(const Matrix& m) const
{
    return matrixImplPtr->Equals(*this, m);
}

void Matrix::SetMatrix(scalar scaleX, scalar skewX, scalar transX, scalar skewY, scalar scaleY, scalar transY,
    scalar persp0, scalar persp1, scalar persp2)
{
    matrixImplPtr->SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
}

void Matrix::MapPoints(std::vector<Point>& dst, const std::vector<Point>& src, uint32_t count) const
{
    matrixImplPtr->MapPoints(dst, src, count);
}

bool Matrix::MapRect(Rect& dst, const Rect& src) const
{
    return matrixImplPtr->MapRect(dst, src);
}

void Matrix::Set(Index index, scalar value)
{
    matrixImplPtr->Set(index, value);
}

scalar Matrix::Get(int index) const
{
    return matrixImplPtr->Get(index);
}

void Matrix::GetAll(Buffer& buffer) const
{
    matrixImplPtr->GetAll(buffer);
}

bool Matrix::IsIdentity() const
{
    return matrixImplPtr->IsIdentity();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
