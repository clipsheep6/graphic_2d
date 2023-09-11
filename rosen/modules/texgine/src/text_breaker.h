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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXT_BREAKER_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXT_BREAKER_H

#include <vector>

#include "variant_span.h"
#include "word_breaker.h"

#ifdef ENABLE_HYPHEN
#include <sys/stat.h>
#include <unistd.h>
#include "hyphenator.h"
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TextBreaker {
public:
#ifdef ENABLE_HYPHEN
    TextBreaker() {
        hyphenator_ = Hyphenator::Instance();
    }
#endif

    int WordBreak(std::vector<VariantSpan> &spans, const TypographyStyle &ys,
        const std::shared_ptr<FontProviders> &fontProviders
#ifdef ENABLE_HYPHEN
         , double widthLimit
#endif
          );

    std::shared_ptr<FontCollection> GenerateFontCollection(const TypographyStyle &ys,
        const TextStyle &xs, const std::shared_ptr<FontProviders> &fontProviders) noexcept(false);

    static int Measure(const TextStyle &xs, const std::vector<uint16_t> &u16vect,
        const FontCollection &fontCollection, CharGroups &cgs, std::vector<Boundary> &boundaries) noexcept(false);

    void BreakWord(const CharGroups &wordcgs, const TypographyStyle &ys,
        const TextStyle &xs, std::vector<VariantSpan> &spans
#ifdef ENABLE_HYPHEN
    , double widthLimit, const std::shared_ptr<FontCollection>& fontCollection
#endif
     ) noexcept(false);
    void GenNewBoundryByTypeface(CharGroups cgs, std::vector<Boundary> &boundaries);
    void GenerateSpan(const CharGroups &currentCgs, const TypographyStyle &ys,
    const TextStyle &xs, std::vector<VariantSpan> &spans
#ifdef ENABLE_HYPHEN    
    , double panetly, HyphenationType hyph, double linePenalty
#endif    
    ) noexcept(false);
#ifdef ENABLE_HYPHEN
    icu::Locale usLocale_;
    HyphenationFrequency mHyphenationFrequency = kHyphenationFrequency_Normal;
    Hyphenator *hyphenator_;
#endif
    double preBreak_ = 0;
    double postBreak_ = 0;   
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXT_BREAKER_H
