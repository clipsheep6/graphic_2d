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
       
    };

    const ColorSpace ColorSpace::SRGB() {
        return {ColorSpaceName::SRGB,
                {
                        0.640f,
                        0.330f,
                        0.300f,
                        0.600f,
                        0.150f,
                        0.060f,
                        0.3127f,
                        0.3290f,
                },
                {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.04045f, 0.0f, 0.0f}};
    }

    const ColorSpace ColorSpace::LinearSRGB() {
        return {ColorSpaceName::LINEAR_SRGB,
                {
                        0.640f,
                        0.330f,
                        0.300f,
                        0.600f,
                        0.150f,
                        0.060f,
                        0.3127f,
                        0.3290f,
                },
                {
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                }};
    }

}
}
