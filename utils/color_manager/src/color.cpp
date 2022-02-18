
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
#include <cstdint>

namespace OHOS {
namespace ColorManager {
static float Alpha(int64_t color)
{
    return ((color >> 56) & 0xff) / 255.0f; // shift 56 bits to get alpha value and 255.0f is the max value
}

static float Red(int64_t color)
{
    return ((color >> 48) & 0xff) / 255.0f; // shift 48 bits to get red value and 255.0f is the max value
}

static float Green(int64_t color)
{
    return ((color >> 40) & 0xff) / 255.0f; // shift 40 bits to get green value and 255.0f is the max value
}

static float Blue(int64_t color)
{
    return ((color >> 32) & 0xff) / 255.0f; // shift 32 bits to get blue value and 255.0f is the max value
}

static ColorSpaceName Name(int64_t color)
{
    return (ColorSpaceName)(color & 0xffffffff);
}

Color::Color(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a) {}

Color::Color(float r, float g, float b, float a, const ColorSpaceName name)
    : r(r), g(g), b(b), a(a), colorSpaceName(name) {}

Color::Color(int color)
    : r(Red((int64_t)color<<32)),
      g(Green((int64_t)color<<32)),
      b(Blue((int64_t)color<<32)),
      a(Alpha((int64_t)color<<32)) {}

Color::Color(int64_t color)
    : r(Red(color)),
      g(Green(color)),
      b(Blue(color)),
      a(Alpha(color)),
      colorSpaceName(Name(color)) {}

int64_t Color::PackValue() const
{
    // shift 48, 40, 32 bits to get rgb value and pack the name into it
    int64_t argbn = ((int64_t)(a * 255.0f + 0.5f) << 56) |
                    ((int64_t)(r * 255.0f + 0.5f) << 48) |
                    ((int64_t)(g * 255.0f + 0.5f) << 40) |
                    ((int64_t)(b * 255.0f + 0.5f) << 32) |
                    ((int32_t)colorSpaceName);
    return argbn;
}

// alpha value not involved in convert
Color Color::Convert(ColorSpaceConvertor &convertor) const
{
    Vector3 dstColor = convertor.Convert( {r, g, b} );
    // dstColor[0], dstColor[1], dstColor[2] : rgb
    return Color(dstColor[0], dstColor[1], dstColor[2], a, convertor.GetDstColorSpace().GetColorSpaceName());
}

Color Color::Convert(const ColorSpace &dst) const
{
    ColorSpaceConvertor convertor(ColorSpace(colorSpaceName), dst, GamutMappingMode::GAMUT_MAP_CONSTANT);
    return Convert(convertor);
}
}
}