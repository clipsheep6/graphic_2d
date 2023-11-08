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

#include "hyphenator.h"

#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <unicode/uchar.h>
#include <unicode/uscript.h>
#include <unistd.h>
#include <vector>

#include "windows_utils.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {

static const uint16_t CHAR_HYPHEN_MINUS = 0x002D;
static const uint16_t CHAR_SOFT_HYPHEN = 0x00AD;
static const uint16_t CHAR_MIDDLE_DOT = 0x00B7;
static const uint16_t CHAR_HYPHEN = 0x2010;
static const uint32_t CODE_POINT = 11;
static const uint32_t LEN = 26;
static const uint32_t SHIFT = 20;
static const size_t HYPHENATE_LEN = 2;

struct AlphabetTable0 {
    uint32_t version;
    uint32_t min_codepoint;
    uint32_t max_codepoint;
    uint8_t data[1];
};

struct AlphabetTable1 {
    uint32_t version;
    uint32_t nEntries;
    uint32_t data[1];

    static uint32_t Codepoint(uint32_t entry)
    {
        return entry >> CODE_POINT;
    }
    static uint32_t Value(uint32_t entry)
    {
        return entry & 0x7ff;
    }
};

struct Trie {
    uint32_t version;
    uint32_t charMask;
    uint32_t linkShift;
    uint32_t linkMask;
    uint32_t patternShift;
    uint32_t nEntries;
    uint32_t data[1];
};

struct Pattern {
    uint32_t version;
    uint32_t nEntries;
    uint32_t patternOffset;
    uint32_t patternSize;
    uint32_t data[1];

    static uint32_t Len(uint32_t entry)
    {
        return entry >> LEN;
    }
    static uint32_t Shift(uint32_t entry)
    {
        return (entry >> SHIFT) & 0x3f;
    }
    const uint8_t* Buf(uint32_t entry) const
    {
        return reinterpret_cast<const uint8_t*>(this) + patternOffset + (entry & 0xfffff);
    }
};

struct Header {
    uint32_t magic;
    uint32_t version;
    uint32_t alphabetOffset;
    uint32_t trieOffset;
    uint32_t patternOffset;
    uint32_t fileSize;

    const uint8_t* Bytes() const
    {
        return reinterpret_cast<const uint8_t*>(this);
    }
    uint32_t AlphabetVersion() const
    {
        return *reinterpret_cast<const uint32_t*>(Bytes() + alphabetOffset);
    }
    const AlphabetTable0* AlphabetTable0() const
    {
        return reinterpret_cast<const struct AlphabetTable0*>(Bytes() + alphabetOffset);
    }
    const AlphabetTable1* AlphabetTable1() const
    {
        return reinterpret_cast<const struct AlphabetTable1*>(Bytes() + alphabetOffset);
    }
    const Trie* TrieTable() const
    {
        return reinterpret_cast<const Trie*>(Bytes() + trieOffset);
    }
    const Pattern* PatternTable() const
    {
        return reinterpret_cast<const Pattern*>(Bytes() + patternOffset);
    }
};

std::vector<uint8_t> ReadWholeFile(const std::string& filePath)
{
    FILE* fp = fopen(filePath.c_str(), "r");
    if (fp == nullptr) {
        return {};
    }
    struct stat st;
    if (fstat(fileno(fp), &st) != 0) {
        return {};
    }

    std::vector<uint8_t> result(st.st_size);
    if (fread(result.data(), 1, st.st_size, fp) != static_cast<size_t>(st.st_size)) {
        return {};
    }
    fclose(fp);
    return result;
}

Hyphenator Hyphenator::result_;

std::vector<uint8_t> Hyphenator::patternData_;

bool Hyphenator::isLoad_ = false;

void Hyphenator::LoadBinary(std::vector<uint8_t> patternData, size_t minPrefix, size_t minSuffix)
{
    patternData_ = patternData;
    result_.minPrefix_ = minPrefix;
    result_.minSuffix_ = minSuffix;

    isLoad_ = true;
}

Hyphenator* Hyphenator::Instance()
{
    if(!Hyphenator::isLoad_) {
        Hyphenator::LoadBinary(ReadWholeFile(EnUsHyph), EnUsMinPrefix, EnUsMinSuffix);
    }
    return &result_;
}

