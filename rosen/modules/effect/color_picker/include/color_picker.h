/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H

#include "effect_type.h"
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class Color;
class ColorPicker {
public:
    ~ColorPicker() {}

    NATIVEEXPORT static std::shared_ptr<ColorPicker> CreateColorPicker(const std::shared_ptr<Media::PixelMap>& pixmap, uint32_t &errorCode);
    NATIVEEXPORT std::shared_ptr<Media::PixelMap> GetPixelMap();
    NATIVEEXPORT uint32_t GetMainColor(Color &color);

private:
    ColorPicker(std::shared_ptr<Media::PixelMap> pixmap);

    // variables
    std::shared_ptr<Media::PixelMap> pixelmap_;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // COLOR_PICKER_H
