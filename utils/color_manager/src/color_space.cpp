
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

#include "color_space.h"

namespace OHOS 
{
namespace ColorManager 
{
    ColorSpace::ColorSpace(ColorSpaceName name, int64_t nativeHandle)
    : colorSpaceName(name), nativeHandle(nativeHandle)
    {
        switch (name) { 
        
        case ColorSpaceName::SRGB:
            skColorSpace = SkColorSpace::MakeSRGB().get();
            break;

        case ColorSpaceName::LINEAR_SRGB:
            skColorSpace = SkColorSpace::MakeSRGBLinear().get();
            break;

        case ColorSpaceName::ADOBE_RGB:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::k2Dot2, 
                SkNamedGamut::kAdobeRGB).get();
            break;

        case ColorSpaceName::DISPLAY_P3:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::k2Dot2, 
                SkNamedGamut::kDisplayP3).get();
            break;
        
        default:
            skColorSpace = SkColorSpace::MakeSRGB().get();
        }
        
    };
}
}