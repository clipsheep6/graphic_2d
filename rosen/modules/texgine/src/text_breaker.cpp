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

#include "text_breaker.h"

#include <cassert>

#include "measurer.h"
#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "text_span.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_converter.h"
#include "word_breaker.h"

#ifdef ENABLE_HYPHEN
#include "hyphenator.h"
#include <algorithm> 
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
int TextBreaker::WordBreak(std::vector<VariantSpan> &spans, const TypographyStyle &ys,
    const std::shared_ptr<FontProviders> &fontProviders
#ifdef ENABLE_HYPHEN
    , double widthLimit
#endif
    )
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("TextBreaker::WordBreak");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "WordBreak");
    std::vector<VariantSpan> visitingSpans;
    std::swap(visitingSpans, spans);
    for (const auto &vspan : visitingSpans) {
        auto span = vspan.TryToTextSpan();
        if (span == nullptr) {
            spans.push_back(vspan);
            continue;
        }

        auto xs = vspan.GetTextStyle();
#ifdef ENABLE_HYPHEN
        usLocale_ = icu::Locale::createFromName(xs.locale.c_str());
#endif
        auto fontCollection = GenerateFontCollection(ys, xs, fontProviders);
        if (fontCollection == nullptr) {
            // WordBreak failed
            return 1;
        }

        CharGroups cgs;
        std::vector<Boundary> boundaries;
        auto ret = Measure(xs, span->u16vect_, *fontCollection, cgs, boundaries);
        if (ret) {
            return 1;
        }

        GenNewBoundryByTypeface(cgs, boundaries);

        LOGSCOPED(sl2, LOGEX_FUNC_LINE_DEBUG(), "TextBreaker::doWordBreak algo");
        preBreak_ = 0;
        postBreak_ = 0;
        for (auto &[start, end] : boundaries) {
            const auto &wordcgs = cgs.GetSubFromU16RangeAll(start, end);
            std::stringstream ss;
            ss << "u16range: [" << start << ", " << end << "), range: " << wordcgs.GetRange();
            LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ss.str());
#ifdef ENABLE_HYPHEN
            BreakWord(wordcgs, ys, xs, spans, widthLimit, fontCollection);
#else
            BreakWord(wordcgs, ys, xs, spans);
#endif
        }
    }
    // WordBreak successed
    return 0;
}

std::shared_ptr<FontCollection> TextBreaker::GenerateFontCollection(const TypographyStyle &ys,
    const TextStyle &xs, const std::shared_ptr<FontProviders> &fontProviders) noexcept(false)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "TextBreaker::GenerateFontCollection");
    auto families = xs.fontFamilies;
    if (families.empty()) {
        families = ys.fontFamilies;
    }

    if (fontProviders == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "fontProviders is nullptr";
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    return fontProviders->GenerateFontCollection(families);
}

int TextBreaker::Measure(const TextStyle &xs, const std::vector<uint16_t> &u16vect,
    const FontCollection &fontCollection, CharGroups &cgs, std::vector<Boundary> &boundaries) noexcept(false)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "TextBreaker::doMeasure");
    auto measurer = Measurer::Create(u16vect, fontCollection);
    if (measurer == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "Measurer::Create return nullptr";
        throw TEXGINE_EXCEPTION(API_FAILED);
    }

    measurer->SetLocale(xs.locale);
    measurer->SetRTL(false);
    measurer->SetSize(xs.fontSize);
    FontStyles style(xs.fontWeight, xs.fontStyle);
    measurer->SetFontStyle(style);
    measurer->SetRange(0, u16vect.size());
    measurer->SetSpacing(xs.letterSpacing, xs.wordSpacing);
    auto ret = measurer->Measure(cgs);
    if (ret != 0) {
        LOGEX_FUNC_LINE(ERROR) << "Measure failed!";
        return ret;
    }
    boundaries = measurer->GetWordBoundary();
    if (boundaries.size() == 0) {
        LOGEX_FUNC_LINE(ERROR) << "Measurer GetWordBoundary failed!";
        return 1;
    }
    return 0;
}

void TextBreaker::GenNewBoundryByTypeface(CharGroups cgs, std::vector<Boundary> &boundaries)
{
    std::vector<Boundary> newBoundary;
    for (auto &[start, end] : boundaries) {
        size_t newStart = start;
        size_t newEnd = start;
        const auto &wordCgs = cgs.GetSubFromU16RangeAll(start, end);
        auto typeface = wordCgs.Get(0).typeface;
        for (auto cg = wordCgs.begin(); cg != wordCgs.end(); cg++) {
            if (typeface == cg->typeface) {
                newEnd++;
                continue;
            }

            newBoundary.push_back({newStart, newEnd});
            newStart = newEnd;
            typeface = cg->typeface;
        }

        newBoundary.push_back({newStart, end});
    }

    boundaries = newBoundary;
}

#ifdef ENABLE_HYPHEN
bool IsSymbol(uint16_t c)
{
    return (c < 'a' || c > 'z');
}

int32_t CountSymbol(uint16_t* cg, size_t size)
{
    int count = 0;
    for (size_t i = 0; i < size; i++) {
        if (IsSymbol(cg[i])) {
            count++;
        }
    }
    return count;
}

