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

#include "impl_factory.h"

#include "skia_adapter/skia_bitmap.h"
#include "skia_adapter/skia_camera.h"
#include "skia_adapter/skia_canvas.h"
#include "skia_adapter/skia_color_filter.h"
#include "skia_adapter/skia_color_space.h"
#include "skia_adapter/skia_image_filter.h"
#include "skia_adapter/skia_mask_filter.h"
#include "skia_adapter/skia_matrix.h"
#include "skia_adapter/skia_path.h"
#include "skia_adapter/skia_path_effect.h"
#include "skia_adapter/skia_shader_effect.h"

namespace OHOS {
namespace Rosen {
CoreCanvasImpl* CoreCanvasImplFactory::CreateCoreCanvasImpl()
{
    return new SkiaCanvas();
}

BitmapImpl* BitmapImplFactory::CreateBitmaImpl()
{
    return new SkiaBitmap();
}

PathImpl* PathImplFactory::CreatePathImpl()
{
    return new SkiaPath();
}

ColorFilterImpl* ColorFilterImplFactory::CreateColorFilterImpl()
{
    return new SkiaColorFilter();
}

MaskFilterImpl* MaskFilterImplFactory::CreateMaskFilterImpl()
{
    return new SkiaMaskFilter();
}

ImageFilterImpl* ImageFilterImplFactory::CreateImageFilterImpl()
{
    return new SkiaImageFilter();
}

PathEffectImpl* PathEffectImplFactory::CreatePathEffectImpl()
{
    return new SkiaPathEffect();
}

ShaderEffectImpl* ShaderEffectImplFactory::CreateShaderEffectImpl()
{
    return new SkiaShaderEffect();
}

ColorSpaceImpl* ColorSpaceImplFactory::CreateColorSpaceImpl()
{
    return new SkiaColorSpace();
}

MatrixImpl* MatrixImplFactory::CreateMatrixImpl()
{
    return new SkiaMatrix();
}

CameraImpl* CameraImplFactory::CreateCameraImpl()
{
    return new SkiaCamera();
}
}
}