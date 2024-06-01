/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MAGNIFIER_PARA_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MAGNIFIER_PARA_H

#include "common/rs_macros.h"
#include <cstdint>
namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSMagnifierPara {
public:
    float factor_;
    float width_;
    float height_;
    float borderWidth_;
    float cornerRadius_;
    float offsetX_;
    float offsetY_;

    float shadowOffsetX_;
    float shadowOffsetY_;
    float shadowSize_;
    float shadowStrength_;

    // rgba
    uint32_t gradientMaskColor1_;
    uint32_t gradientMaskColor2_;
    uint32_t outerContourColor1_;
    uint32_t outerContourColor2_;

    explicit RSMagnifierPara(float factor, float width, float height, float borderWidth, float cornerRadius,
        float offsetX, float offsetY, float shadowOffsetX, float shadowOffsetY, float shadowSize,
        float shadowStrength, uint32_t gradientMaskColor1, uint32_t gradientMaskColor2, uint32_t outerContourColor1,
        uint32_t outerContourColor2)
        : factor_(factor), width_(width), height_(height), borderWidth_(borderWidth), cornerRadius_(cornerRadius),
          offsetX_(offsetX), offsetY_(offsetY), shadowOffsetX_(shadowOffsetX),
          shadowOffsetY_(shadowOffsetY), shadowSize_(shadowSize), shadowStrength_(shadowStrength),
          gradientMaskColor1_(gradientMaskColor1), gradientMaskColor2_(gradientMaskColor2),
          outerContourColor1_(outerContourColor1), outerContourColor2_(outerContourColor2) {}

    ~RSMagnifierPara() = default;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MAGNIFIER_PARA_H
