/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_JS_PARSER_FONT_SOURCE_H
#define OHOS_ROSEN_JS_PARSER_FONT_SOURCE_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <set>

#include "font_parser.h"

namespace OHOS::Rosen {
using FontDescriptorPtr = std::shared_ptr<TextEngine::FontParser::FontDescriptor>;
class ParserFontSource {
public:
    ParserFontSource();
    ~ParserFontSource();
    void ParserFontDirectory(const std::string& path = "/system/fonts");
    void MatchFromFontDescriptor(FontDescriptorPtr desc, std::set<FontDescriptorPtr>& result);
    void ClearFontFileCache();

private:
    void FontDescriptorScatter(FontDescriptorPtr desc);
    void HandleFontFile(const std::string& path);
    void HandlePath(char* fullPath);
    bool HandleMapIntersection(std::set<FontDescriptorPtr>& finishRet, const std::string& name,
        std::unordered_map<std::string, std::set<FontDescriptorPtr>>& map);
    bool FilterBoldCache(int weight, std::set<FontDescriptorPtr>& finishRet);
    bool FilterWidthCache(int width, std::set<FontDescriptorPtr>& finishRet);
    bool FilterItalicCache(int italic, std::set<FontDescriptorPtr>& finishRet);
    bool FilterMonoSpaceCache(bool monoSpace, std::set<FontDescriptorPtr>& finishRet);
    bool FilterSymbolicCache(bool symbolic, std::set<FontDescriptorPtr>& finishRet);
    bool FilterTypeStyle(int typeStyle, std::set<FontDescriptorPtr>& finishRet);

private:
    TextEngine::FontParser parser_;

    struct FontDescriptorEqual {
        bool operator()(const FontDescriptorPtr& lhs, const FontDescriptorPtr& rhs) const
        {
            if (lhs->fontFamily == rhs->fontFamily) {
                return lhs->fullName < rhs->fullName;
            }
            return lhs->fontFamily < rhs->fontFamily;
        }
    };
    std::set<FontDescriptorPtr, FontDescriptorEqual> allFontDescriptor_;
    std::unordered_map<std::string, std::set<FontDescriptorPtr>> fontFamilyMap_;
    std::unordered_map<std::string, std::set<FontDescriptorPtr>> fullNameMap_;
    std::unordered_map<std::string, std::set<FontDescriptorPtr>> postScriptNameMap_;
    std::unordered_map<std::string, std::set<FontDescriptorPtr>> fontSubfamilyNameMap_;
    std::set<FontDescriptorPtr> boldCache_;
    std::set<FontDescriptorPtr> italicCache_;
    std::set<FontDescriptorPtr> monoSpaceCache_;
    std::set<FontDescriptorPtr> symbolicCache_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_JS_PARSER_FONT_SOURCE_H