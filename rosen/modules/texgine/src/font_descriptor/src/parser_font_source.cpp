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

#include "parser_font_source.h"

#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log_wrapper.h"
#include "font_config.h"

#define INSTALL_FONT_CONFIG_FILE "/system/etc/install_fontconfig.json"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t WEIGHT_400 = 400;
}

ParserFontSource::ParserFontSource() {}

ParserFontSource::~ParserFontSource() {}

void ParserFontSource::ClearFontFileCache()
{
    allFontDescriptor_.clear();
    fontFamilyMap_.clear();
    fullNameMap_.clear();
    postScriptNameMap_.clear();
    fontSubfamilyNameMap_.clear();
    boldCache_.clear();
    italicCache_.clear();
    monoSpaceCache_.clear();
    symbolicCache_.clear();
    installFullNameMap_.clear();
    stylishFullNameMap_.clear();
}

void ParserFontSource::ParserSystemFonts()
{
    for (auto& item : parser_.GetSystemFonts()) {
        FontDescriptorScatter(item);    
	}
	Dump();
}

void ParserFontSource::ParserStylishFonts()
{
    std::vector<TextEngine::FontParser::FontDescriptor> descriptors = 
        parser_.GetVisibilityFonts();
    for(const auto& descriptor : descriptors)
    {
        auto descriptorPtr = std::make_shared<TextEngine::FontParser::FontDescriptor>(descriptor);
        CreateStylishFullNameMap(descriptorPtr);
    }
}

void ParserFontSource::ParserInstallFonts()
{
    std::vector<std::string> fontPathList;
    TextEngine::FontConfigJson fontConfigJson;
    fontConfigJson.ParseInstallConfig(INSTALL_FONT_CONFIG_FILE, fontPathList);
    for(const auto& path : fontPathList) {
        HandleInstallFontFile(path);
    }
}

void ParserFontSource::HandleInstallFontFile(const std::string& path)
{
    auto ends_with = [&] (const std::string& str, const std::string& suffix) -> bool {
        if (str.size() < suffix.size()) {
            return false;
        }
        return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    };

    if (ends_with(path, ".ttc")) {
        std::vector<FontDescriptorPtr> descriptors;
        if (!parser_.ParserFontDescriptorFromPath(path, descriptors)) {
            TEXT_LOGE("%{public}s ParserFontDescriptorFromPath failed", path.c_str());
            return;
        }
        for (auto& item : descriptors) {
            CreateInstallFullNameMap(item);
            i++;
        }
    } else {
        TextEngine::FontParser::FontDescriptor desc;
        if (!parser_.ParserFontDescriptorFromPath(path, desc)) {
            TEXT_LOGE("%{public}s ParserFontDescriptorFromPath failed", path.c_str());
            return;
        }
        CreateInstallFullNameMap(std::make_shared<TextEngine::FontParser::FontDescriptor>(desc));
        i++;
    }
}

void ParserFontSource::FontDescriptorScatter(FontDescriptorPtr desc)
{
    auto ret = allFontDescriptor_.emplace(desc);
    if (!ret.second) {
        return;
    }

    auto handleMapScatter = [&](auto& map, const auto& key) {
        map[key].emplace(desc);
    };

    handleMapScatter(fontFamilyMap_, desc->fontFamily);
    handleMapScatter(fullNameMap_, desc->fullName);
    handleMapScatter(postScriptNameMap_, desc->postScriptName);
    handleMapScatter(fontSubfamilyNameMap_, desc->fontSubfamily);

    if (desc->weight > WEIGHT_400) {
        desc->typeStyle |= TextEngine::FontParser::FontTypeStyle::BOLD;
        boldCache_.emplace(desc);
    }

    if (desc->italic != 0) {
        desc->typeStyle |= TextEngine::FontParser::FontTypeStyle::ITALIC;
        italicCache_.emplace(desc);
    }

    if (desc->monoSpace) {
        monoSpaceCache_.emplace(desc);
    }

    if (desc->symbolic) {
        symbolicCache_.emplace(desc);
    }
}

void ParserFontSource::CreateInstallFullNameMap(FontDescriptorPtr desc)
{
    auto handleMapScatter = [&](auto& map, const auto& key) {
            map[key].emplace(desc);
    };

    if(desc->fullName.size() > 0) {
        handleMapScatter(installFullNameMap_, desc->fullName);
    }
}

void ParserFontSource::CreateStylishFullNameMap(FontDescriptorPtr desc)
{
    auto handleMapScatter = [&](auto& map, const auto& key) {
            map[key].emplace(desc);
    };

    if(desc->fullName.size() > 0) {
        handleMapScatter(stylishFullNameMap_, desc->fullName);
    }
}

std::set<std::string> ParserFontSource::GetInstallFontList()
{
    std::set<std::string> fullNameList;
    installFullNameMap_.clear();
    ParserInstallFonts();
    for(const auto& temp : installFullNameMap_) {
        fullNameList.emplace(temp.first);
    }
    return fullNameList;
}

std::set<std::string> ParserFontSource::GetStylishFontList()
{
    std::set<std::string> fullNameList;
    for(const auto& temp : stylishFullNameMap_)
    {
        fullNameList.emplace(temp.first);
    }
    return fullNameList;
}

