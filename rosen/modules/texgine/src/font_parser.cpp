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

#include "font_parser.h"

#include <iomanip>

#include "font_config.h"
#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define SUCCESSED 0
#define FAILED 1

#define FONT_CONFIG_FILE  "/system/fonts/visibility_list.json"

FontParser::FontDescriptor::FontDescriptor()
{
    path = "";
    postScriptName = "";
    fullName = "";
    fontFamily = "";
    fontSubfamily = "";
    weight = 0;
    width = 0;
    italic = 0;
    monoSpace = 0;
    symbolic = 0;
}

FontParser::FontParser()
{
    data_ = nullptr;
    length_ = 0;
    FontConfig fontConfig(FONT_CONFIG_FILE);
    fontSet_ = fontConfig.GetFontSet();
}

void FontParser::ProcessCmapTable(const struct CmapTables* cmapTable, FontParser::FontDescriptor& fontDescriptor) const
{
    for (auto i = 0; i < cmapTable->numTables.Get(); ++i) {
        const auto& record = cmapTable->encodingRecords[i];
        FontParser::PlatformId platformId = static_cast<FontParser::PlatformId>(record.platformID.Get());
        FontParser::EncodingIdWin encodingId = static_cast<FontParser::EncodingIdWin>(record.encodingID.Get());
        if (platformId == FontParser::PlatformId::WINDOWS && encodingId == FontParser::EncodingIdWin::SYMBOL) {
            fontDescriptor.symbolic = true;
            break;
        }
    }
}

void FontParser::GetStringFromNameId(FontParser::NameId nameId, const char* data, uint16_t len,
    FontParser::FontDescriptor& fontDescriptor) const
{
    switch (nameId) {
        case FontParser::NameId::FONT_FAMILY: {
            if (fontDescriptor.fontFamily.size() == 0) {
                std::string fontFamily(data, len);
                fontDescriptor.fontFamily = fontFamily;
            }
            break;
        }
        case FontParser::NameId::FONT_SUBFAMILY: {
            if (fontDescriptor.fontSubfamily.size() == 0) {
                std::string fontSubfamily(data, len);
                fontDescriptor.fontSubfamily = fontSubfamily;
            }
            break;
        }
        case FontParser::NameId::FULL_NAME: {
            if (fontDescriptor.fullName.size() == 0) {
                std::string fullName(data, len);
                fontDescriptor.fullName = fullName;
            }
            break;
        }
        case FontParser::NameId::POSTSCRIPT_NAME: {
            if (fontDescriptor.postScriptName.size() == 0) {
                std::string postScriptName(data, len);
                fontDescriptor.postScriptName = postScriptName;
            }
            break;
        }
        default: {
            break;
        }
    }
}

void FontParser::ProcessNameTable(const struct NameTable* nameTable, FontParser::FontDescriptor& fontDescriptor) const
{
    auto count = nameTable->count.Get();
    auto storageOffset = nameTable->storageOffset.Get();
    auto stringStorage = data_ + storageOffset;
    for (int i = 0; i < count; ++i) {
        if (nameTable->nameRecord[i].stringOffset.Get() == 0 || nameTable->nameRecord[i].length.Get() == 0) {
            LOGSO_FUNC_LINE(ERROR) << "empty";
            continue;
        }
        FontParser::NameId nameId = static_cast<FontParser::NameId>(nameTable->nameRecord[i].nameId.Get());
        FontParser::PlatformId platformId =
            static_cast<FontParser::PlatformId>(nameTable->nameRecord[i].platformId.Get());
        auto len = nameTable->nameRecord[i].length.Get();
        auto stringOffset = nameTable->nameRecord[i].stringOffset.Get();
        const char* data = stringStorage + stringOffset;
        if (platformId == FontParser::PlatformId::MACINTOSH) {
            GetStringFromNameId(nameId, data, len, fontDescriptor);
        } else if (platformId == FontParser::PlatformId::WINDOWS) {
            char* buffer = new char[len + 1];
            int index = 0;
            for (int j = 0; j < len; ++j) {
                buffer[index++] = data[++j];
            }
            buffer[index] = '\0';
            data = buffer;
            GetStringFromNameId(nameId, data, index, fontDescriptor);
            delete[] buffer;
        }
    }
}

void FontParser::ProcessPostTable(const struct PostTable* postTable, FontParser::FontDescriptor& fontDescriptor) const
{
    if (postTable->italicAngle.Get() != 0) {
        fontDescriptor.italic = true;
    }
    if (postTable->isFixedPitch.Get() == 1) {
        fontDescriptor.monoSpace = true;
    }
}