int32_t TxtSize(uint16_t* txt, size_t size)
{
    ssize_t result = size - 1;
    while (result > 0) {
        UChar32 c;
        ssize_t ix = result;
        U16_PREV(txt, 0, ix, c);
        const int32_t gc_mask = U_GET_GC_MASK(c);
        if ((gc_mask & (U_GC_ZS_MASK | U_GC_P_MASK)) == 0) {
            break;
        }
        result = ix;
  }
  return result;
}
#endif

void TextBreaker::BreakWord(const CharGroups &wordcgs, const TypographyStyle &ys,
    const TextStyle &xs, std::vector<VariantSpan> &spans
#ifdef ENABLE_HYPHEN
    , double widthLimit, const std::shared_ptr<FontCollection>& fontCollection
#endif
    )
{

#ifdef ENABLE_HYPHEN
    double hyphenPenalty = 0.0;
    hyphenPenalty = 0.5 * ys.fontSize * ys.heightScale * widthLimit;
    if (mHyphenationFrequency == kHyphenationFrequency_Normal) {
      hyphenPenalty *= 4.0;  
    }
    double linePenalty = 0;
    if (ys.align == TextAlign::JUSTIFY) {
      hyphenPenalty *= 0.25;
    } else {
      linePenalty = std::max(linePenalty, hyphenPenalty * LINE_PENALTY_MULTIPLIER);
    }
#endif

    size_t rangeOffset = 0;
    for (size_t i = 0; i < wordcgs.GetNumberOfCharGroup(); i++) {
        const auto &cg = wordcgs.Get(i);
        postBreak_ += cg.GetWidth();
        if (u_isWhitespace(cg.chars[0]) == 0) {
            // not white space
            preBreak_ = postBreak_;
        }

        LOGEX_FUNC_LINE_DEBUG() << "Now: [" << i << "] '" << TextConverter::ToStr(cg.chars) << "'"
            << " preBreak_: " << preBreak_ << ", postBreak_: " << postBreak_;

        const auto &breakType = ys.wordBreakType;
        bool isBreakAll = (breakType == WordBreakType::BREAK_ALL);
        bool isBreakWord = (breakType == WordBreakType::BREAK_WORD);
        bool isFinalCharGroup = (i == wordcgs.GetNumberOfCharGroup() - 1);
        bool needGenerateSpan = isBreakAll;
        needGenerateSpan = needGenerateSpan || (isBreakWord && isFinalCharGroup);
        if (needGenerateSpan == false) {
            continue;
        }

       

        CharGroups currentCgs = wordcgs.GetSub(rangeOffset, i + 1);
#ifdef ENABLE_HYPHEN
        std::string str = TextConverter::ToStr(currentCgs.ToUTF16());
        std::vector<HyphenationType> mHyphBuf;
        int size = TxtSize(currentCgs.ToUTF16().data(), currentCgs.ToUTF16().size());
        if((size > 0) ) {
            hyphenator_->Hyphenate(&mHyphBuf, currentCgs.ToUTF16().data(), size , usLocale_);
            int subStart = 0;
            for (size_t j = 0; j < size; j++) {
                if ( mHyphBuf[j] != HyphenationType::DONT_BREAK ) {
                    auto  subCgs = currentCgs.GetSub(subStart, j);
                    GenerateSpan(subCgs, ys, xs, spans, hyphenPenalty, mHyphBuf[j], linePenalty);
                    subStart = j;
                }
            
            }
            auto  subCgs = currentCgs.GetSub(subStart, currentCgs.ToUTF16().size());
            
            GenerateSpan(subCgs, ys, xs, spans, hyphenPenalty, HyphenationType::DONT_BREAK, linePenalty);

            rangeOffset = i + 1;
        } else {
            float penalty = hyphenPenalty * 0;
            GenerateSpan(currentCgs, ys, xs, spans, penalty, HyphenationType::DONT_BREAK, linePenalty);
            rangeOffset = i + 1;
        }
#else
            GenerateSpan(currentCgs, ys, xs, spans);
            rangeOffset = i + 1;
#endif
    }
      
}

void TextBreaker::GenerateSpan(const CharGroups &currentCgs, const TypographyStyle &ys,
    const TextStyle &xs, std::vector<VariantSpan> &spans
#ifdef ENABLE_HYPHEN
    , double panetly, HyphenationType hyph, double linePenalty
#endif
    )
{
    if (!currentCgs.IsValid() || currentCgs.GetSize() == 0) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    LOGEX_FUNC_LINE_DEBUG(Logger::SetToNoReturn) << "AddWord " << spans.size()
        << " " << currentCgs.GetRange() << ": \033[40m'";
    LOGCEX_DEBUG() << TextConverter::ToStr(currentCgs.ToUTF16()) << "'\033[0m";
    auto newSpan = std::make_shared<TextSpan>();
    newSpan->cgs_ = currentCgs;
    newSpan->postBreak_ = postBreak_;
    newSpan->preBreak_ = preBreak_;
    newSpan->typeface_ = currentCgs.Get(0).typeface;
#ifdef ENABLE_HYPHEN
    newSpan->penalty_ = panetly;
    newSpan->hyph_ = hyph;
    newSpan->linePenalty_ = linePenalty;
#endif
    double spanWidth = 0.0;
    for (const auto &cg : currentCgs) {
        spanWidth += cg.GetWidth();
    }
    newSpan->width_ = spanWidth;


    VariantSpan vs(newSpan);
    vs.SetTextStyle(xs);
    spans.push_back(vs);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS