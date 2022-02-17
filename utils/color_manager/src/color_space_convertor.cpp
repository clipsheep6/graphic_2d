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

static const float2 ILLUMINANT_D50_XY = {0.34567f, 0.35850f};
static const float3 ILLUMINANT_D50_XYZ = {0.964212f, 1.0f, 0.825188f};
static const mat3 BRADFORD = mat3{
    float3{ 0.8951f, -0.7502f,  0.0389f},
    float3{ 0.2664f,  1.7135f, -0.0685f},
    float3{-0.1614f,  0.0367f,  1.0296f}
};

static Matrix3x3 adaptation(const Matrix3x3& matrix, const float3& srcWhitePoint, const float3& dstWhitePoint) {
    float3 srcLMS = matrix * srcWhitePoint;
    float3 dstLMS = matrix * dstWhitePoint;
    return inverse(matrix) * mat3{dstLMS / srcLMS} * matrix;
}

ColorSpaceConvertor::ColorSpaceConvertor(ColorSpaceName src, ColorSpaceName dst, GamutMappingMode mapingMode)
	: srcColorSpace(src), dstColorSpace(dst), mappingMode(mapingMode)
{
};

SkColorSpaceXformSteps* ColorSpaceConvertor::ToSkiaCnvertor() const
{
    ColorSpace srcCS = ColorSpace(srcColorSpace, 3);
    ColorSpace dstCS = ColorSpace(dstColorSpace, 3);
    return &SkColorSpaceXformSteps(srcCS, dstCS);
}

std::vector<float> ColorSpaceConvertor::Convert(float r, float g, float b)
{
    float rgba[4] = {r, g, b, 0.0f}; 
	// SkConvertor.apply(rgba);
	std::vector<float> dstColor(3);
	
	dstColor[0] = rgba[0];
	dstColor[1] = rgba[1];
	dstColor[2] = rgba[2];

	return dstColor;
};


// GreatThan
bool ColorSpaceConvertor::AnyGreatThan(std::array<float, 2> vec1, std::array<float, 2> vec2) 
{
    std::array<bool, 2> result;
    for (int i = 0; i < vec1.size(); i++) {
        if (vec1[i] > vec2[i]) {
            return true;
        }
    }
    return false;
}

ColorSpaceConvertor::ColorSpaceConvertor(ColorSpace& src, ColorSpace& dst): src(src), dst(dst) 
{
    if (all(lessThan(abs(src.getWhitePoint() - dst.getWhitePoint()), float2{1e-3f}))) {
        transferMatrix = dst.getXYZtoRGB() * src.getRGBtoXYZ();
    } else {
        Matrix3x3 rgbToXYZ(src.getRGBtoXYZ());  
        Matrix3x3 xyzToRGB(dst.getXYZtoRGB());
        
        std::array<float, 3> srcXYZ = ColorSpace::XYZ(float3{src.getWhitePoint(), 1});
        std::array<float, 3> dstXYZ = ColorSpace::XYZ(float3{dst.getWhitePoint(), 1});
        if (any(greaterThan(abs(src.getWhitePoint() - ILLUMINANT_D50_XY), float2{1e-3f}))) {
            rgbToXYZ = adaptation(BRADFORD, srcXYZ, ILLUMINANT_D50_XYZ) * src.getRGBtoXYZ();
        }
        if (any(greaterThan(abs(dst.getWhitePoint() - ILLUMINANT_D50_XY), float2{1e-3f}))) {
            xyzToRGB = inverse(adaptation(BRADFORD, dstXYZ, ILLUMINANT_D50_XYZ) * dst.getRGBtoXYZ());
        }
        transferMatrix = xyzToRGB * rgbToXYZ;
    }

}

}
}