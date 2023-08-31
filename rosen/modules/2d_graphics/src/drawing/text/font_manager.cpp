/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "text/font_manager.h"
#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
FontManager::FontManager() noexcept : fontManagerImpl_(ImplFactory::CreateFontManagerImpl())
{}

std::shared_ptr<FontManager> FontManager::RefDefault()
{
    return ImplFactory::CreateFontManagerImpl()->RefDefault();
}

std::shared_ptr<Typeface> FontManager::MatchFamilyStyleCharacter(const std::string &familyName,
        const FontStyle &style, const char* bcp47[], int bcp47Count, int32_t character)
{
    return fontManagerImpl_->MatchFamilyStyleCharacter(familyName,style,bcp47,bcp47Count,character);
}

std::shared_ptr<FontStyleSet> FontManager::MatchFamily(const std::string &familyName)
{
    return fontManagerImpl_->MatchFamily(familyName);
}

void FontManager::InitFontManagerImpl(std::shared_ptr<FontManagerImpl> replaceFontManagerImpl)
{
    fontManagerImpl_ = replaceFontManagerImpl;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
