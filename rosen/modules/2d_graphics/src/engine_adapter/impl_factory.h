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

#ifndef IMPLFACTORY_H
#define IMPLFACTORY_H

#include "impl_interface/bitmap_impl.h"
#include "impl_interface/camera_impl.h"
#include "impl_interface/color_filter_impl.h"
#include "impl_interface/color_space_impl.h"
#include "impl_interface/core_canvas_impl.h"
#include "impl_interface/image_filter_impl.h"
#include "impl_interface/mask_filter_impl.h"
#include "impl_interface/matrix_impl.h"
#include "impl_interface/path_effect_impl.h"
#include "impl_interface/path_impl.h"
#include "impl_interface/shader_effect_impl.h"

namespace OHOS {
namespace Rosen {
class CoreCanvasImplFactory {
public:
    static CoreCanvasImpl* CreateCoreCanvasImpl();
};

class BitmapImplFactory {
public:
    static BitmapImpl* CreateBitmaImpl();
};

class PathImplFactory {
public:
    static PathImpl* CreatePathImpl();
};

class ColorFilterImplFactory {
public:
    static ColorFilterImpl* CreateColorFilterImpl();
};

class MaskFilterImplFactory {
public:
    static MaskFilterImpl* CreateMaskFilterImpl();
};

class ImageFilterImplFactory {
public:
    static ImageFilterImpl* CreateImageFilterImpl();
};

class PathEffectImplFactory {
public:
    static PathEffectImpl* CreatePathEffectImpl();
};

class ShaderEffectImplFactory {
public:
    static ShaderEffectImpl* CreateShaderEffectImpl();
};

class ColorSpaceImplFactory {
public:
    static ColorSpaceImpl* CreateColorSpaceImpl();
};

class MatrixImplFactory {
public:
    static MatrixImpl* CreateMatrixImpl();
};

class CameraImplFactory {
public:
    static CameraImpl* CreateCameraImpl();
};
}
}
#endif