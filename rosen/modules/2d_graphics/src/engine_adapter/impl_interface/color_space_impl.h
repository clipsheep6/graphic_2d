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

#ifndef COLOR_SPACE_IMPL_H
#define COLOR_SPACE_IMPL_H

#include "color_space_data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorSpaceImpl {
public:
    ColorSpaceImpl() noexcept {}
    virtual ~ColorSpaceImpl() {}

    virtual void InitWithSRGB() = 0;
    virtual void InitWithSRGBLinear() = 0;
    const ColorSpaceData * GetColorSpaceData() const { return colorSpaceData_.get(); }
protected:
    std::shared_ptr<ColorSpaceData> colorSpaceData_;
};
}
}
}
#endif