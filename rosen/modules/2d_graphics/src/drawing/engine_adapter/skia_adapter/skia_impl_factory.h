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

#ifndef SKIA_IMPLFACTORY_H
#define SKIA_IMPLFACTORY_H

#include "impl_interface/bitmap_impl.h"
#include "impl_interface/camera_impl.h"
#include "impl_interface/color_filter_impl.h"
#include "impl_interface/color_space_impl.h"
#include "impl_interface/core_canvas_impl.h"
#include "impl_interface/data_impl.h"
#ifdef ACE_ENABLE_GPU
#include "impl_interface/gpu_context_impl.h"
#endif
#include "impl_interface/image_filter_impl.h"
#include "impl_interface/image_impl.h"
#include "impl_interface/mask_filter_impl.h"
#include "impl_interface/matrix_impl.h"
#include "impl_interface/matrix44_impl.h"
#include "impl_interface/path_effect_impl.h"
#include "impl_interface/path_impl.h"
#include "impl_interface/picture_impl.h"
#include "impl_interface/region_impl.h"
#include "impl_interface/shader_effect_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImplFactory {
public:
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvas();
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvas(void* rawCanvas);
    static std::unique_ptr<CoreCanvasImpl> CreateCoreCanvas(int32_t width, int32_t height);
    static std::unique_ptr<DataImpl> CreateData();
#ifdef ACE_ENABLE_GPU
    static std::unique_ptr<GPUContextImpl> CreateGPUContext();
#endif
    static std::unique_ptr<BitmapImpl> CreateBitmap();
    static std::unique_ptr<ImageImpl> CreateImage();
    static std::unique_ptr<ImageImpl> CreateImage(void* rawImg);
    static std::unique_ptr<PictureImpl> CreatePicture();
    static std::unique_ptr<PathImpl> CreatePath();
    static std::unique_ptr<ColorFilterImpl> CreateColorFilter();
    static std::unique_ptr<MaskFilterImpl> CreateMaskFilter();
    static std::unique_ptr<ImageFilterImpl> CreateImageFilter();
    static std::unique_ptr<ShaderEffectImpl> CreateShaderEffect();
    static std::unique_ptr<PathEffectImpl> CreatePathEffect();
    static std::unique_ptr<ColorSpaceImpl> CreateColorSpace();
    static std::unique_ptr<MatrixImpl> CreateMatrix();
    static std::unique_ptr<Matrix44Impl> CreateMatrix44();
    static std::unique_ptr<CameraImpl> CreateCamera();
    static std::unique_ptr<RegionImpl> CreateRegion();
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif