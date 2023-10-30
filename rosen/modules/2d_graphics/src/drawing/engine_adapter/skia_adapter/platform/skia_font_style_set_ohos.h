/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SKIA_FONT_STYLE_SET_OHOS_H
#define SKIA_FONT_STYLE_SET_OHOS_H

#include <cstdint>
#include <string>

#include "src/ports/skia_ohos/FontConfig_ohos.h"

#include "skia_adapter/skia_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontStyleSetOhos : public SkiaFontStyleSet {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    explicit SkiaFontStyleSetOhos(const std::shared_ptr<FontConfig_OHOS>& fontConfig, int index, bool isFallback);
    ~SkiaFontStyleSetOhos() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif