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

namespace OHOS {
namespace ColorManager {
ColorSpace::ColorSpace(ColorSpaceName name, int64_t nativeHandle) {
    switch (name) {
        case ColorSpaceName::SRGB:
            SRGB();
            break;
        case ColorSpaceName::DISPLAY_P3:
            DisplayP3();
            break;
        case ColorSpaceName::ADOBE_RGB:
            AdobeRGB();
            break;
        default:
            SRGB();
            break;
    }
}

ColorSpace::ColorSpace(ColorSpaceName name,
                       int64_t nativeHandle,
                       ColorSpacePrimaries primaries,
                       Matrix3x3 toXYZ,
                       TransferFunc transferFunc)
        : colorSpaceName(name)
        , nativeHandle(nativeHandle)
        , primaries(primaries)
        , toXYZ(toXYZ)
        , transferFunc(transferFunc) {}

ColorSpace::ColorSpace(ColorSpaceName name,
                       int64_t nativeHandle,
                       ColorSpacePrimaries primaries,
                       Matrix3x3 toXYZ,
                       float gamma)
        : colorSpaceName(name)
        , nativeHandle(nativeHandle)
        , primaries(primaries)
        , toXYZ(toXYZ)
        , transferFunc({gamma, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0}) {}

ColorSpace::ColorSpace(ColorSpaceName name,
                       int64_t nativeHandle,
                       ColorSpacePrimaries primaries,
                       TransferFunc transferFunc)
        : colorSpaceName(name)
        , nativeHandle(nativeHandle)
        , primaries(primaries)
        , toXYZ(ComputeXYZ(primaries))
        , transferFunc(transferFunc) {}

ColorSpace::ColorSpace(ColorSpaceName name,
                       ColorSpacePrimaries primaries,
                       TransferFunc transferFunc)
        : colorSpaceName(name)
        , nativeHandle(0)
        , primaries(primaries)
        , toXYZ(ComputeXYZ(primaries))
        , transferFunc(transferFunc) {}

const ColorSpace ColorSpace::SRGB() {
    return {ColorSpaceName::SRGB,
            {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
            {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.04045f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::LinearSRGB() {
    return {ColorSpaceName::LINEAR_SRGB,
            {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::NTSC() {
    return {ColorSpaceName::NTSC_1953,
            {0.67f, 0.33f, 0.21f, 0.71f, 0.14f, 0.08f, 0.310f, 0.316f},
            {1 / 0.45f, 1 / 1.099f, 0.099f / 1.099f, 1 / 4.5f, 0.081f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::BT709() {
    return {ColorSpaceName::BT709,
            {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
            {1 / 0.45f, 1 / 1.099f, 0.099f / 1.099f, 1 / 4.5f, 0.081f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::BT2020() {
    return {ColorSpaceName::BT2020,
            {0.708f, 0.292f, 0.170f, 0.797f, 0.131f, 0.046f, 0.3127f, 0.3290f},
            {1 / 0.45f, 1 / 1.099f, 0.099f / 1.099f, 1 / 4.5f, 0.081f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::AdobeRGB() {
    return {ColorSpaceName::ADOBE_RGB,
            {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
            {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::ProPhotoRGB() {
    return {ColorSpaceName::PRO_PHOTO_RGB,
            {0.7347f, 0.2653f, 0.1596f, 0.8404f, 0.0366f, 0.0001f, 0.34567f, 0.35850f},
            {1.8f, 1.0f, 0.0f, 1 / 16.0f, 0.031248f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::DisplayP3() {
    return {ColorSpaceName::DISPLAY_P3,
            {0.680f, 0.320f, 0.265f, 0.690f, 0.150f, 0.060f, 0.3127f, 0.3290f},
            {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.039f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::DCIP3() {
    return {ColorSpaceName::DCI_P3,
            {0.680f, 0.320f, 0.265f, 0.690f, 0.150f, 0.060f, 0.314f, 0.351f},
            {2.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}


Matrix3x3 ColorSpace::ComputeXYZ(const ColorSpacePrimaries& primaries) {
    float oneRxRy = (1 - primaries.RX) / primaries.RY;
    float oneGxGy = (1 - primaries.GX) / primaries.GY;
    float oneBxBy = (1 - primaries.BX) / primaries.BY;
    float oneWxWy = (1 - primaries.WX) / primaries.WY;

    float RxRy = primaries.RX / primaries.RY;
    float GxGy = primaries.GX / primaries.GY;
    float BxBy = primaries.BX / primaries.BY;
    float WxWy = primaries.WX / primaries.WY;

    float BY = ((oneWxWy - oneRxRy) * (GxGy - RxRy) - (WxWy - RxRy) * (oneGxGy - oneRxRy)) /
               ((oneBxBy - oneRxRy) * (GxGy - RxRy) - (BxBy - RxRy) * (oneGxGy - oneRxRy));
    float GY = (WxWy - RxRy - BY * (BxBy - RxRy)) / (GxGy - RxRy);
    float RY = 1 - GY - BY;

    float RYRy = RY / primaries.RY;
    float GYGy = GY / primaries.GY;
    float BYBy = BY / primaries.BY;

    return {{{RYRy * primaries.RX, RY, RYRy * (1 - primaries.RX - primaries.RY)},
             {GYGy * primaries.GX, GY, GYGy * (1 - primaries.GX - primaries.GY)},
             {BYBy * primaries.BX, BY, BYBy * (1 - primaries.BX - primaries.BY)}}};
}

static bool isfinitef_(float x) { return 0 == x * 0; }

// inverse src Matrix to dst Matrix
Matrix3x3 ColorSpace::invert(const Matrix3x3& src) {
    Matrix3x3 dst{};

    double a00 = src[0][0], a01 = src[1][0], a02 = src[2][0],
           a10 = src[0][1], a11 = src[1][1], a12 = src[2][1],
           a20 = src[0][2], a21 = src[1][2], a22 = src[2][2];

    double b0 = a00 * a11 - a01 * a10, b1 = a00 * a12 - a02 * a10, b2 = a01 * a12 - a02 * a11,
           b3 = a20, b4 = a21, b5 = a22;

    double determinant = b0 * b5 - b1 * b4 + b2 * b3;

    if (determinant == 0) {
        return dst;
    }

    double invdet = 1.0 / determinant;
    if (invdet > +FLT_MAX || invdet < -FLT_MAX || !isfinitef_((float)invdet)) {
        return dst;
    }

    b0 *= invdet;
    b1 *= invdet;
    b2 *= invdet;
    b3 *= invdet;
    b4 *= invdet;
    b5 *= invdet;

    dst[0][0] = (float)(a11 * b5 - a12 * b4);
    dst[1][0] = (float)(a02 * b4 - a01 * b5);
    dst[2][0] = (float)(+b2);
    dst[0][1] = (float)(a12 * b3 - a10 * b5);
    dst[1][1] = (float)(a00 * b5 - a02 * b3);
    dst[2][1] = (float)(-b1);
    dst[0][2] = (float)(a10 * b4 - a11 * b3);
    dst[1][2] = (float)(a01 * b3 - a00 * b4);
    dst[2][2] = (float)(+b0);

    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) {
            if (!isfinitef_(dst[r][c])) {
                return dst;
            }
        }
    return dst;
}

SkColorSpace* ColorSpace::colorSpace() const {
    skcms_Matrix3x3 toXYZ = ToSkiaXYZ();

    return SkColorSpace::MakeRGB(
                   {
                           transferFunc.g,
                           transferFunc.a,
                           transferFunc.b,
                           transferFunc.c,
                           transferFunc.d,
                           transferFunc.e,
                           transferFunc.f,
                   },toXYZ).get();
}

skcms_Matrix3x3 ColorSpace::ToSkiaXYZ() const {
    skcms_Matrix3x3 skToXYZMatrix;
    SkColorSpacePrimaries skPrimaries = {
            transferFunc.g,
            transferFunc.a,
            transferFunc.b,
            transferFunc.c,
            transferFunc.d,
            transferFunc.e,
            transferFunc.f,
    };
    skPrimaries.toXYZD50(&skToXYZMatrix);

    return skToXYZMatrix;
}

std::array<float, 2> ColorSpace::getWhitePoint() {
    return std::array<float, 2>{primaries.WX, primaries.WY};
}


std::array<float, 3> ColorSpace::toLinear(std::array<float, 3> v, float g) {
    if (g == 1.0f) {
        return v;
    }
    std::array<float, 3> res = v;
    for (auto& n : res) {
        n = std::pow(n, g);
    }
    return res;
}

std::array<float, 3> ColorSpace::toNonLinear(std::array<float, 3> v, float g) {
    if (g == 1.0f) {
        return v;
    }
    std::array<float, 3> res = v;
    for (auto& n : res) {
        n = std::pow(n, 1.0f / g);
    }

    return res;
}

std::array<float, 3> ColorSpace::toLinear(std::array<float, 3> v,
                                          const TransferFunc& p) {
    std::array<float, 3> res = v;
    for (auto& n : res) {
        n = n >= p.d ? std::pow(p.a * n + p.b, p.g) + p.e : p.c * n + p.f;
    }
    return res;
}

std::array<float, 3> ColorSpace::toNonLinear(std::array<float, 3> v, const TransferFunc& p) {
    std::array<float, 3> res = v;
    for (auto& n : res) {
        n = n >= p.d * p.c ? (std::pow(n - p.e, 1.0f / p.g) - p.b) / p.a : (n - p.f) / p.c;
    }
    return res;
}

}
}
