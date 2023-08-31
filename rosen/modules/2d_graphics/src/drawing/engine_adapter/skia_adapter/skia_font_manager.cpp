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
 
 #include "skia_font_manager.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaFontManager::SkiaFontManager() noexcept
{
    skFontMgr_ = SkFontMgr::RefDefault();
}

std::shared_ptr<FontManager> SkiaFontManager::RefDefault()
{
    auto fontManager = std::make_shared<FontManager>();
    auto skiaFontManager = std::make_shared<SkiaFontManager>();
    skiaFontManager->SetSkFontMgr(SkFontMgr::RefDefault());
    fontManager->InitFontManagerImpl(skiaFontManager);
    return fontManager;
}

std::shared_ptr<Typeface> SkiaFontManager::MatchFamilyStyleCharacter(const std::string &familyName,
        const FontStyle &style, const char* bcp47[], int bcp47Count, int32_t character)
{
    auto skiaStyle = style.GetImpl<SkiaFontStyle>();
    if (skFontMgr_ == nullptr || skiaStyle->GetSkFontStyle() == nullptr) {
        return nullptr;
    }
    auto tf = skFontMgr_->matchFamilyStyleCharacter(familyName.c_str(),
        *skiaStyle->GetSkFontStyle(), bcp47, bcp47Count, character);
    auto skiaTypeFace = std::make_shared<SkiaTypeface>(tf);
    auto typeFace = std::make_shared<Typeface>();
    typeFace->ChangeInitTypefaceImpl(skiaTypeFace);
    return typeFace;
}

std::shared_ptr<FontStyleSet> SkiaFontManager::MatchFamily(const std::string &familyName)
{
    if (skFontMgr_ == nullptr) {
        return nullptr;
    }
    auto set = skFontMgr_->matchFamily(familyName.c_str());
    auto skiaFontStyleSet = std::make_shared<SkiaFontStyleSet>(set);
    auto fontStyleSet = std::make_shared<FontStyleSet>();
    fontStyleSet->InitFontStyleSetImpl(skiaFontStyleSet);
    return fontStyleSet;
}
sk_sp<SkFontMgr> SkiaFontManager::GetSkFontMgr() const
{
    return skFontMgr_;
}

void SkiaFontManager::SetSkFontMgr(const sk_sp<SkFontMgr> mgr)
{
    skFontMgr_ = mgr;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