std::set<std::string> ParserFontSource::GetGenericFontList()
{
    std::set<std::string> fullNameList;
    for(const auto& temp : allFontDescriptor_)
    {
        fullNameList.emplace(temp->fullName);
    }
    return fullNameList;
}

void ParserFontSource::GetSystemFontList(const int32_t& systemFontType, std::set<std::string>& fontList)
{
    auto flags = systemFontType;

    if(systemFontType == TextEngine::FontParser::SystemFontType::ALL) {
        flags = TextEngine::FontParser::SystemFontType::GENERIC | 
            TextEngine::FontParser::SystemFontType::STYLISH |
            TextEngine::FontParser::SystemFontType::INSTALLED;
    }

    if(flags & TextEngine::FontParser::SystemFontType::GENERIC) {
        auto fullNameList = GetGenericFontList();
        fontList.insert(fullNameList.begin(), fullNameList.end());
    }

    if(flags & TextEngine::FontParser::SystemFontType::STYLISH) {
        auto fullNameList = GetStylishFontList();
        fontList.insert(fullNameList.begin(), fullNameList.end());
    }

    if(flags & TextEngine::FontParser::SystemFontType::INSTALLED) {
        auto fullNameList = GetInstallFontList();
        fontList.insert(fullNameList.begin(), fullNameList.end());
    }
}

void ParserFontSource::GetFontDescriptorByName(const std::string& fullName, FontDescriptorPtr& result)
{
    if (fullName.empty()) {
        TEXT_LOGI("Empty fullName provided");
        return;
    }
    installFullNameMap_.clear();
    ParserInstallFonts();
    for (const auto& fontDescriptor : allFontDescriptor_) {
        if(fontDescriptor->fullName == fullName) {
            result = fontDescriptor;
            return;
        }
    }
    auto stylishIt = stylishFullNameMap_.find(fullName);
    if (stylishIt != stylishFullNameMap_.end()) {
        result = *(stylishIt->second.begin());
        return;
    } 
    auto installIt = installFullNameMap_.find(fullName);
    if (installIt != installFullNameMap_.end()) {
        result = *(installIt->second.begin());
        return;
    } else {
        TEXT_LOGI("Font descriptor not found for this fullName");
        result.reset();
        return;
    }
}

bool ParserFontSource::HandleMapIntersection(std::set<FontDescriptorPtr>& finishRet, const std::string& name,
    std::unordered_map<std::string, std::set<FontDescriptorPtr>>& map)
{
    if (name.empty()) {
        return true;
    }
    auto iter = map.find(name);
    if (iter == map.end()) {
        return false;
    }
    if (finishRet.empty()) {
        finishRet = iter->second;
    } else {
        std::set<FontDescriptorPtr> temp;
        std::set_intersection(iter->second.begin(), iter->second.end(), finishRet.begin(), finishRet.end(),
            std::insert_iterator<std::set<FontDescriptorPtr>>(temp, temp.begin()));
        if (temp.empty()) {
            return false;
        }
        finishRet = std::move(temp);
    }
    return true;
}

