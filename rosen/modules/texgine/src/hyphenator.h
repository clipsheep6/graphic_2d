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

#include <memory>
#include <unordered_map>
#include <vector>

#include "unicode/locid.h"

#ifndef ROSEN_MODULES_TEXGINE_SRC_HYPHENATOR_H
#define ROSEN_MODULES_TEXGINE_SRC_HYPHENATOR_H

namespace OHOS {
namespace Rosen {
namespace TextEngine {
const float LINE_PENALTY_MULTIPLIER = 2.0f;
enum class HyphenationType : uint8_t {

    DONT_BREAK = 0,
    BREAK_AND_INSERT_HYPHEN = 1,
    BREAK_AND_INSERT_ARMENIAN_HYPHEN = 2,
    BREAK_AND_INSERT_MAQAF = 3,
    BREAK_AND_INSERT_UCAS_HYPHEN = 4,
    BREAK_AND_DONT_INSERT_HYPHEN = 5,
    BREAK_AND_REPLACE_WITH_HYPHEN = 6,
    BREAK_AND_INSERT_HYPHEN_AT_NEXT_LINE = 7,
    BREAK_AND_INSERT_HYPHEN_AND_ZWJ = 8
};

enum HyphenationFrequency {
    kHyphenationFrequency_None = 0,
    kHyphenationFrequency_Normal = 1,
    kHyphenationFrequency_Full = 2
};

const std::string EnUsHyph = "/system/usr/hyphen-data/hyph-en-us.hyb";
const int EnUsMinPrefix = 2;
const int EnUsMinSuffix = 3;
std::vector<uint8_t> ReadWholeFile(const std::string& filePath);

class HyphenEdit {
public:
    static const uint32_t NO_EDIT = 0x00;

    static const uint32_t INSERT_HYPHEN_AT_END = 0x01;
    static const uint32_t INSERT_ARMENIAN_HYPHEN_AT_END = 0x02;
    static const uint32_t INSERT_MAQAF_AT_END = 0x03;
    static const uint32_t INSERT_UCAS_HYPHEN_AT_END = 0x04;
    static const uint32_t INSERT_ZWJ_AND_HYPHEN_AT_END = 0x05;
    static const uint32_t REPLACE_WITH_HYPHEN_AT_END = 0x06;
    static const uint32_t BREAK_AT_END = 0x07;

    static const uint32_t INSERT_HYPHEN_AT_START = 0x01 << 3;
    static const uint32_t INSERT_ZWJ_AT_START = 0x02 << 3;
    static const uint32_t BREAK_AT_START = 0x03 << 3;

    static const uint32_t MASK_END_OF_LINE = 0x07;
    static const uint32_t MASK_START_OF_LINE = 0x03 << 3;

    inline static bool IsReplacement(uint32_t hyph)
    {
        return hyph == REPLACE_WITH_HYPHEN_AT_END;
    }

    inline static bool IsInsertion(uint32_t hyph)
    {
        return (hyph == INSERT_HYPHEN_AT_END || hyph == INSERT_ARMENIAN_HYPHEN_AT_END || hyph == INSERT_MAQAF_AT_END ||
                hyph == INSERT_UCAS_HYPHEN_AT_END || hyph == INSERT_ZWJ_AND_HYPHEN_AT_END ||
                hyph == INSERT_HYPHEN_AT_START || hyph == INSERT_ZWJ_AT_START);
    }

    const static uint32_t* GetHyphenString(uint32_t hyph);

    static uint32_t EditForThisLine(HyphenationType type);

    static uint32_t EditForNextLine(HyphenationType type);

    HyphenEdit() : hyphen_(NO_EDIT) {}

    HyphenEdit(uint32_t hyphenInt) : hyphen_(hyphenInt) {}

    uint32_t GetHyphen() const;

    bool operator==(const HyphenEdit& other) const;

    uint32_t GetEnd() const;

    uint32_t GetStart() const;

private:
    uint32_t hyphen_;
};

struct Header;

class Hyphenator {
public:
    void Hyphenate(std::vector<HyphenationType>* result, const uint16_t* word, size_t len, const icu::Locale& locale);

    static bool IsLineBreakingHyphen(uint32_t cp);

    static void LoadBinary(std::vector<uint8_t> patternData, size_t minPrefix, size_t minSuffix);

    static Hyphenator* Instance();

    static Hyphenator result_;
    static std::vector<uint8_t> patternData_;
    static bool isLoad_;

private:
    void HyphenateWithNoPatterns(HyphenationType* result, const uint16_t* word, size_t len, const icu::Locale& locale);

    HyphenationType AlphabetLookup(uint16_t* alphaCodes, const uint16_t* word, size_t len);

    void HyphenateFromCodes(HyphenationType* result, const uint16_t* codes, size_t len, HyphenationType hyphenValue);
    
    const Header* GetHeader() const;

    static const size_t MAX_HYPHENATED_SIZE = 64;

    size_t minPrefix_;
    size_t minSuffix_;   
};

} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
#endif
