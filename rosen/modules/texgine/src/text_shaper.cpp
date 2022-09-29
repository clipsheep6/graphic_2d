/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "text_shaper.h"

#include "measurer.h"
#include "texgine_exception.h"
#include "texgine/typography_types.h"
#include "texgine/utils/exlog.h"
#include "texgine/utils/trace.h"

namespace Texgine {
int TextShaper::Shape(const VariantSpan &span,
                      const TypographyStyle &ys,
                      const std::unique_ptr<FontProviders> &fontProviders)
{
    ScopedTrace scope("TextShaper::ShapeLineSpans");
    LOGSCOPED(sl, LOG2EX_DEBUG(), "TextShaper::ShapeLineSpans");
    if (span == nullptr) {
        LOG2EX(ERROR) << "span is nullptr";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (span.TryToAnySpan() != nullptr) {
        return 0;
    }

    auto xs = span.GetTextStyle();
    auto ts = span.TryToTextSpan();
    auto ret = DoShape(ts, xs, ys, fontProviders);
    if (ret) {
        LOG2EX(ERROR) << "DoShape is failed";
        return 1;
    }

    if (!ts->cgs_.IsValid()) {
        LOG2EX(ERROR) << "cgs is inValid";
        throw TEXGINE_EXCEPTION(InvalidCharGroups);
    }

    if (ts->cgs_.GetSize() <= 0) {
        LOG2EX(ERROR) << "cgs have no cg";
        return 1;
    }

    if (ts->cgs_.Get(0).typeface_ == nullptr) {
        LOG2EX(ERROR) << "first cgs have no typeface";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    SkFont font;
    font.setTypeface(ts->cgs_.Get(0).typeface_->Get());
    font.setSize(xs.fontSize_);
    font.getMetrics(&ts->smetrics_);

    auto blob = GenerateTextBlob(font, ts->cgs_, ts->width_, ts->glyphWidths_);
    if (blob == nullptr) {
        LOG2EX(ERROR) << "Generate text blob is failed";
        return 1;
    }

    ts->textBlob_ = blob;
    return 0;
}

int TextShaper::DoShape(std::shared_ptr<TextSpan> &span,
                        const TextStyle &xs,
                        const TypographyStyle &ys,
                        const std::unique_ptr<FontProviders> &fontProviders)
{
    if (fontProviders == nullptr || span == nullptr) {
        LOG2EX(ERROR) << "providers or span is nullptr";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    auto families = xs.fontFamilies_;
    if (families.empty()) {
        families = ys.fontFamilies_;
    }

    auto fontCollection = fontProviders->GenerateFontCollection(families);
    if (fontCollection == nullptr) {
        LOG2EX(ERROR) << "fontCollection is nullptr";
        return 1;
    }

    span->u16vect_ = span->cgs_.ToUTF16();
    auto measurer = Measurer::Create(span->u16vect_, *fontCollection);
    measurer->SetLocale(xs.locale_);
    measurer->SetRTL(span->rtl_);
    FontStyles style(xs.fontWeight_, xs.fontStyle_);
    measurer->SetFontStyle(style);
    measurer->SetSize(xs.fontSize_);
    measurer->SetFontFeatures(xs.fontFeature_);
    measurer->SetRange(0, span->u16vect_.size());
    measurer->SetSpacing(xs.letterSpacing_, xs.wordSpacing_);
    if (measurer->Measure(span->cgs_)) {
        LOG2EX(ERROR) << "Measurer::Measure failed";
        return 1;
    }

    return 0;
}

sk_sp<SkTextBlob> TextShaper::GenerateTextBlob(const SkFont &font, const CharGroups &cgs,
    double &spanWidth, std::vector<double> &glyphWidths)
{
    SkTextBlobBuilder builder;
    auto blob = builder.allocRunPos(font, cgs.GetNumberOfGlyph());
    if (blob.glyphs == nullptr || blob.pos == nullptr) {
        LOG2EX(ERROR) << "allocRunPos return unavailable buffer";
        throw TEXGINE_EXCEPTION(APIFailed);
    }

    auto offset = 0.0;
    auto index = 0;
    for (const auto &cg : cgs) {
        glyphWidths.push_back(cg.GetWidth());
        auto drawingOffset = offset;
        offset += cg.GetWidth();
        for (const auto &[cp, ax, ay, ox, oy] : cg.glyphs_) {
            blob.glyphs[index] = cp;
            blob.pos[index * 2 + 0] = drawingOffset + ox;
            blob.pos[index * 2 + 1] = ay - oy;
            index++;
            drawingOffset += ax;
        }
    }

    spanWidth = offset;
    return builder.make();
}
} // namespace Texgine