void Hyphenator::Hyphenate(
    std::vector<HyphenationType>* result, const uint16_t* word, size_t len, const icu::Locale& locale)
{
    result->clear();
    result->resize(len);
    const size_t paddedLen = len + HYPHENATE_LEN;
    if (patternData_.size() > 0 && len >= minPrefix_ + minSuffix_ && paddedLen <= MAX_HYPHENATED_SIZE) {
        uint16_t alphaCodes[MAX_HYPHENATED_SIZE];
        const HyphenationType hyphenValue = AlphabetLookup(alphaCodes, word, len);
        if (hyphenValue != HyphenationType::DONT_BREAK) {
            HyphenateFromCodes(result->data(), alphaCodes, paddedLen, hyphenValue);
            return;
        }
    }

    HyphenateWithNoPatterns(result->data(), word, len, locale);
}

uint32_t HyphenEdit::GetHyphen() const
{
    return hyphen_;
}

bool HyphenEdit::operator==(const HyphenEdit& other) const
{
    return hyphen_ == other.hyphen_;
}

uint32_t HyphenEdit::GetEnd() const
{
    return hyphen_ & MASK_END_OF_LINE;
}
uint32_t HyphenEdit::GetStart() const
{
    return hyphen_ & MASK_START_OF_LINE;
}

bool Hyphenator::IsLineBreakingHyphen(uint32_t c)
{
    return (c == 0x002D || c == 0x058A || c == 0x05BE || c == 0x1400 || c == 0x2010 || c == 0x2013 || c == 0x2027 ||
            c == 0x2E17 || c == 0x2E40);
}

const static uint32_t HYPHEN_STR[] = { 0x2010, 0 };
const static uint32_t ARMENIAN_HYPHEN_STR[] = { 0x058A, 0 };
const static uint32_t MAQAF_STR[] = { 0x05BE, 0 };
const static uint32_t UCAS_HYPHEN_STR[] = { 0x1400, 0 };
const static uint32_t ZWJ_STR[] = { 0x200D, 0 };
const static uint32_t ZWJ_AND_HYPHEN_STR[] = { 0x200D, 0x2010, 0 };

const uint32_t* HyphenEdit::GetHyphenString(uint32_t hyph)
{
    switch (hyph) {
        case INSERT_HYPHEN_AT_END:
        case REPLACE_WITH_HYPHEN_AT_END:
        case INSERT_HYPHEN_AT_START:
            return HYPHEN_STR;
        case INSERT_ARMENIAN_HYPHEN_AT_END:
            return ARMENIAN_HYPHEN_STR;
        case INSERT_MAQAF_AT_END:
            return MAQAF_STR;
        case INSERT_UCAS_HYPHEN_AT_END:
            return UCAS_HYPHEN_STR;
        case INSERT_ZWJ_AND_HYPHEN_AT_END:
            return ZWJ_AND_HYPHEN_STR;
        case INSERT_ZWJ_AT_START:
            return ZWJ_STR;
        default:
            return nullptr;
    }
}

uint32_t HyphenEdit::EditForThisLine(HyphenationType type)
{
    switch (type) {
        case HyphenationType::DONT_BREAK:
            return NO_EDIT;
        case HyphenationType::BREAK_AND_INSERT_HYPHEN:
            return INSERT_HYPHEN_AT_END;
        case HyphenationType::BREAK_AND_INSERT_ARMENIAN_HYPHEN:
            return INSERT_ARMENIAN_HYPHEN_AT_END;
        case HyphenationType::BREAK_AND_INSERT_MAQAF:
            return INSERT_MAQAF_AT_END;
        case HyphenationType::BREAK_AND_INSERT_UCAS_HYPHEN:
            return INSERT_UCAS_HYPHEN_AT_END;
        case HyphenationType::BREAK_AND_REPLACE_WITH_HYPHEN:
            return REPLACE_WITH_HYPHEN_AT_END;
        case HyphenationType::BREAK_AND_INSERT_HYPHEN_AND_ZWJ:
            return INSERT_ZWJ_AND_HYPHEN_AT_END;
        default:
            return BREAK_AT_END;
    }
}

uint32_t HyphenEdit::EditForNextLine(HyphenationType type)
{
    switch (type) {
        case HyphenationType::DONT_BREAK:
            return NO_EDIT;
        case HyphenationType::BREAK_AND_INSERT_HYPHEN_AT_NEXT_LINE:
            return INSERT_HYPHEN_AT_START;
        case HyphenationType::BREAK_AND_INSERT_HYPHEN_AND_ZWJ:
            return INSERT_ZWJ_AT_START;
        default:
            return BREAK_AT_START;
    }
}

