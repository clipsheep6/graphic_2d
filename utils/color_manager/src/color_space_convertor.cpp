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
#include "src/core/SkVM.h"
#include "color_space_convertor.h"

namespace OHOS {
namespace ColorManager {

static const std::array<float, 2> ILLUMINANT_D50_XY = {0.34567f, 0.35850f};
static const std::array<float, 3> ILLUMINANT_D50_XYZ = {0.964212f, 1.0f, 0.825188f};
static const Matrix3x3 BRADFORD = {{{0.8951f, -0.7502f, 0.0389f},
                                    {0.2664f, 1.7135f, -0.0685f},
                                    {-0.1614f, 0.0367f, 1.0296f}}};


static Matrix3x3 adaptation(const Matrix3x3& matrix,
                            const std::array<float, 3>& srcWhitePoint,
                            const std::array<float, 3>& dstWhitePoint) {
    std::array<float, 3> srcLMS = matrix * srcWhitePoint;
    std::array<float, 3> dstLMS = matrix * dstWhitePoint;
    return ColorSpace::invert(matrix) * (dstLMS / srcLMS) * matrix;
}

ColorSpaceConvertor::ColorSpaceConvertor(ColorSpaceName srcName,
                                         ColorSpaceName dstName,
                                         GamutMappingMode gamutMappingMode)
        : srcName(srcName)
        , dstName(dstName)
        , srcColorSpace(ColorSpace(srcName, 0))
        , dstColorSpace(ColorSpace(srcName, 0))
        , mappingMode(gamutMappingMode) {

    if (AllLessThan(srcColorSpace.getWhitePoint(), dstColorSpace.getWhitePoint())) {
        transferMatrix = dstColorSpace.getXYZToRGB() * srcColorSpace.getRGBToXYZ();
    }else {
        Matrix3x3 rgbToXYZ(srcColorSpace.getRGBToXYZ());
        Matrix3x3 xyzToRGB(dstColorSpace.getXYZToRGB());

        std::array<float, 3> srcXYZ = ColorSpace::XYZ(std::array<float, 3>{
                srcColorSpace.getWhitePoint()[0], srcColorSpace.getWhitePoint()[0], 1});
        std::array<float, 3> dstXYZ = ColorSpace::XYZ(std::array<float, 3>{
                dstColorSpace.getWhitePoint()[0], dstColorSpace.getWhitePoint()[1], 1});

        if (AnyGreatThan(dstColorSpace.getWhitePoint(), ILLUMINANT_D50_XY)) {
            rgbToXYZ = adaptation(BRADFORD, srcXYZ, ILLUMINANT_D50_XYZ) * srcColorSpace.getRGBToXYZ();
        }

        if (AnyGreatThan(srcColorSpace.getWhitePoint(), ILLUMINANT_D50_XY)) {
            xyzToRGB = ColorSpace::invert(adaptation(BRADFORD, dstXYZ, ILLUMINANT_D50_XYZ) *
                                       dstColorSpace.getRGBToXYZ());
        }

        transferMatrix = xyzToRGB * rgbToXYZ;
    }
}

std::array<float, 3> ColorSpaceConvertor::Transform(const std::array<float, 3>& v) {
    std::array<float, 3> srcLinear = v;
    for (auto& n : srcLinear) {
        n = std::clamp(n, srcColorSpace.clampMin, srcColorSpace.clampMax);
    }
    srcLinear = srcColorSpace.toLinear(srcLinear, srcColorSpace.GetGamma());

    std::array<float, 3> dstNonLinear =
            dstColorSpace.toNonLinear(transferMatrix * srcLinear, dstColorSpace.GetGamma());
    for (auto& n : dstNonLinear) {
        n = std::clamp(n, dstColorSpace.clampMin, dstColorSpace.clampMax);
    }
    return dstNonLinear;
}

std::array<float, 3> ColorSpaceConvertor::TransformLinear(const std::array<float, 3>& v) {
    std::array<float, 3> srcLinear = v;
    for (auto& n : srcLinear) {
        n = std::clamp(n, srcColorSpace.clampMin, srcColorSpace.clampMax);
    }

    std::array<float, 3> dstLinear = transferMatrix * srcLinear;
    for (auto& n : dstLinear) {
        n = std::clamp(n, dstColorSpace.clampMin, dstColorSpace.clampMax);
    }
    return dstLinear;
}


std::array<float, 3> ColorSpaceConvertor::Convert(float r, float g, float b) {
    std::array<float, 3> tempRes{};
    std::array<float, 3> dst = Transform(std::array<float, 3> {r, g, b});
    tempRes[0] = dst[0];
    tempRes[1] = dst[1];
    tempRes[2] = dst[2];

    return tempRes;
};

// AnyGreatThan
bool ColorSpaceConvertor::AnyGreatThan(std::array<float, 2> vecA, std::array<float, 2> vecB) {
    for (unsigned int i = 0; i < vecA.size(); i++) {
        if (std::abs(vecA[i]) - std::abs(vecB[i]) > 1e-3f) {
            return true;
        }
    }
    return false;
}

// AllLessThan
bool ColorSpaceConvertor::AllLessThan(std::array<float, 2> vecA, std::array<float, 2> vecB) {
    for (unsigned i = 0; i < vecA.size(); i++) {
        if (std::abs(vecA[i]) - std::abs(vecB[i]) > 1e-3f) {
            return false;
        }
    }
    return true;
}

}  // namespace ColorManager
}  // namespace OHOS