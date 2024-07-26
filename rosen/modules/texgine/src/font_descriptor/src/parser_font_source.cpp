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

#include <filesystem>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log_wrapper.h"

namespace fs = std::filesystem;
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
}

void ParserFontSource::ParserFontDirectory(const std::string& path)
{
    if (path.empty() || !fs::exists(path)) {
        TEXT_LOGE("%{public}s path:%{public}s invalid", __FUNCTION__, path.c_str());
        return;
    }

    struct dirent *entry;
    DIR *dp = opendir(path.c_str());

    if (dp == NULL) {
        TEXT_LOGE("%{public}s dp is nullptr", __FUNCTION__);
        return;
    }

    while ((entry = readdir(dp))) {
        char fullPath[PATH_MAX] = { '\0' };
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        int len = snprintf_s(fullPath, sizeof(fullPath), sizeof(fullPath) - 1, "%s/%s", path.c_str(), entry->d_name);
        if (len < 0) {
            TEXT_LOGE("snprintf_s failed");
            continue;
        }

        struct stat entryInfo;
        if (lstat(fullPath, &entryInfo) == 0 && S_ISLNK(entryInfo.st_mode)) {
            char linkTarget[PATH_MAX] = { '\0' };
            ssize_t len = readlink(fullPath, linkTarget, sizeof(linkTarget) - 1);
            if (len != -1) {
                linkTarget[len] = '\0';
                char absLinkTarget[PATH_MAX];
                if (realpath(linkTarget, absLinkTarget) != NULL) {
                    HandleFontFile(absLinkTarget);
                } else {
                    TEXT_LOGE("symbol link realpath failed");
                }
            } else {
                TEXT_LOGE("symbol link readlink failed");
            }
        } else {
            char absPath[PATH_MAX] = { '\0' };
            if (realpath(fullPath, absPath) != NULL) {
                HandleFontFile(absPath);
            } else {
                TEXT_LOGE("realpath failed");
            }
        }
    }

    closedir(dp);
    if (allFontDescriptor_.empty()) {
        TEXT_LOGE("%{public}s parser failed", path.c_str());
    }

    TEXT_LOGD("allFontDescriptor_ size: %{public}zu, fontFamilyMap_ size: %{public}zu, \
        fullNameMap_ size: %{public}zu, postScriptNameMap_ size: %{public}zu, \
        fontSubfamilyNameMap_ size: %{public}zu, boldCache_ size: %{public}zu, italicCache_ size: %{public}zu, \
        monoSpaceCache_ size: %{public}zu, symbolicCache_ size: %{public}zu",
        allFontDescriptor_.size(), fontFamilyMap_.size(), fullNameMap_.size(), postScriptNameMap_.size(),
        fontSubfamilyNameMap_.size(), boldCache_.size(), italicCache_.size(), monoSpaceCache_.size(),
        symbolicCache_.size());
}

void ParserFontSource::HandleFontFile(const std::string& path)
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
            FontDescriptorScatter(item);
        }
    } else {
        TextEngine::FontParser::FontDescriptor desc;
        if (!parser_.ParserFontDescriptorFromPath(path, desc)) {
            TEXT_LOGE("%{public}s ParserFontDescriptorFromPath failed", path.c_str());
            return;
        }
        FontDescriptorScatter(std::make_shared<TextEngine::FontParser::FontDescriptor>(desc));
    }
}

