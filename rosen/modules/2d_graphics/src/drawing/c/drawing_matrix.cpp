/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "c/drawing_matrix.h"

#include "utils/matrix.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Matrix* CastToMatrix(OH_Drawing_Matrix* cMatrix)
{
    return reinterpret_cast<Matrix*>(cMatrix);
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreate()
{
    return (OH_Drawing_Matrix*)new Matrix();
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreateRotation(float deg, float x, float y)
{
    Matrix* matrix = new Matrix();
    if (matrix == nullptr) {
        return nullptr;
    }
    matrix->Rotate(deg, x, y);
    return (OH_Drawing_Matrix*)matrix;
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreateScale(float sx, float sy, float px, float py)
{
    Matrix* matrix = new Matrix();
    if (matrix == nullptr) {
        return nullptr;
    }
    matrix->Scale(sx, sy, px, py);
    return (OH_Drawing_Matrix*)matrix;
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreateTranslation(float dx, float dy)
{
    Matrix* matrix = new Matrix();
    if (matrix == nullptr) {
        return nullptr;
    }
    matrix->Translate(dx, dy);
    return (OH_Drawing_Matrix*)matrix;
}

void OH_Drawing_MatrixSetMatrix(OH_Drawing_Matrix* cMatrix, float scaleX, float skewX, float transX,
    float skewY, float scaleY, float transY, float persp0, float persp1, float persp2)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
}

void OH_Drawing_MatrixPreConcat(OH_Drawing_Matrix* cMatrix, OH_Drawing_Matrix* other)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    Matrix* otherMatrix = CastToMatrix(other);
    if (otherMatrix == nullptr) {
        return;
    }
    matrix->PreConcat(*otherMatrix);
}

void OH_Drawing_MatrixRotate(OH_Drawing_Matrix* cMatrix, float degree, float px, float py)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->Rotate(degree, px, py);
}

void OH_Drawing_MatrixTranslate(OH_Drawing_Matrix* cMatrix, float dx, float dy)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->Translate(dx, dy);
}

void OH_Drawing_MatrixScale(OH_Drawing_Matrix* cMatrix, float sx, float sy, float px, float py)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->Scale(sx, sy, px, py);
}

bool OH_Drawing_MatrixInvert(OH_Drawing_Matrix* cMatrix, OH_Drawing_Matrix* inverse)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return false;
    }
    Matrix* inverseMatrix = CastToMatrix(inverse);
    if (inverseMatrix == nullptr) {
        return false;
    }
    return matrix->Invert(*inverseMatrix);
}

bool OH_Drawing_MatrixIsEqual(OH_Drawing_Matrix* cMatrix, OH_Drawing_Matrix* other)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return false;
    }
    Matrix* otherMatrix = CastToMatrix(other);
    if (otherMatrix == nullptr) {
        return false;
    }
    return (*matrix == *otherMatrix);
}

bool OH_Drawing_MatrixIsIdentity(OH_Drawing_Matrix* cMatrix)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return false;
    }
    return matrix->IsIdentity();
}

void OH_Drawing_MatrixDestroy(OH_Drawing_Matrix* cMatrix)
{
    delete CastToMatrix(cMatrix);
}

void OH_Drawing_MatrixScale(OH_Drawing_Matrix* cMatrix, float scaleX, float scaleY, float pivotX, float pivotY)
{
     Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->Scale(scaleX, scaleY, pivotX, pivotY);
}