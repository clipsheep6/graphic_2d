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

#ifndef FONT_STYLE_H
#define FONT_STYLE_H
#include "impl_interface/font_style_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class Slant {
    K_UPRIGHT_SLANT,
    K_ITALIC_SLANT,
    K_OBLIQUE_SLANT,
};

class FontStyle {
public:
    FontStyle() noexcept;
    virtual ~FontStyle() = default;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return fontStyleImpl_->DowncastingTo<T>();
    }

    void InitFontStyleImpl(std::shared_ptr<FontStyleImpl> replaceFontStyleImpl);

private:
    std::shared_ptr<FontStyleImpl> fontStyleImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif