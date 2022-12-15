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

#include "font_collection.h"

#include "texgine/dynamic_font_provider.h"
#include "texgine/system_font_provider.h"
#include "texgine/typography_types.h"
#include "texgine/utils/exlog.h"

namespace Texgine {
FontCollection::FontCollection(std::vector<std::shared_ptr<FontStyleSet>> &&fontStyleSets)
    : fontStyleSets_(fontStyleSets)
{
}

std::shared_ptr<Typeface> FontCollection::GetTypefaceForChar(const uint32_t &ch,
    const std::string &locale, const FontStyles &style) const
{
    for (const auto &fontStyleSet : fontStyleSets_) {
        std::shared_ptr<Typeface> typeface = nullptr;
        struct TypefaceCacheKey key = {.fss = fontStyleSet, .fs = style};
        if (auto it = typefaceCache_.find(key); it != typefaceCache_.end()) {
            typeface = it->second;
        } else {
            auto styleSet = fontStyleSet->Get();
            if (styleSet == nullptr) {
                continue;
            }

            auto skTypeface = styleSet->matchStyle(style.ToSkFontStyle());
            if (skTypeface == nullptr) {
                continue;
            }

            typeface = std::make_shared<Typeface>(skTypeface);
            typefaceCache_[key] = typeface;
        }

        if (typeface->Has(ch)) {
            return typeface;
        }
    }

    return FindFallBackTypeface(ch, style, locale);
}

std::shared_ptr<Typeface> FontCollection::GetTypefaceForFontStyles(const FontStyles &style, const std::string &locale) const
{
    auto providingStyle = style.ToSkFontStyle();

    int bestScore = 0;
    int bestIndex = 0;
    std::shared_ptr<FontStyleSet> bestFontStyleSet = nullptr;
    for (auto &fontStyleSet : fontStyleSets_) {
        auto count = fontStyleSet->Get()->count();
        for (auto i = 0; i < count; i++) {
            SkFontStyle matchingStyle;
            SkString styleName;
            fontStyleSet->Get()->getStyle(i, &matchingStyle, &styleName);

            int score = 0;
            score += (10 - std::abs(providingStyle.width() - matchingStyle.width())) * 100;
            score += (2- std::abs(providingStyle.slant() - matchingStyle.slant())) * 10000;
            score += (10 - std::abs(providingStyle.weight() / 100 - matchingStyle.weight() / 100));
            if (score > bestScore) {
                bestScore = score;
                bestIndex = i;
                bestFontStyleSet = fontStyleSet;
            }
        }
    }

    if (bestFontStyleSet != nullptr && bestFontStyleSet->Get() != nullptr) {
        return std::make_shared<Typeface>(bestFontStyleSet->Get()->createTypeface(bestIndex));
    }

    return FindFallBackTypeface(' ', style, locale);
}

std::shared_ptr<Typeface> FontCollection::FindFallBackTypeface(const uint32_t &ch, const FontStyles &style,
    const std::string &locale) const
{
    if (!enableFallback_) {
        return nullptr;
    }

    // fallback cache
    struct FallbackCacheKey key = {.locale = locale, .fs = style};
    if (auto it = fallbackCache_.find(key); it != fallbackCache_.end()) {
        return it->second;
    }

    // fallback
    std::vector<const char *> bcp47;
    if (locale.empty() == false) {
        bcp47.push_back(locale.data());
    }

    auto fm = SkFontMgr::RefDefault();
    if (fm == nullptr) {
        return nullptr;
    }

    auto fallbackSkTypeface = fm->matchFamilyStyleCharacter(nullptr,
        style.ToSkFontStyle(), bcp47.data(), bcp47.size(), ch);
    if (fallbackSkTypeface == nullptr) {
        return nullptr;
    }

    auto typeface = std::make_shared<Typeface>(fallbackSkTypeface);
    fallbackCache_[key] = typeface;
    return typeface;
}

void FontCollection::DisableFallback()
{
    enableFallback_ = false;
}
} // namespace Texgine
