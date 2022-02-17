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

#ifndef COLORSPACECONVERTOR
#define COLORSPACECONVERTOR

#include "src/core/SkColorSpaceXformSteps.h"
#include "color_space.h"
#include <vector>

namespace OHOS {
namespace ColorManager {
    /**
    * @brief use this convertor to transfer a color from one color space to another
    *
    * @param src source color space of the input color
    * @param dst the destination color space
    * The underlying color space converting strategy is based on skia SkColorSpaceXformSteps,
    * while we omite the alpha argument. Alpha should not involved in color management.
    * always unpremultiply -> unpremultiply.
    */
class ColorSpaceConvertor {
public:
    ColorSpaceConvertor(ColorSpaceName src, ColorSpaceName dst, GamutMappingMode mappingMode);

    // use convert func to transfer a color from one gamut to another.
    std::vector<float> Convert(float r, float g, float b);

    ColorSpaceName GetSrcColorSpace() const { srcName; }

    // OHOS ColorSpace Convertor -> Skis ColorSpace convertor
    SkColorSpaceXformSteps* ToSkiaCnvertor() const;

    const ColorSpaceName getSource() const { return srcName; }
    const ColorSpaceName getDestination() const { return dstName; }
    const Matrix3x3& getTransform() const { return transferMatrix; }
    std::array<float, 3> Transform(const std::array<float, 3>& v);
    std::array<float, 3> TransformLinear(const std::array<float, 3>& v);

private:
    bool AnyGreatThan(std::array<float, 2> vec1, std::array<float, 2> vec2);
    bool AllLessThan(std::array<float, 2> vec1, std::array<float, 2> vec2);
    ColorSpaceName srcName;
    ColorSpaceName dstName;
    ColorSpace srcColorSpace;
    ColorSpace dstColorSpace;
    /*
     * 1. convert those unpremultiplied, linear source colors to XYZ D50 gamut by multiplying by a 3x3 matrix
     * 2. convert those XYZ D50 colors to the destination gamut by multiplying by a 3x3 matrix
     * transferMatrix = step1 * step2
     * Namely: 
     * transferMatrix = (source space to XYZ matrix) * (XYZ matrix to destination matrix)
     */ 
    GamutMappingMode mappingMode;
    Matrix3x3 transferMatrix;
};
}  // namespace ColorManager
}  // namespace OHOS
#endif