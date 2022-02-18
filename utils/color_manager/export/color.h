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

#ifndef COLOR
#define COLOR

#include "color_space_convertor.h"

namespace OHOS {
namespace ColorManager {

class Color {
public:
    Color() {};
    Color(float r, float g, float b, float a);
    Color(float r, float g, float b, float a, const ColorSpaceName name);
    Color(int color);
    Color(int64_t color);
    // int PackValue(int color); 

    /**
     * Packs the 3 component color defined by the specified red, green, blue and
     * alpha component values into a color int in the specified color space.
     */ 
    int64_t PackValue() const;

    Color Convert(ColorSpaceConvertor &convertor) const;
    Color Convert(const ColorSpace &dst) const;

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;
    ColorSpaceName colorSpaceName = ColorSpaceName::SRGB;
};
}  // namespace ColorManager
}  // namespace OHOS
#endif