bool ParserFontSource::FilterBoldCache(int weight, std::set<FontDescriptorPtr>& finishRet)
{
    if (weight < 0) {
        return false;
    }

    if (weight == 0) {
        return true;
    }

    std::set<FontDescriptorPtr> temp;
    std::set<FontDescriptorPtr>::iterator begin;
    std::set<FontDescriptorPtr>::iterator end;
    if (!finishRet.empty()) {
        begin = finishRet.begin();
        end = finishRet.end();
    } else if (weight > WEIGHT_400) {
        begin = boldCache_.begin();
        end = boldCache_.end();
    } else {
        begin = allFontDescriptor_.begin();
        end = allFontDescriptor_.end();
    }
    std::for_each(begin, end, [&](FontDescriptorPtr item) {
        if (item->weight == weight) {
            temp.emplace(item);
        }
    });

    if (temp.empty()) {
        TEXT_LOGI("weight match failed");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool ParserFontSource::FilterWidthCache(int width, std::set<FontDescriptorPtr>& finishRet)
{
    if (width < 0) {
        return false;
    }

    if (width == 0) {
        return true;
    }

    std::set<FontDescriptorPtr> temp;
    std::set<FontDescriptorPtr>::iterator begin;
    std::set<FontDescriptorPtr>::iterator end;
    if (!finishRet.empty()) {
        begin = finishRet.begin();
        end = finishRet.end();
    } else {
        begin = allFontDescriptor_.begin();
        end = allFontDescriptor_.end();
    }
    std::for_each(begin, end, [&](FontDescriptorPtr item) {
        if (item->width == width) {
            temp.emplace(item);
        }
    });

    if (temp.empty()) {
        TEXT_LOGI("width match failed");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool ParserFontSource::FilterItalicCache(int italic, std::set<FontDescriptorPtr>& finishRet)
{
    if (italic == 0) {
        return true;
    }
    std::set<FontDescriptorPtr> temp;
    if (!finishRet.empty()) {
        std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescriptorPtr item) {
            if (item->italic != 0) {
                temp.emplace(item);
            }
        });
    } else {
        temp = italicCache_;
    }
    if (temp.empty()) {
        TEXT_LOGI("italic match failed");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool ParserFontSource::FilterMonoSpaceCache(bool monoSpace, std::set<FontDescriptorPtr>& finishRet)
{
    if (!monoSpace) {
        return true;
    }

    std::set<FontDescriptorPtr> temp;
    if (!finishRet.empty()) {
        std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescriptorPtr item) {
            if (item->monoSpace) {
                temp.emplace(item);
            }
        });
    } else {
        temp = monoSpaceCache_;
    }
    if (temp.empty()) {
        TEXT_LOGI("monoSpace match failed");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool ParserFontSource::FilterSymbolicCache(bool symbolic, std::set<FontDescriptorPtr>& finishRet)
{
    if (!symbolic) {
        return true;
    }
    std::set<FontDescriptorPtr> temp;
    if (!finishRet.empty()) {
        std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescriptorPtr item) {
            if (item->symbolic) {
                temp.emplace(item);
            }
        });
    } else {
        temp = symbolicCache_;
    }
    if (temp.empty()) {
        TEXT_LOGI("symbolic match failed");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool ParserFontSource::FilterTypeStyle(int typeStyle, std::set<FontDescriptorPtr>& finishRet)
{
    if (typeStyle < 0) {
        return false;
    }
    if (typeStyle == 0) {
        return true;
    }

    bool italicFlag = typeStyle & TextEngine::FontParser::FontTypeStyle::ITALIC;
    bool boldFlag = typeStyle & TextEngine::FontParser::FontTypeStyle::BOLD;
    auto handleCache = [&](const std::set<FontDescriptorPtr>& cache, const char* cacheName) {
        if (cache.empty()) {
            TEXT_LOGI("%s is empty", cacheName);
            return false;
        }
        if (finishRet.empty()) {
            finishRet = cache;
        } else {
            std::set<FontDescriptorPtr> temp;
            std::set_intersection(finishRet.begin(), finishRet.end(), cache.begin(), cache.end(),
                                  std::inserter(temp, temp.begin()));
            if (temp.empty()) {
                TEXT_LOGI("typeStyle %s match failed", cacheName);
                return false;
            }
            finishRet = std::move(temp);
        }
        return true;
    };
    if (italicFlag && !handleCache(italicCache_, "italic")) {
        return false;
    }
    if (boldFlag && !handleCache(boldCache_, "bold")) {
        return false;
    }
    return true;
}

void ParserFontSource::MatchFromFontDescriptor(FontDescriptorPtr desc, std::set<FontDescriptorPtr>& result)
{
    if (desc == nullptr) {
        TEXT_LOGE("desc is nullptr");
        return;
    }
    if (desc->fontFamily.empty() && desc->fullName.empty() && desc->postScriptName.empty()
        && desc->fontSubfamily.empty() && desc->weight == 0 && desc->width == 0 && desc->italic == 0
        && !desc->monoSpace && !desc->symbolic && desc->typeStyle == 0) {
        result = std::set<FontDescriptorPtr>(allFontDescriptor_.begin(), allFontDescriptor_.end());
        return;
    }

    std::set<FontDescriptorPtr> finishRet;
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fontFamily, fontFamilyMap_), return,
        "fontFamily match failed");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fullName, fullNameMap_), return, "fullName match failed");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->postScriptName, postScriptNameMap_), return,
        "postScriptName match failed");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fontSubfamily, fontSubfamilyNameMap_), return,
        "fontSubfamily match failed");

    TEXT_CHECK(FilterBoldCache(desc->weight, finishRet), return);
    TEXT_CHECK(FilterWidthCache(desc->width, finishRet), return);
    TEXT_CHECK(FilterItalicCache(desc->italic, finishRet), return);
    TEXT_CHECK(FilterMonoSpaceCache(desc->monoSpace, finishRet), return);
    TEXT_CHECK(FilterSymbolicCache(desc->symbolic, finishRet), return);
    TEXT_CHECK(FilterTypeStyle(desc->typeStyle, finishRet), return);
    result = finishRet;
}

void ParserFontSource::Dump()
{
    TEXT_LOGI("allFontDescriptor size: %{public}zu", allFontDescriptor_.size());
    TEXT_LOGI("fontFamilyMap size: %{public}zu", fontFamilyMap_.size());
    TEXT_LOGI("fullNameMap size: %{public}zu", fullNameMap_.size());
    TEXT_LOGI("postScriptNameMap size: %{public}zu", postScriptNameMap_.size());
    TEXT_LOGI("fontSubfamilyNameMap size: %{public}zu", fontSubfamilyNameMap_.size());
    TEXT_LOGI("boldCache size: %{public}zu", boldCache_.size());
    TEXT_LOGI("italicCache size: %{public}zu", italicCache_.size());
    TEXT_LOGI("monoSpaceCache size: %{public}zu", monoSpaceCache_.size());
    TEXT_LOGI("symbolicCache size: %{public}zu", symbolicCache_.size());
}
}