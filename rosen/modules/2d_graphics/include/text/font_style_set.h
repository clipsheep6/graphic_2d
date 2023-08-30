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

#ifndef FONT_STYLE_SET_H
#define FONT_STYLE_SET_H

#include "impl_interface/font_style_set_impl.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontStyleSet {
public:
    FontStyleSet() noexcept;
    virtual ~FontStyleSet() = default;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return fontStyleSetImpl_->DowncastingTo<T>();
    }

    int Count() const;

    void GetStyle(const int index, std::shared_ptr<FontStyle> style, std::shared_ptr<FontString> name) const;

    std::shared_ptr<Typeface> CreateTypeface(int index);

    std::shared_ptr<Typeface> MatchStyle(const std::shared_ptr<FontStyle> pattern);

    void InitFontStyleSetImpl(std::shared_ptr<FontStyleSetImpl> replaceStyleSetImpl);

    static std::shared_ptr<FontStyleSet> CreateEmpty();

private:
    std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif