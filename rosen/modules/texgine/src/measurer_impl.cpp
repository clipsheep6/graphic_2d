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

#include "measurer_impl.h"

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#include "texgine/utils/trace.h"
#include "text_converter.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define INVALID_TEXT_LENGTH (-1)

namespace {
void DumpCharGroup(int32_t index, const CharGroup &cg, double glyphEm,
    hb_glyph_info_t &info, hb_glyph_position_t &position)
{
    auto uTF8Str = TextConverter::ToUTF8(cg.chars_);
    // 0xffffff is the default char when the cg is null or invalid
    auto ch = (cg.chars_.size() > 0) ? cg.chars_[0] : 0xffffff;
    // the follow numbers is to align log
    LOG2EX_DEBUG() << std::setw(2) << std::setfill('0') << index <<
        std::hex << std::uppercase <<
        ": " << std::right << std::setw(4) << std::setfill(' ') << "\033[40m" << "'" <<
        uTF8Str.data() << "'" << "\033[0m" <<
        " (char: 0x" << std::setw(6) << std::setfill('0') << ch <<
        ", codepoint: 0x" << std::setw(4) << std::setfill('0') << info.codepoint <<
        ", cluster: " << std::setw(2) << std::setfill(' ') << std::dec << info.cluster << ")" <<
        " {" << glyphEm * position.x_advance << ", " << glyphEm * position.y_advance <<
        ", " << glyphEm * position.x_offset << ", " << glyphEm * position.y_offset << "}";
}
} // namespace

hb_blob_t *HbFaceReferenceTableTypeface(hb_face_t *face, hb_tag_t tag, void *context)
{
    if (context == nullptr) {
        return nullptr;
    }

    std::shared_ptr<TexgineTypeface> typeface = std::make_shared<TexgineTypeface>(context);
    if (typeface->GetTypeface() == nullptr) {
        return nullptr;
    }

    // 584420 is five times of the tag`s max size now
    const size_t maxSize = 584420;
    const size_t tableSize = typeface->GetTableSize(tag);
    if (tableSize == 0 || tableSize > maxSize) {
        return nullptr;
    }

    void *buffer = malloc(tableSize);
    if (buffer == nullptr) {
        return nullptr;
    }

    size_t actualSize = typeface->GetTableData(tag, 0, tableSize, buffer);
    if (tableSize != actualSize) {
        free(buffer);
        buffer = nullptr;
        return nullptr;
    }

    return hb_blob_create(reinterpret_cast<char *>(buffer),
                          tableSize, HB_MEMORY_MODE_WRITABLE, buffer, free);
}

std::unique_ptr<Measurer> Measurer::Create(const std::vector<uint16_t> &text, const FontCollection &fontCollection)
{
    return std::make_unique<MeasurerImpl>(text, fontCollection);
}

MeasurerImpl::MeasurerImpl(const std::vector<uint16_t> &text, const FontCollection &fontCollection)
    : Measurer(text, fontCollection)
{
}

const std::vector<Boundary> &MeasurerImpl::GetWordBoundary() const
{
    return boundaries_;
}

int MeasurerImpl::Measure(CharGroups &cgs)
{
    ScopedTrace scope("MeasurerImpl::Measure");
    LOGSCOPED(sl, LOG2EX_DEBUG(), "MeasurerImpl::Measure");
    struct MeasurerCacheKey key = {
        .text = text_,
        .style = style_,
        .locale = locale_,
        .rtl = rtl_,
        .size = size_,
        .startIndex = startIndex_,
        .endIndex = endIndex_,
        .letterSpacing = letterSpacing_,
        .wordSpacing = wordSpacing_,
    };

    if (fontFeatures_ == nullptr || fontFeatures_->GetFeatures().size() == 0) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            cgs = it->second.cgs_.Clone();
            boundaries_ = it->second.boundaries_;
            return 0;
        }
    }

    WordBreaker wb;
    wb.SetLocale(icu::Locale::createFromName(locale_.c_str()));
    wb.SetRange(0, text_.size());
    boundaries_ = wb.GetBoundary(text_);

    std::list<struct MeasuringRun> runs;
    SeekScript(runs);
    SeekTypeface(runs);
    if (auto ret = Shape(cgs, runs, boundaries_); ret) {
        LOG2EX(ERROR) << "Shape failed";
        return ret;
    }

    if (fontFeatures_ == nullptr || fontFeatures_->GetFeatures().size() == 0) {
        cache_[key] = {cgs.Clone(), boundaries_};
    }
    return 0;
}

void MeasurerImpl::SeekTypeface(std::list<struct MeasuringRun> &runs)
{
    ScopedTrace scope("MeasurerImpl::SeekTypeface");
    LOGSCOPED(sl, LOG2EX_DEBUG(), "typeface");
    int index = 0;
    for (auto runsit = runs.begin(); runsit != runs.end(); runsit++) {
        if (runsit->end > text_.size()) {
            LOG2EX(ERROR) << "runsit->end overflow of text_";
            throw TEXGINE_EXCEPTION(ErrorStatus);
        }
        size_t utf16Index = runsit->start;
        uint32_t cp = 0;
        std::shared_ptr<Typeface> lastTypeface = nullptr;
        while (utf16Index < runsit->end) {
            U16_NEXT(text_.data(), utf16Index, runsit->end, cp);
            LOG2EX_DEBUG(Logger::NoReturn) << "[" << std::setw(2) << std::setfill('0') << index++
                << ": 0x" << std::setw(6) << std::setfill('0') << std::hex << std::uppercase << cp << "]";
            if (lastTypeface && lastTypeface->Has(cp)) {
                LOGCEX_DEBUG() << " cached";
                continue;
            }
            if (runsit->typeface) {
                index--;
                LOGCEX_DEBUG() << " new";
                auto next = runsit;
                runs.insert(++next, {
                    .start = utf16Index - U16_LENGTH(cp),
                    .end = runsit->end,
                    .script = runsit->script,
                });
                runsit->end = utf16Index - U16_LENGTH(cp);
                break;
            }

            const int firstByte = 24;
            const int secondByte = 16;
            const int thirdByte = 8;
            char runScript[5] = {(char)(((runsit->script) >> firstByte) & 0xFF),
                                 (char)(((runsit->script) >> secondByte) & 0xFF),
                                 (char)(((runsit->script) >> thirdByte) & 0xFF),
                                 (char)((runsit->script) & 0xFF), '\0'};
            lastTypeface = fontCollection_.GetTypefaceForChar(cp, style_, runScript, locale_);
            if (lastTypeface == nullptr) {
                LOGCEX_DEBUG() << " no typeface";
                continue;
            }

            LOGCEX_DEBUG() << " found at " << lastTypeface->GetName();
            runsit->typeface = lastTypeface;
        }
    }
}

void MeasurerImpl::SeekScript(std::list<struct MeasuringRun> &runs)
{
    ScopedTrace scope("MeasurerImpl::SeekScript");
    LOGSCOPED(sl, LOG2EX_DEBUG(), "script");
    auto icuGetUnicodeFuncs = hb_unicode_funcs_create(hb_icu_get_unicode_funcs());
    if (icuGetUnicodeFuncs == nullptr) {
        LOG2EX(ERROR) << "hb_unicode_funcs_create return nullptr";
        throw TEXGINE_EXCEPTION(APIFailed);
    }

    runs.push_back({.start = startIndex_, .end = endIndex_});
    DoSeekScript(runs, icuGetUnicodeFuncs);
    hb_unicode_funcs_destroy(icuGetUnicodeFuncs);
}

