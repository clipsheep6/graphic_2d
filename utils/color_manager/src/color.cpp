
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

#include "color.h"
#include "iostream"

namespace OHOS 
{
namespace ColorManager 
{

    Color Color::GetInstanceOf(float r, float g, float b, float a, ColorSpaceName name) 
    {          
        return Color(r, g, b, a, name);
    }

    Color Color::GetInstanceOf(int64_t color) { return GetInstanceOf((int)color); }

    static float Alpha(int color) { return ((color >> 24) & 0xff) / 255.0f; }

    static float Red(int color) { return ((color >> 16) & 0xff) / 255.0f; }

    static float Green(int color) { return ((color >> 8) & 0xff) / 255.0f; }

    static float Blue(int color) { return (color & 0xff) / 255.0f; }

 
    Color Color::GetInstanceOf(int color) {
        float r = Red(color);
        float g = Green(color);
        float b = Blue(color);
        float a = Alpha(color);
        return Color(r, g, b, a, ColorSpaceName::SRGB);
    }

    int Color::PackValue(float r, float g, float b, float a, ColorSpaceName name) 
    {
        int argb = ((int) (a * 255.0f + 0.5f) << 24) | 
                    ((int) (r * 255.0f + 0.5f) << 16) |
                    ((int) (g * 255.0f + 0.5f) << 8)  | 
                    (int) (b * 255.0f + 0.5f);
        return (int)argb;
    }

    int Color::PackValue(float r, float g, float b, float a) {
        int argb = ((int)(a * 255.0f + 0.5f) << 24) | 
                    ((int)(r * 255.0f + 0.5f) << 16) |
                    ((int)(g * 255.0f + 0.5f) << 8)  | 
                    (int)(b * 255.0f + 0.5f);
        return (int)argb;
    }

    int Color::PackValue(int color) 
    { 
        int argb = ((int)(Alpha(color) * 255.0f + 0.5f) << 24) | 
                    ((int)(Red(color)   * 255.0f + 0.5f) << 16) |
                    ((int)(Green(color) * 255.0f + 0.5f) << 8)  | 
                    (int)(Blue(color)  * 255.0f + 0.5f);
        return (int)argb;
    }

    // alpha value not involved in convert
    int Color::Convert(float r, float g, float b, float a, ColorSpaceConvertor convertor)
    {
        std::array<float, 3>  dstColor = convertor.Convert(r, g, b);

        return PackValue(dstColor[0], dstColor[1], dstColor[2], a);
    };
    
    int Color::Convert(int color, ColorSpaceConvertor convertor) 
    { 
        return Convert(Red(color), Green(color), Blue(color), Alpha(color), convertor);
    }

    int Color::Convert(float r, float g, float b, float a, 
        ColorSpaceName src, ColorSpaceName dst) 
    {
        // gamut 线性化实际在skia中完成
        ColorSpaceConvertor csc(src, dst, GamutMappingMode::GAMUT_MAP_CONSTANT);
        return Convert(r, g, b, a, csc);
    }
    
    int Color::Convert(int color, ColorSpaceName dst) 
    { 
        return Convert(
                Red(color), Green(color), Blue(color), Alpha(color), colorSpace, dst);
    }

    int Color::Convert(int64_t color, ColorSpaceName dst) 
    { 
        return Convert((int)color, dst);
    }

}
}