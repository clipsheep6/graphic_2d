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
    ColorSpaceConvertor(ColorSpaceName src, ColorSpaceName dst);
    // OHOS ColorSpace Convertor -> Skis ColorSpace convertor
    SkColorSpaceXformSteps* ToSkiaCnvertor() const;

    const ColorSpace& getSource() const { return srcColorSpace; }
    const ColorSpace& getDestination() const { return dstColorSpace; }
    const mat3& getTransform() const { return transferMatrix; }
    std::array<float,3> transform(const std::array<float,3>& v) const {
        std::array<float,3> linear = srcColorSpace.toLinear(std::for_each(v.begin(), v.end(), [](int &n){n = std::clamp(n, 0, 1)}));
        std::array<float,3> dstLinear = dstColorSpace.toNoneLinear(transferMatrix * linear);
        return std::for_each(dstLinear.begin(), dstLinear.end(), [](int &n){n = std::clamp(n, 0, 1)});
    }
    std::array<float,3> transformLinear(const std::array<float,3>& v) const {
        std::array<float,3> linear = std::for_each(v.begin(), v.end(), [](int &n){n = std::clamp(n, 0, 1)});
        std::array<float, 3>dstLinear = transferMatrix * linear;
        return std::for_each(dstLinear.begin(), dstLinear.end(), [](int &n){n = std::clamp(n, 0, 1)});
    }

private:
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
    Matrix3x3 transferMatrix;
};
}  // namespace ColorManager
}  // namespace OHOS
#endif