void MeasurerImpl::DoSeekScript(std::list<struct MeasuringRun> &runs, hb_unicode_funcs_t *icuGetUnicodeFuncs)
{
    int index = 0;
    for (auto it = runs.begin(); it != runs.end(); it++) {
        std::stringstream ss;
        ss << "[" << it->start << ", " << it->end << ")";
        LOGSCOPED(sl, LOG2EX_DEBUG(), ss.str());

        size_t utf16Index = it->start;
        uint32_t cp = 0;
        auto &script = it->script;
        while (utf16Index < it->end) {
            if (utf16Index >= text_.size()) {
                LOG2EX(ERROR) << "u16index overflow of text_";
                throw TEXGINE_EXCEPTION(ErrorStatus);
            }

            U16_NEXT(text_.data(), utf16Index, it->end, cp);
            auto s = hb_unicode_script(icuGetUnicodeFuncs, cp);
            LOG2EX_DEBUG() << "[" << std::setw(2) << std::setfill('0') << index++ << ": 0x"
                << std::setw(6) << std::setfill('0') << std::hex << std::uppercase << cp << "]" << " " << s;
            if (script == HB_SCRIPT_INVALID) {
                script = s;
            }

            if (s == script) {
                continue;
            }

            if (script == HB_SCRIPT_INHERITED || script == HB_SCRIPT_COMMON) {
                script = s;
            } else if (s == HB_SCRIPT_INHERITED || s == HB_SCRIPT_COMMON) {
                continue;
            } else {
                index--;
                auto next = it;
                runs.insert(++next, { .start = utf16Index - U16_LENGTH(cp), .end = it->end });
                it->end = utf16Index - U16_LENGTH(cp);
                break;
            }
        }

        if (it->end <= it->start) {
            LOG2EX(ERROR) << "run have error range";
            throw TEXGINE_EXCEPTION(ErrorStatus);
        }

        it->script = script;
    }
}

int MeasurerImpl::Shape(CharGroups &cgs, std::list<struct MeasuringRun> &runs, std::vector<Boundary> boundaries)
{
    ScopedTrace scope("MeasurerImpl::Shape");
    cgs = CharGroups::CreateEmpty();
    LOGSCOPED(sl, LOG2EX_DEBUG(), "shape");
    size_t index = 0;
    for (auto &run : runs) {
        if (run.end <= run.start) {
            LOG2EX(ERROR) << "run have error range";
            throw TEXGINE_EXCEPTION(ErrorStatus);
        }

        std::stringstream ss;
        ss << "[" << run.start << ", " << run.end << ")";
        LOGSCOPED(sl, LOG2EX_DEBUG(), ss.str());

        if (DoShape(cgs, run, index)) {
            return 1;
        }
    }

    for (auto &[start, end] : boundaries) {
        const auto &wordcgs = cgs.GetSubFromU16RangeAll(start, end);
        wordcgs.Get(wordcgs.GetNumberOfCharGroup() - 1).invisibleWidth_ += wordSpacing_;
    }
    return 0;
}

