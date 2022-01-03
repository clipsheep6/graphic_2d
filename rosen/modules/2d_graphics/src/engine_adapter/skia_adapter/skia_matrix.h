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

#ifndef SKIA_MATRIX_H
#define SKIA_MATRIX_H

#include <memory>

#include "include/core/SkMatrix.h"

#include "impl_interface/matrix_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaMatrix : public MatrixImpl {
public:
    SkiaMatrix();
    virtual ~SkiaMatrix() override {}
    void Rotate(scalar degree, scalar px, scalar py) override;
    void Translate(scalar dx, scalar dy) override;
    void Scale(scalar sx, scalar sy, scalar px, scalar py) override;
    const SkMatrix& ExportSkiaMatrix() const;
    void Multiply(const Matrix& a, const Matrix& b) override;
    bool Equals(const Matrix& a, const Matrix& b) override;
    void SetMatrix(scalar scaleX, scalar skewX, scalar transX,
        scalar skewY,  scalar scaleY, scalar transY,
        scalar persp0, scalar persp1, scalar persp2) override;
    scalar Get(int index) override;
private:
    std::unique_ptr<SkMatrix> skMatrix_;
};
}
}
}
#endif