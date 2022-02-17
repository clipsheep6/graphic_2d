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

#ifndef COLORSPACE
#define COLORSPACE

#include "src/core/SkColorSpaceXformSteps.h"
#include "third_party/skcms/skcms.h"

namespace OHOS 
{
namespace ColorManager 
{
    enum ColorSpaceName
    {   
        NONE = 0,
        ACES,
        ACESCG,
        ADOBE_RGB,
        BT2020,
        BT709,
        CIE_LAB,
        CIE_XYZ,
        DCI_P3,
        DISPLAY_P3,
        EXTENDED_SRGB,
        LINEAR_EXTENDED_SRGB,
        LINEAR_SRGB,
        NTSC_1953,
        PRO_PHOTO_RGB,
        SMPTE_C,
        SRGB,
    };

    enum GamutMappingMode 
    {
        GAMUT_MAP_CONSTANT,
        GAMUT_MAP_EXPENSION,
        GAMUT_MAP_HDR_CONSTANT,
        GAMUT_MAP_HDR_EXPENSION,
    };

    using Matrix3x3 = std::array<std::array<float, 3>, 3>;

    struct ColorSpacePrimaries {
        float RX;
        float RY;
        float GX;
        float GY;
        float BX;
        float BY;
        float WX;
        float WY;
    };

    struct TransferFunc {
        float g;
        float a; 
        float b;
        float c;
        float d;
        float e;
        float f;
    };

std::array<std::array<float, 3>, 3> operator*(const std::array<std::array<float, 3>, 3>& a,
                                                const std::array<std::array<float, 3>, 3>& b);

std::array<float, 3> operator*(const std::array<float, 3>& x,
                                const std::array<std::array<float, 3>, 3>& a);

std::array<float, 3> operator*(const std::array<std::array<float, 3>, 3>& a,
                                const std::array<float, 3>& x);


std::array<std::array<float, 3>, 3> operator/(const std::array<float, 3>& a,
                                const std::array<float, 3>& b);


class ColorSpace {
public:

    ColorSpace(ColorSpaceName name, int64_t nativeHandle);

    ColorSpace(const ColorSpaceName name,
               const int64_t nativeHandle, 
               const ColorSpacePrimaries primaries,
               const Matrix3x3 toXYZ,
               const TransferFunc transferFunc);

    ColorSpace(const ColorSpaceName name,
               const int64_t nativeHandle,
               const ColorSpacePrimaries primaries,
               const Matrix3x3 toXYZ,
               const float gamma);

    ColorSpace(const ColorSpaceName name,
               const int64_t nativeHandle,
               const ColorSpacePrimaries primaries,
               const TransferFunc transferFunc);

    ColorSpace(const ColorSpaceName name,
               const ColorSpacePrimaries primaries,
               const TransferFunc transferFunc);

    ColorSpaceName GetColorSpaceName() const { return colorSpaceName;};

    // remained for multimedia 
    int64_t GetNativeHandle() const { return nativeHandle;};
    
    // some common color gamut constructor
    static const ColorSpace SRGB();
    static const ColorSpace LinearSRGB();
    static const ColorSpace ExtendedSRGB();
    static const ColorSpace LinearExtendedSRGB();
    static const ColorSpace NTSC();
    static const ColorSpace BT709();
    static const ColorSpace BT2020();
    static const ColorSpace AdobeRGB();
    static const ColorSpace ProPhotoRGB();
    static const ColorSpace DCIP3();
    static const ColorSpace DisplayP3();
    static const ColorSpace ACES();
    static const ColorSpace ACEScg();

    const Matrix3x3 getRGBToXYZ() { return toXYZ; }

    const Matrix3x3 getXYZToRGB() {
        Matrix3x3 toRGB = invert(toXYZ);
        return toRGB;
    }

    

    std::array<float, 2> getWhitePoint();  

    float GetGamma() { return transferFunc.g; }

    static std::array<float, 3> XYZ(const std::array<float, 3>& xyY) {
        return std::array<float, 3>{
                (xyY[0] * xyY[2]) / xyY[1], xyY[2], ((1 - xyY[0] - xyY[1]) * xyY[2]) / xyY[1]};
    }

    // invert
    static Matrix3x3 invert(const Matrix3x3& src);
    std::array<float, 3> toLinear(std::array<float, 3>, float gamma);
    std::array<float, 3> toLinear(std::array<float, 3>, const TransferFunc& p);
    std::array<float, 3> toNonLinear(std::array<float, 3>, float gamma);
    std::array<float, 3> toNonLinear(std::array<float, 3>, const TransferFunc& p);  

    float clampMin = 0.0f;
    float clampMax = 1.0f;

private:
    friend class SkColorSpaceXformSteps;
    // SkColorSpace* skColorSpace = SkColorSpace::MakeSRGB().get();
    // OHOS ColorSpce -> Skia ColorSpace
    SkColorSpace* ToSkColorSpace() const;
    SkAlphaType alphaType() const { return SkAlphaType::kUnpremul_SkAlphaType; };
    skcms_Matrix3x3 ToSkiaXYZ() const; 
    // Compute a toXYZD50 matrics from a given rgb and white point; D50??
    Matrix3x3 ComputeXYZ(const ColorSpacePrimaries& primaries); 

    ColorSpaceName       colorSpaceName;  
    ColorSpacePrimaries  primaries;
    TransferFunc         transferFunc;
    int64_t              nativeHandle;  // ??
    Matrix3x3            toXYZ;
};

} // namespace ColorSpace
} // namespace OHOS

#endif  // COLORSPACE
