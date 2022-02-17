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


const ColorSpace ColorSpace::SRGB()
{
return {ColorSpaceName::SRGB,
        {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
        {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.04045f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::LinearSRGB() {
return {ColorSpaceName::LINEAR_SRGB,
        {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::LinearSRGB() {
return {ColorSpaceName::LINEAR_SRGB,
        {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}

// const ColorSpace ColorSpace::extendedSRGB() {
// return {ColorSpaceName::EXTENDED_SRGB,
//         {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
//         std::bind(absRcpResponse, _1, 2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.04045f),
//         std::bind(absResponse,    _1, 2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.04045f),
//         std::bind(clamp<float>, _1, -0.799f, 2.399f)
// };
// }
const ColorSpace ColorSpace::linearExtendedSRGB() {
return {ColorSpaceName::LINEAR_EXTENDED_SRGB
        {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
        {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::NTSC() {
return {ColorSpaceName::NTSC_1953
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
        {0.708f, 0.292f, 0.170f, 0.797f, 0.131f, 0.046f, 0.3127f, 0.3290f}
        {1 / 0.45f, 1 / 1.099f, 0.099f / 1.099f, 1 / 4.5f, 0.081f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::AdobeRGB() {
return {ColorSpaceName::ADOBE_RGB,
        {0.640f, 0.330f, 0.300f, 0.600f, 0.150f, 0.060f, 0.3127f, 0.3290f},
        {2.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}

// const ColorSpace ColorSpace::ProPhotoRGB() {
// return {ColorSpaceName::
//         "ROMM RGB ISO 22028-2:2013",
//         {{float2{0.7347f, 0.2653f}, {0.1596f, 0.8404f}, {0.0366f, 0.0001f}}},
//         {0.34567f, 0.35850f},
//         {1.8f, 1.0f, 0.0f, 1 / 16.0f, 0.031248f, 0.0f, 0.0f}
// };
// }

const ColorSpace ColorSpace::DisplayP3() {
return {ColorSpaceName::DISPLAY_P3,
        {0.680f, 0.320f, 0.265f, 0.690f, 0.150f, 0.060f, 0.3127f, 0.3290f},
        {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.039f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::DCIP3() {
return {ColorSpaceName::DCI_P3
        {0.680f, 0.320f, 0.265f, 0.690f, 0.150f, 0.060f, 0.314f, 0.351f},
        {2.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}

const ColorSpace ColorSpace::ACES() {
return {ColorSpaceName::ACES,
        {0.73470f, 0.26530f, 0.0f, 1.0f, 0.00010f, -0.0770f, 0.32168f, 0.33767f},
        {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};

}

const ColorSpace ColorSpace::ACEScg() {
return {ColorSpaceName::ACESCG,
        {0.713f, 0.293f, 0.165f, 0.830f, 0.128f, 0.044f, 0.32168f, 0.33767f},
        {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
}

ColorSpaceConnector::ColorSpaceConnector(
        const ColorSpace& src,
        const ColorSpace& dst) noexcept
        : mSource(src)
        , mDestination(dst) {
    if (all(lessThan(abs(src.getWhitePoint() - dst.getWhitePoint()), float2{1e-3f}))) {
        mTransform = dst.getXYZtoRGB() * src.getRGBtoXYZ();
    } else {
        mat3 rgbToXYZ(src.getRGBtoXYZ());
        mat3 xyzToRGB(dst.getXYZtoRGB());
        float3 srcXYZ = ColorSpace::XYZ(float3{src.getWhitePoint(), 1});
        float3 dstXYZ = ColorSpace::XYZ(float3{dst.getWhitePoint(), 1});
        if (any(greaterThan(abs(src.getWhitePoint() - ILLUMINANT_D50_XY), float2{1e-3f}))) {
            rgbToXYZ = adaptation(BRADFORD, srcXYZ, ILLUMINANT_D50_XYZ) * src.getRGBtoXYZ();
        }
        if (any(greaterThan(abs(dst.getWhitePoint() - ILLUMINANT_D50_XY), float2{1e-3f}))) {
            xyzToRGB = inverse(adaptation(BRADFORD, dstXYZ, ILLUMINANT_D50_XYZ) * dst.getRGBtoXYZ());
        }
        mTransform = xyzToRGB * rgbToXYZ;
    }
}

}
}