static UScriptCode GetScript(uint32_t codePoint)
{
    UErrorCode errorCode = U_ZERO_ERROR;
    const UScriptCode script = uscript_getScript(static_cast<UChar32>(codePoint), &errorCode);
    if (U_SUCCESS(errorCode)) {
        return script;
    } else {
        return USCRIPT_INVALID_CODE;
    }
}

static HyphenationType HyphenationTypeBasedOnScript(uint32_t codePoint)
{
    const UScriptCode script = GetScript(codePoint);
    if (script == USCRIPT_KANNADA || script == USCRIPT_MALAYALAM || script == USCRIPT_TAMIL ||
        script == USCRIPT_TELUGU) {
        return HyphenationType::BREAK_AND_DONT_INSERT_HYPHEN;
    } else if (script == USCRIPT_ARMENIAN) {
        return HyphenationType::BREAK_AND_INSERT_ARMENIAN_HYPHEN;
    } else if (script == USCRIPT_CANADIAN_ABORIGINAL) {
        return HyphenationType::BREAK_AND_INSERT_UCAS_HYPHEN;
    } else {
        return HyphenationType::BREAK_AND_INSERT_HYPHEN;
    }
}

static inline int32_t GetJoiningType(UChar32 codepoint)
{
    return u_getIntPropertyValue(codepoint, UCHAR_JOINING_TYPE);
}

static inline HyphenationType GetHyphTypeForArabic(const uint16_t* word, size_t len, size_t location)
{
    ssize_t i = location;
    int32_t type = U_JT_NON_JOINING;
    while (static_cast<size_t>(i) < len && (type = GetJoiningType(word[i])) == U_JT_TRANSPARENT) {
        i++;
    }
    if (type == U_JT_DUAL_JOINING || type == U_JT_RIGHT_JOINING || type == U_JT_JOIN_CAUSING) {
        i = location - HYPHENATE_LEN;
        type = U_JT_NON_JOINING;
        while (i >= 0 && (type = GetJoiningType(word[i])) == U_JT_TRANSPARENT) {
            i--;
        }
        if (type == U_JT_DUAL_JOINING || type == U_JT_LEFT_JOINING || type == U_JT_JOIN_CAUSING) {
            return HyphenationType::BREAK_AND_INSERT_HYPHEN_AND_ZWJ;
        }
    }
    return HyphenationType::BREAK_AND_INSERT_HYPHEN;
}

void Hyphenator::HyphenateWithNoPatterns(
    HyphenationType* result, const uint16_t* word, size_t len, const icu::Locale& locale)
{
    result[0] = HyphenationType::DONT_BREAK;
    for (size_t i = 1; i < len; i++) {
        const uint16_t prevChar = word[i - 1];
        if (i > 1 && IsLineBreakingHyphen(prevChar)) {
            if ((prevChar == CHAR_HYPHEN_MINUS || prevChar == CHAR_HYPHEN) && strcmp(locale.getLanguage(), "pl") == 0 &&
                GetScript(word[i]) == USCRIPT_LATIN) {
                result[i] = HyphenationType::BREAK_AND_INSERT_HYPHEN_AT_NEXT_LINE;
            } else {
                result[i] = HyphenationType::BREAK_AND_DONT_INSERT_HYPHEN;
            }
        } else if (i > 1 && prevChar == CHAR_SOFT_HYPHEN) {
            if (GetScript(word[i]) == USCRIPT_ARABIC) {
                result[i] = GetHyphTypeForArabic(word, len, i);
            } else {
                result[i] = HyphenationTypeBasedOnScript(word[i]);
            }
        } else if (prevChar == CHAR_MIDDLE_DOT && minPrefix_ < i && i <= len - minSuffix_ &&
             ((word[i - HYPHENATE_LEN] == 'l' && word[i] == 'l') ||
              (word[i - HYPHENATE_LEN] == 'L' && word[i] == 'L')) && strcmp(locale.getLanguage(), "ca") == 0) {
            result[i] = HyphenationType::BREAK_AND_REPLACE_WITH_HYPHEN;
        } else {
            result[i] = HyphenationType::DONT_BREAK;
        }
    }
}

