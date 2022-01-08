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

#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <stdint.h>

#include "effect/color_space.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum ColorType {
    COLORTYPE_UNKNOWN = 0,
    COLORTYPE_ALPHA_8,
    COLORTYPE_RGB_565,
    COLORTYPE_ARGB_4444,
    COLORTYPE_RGBA_8888,
    COLORTYPE_BGRA_8888,
};

enum AlphaType {
    ALPHATYPE_UNKNOWN = 0,
    ALPHATYPE_OPAQUYE,
    ALPHATYPE_PREMUL,
    ALPHATYPE_UNPREMUL,
};

struct Color4f {
    scalar redF_ = 0;
    scalar greenF_ = 0;
    scalar blueF_ = 0;
    scalar alphaF_ = 1;
};

typedef uint32_t ColorQuad;
class Color {
public:
    constexpr static ColorQuad COLOR_TRANSPARENT = 0;
    constexpr static ColorQuad COLOR_BLACK = 0xFF000000;
    constexpr static ColorQuad COLOR_DKGRAY = 0xFF444444;
    constexpr static ColorQuad COLOR_GRAY = 0xFF888888;
    constexpr static ColorQuad COLOR_LTGRAY = 0xFFCCCCCC;
    constexpr static ColorQuad COLOR_WHITE = 0xFFFFFFFF;
    constexpr static ColorQuad COLOR_RED = 0xFFFF0000;
    constexpr static ColorQuad COLOR_GREEN = 0xFF00FF00;
    constexpr static ColorQuad COLOR_BLUE = 0xFF0000FF;
    constexpr static ColorQuad COLOR_YELLOW = 0xFFFFFF00;
    constexpr static ColorQuad COLOR_CYAN = 0xFF00FFFF;
    constexpr static ColorQuad COLOR_MAGENTA = 0xFFFF00FF;

    constexpr static uint8_t RGB_MAX = 255;
    static inline ColorQuad ColorQuadSetARGB(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
    {
        return ((a & 0xffu) << 24) | ((r & 0xffu) << 16) | ((g & 0xffu) << 8) | ((b & 0xffu) << 0);
    }

    static inline int ColorQuadGetA(ColorQuad c)
    {
        return ((c >> 24) & 0xff);
    }

    static inline int ColorQuadGetR(ColorQuad c)
    {
        return ((c >> 16) & 0xff);
    }

    static inline int ColorQuadGetG(ColorQuad c)
    {
        return ((c >> 8) & 0xff);
    }

    static inline int ColorQuadGetB(ColorQuad c)
    {
        return ((c >> 0) & 0xff);
    }

    ~Color() {}

public:
    Color() noexcept;
    Color(const Color &c) noexcept;
    Color(int r, int g, int b, int a) noexcept;
    Color(ColorQuad rgba) noexcept;

    int GetRed() const;
    int GetGreen() const;
    int GetBlue() const;
    int GetAlpha() const;
    void SetRed(int r);
    void SetGreen(int g);
    void SetBlue(int b);
    void SetAlpha(int a);

    scalar GetRedF() const;
    scalar GetGreenF() const;
    scalar GetBlueF() const;
    scalar GetAlphaF() const;
    Color4f GetColor4f();
    void SetRedF(scalar r);
    void SetGreenF(scalar g);
    void SetBlueF(scalar b);
    void SetAlphaF(scalar a);

    void SetRgb(int r, int g, int b, int a = 255);
    void SetRgbF(scalar r, scalar g, scalar b, scalar a = 1.0);

    void SetColorQuad(int c);
    ColorQuad CastToColorQuad() const;

    friend bool operator==(const Color &c1, const Color &c2);
    friend bool operator!=(const Color &c1, const Color &c2);
private:
    int alpha_;
    int red_;
    int green_;
    int blue_;
    Color4f color4f_;
};
}
}
}
#endif