void ParserFontSource::FontDescriptorScatter(FontDescriptorPtr desc)
{
    auto ret = allFontDescriptor_.emplace(desc);
    if (!ret.second) {
        return;
    }

    auto handleMapScatter = [&](auto& map, const auto& key) {
        if (map.find(key) == map.end()) {
            map[key] = { desc };
        } else {
            map[key].emplace(desc);
        }
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

void ParserFontSource::MatchFromFontDescriptor(FontDescriptorPtr desc, std::set<FontDescriptorPtr>& result)
{
    if (desc == nullptr) {
        TEXT_LOGE("desc is nullptr");
        return;
    }
    if (desc->fontFamily.empty() && desc->fullName.empty() && desc->postScriptName.empty() && desc->fontSubfamily.empty()
        && desc->weight == 0 && desc->width == 0 && desc->italic == 0 && !desc->monoSpace && !desc->symbolic
        && desc->typeStyle == 0) {
        result = std::set<FontDescriptorPtr>(allFontDescriptor_.begin(), allFontDescriptor_.end());
        return;
    }

    std::set<FontDescriptorPtr> finishRet;
    auto handleMapIntersection = [&] (const std::string& name, auto& map) -> bool {
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
    };

    TEXT_INFO_CHECK(handleMapIntersection(desc->fontFamily, fontFamilyMap_), return, "fontFamily match failed");
    TEXT_INFO_CHECK(handleMapIntersection(desc->fullName, fullNameMap_), return, "fullName match failed");
    TEXT_INFO_CHECK(handleMapIntersection(desc->postScriptName, postScriptNameMap_), return,
        "postScriptName match failed");
    TEXT_INFO_CHECK(handleMapIntersection(desc->fontSubfamily, fontSubfamilyNameMap_), return,
        "fontSubfamily match failed");

    if (desc->weight > 0) {
        std::set<FontDescriptorPtr> temp;
        std::set<FontDescriptorPtr>::iterator begin;
        std::set<FontDescriptorPtr>::iterator end;
        if (!finishRet.empty()) {
            begin = finishRet.begin();
            end = finishRet.end();
        } else if (desc->weight > WEIGHT_400) {
            begin = boldCache_.begin();
            end = boldCache_.end();
        } else {
            begin = allFontDescriptor_.begin();
            end = allFontDescriptor_.end();
        }
        std::for_each(begin, end, [&](FontDescriptorPtr item) {
            if (item->weight == desc->weight) {
                temp.emplace(item);
            }
        });

        if (temp.empty()) {
            TEXT_LOGI("weight match failed");
            return;
        }
        finishRet = std::move(temp);
    }

    if (desc->width > 0) {
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
            if (item->width == desc->width) {
                temp.emplace(item);
            }
        });

        if (temp.empty()) {
            TEXT_LOGI("width match failed");
            return;
        }
        finishRet = std::move(temp);
    }

    if (desc->italic != 0) {
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
            return;
        }
        finishRet = std::move(temp);
    }

    if (desc->monoSpace) {
        if (!finishRet.empty()) {
            std::set<FontDescriptorPtr> temp;
            std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescriptorPtr item) {
                if (item->monoSpace) {
                    temp.emplace(item);
                }
            });
            if (temp.empty()) {
                TEXT_LOGI("monoSpace match failed");
                return;
            }
        } else {
            finishRet = monoSpaceCache_;
        }
    }

    if (desc->symbolic) {
        if (!finishRet.empty()) {
            std::set<FontDescriptorPtr> temp;
            std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescriptorPtr item) {
                if (desc->symbolic) {
                    temp.emplace(desc);
                }
            });
            if (temp.empty()) {
                TEXT_LOGI("symbolic match failed");
                return;
            }
        } else {
            finishRet = symbolicCache_;
        }
    }

    if (desc->typeStyle > 0) {
        bool italicFlag = desc->typeStyle & TextEngine::FontParser::FontTypeStyle::ITALIC;
        bool boldFlag = desc->typeStyle & TextEngine::FontParser::FontTypeStyle::BOLD;

        if (finishRet.empty()) {
            if (italicFlag) {
                if (italicCache_.empty()) {
                    TEXT_LOGI("italicCache_ is empty");
                    return;
                }
                finishRet = italicCache_;
            }

            if (boldFlag) {
                if (boldCache_.empty()) {
                    TEXT_LOGI("boldCache_ is empty");
                    return;
                }

                if (finishRet.empty()) {
                    finishRet = boldCache_;
                } else {
                    std::set<FontDescriptorPtr> temp;
                    std::set_intersection(finishRet.begin(), finishRet.end(), boldCache_.begin(), boldCache_.end(),
                        std::insert_iterator<std::set<FontDescriptorPtr>>(temp, temp.begin()));
                    if (temp.empty()) {
                        TEXT_LOGI("typeStyle bold match failed");
                        return;
                    }
                    finishRet = std::move(temp);
                }
            }
        } else {
            std::set<FontDescriptorPtr> temp;
            std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescriptorPtr item) {
                if ((italicFlag && item->italic != 0) || (boldFlag && item->weight > WEIGHT_400)) {
                    temp.emplace(item);
                }
            });
            if (temp.empty()) {
                TEXT_LOGI("typeStyle match failed");
                return;
            }
            finishRet = std::move(temp);
        }
    }

    result = finishRet;
}
}