HyphenationType Hyphenator::AlphabetLookup(uint16_t* alphaCodes, const uint16_t* word, size_t len)
{
    const Header* header = GetHeader();
    HyphenationType result = HyphenationType::BREAK_AND_INSERT_HYPHEN;
    uint32_t alphabetVersion = header->AlphabetVersion();
    if (alphabetVersion == 0) {
        const AlphabetTable0* alphabet = header->AlphabetTable0();
        uint32_t min_codepoint = alphabet->min_codepoint;
        uint32_t max_codepoint = alphabet->max_codepoint;
        alphaCodes[0] = 0;
        for (size_t i = 0; i < len; i++) {
            uint16_t c = word[i];
            if (c < min_codepoint || c >= max_codepoint) {
                return HyphenationType::DONT_BREAK;
            }
            uint8_t code = alphabet->data[c - min_codepoint];
            if (code == 0) {
                return HyphenationType::DONT_BREAK;
            }
            if (result == HyphenationType::BREAK_AND_INSERT_HYPHEN) {
                result = HyphenationTypeBasedOnScript(c);
            }
            alphaCodes[i + 1] = code;
        }
        alphaCodes[len + 1] = 0;
        return result;
    } else if (alphabetVersion == 1) {
        const AlphabetTable1* alphabet = header->AlphabetTable1();
        size_t nEntries = alphabet->nEntries;
        const uint32_t* begin = alphabet->data;
        const uint32_t* end = begin + nEntries;
        alphaCodes[0] = 0;
        for (size_t i = 0; i < len; i++) {
            uint16_t c = word[i];
            auto p = std::lower_bound<const uint32_t*, uint32_t>(begin, end, c << CODE_POINT);
            if (p == end) {
                return HyphenationType::DONT_BREAK;
            }
            uint32_t entry = *p;
            if (AlphabetTable1::Codepoint(entry) != c) {
                return HyphenationType::DONT_BREAK;
            }
            if (result == HyphenationType::BREAK_AND_INSERT_HYPHEN) {
                result = HyphenationTypeBasedOnScript(c);
            }
            alphaCodes[i + 1] = AlphabetTable1::Value(entry);
        }
        alphaCodes[len + 1] = 0;
        return result;
    }
    return HyphenationType::DONT_BREAK;
}

void Hyphenator::HyphenateFromCodes (
    HyphenationType* result, const uint16_t* codes, size_t len, HyphenationType hyphenValue)
{
    static_assert(sizeof(HyphenationType) == sizeof(uint8_t), "HyphnationType must be uint8_t.");
    uint8_t* buffer = reinterpret_cast<uint8_t*>(result);

    const Header* header = GetHeader();
    const Trie* trie = header->TrieTable();
    const Pattern* pattern = header->PatternTable();
    uint32_t charMask = trie->charMask;
    uint32_t linkShift = trie->linkShift;
    uint32_t linkMask = trie->linkMask;
    uint32_t patternShift = trie->patternShift;
    size_t maxOffset = len - minSuffix_ - 1;
    for (size_t i = 0; i < len - 1; i++) {
        uint32_t node = 0;
        for (size_t j = i; j < len; j++) {
            uint16_t c = codes[j];
            uint32_t entry = trie->data[node + c];
            if ((entry & charMask) == c) {
                node = (entry & linkMask) >> linkShift;
            } else {
                break;
            }
            uint32_t pat_ix = trie->data[node] >> patternShift;

            if (pat_ix != 0) {
                uint32_t pat_entry = pattern->data[pat_ix];
                int pat_len = Pattern::Len(pat_entry);
                int pat_shift = Pattern::Shift(pat_entry);
                const uint8_t* pat_buf = pattern->Buf(pat_entry);
                int offset = j + 1 - (pat_len + pat_shift);

                int start = std::max((int)minPrefix_ - offset, 0);
                int end = std::min(pat_len, (int)maxOffset - offset);
                for (int k = start; k < end; k++) {
                    buffer[offset + k] = std::max(buffer[offset + k], pat_buf[k]);
                }
            }
        }
    }

    for (size_t i = minPrefix_; i < maxOffset; i++) {
        result[i] = (buffer[i] & 1u) ? hyphenValue : HyphenationType::DONT_BREAK;
    }
}

const Header* Hyphenator::GetHeader() const
{
    return reinterpret_cast<const Header*>(patternData_.data());
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