int MeasurerImpl::DoShape(CharGroups &cgs, MeasuringRun &run, int &index)
{
    auto typeface = run.typeface;
    if (typeface == nullptr) {
        LOG2EX(ERROR) << "there is no typeface";
        return 1;
    }

    auto hbuffer = hb_buffer_create();
    if (!hbuffer) {
        LOG2EX(ERROR) << "hbuffer is nullptr";
        return 1;
    }

    hb_buffer_add_utf16(hbuffer, reinterpret_cast<const uint16_t *>(text_.data()),
        INVALID_TEXT_LENGTH, run.start, run.end - run.start);
    hb_buffer_set_direction(hbuffer, rtl_ ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
    hb_buffer_set_unicode_funcs(hbuffer, hb_unicode_funcs_create(hb_icu_get_unicode_funcs()));
    hb_buffer_set_script(hbuffer, run.script);
    hb_buffer_set_language(hbuffer, hb_language_from_string(locale_.c_str(), INVALID_TEXT_LENGTH));

    auto hface = hb_face_create_for_tables(HbFaceReferenceTableTypeface, typeface->Get()->GetTypeface().get(), 0);
    if (!hface) {
        LOG2EX(ERROR) << "hface is nullptr";
        return 1;
    }

    auto hfont = hb_font_create(hface);
    if (!hfont) {
        LOG2EX(ERROR) << "hfont is nullptr";
        return 1;
    }

    std::vector<hb_feature_t> ff;
    GenerateHBFeatures(ff, fontFeatures_);
    hb_shape(hfont, hbuffer, ff.data(), ff.size());

    if (GetGlyphs(cgs, run, index, hbuffer, typeface)) {
        return 1;
    }

    hb_buffer_destroy(hbuffer);
    hb_font_destroy(hfont);
    hb_face_destroy(hface);
    return 0;
}

int MeasurerImpl::GetGlyphs(CharGroups &cgs, MeasuringRun &run, int &index, hb_buffer_t *hbuffer,
    std::shared_ptr<Texgine::Typeface> typeface)
{
    uint32_t ng = 0u;
    auto hginfos = hb_buffer_get_glyph_infos(hbuffer, &ng);
    if (!hginfos) {
        LOG2EX(ERROR) << "hginfos is nullptr";
        return 1;
    }

    auto hgpositions = hb_buffer_get_glyph_positions(hbuffer, nullptr);
    if (!hgpositions) {
        LOG2EX(ERROR) << "hgpositions is nullptr";
        return 1;
    }

    LOG2EX_DEBUG() << "ng: " << ng;
    auto upe = typeface->Get()->GetUnitsPerEm();
    if (!upe) {
        LOG2EX(ERROR) << "upe is 0";
        return 1;
    }

    auto glyphEm = 1.0 * size_ / upe;
    std::map<uint32_t, CharGroup> cgsByCluster{{run.end, {}}};
    for (uint32_t i = 0; i < ng; i++) {
        auto &cg = cgsByCluster[hginfos[i].cluster];
        cg.glyphs_.push_back({
            .codepoint_ = hginfos[i].codepoint,
            .advanceX_ = glyphEm * hgpositions[i].x_advance,
            .advanceY_ = glyphEm * hgpositions[i].y_advance,
            .offsetX_ = glyphEm * hgpositions[i].x_offset,
            .offsetY_ = glyphEm * hgpositions[i].y_offset,
        });
        cg.typeface_ = typeface;
    }

    DoCgsByCluster(cgsByCluster);
    cgsByCluster.erase(run.end);
    for (uint32_t i = 0; i < ng; i++) {
        DumpCharGroup(index++, cgsByCluster[hginfos[i].cluster], glyphEm, hginfos[i], hgpositions[i]);
    }

    for (const auto &[cluster, cg] : cgsByCluster) {
        cgs.PushBack(cg);
    }

    if (rtl_) {
        cgs.ReverseAll();
    }
    return 0;
}

void MeasurerImpl::DoCgsByCluster(std::map<uint32_t, Texgine::CharGroup> &cgsByCluster)
{
    for (auto it = cgsByCluster.begin(); it != cgsByCluster.end(); it++) {
        auto start = (it++)->first;
        if (it == cgsByCluster.end()) {
            break;
        }

        auto end = (it--)->first;
        if (start > text_.size() || end > text_.size()) {
            LOG2EX(ERROR) << "cgsByCluster is not align with text_";
            throw TEXGINE_EXCEPTION(ErrorStatus);
        }

        it->second.chars_.insert(it->second.chars_.end(), text_.begin() + start, text_.begin() + end);
        it->second.visibleWidth_ = 0;
        for (const auto &glyph : it->second.glyphs_) {
            it->second.visibleWidth_ += glyph.advanceX_;
        }

        it->second.invisibleWidth_ = letterSpacing_;
        if (u_isWhitespace(it->second.chars_[0]) != 0) {
            it->second.invisibleWidth_ += it->second.visibleWidth_;
            it->second.visibleWidth_ = 0;
        }
    }
}

void MeasurerImpl::GenerateHBFeatures(std::vector<hb_feature_t> &fontFeatures, const FontFeatures *ff)
{
    LOGSCOPED(sl, LOG2EX_DEBUG(), "GenerateHBFeatures");
    if (ff == nullptr) {
        return;
    }

    auto features = ff->GetFeatures();
    if (features.empty()) {
        return;
    }

    for (auto &[ft, fv] : features) {
        hb_feature_t hf;
        if (hb_feature_from_string(ft.c_str(), ft.size(), &hf)) {
            hf.value = fv;
            fontFeatures.push_back(hf);
        }

        LOG2EX_DEBUG() << "feature tag:" << hf.tag << ", feature value:" << hf.value;
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
