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

#include <mutex>

#include "texgine/utils/exlog.h"
#include "texgine/font_manager.h"
#include "font_parser.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {

#define MAXWIDTH 10
#define MAXWEIGHT 10
#define FIRSTPRIORITY 10000
#define SECONDPRIORITY 100
#define MULTIPLE 100

sptr<FontManager> FontManager::GetInstance()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
       instance = new FontManager();
    }
    return instance;
}

bool FontManager::ResAccurateMatch(FontParser::FontDescriptor& descSrc, FontParser::FontDescriptor& descFind)
{
    if (!descFind.postScriptName.empty() && descFind.postScriptName == descSrc.postScriptName)
       return true;

    if (!descFind.fullName.empty() && descFind.fullName == descSrc.fullName)
       return true;

    return false;
}

bool FontManager::ResFallbackMatch(FontParser::FontDescriptor& descSrc, FontParser::FontDescriptor& descFind)
{
    if (descFind.fontFamily != descSrc.fontFamily)
       return false;

    if (!descFind.fontSubfamily.empty() && descFind.fontSubfamily != descSrc.fontSubfamily)
       return false;

    if (descFind.weight !=-1 && descFind.weight != descSrc.weight)
       return false;

    if (descFind.italic !=-1 && descFind.italic != descSrc.italic)
       return false;

    return true;
}

int FontManager::ScoreFallbackMatch(FontParser::FontDescriptor& descSrc, FontParser::FontDescriptor& descFind)
{
    int score = 0;
    score += (MAXWIDTH - std::abs(descFind.width - descSrc.width)) * SECONDPRIORITY;
    score += (MAXWIDTH - std::abs(descFind.italic - descSrc.italic)) * FIRSTPRIORITY;
    score += (MAXWEIGHT - std::abs(descFind.weight / 100 - descSrc.weight / MULTIPLE));
    return score;
}

FontParser::FontDescriptor* FontManager::FindFont(ResultSet* fontSrc, FontParser::FontDescriptor& descFind)
{
    FontParser::FontDescriptor* descResPtr = nullptr;

    if (!resultCache.empty()) {
       for (ResultCache::iterator it = resultCache.begin(); it != resultCache.end(); it++) {
           if (!ResAccurateMatch(*it, descFind)) {
              continue;
           } else {
              descResPtr = &(*it);
              break;
           }
       }
       if (descResPtr != nullptr) {
          LOGEX_FUNC_LINE_DEBUG() << "cache get accurate result";
          return descResPtr;  
       }
    }

    for (ResultSet::iterator it = fontSrc->begin(); it != fontSrc->end(); it++) {
        if (!ResAccurateMatch(*it, descFind)) {
           continue;
        } else {
           descResPtr = &(*it);
		   LOGEX_FUNC_LINE_DEBUG() << "os get accurate result";
           break;
        }
    }

    if (descResPtr != nullptr) {
       resultCache.push_back(*descResPtr);
       return descResPtr;
    }

    if (descFind.fontFamily.empty()) {
       return descResPtr;
    }
       
    ResultSet fbkFonts;
    if (!resultCache.empty()) {
       for (ResultCache::iterator it = resultCache.begin(); it != resultCache.end(); it++) {
           if (ResFallbackMatch(*it, descFind)) {
              fbkFonts.push_back(*it);
			  LOGEX_FUNC_LINE_DEBUG() << "cache get best result";
           }
       }
    }

    if (fbkFonts.empty()) {
       for (ResultSet::iterator it = fontSrc->begin(); it != fontSrc->end(); it++) {
           if (ResFallbackMatch(*it, descFind)) {
              fbkFonts.push_back(*it);
			  LOGEX_FUNC_LINE_DEBUG() << "os get best result";
           }
       }
    }

    int bestScore = 0;
    for (ResultSet::iterator it = fbkFonts.begin(); it != fbkFonts.end(); it++) {
        int score = ScoreFallbackMatch(*it, descFind);
        if (score > bestScore) {
           bestScore = score;
           descResPtr = &(*it);
        } 
    }
       
    if (descResPtr != nullptr) {
       resultCache.push_back(*descResPtr);
       return descResPtr;
    }
    return descResPtr;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
