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

#ifndef SKIA_FONT_STYLE_H
#define SKIA_FONT_STYLE_H

#include <include/core/SkFontStyle.h>
#include <memory>

#include "impl_interface/font_style_impl.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontStyle : public FontStyleImpl{
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaFontStyle();
    SkiaFontStyle(int weight, int width, Slant slant);
    explicit SkiaFontStyle(const std::shared_ptr<SkFontStyle> style);
    ~SkiaFontStyle() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    /*
     * @brief Returns SkFontStyle
     */
    std::shared_ptr<SkFontStyle> GetFontStyle() const;
    /*
     * @brief Sets SkFontStyle to TexgineFontStyle
     */
    void SetFontStyle(const std::shared_ptr<SkFontStyle> fontStyle);

    /*
     * @brief Sets SkFontStyle to TexgineFontStyle
     */
    void SetStyle(const SkFontStyle &style);

private:
    std::shared_ptr<SkFontStyle> fontStyle_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif 
