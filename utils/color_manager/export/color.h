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
    Color(float r, float g, float b, float a, const ColorSpaceName name = ColorSpaceName::SRGB)
            : R(r), G(g), B(b), A(a), colorSpace(name){};
    
    Color GetInstanceOf(float r, float g, float b, float a, ColorSpaceName name);
    Color GetInstanceOf(int color);
    Color GetInstanceOf(int64_t color);
    int packValue(int color);

    /**
     * Packs the 3 component color defined by the specified red, green, blue and
     * alpha component values into a color int in the specified color space.
     */ 
    int PackValue(float r, float g, float b, float a);
    int PackValue(float r, float g, float b, float a, ColorSpaceName name);

    int Convert(float r, float g, float b, float a, ColorSpaceConvertor convertor);
    int Convert(int color, ColorSpaceConvertor convertor);
    int Convert(float r, float g, float b, float a, ColorSpaceName src, ColorSpaceName dst);
    int Convert(int color, ColorSpaceName dst);
    int Convert(int64_t color, ColorSpaceName dst);

private:
    float R = 0.0f;
    float G = 0.0f;
    float B = 0.0f;
    float A = 0.0f;
    ColorSpaceName colorSpace = ColorSpaceName::SRGB;
};
}  // namespace ColorManager
}  // namespace OHOS
#endif
