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

#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H

#include <string>
#include "engine_adapter/impl_interface/color_space_impl.h"

namespace OHOS {
namespace Rosen {
class ColorSpaceImpl;
struct ColorSpaceData;
class ColorSpace {
public:
    enum class ColorSpaceType {
        NO_TYPE,
        SRGB,
        SRGB_LINEAR,
    };

    static std::shared_ptr<ColorSpace> CreateSRGB();
    static std::shared_ptr<ColorSpace> CreateSRGBLinear();

    ~ColorSpace() {}
    ColorSpaceType GetType() const;
    const ColorSpaceData* GetColorSpaceData() const;

    ColorSpace(ColorSpaceType t) noexcept;
protected:
    ColorSpace() noexcept;
private:
    ColorSpaceType type_;
    std::unique_ptr<ColorSpaceImpl> impl_;
};
}
}
#endif