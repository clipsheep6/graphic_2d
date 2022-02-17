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

#include "../skia/src/core/SkColorSpaceXformSteps.h"

namespace OHOS {
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

    struct Matrix3x3 {
        float vals[3][3];
    };

    struct ColorSpacePrimaries {
        float fRX;
        float fRY;
        float fGX;
        float fGY;
        float fBX;
        float fBY;
        float fWX;
        float fWY;
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

    static constexpr Matrix3x3 OHSRGB = {{{0.436065674f, 0.385147095f, 0.143066406f},
                                          {0.222488403f, 0.716873169f, 0.060607910f},
                                          {0.013916016f, 0.097076416f, 0.714096069f}}};

    static constexpr Matrix3x3 OHAdobeRGB = {{{0.60974, 0.20528, 0.14919,},
                                              {0.31111, 0.62567, 0.06322,},
                                              {0.01947, 0.06087, 0.74457,}}};

    static constexpr Matrix3x3 OHP3 = {{{0.515102f, 0.291965f, 0.157153f},
                                        {0.241182f, 0.692236f, 0.0665819f},
                                        {-0.00104941f, 0.0418818f, 0.784378f}}};

    static constexpr Matrix3x3 OHRec2020 = {{{0.673459f, 0.165661f, 0.125100f},
                                             {0.279033f, 0.675338f, 0.0456288f},
                                             {-0.00193139f, 0.0299794f, 0.797162f}}};

    static constexpr Matrix3x3 OHXYZ = {{{1.0f, 0.0f, 0.0f}, 
                                         {0.0f, 1.0f, 0.0f},
                                         {0.0f, 0.0f, 1.0f}}};

class ColorSpace {
public:

    ColorSpace(ColorSpaceName name, int64_t nativeHandle);

    ColorSpace(const ColorSpaceName name,
               const int64_t nativeHandle, 
               const ColorSpacePrimaries primaries,
               const Matrix3x3 toXYZ,
               const float gamma,
               const TransferFunc transferFunc);

    ColorSpace(const ColorSpaceName name,
               const int64_t nativeHandle,
               const ColorSpacePrimaries primaries,
               const float gamma,
               const TransferFunc transferFunc);

    ColorSpace(const ColorSpaceName name,
               const int64_t nativeHandle,
               const ColorSpacePrimaries primaries);

    ColorSpace(const ColorSpaceName name,
               const ColorSpacePrimaries primaries,
               const TransferFunc transferFunc);

    ColorSpace(const ColorSpaceName name,
               const ColorSpacePrimaries primaries,
               const TransferFunc transferFunc,
               float clampMin,
               float clampMax);


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

    const Matrix3x3 getRGBToXYZ() const {
        return toXYZ;
    }

    const Matrix3x3 getXYZToRGB() const {
        return inverse(toXYZ);
    }


    // OHOS ColorSpce -> Skia ColorSpace
    SkColorSpace* colorSpace() const;


private:
    friend class SkColorSpaceXformSteps;
    // SkColorSpace* skColorSpace = SkColorSpace::MakeSRGB().get();

    // Compute a toXYZD50 matrics from a given rgb and white point; D50??
    Matrix3x3 ComputeXYZD50(const ColorSpacePrimaries primaries); 
    std::array<float, 3> toLinear(std::array<float, 3>, float gamma);
    std::array<float, 3> toLinear(std::array<float, 3>, ColorSpacePrimaries primaries);
    std::array<float, 3> toNonLinear(std::array<float, 3>, float gamma);
    std::array<float, 3> toNonLinear(std::array<float, 3>, ColorSpacePrimaries primaries);
    
    ColorSpaceName       colorSpaceName;  
    ColorSpacePrimaries  primaries;
    TransferFunc         transferFunc;
    
    Matrix3x3            toXYZ;
    int64_t              nativeHandle = 0;  // ??
    float                clampMin = 0.0f;
    float                clampMax = 1.0f;
};

} // namespace ColorSpace
} // namespace OHOS

#endif  // COLORSPACE
