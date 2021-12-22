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

#include "skia_camera.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaCamera::SkiaCamera() noexcept : view_() {}

void SkiaCamera::Save()
{
    view_.save();
}

void SkiaCamera::Restore()
{
    view_.restore();
}

void SkiaCamera::Translate(scalar x, scalar y, scalar z)
{
    view_.translate(x, y, z);
}

void SkiaCamera::RotateX(scalar deg)
{
    view_.rotateX(deg);
}

void SkiaCamera::RotateY(scalar deg)
{
    view_.rotateY(deg);
}

void SkiaCamera::RotateZ(scalar deg)
{
    view_.rotateZ(deg);
}

void SkiaCamera::SetCameraPos(scalar x, scalar y, scalar z)
{
    view_.setCameraLocation(x, y, z);
}

scalar SkiaCamera::GetCameraPosX() const
{
    return view_.getCameraLocationX();
}

scalar SkiaCamera::GetCameraPosY() const
{
    return view_.getCameraLocationY();
}

scalar SkiaCamera::GetCameraPosZ() const
{
    return view_.getCameraLocationZ();
}

void SkiaCamera::ApplyToMatrix(Matrix& m)
{
    SkMatrix matrix;
    view_.getMatrix(&matrix);
    m.SetMatrix(matrix.getScaleX(), matrix.getSkewX(), matrix.getTranslateX(),
                matrix.getScaleY(), matrix.getSkewY(), matrix.getTranslateY(),
                matrix.getPerspX(), matrix.getPerspY(), matrix.get(SkMatrix::kMPersp2));
}
}
}
}