int FontParser::ParseCmapTable(sk_sp<SkTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('c', 'm', 'a', 'p');
    auto size = typeface->getTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't cmap";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->getTableData(tag, 0, size, tableData.get());
    if (size != retTableData) {
        LOGSO_FUNC_LINE(ERROR) <<"get table data failed size: " << size << ", ret: " << retTableData;
        return FAILED;
    }
    hb_blob_t* hblob = nullptr;
    hblob = hb_blob_create(
        reinterpret_cast<const char*>(tableData.get()), size, HB_MEMORY_MODE_WRITABLE, tableData.get(), nullptr);
    if (hblob == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "hblob is nullptr";
        return FAILED;
    }
    data_ = hb_blob_get_data(hblob, nullptr);
    length_ = hb_blob_get_length(hblob);
    auto parseCmap = std::make_shared<CmapTableParser>(data_, length_);
    auto cmapTable = parseCmap->Parse(data_, length_);
    ProcessCmapTable(cmapTable, fontDescriptor);

    return SUCCESSED;
}

int FontParser::ParseNameTable(sk_sp<SkTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('n', 'a', 'm', 'e');
    auto size = typeface->getTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't name";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->getTableData(tag, 0, size, tableData.get());
    if (size != retTableData) {
        LOGSO_FUNC_LINE(ERROR) <<"get table data failed size: " << size << ", ret: " << retTableData;
        return FAILED;
    }
    hb_blob_t* hblob = nullptr;
    hblob = hb_blob_create(
        reinterpret_cast<const char*>(tableData.get()), size, HB_MEMORY_MODE_WRITABLE, tableData.get(), nullptr);
    if (hblob == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "hblob is nullptr";
        return FAILED;
    }
    data_ = hb_blob_get_data(hblob, nullptr);
    length_ = hb_blob_get_length(hblob);
    auto parseName = std::make_shared<NameTableParser>(data_, length_);
    auto nameTable = parseName->Parse(data_, length_);
    ProcessNameTable(nameTable, fontDescriptor);

    return SUCCESSED;
}

int FontParser::ParsePostTable(sk_sp<SkTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('p', 'o', 's', 't');
    auto size = typeface->getTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't post";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->getTableData(tag, 0, size, tableData.get());
    if (size != retTableData) {
        LOGSO_FUNC_LINE(ERROR) <<"get table data failed size: " << size << ", ret: " << retTableData;
        return FAILED;
    }
    hb_blob_t* hblob = nullptr;
    hblob = hb_blob_create(
        reinterpret_cast<const char*>(tableData.get()), size, HB_MEMORY_MODE_WRITABLE, tableData.get(), nullptr);
    if (hblob == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "hblob is nullptr";
        return FAILED;
    }
    data_ = hb_blob_get_data(hblob, nullptr);
    length_ = hb_blob_get_length(hblob);
    auto parsePost = std::make_shared<PostTableParser>(data_, length_);
    auto postTable = parsePost->Parse(data_, length_);
    ProcessPostTable(postTable, fontDescriptor);

    return SUCCESSED;
}

int FontParser::ParseTable(sk_sp<SkTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    if (ParseCmapTable(typeface, fontDescriptor) != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "parse cmap failed";
        return FAILED;
    }
    if (ParseNameTable(typeface, fontDescriptor) != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "parse name failed";
        return FAILED;
    }
    if (ParsePostTable(typeface, fontDescriptor) != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "parse post failed";
        return FAILED;
    }

    return SUCCESSED;
}

int FontParser::SetFontDescriptor()
{
    for (unsigned int i = 0; i < fontSet_.size(); ++i) {
        FontParser::FontDescriptor fontDescriptor;
        fontDescriptor.path = fontSet_[i];
        const char* path = fontSet_[i].c_str();
        auto typeface = SkTypeface::MakeFromFile(path);
        if (typeface == nullptr) {
            LOGSO_FUNC_LINE(ERROR) << "typeface is nullptr";
            return FAILED;
        }
        auto fontStyle = typeface->fontStyle();
        fontDescriptor.weight = fontStyle.weight();
        fontDescriptor.width = fontStyle.width();
        if (ParseTable(typeface, fontDescriptor) !=  SUCCESSED) {
            LOGSO_FUNC_LINE(ERROR) << "parse table failed";
            return FAILED;
        }
        visibilityFonts_.emplace_back(fontDescriptor);
    }

    return SUCCESSED;
}

std::vector<FontParser::FontDescriptor> FontParser::GetVisibilityFonts()
{
    if (SetFontDescriptor() != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "set visibility font descriptor failed";
    }

    return visibilityFonts_;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
