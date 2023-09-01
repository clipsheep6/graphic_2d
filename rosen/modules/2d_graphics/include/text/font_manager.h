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

#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "impl_interface/font_manager_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class  FontManager {
public:
    FontManager() noexcept;
    virtual ~FontManager() = default;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return fontManagerImpl_->DowncastingTo<T>();
    }

    static std::shared_ptr<FontManager> RefDefault();

    std::shared_ptr<Typeface> MatchFamilyStyleCharacter(const std::string &familyName,
        const FontStyle &style, const char* bcp47[], int bcp47Count, int32_t character);

    std::shared_ptr<FontStyleSet> MatchFamily(const std::string &familyName);

    void InitFontManagerImpl(std::shared_ptr<FontManagerImpl> replaceFontManagerImpl);

private:
    std::shared_ptr<FontManagerImpl> fontManagerImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif