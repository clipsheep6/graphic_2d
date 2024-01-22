/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "paragraph_builder_impl.h"

#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "paragraph_impl.h"
#include "txt/paragraph_style.h"

namespace skt = skia::textlayout;

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace {
constexpr size_t axisLen = 4;

SkFontStyle::Weight ConvertToSkFontWeight(FontWeight fontWeight)
{
    constexpr int weightBase = 100;
    return static_cast<SkFontStyle::Weight>(static_cast<int>(fontWeight) * weightBase + weightBase);
}

SkFontStyle MakeSkFontStyle(FontWeight fontWeight, FontStyle fontStyle)
{
    auto weight = ConvertToSkFontWeight(fontWeight);
    auto slant = fontStyle == FontStyle::NORMAL ?
        SkFontStyle::Slant::kUpright_Slant : SkFontStyle::Slant::kItalic_Slant;
    return SkFontStyle(weight, SkFontStyle::Width::kNormal_Width, slant);
}
} // anonymous namespace

ParagraphBuilderImpl::ParagraphBuilderImpl(
    const ParagraphStyle& style, std::shared_ptr<txt::FontCollection> fontCollection)
    : baseStyle_(style.ConvertToTextStyle())
{
    builder_ = skt::ParagraphBuilder::make(TextStyleToSkStyle(style), fontCollection->CreateSktFontCollection());
}

ParagraphBuilderImpl::~ParagraphBuilderImpl() = default;

void ParagraphBuilderImpl::PushStyle(const TextStyle& style)
{
    builder_->pushStyle(TextStyleToSkStyle(style));
    styleStack_.push(style);
}

void ParagraphBuilderImpl::Pop()
{
    builder_->pop();
    styleStack_.pop();
}

const TextStyle& ParagraphBuilderImpl::PeekStyle()
{
    return styleStack_.empty() ? baseStyle_ : styleStack_.top();
}

void ParagraphBuilderImpl::AddText(const std::u16string& text)
{
    builder_->addText(text);
}

void ParagraphBuilderImpl::AddPlaceholder(PlaceholderRun& run)
{
    skt::PlaceholderStyle placeholderStyle;
    placeholderStyle.fHeight = run.height;
    placeholderStyle.fWidth = run.width;
    placeholderStyle.fBaseline = static_cast<skt::TextBaseline>(run.baseline);
    placeholderStyle.fBaselineOffset = run.baselineOffset;
    placeholderStyle.fAlignment = static_cast<skt::PlaceholderAlignment>(run.alignment);

    builder_->addPlaceholder(placeholderStyle);
}

std::unique_ptr<Paragraph> ParagraphBuilderImpl::Build()
{
    return std::make_unique<ParagraphImpl>(builder_->Build(), std::move(paints_));
}

skt::ParagraphPainter::PaintID ParagraphBuilderImpl::AllocPaintID(const PaintRecord& paint)
{
    paints_.push_back(paint);
    return paints_.size() - 1;
}

skt::ParagraphStyle ParagraphBuilderImpl::TextStyleToSkStyle(const ParagraphStyle& txt)
{
    skt::ParagraphStyle skStyle;
    skt::TextStyle textStyle;

    PaintRecord paint;
    paint.SetColor(textStyle.getColor());
    textStyle.setForegroundPaintID(AllocPaintID(paint));

    textStyle.setFontStyle(MakeSkFontStyle(txt.fontWeight, txt.fontStyle));
    textStyle.setFontSize(SkDoubleToScalar(txt.fontSize));
    textStyle.setHeight(SkDoubleToScalar(txt.height));
    textStyle.setHeightOverride(txt.heightOverride);
    textStyle.setFontFamilies({ SkString(txt.fontFamily.c_str()) });
    textStyle.setLocale(SkString(txt.locale.c_str()));
    skStyle.setTextStyle(textStyle);

    skt::StrutStyle strutStyle;
    strutStyle.setFontStyle(MakeSkFontStyle(txt.strutFontWeight, txt.strutFontStyle));
    strutStyle.setFontSize(SkDoubleToScalar(txt.strutFontSize));
    strutStyle.setHeight(SkDoubleToScalar(txt.strutHeight));
    strutStyle.setHeightOverride(txt.strutHeightOverride);

    std::vector<SkString> strutFonts;
    std::transform(txt.strutFontFamilies.begin(), txt.strutFontFamilies.end(), std::back_inserter(strutFonts),
        [](const std::string& f) { return SkString(f.c_str()); });
    strutStyle.setFontFamilies(strutFonts);
    strutStyle.setLeading(txt.strutLeading);
    strutStyle.setForceStrutHeight(txt.forceStrutHeight);
    strutStyle.setStrutEnabled(txt.strutEnabled);
    skStyle.setStrutStyle(strutStyle);

    skStyle.setTextAlign(static_cast<skt::TextAlign>(txt.textAlign));
    skStyle.setTextDirection(static_cast<skt::TextDirection>(txt.textDirection));
    skStyle.setMaxLines(txt.maxLines);
    skStyle.setEllipsis(txt.ellipsis);
    skStyle.setTextHeightBehavior(static_cast<skt::TextHeightBehavior>(txt.textHeightBehavior));

    skStyle.turnHintingOff();
    skStyle.setReplaceTabCharacters(true);

    return skStyle;
}

skt::TextStyle ParagraphBuilderImpl::TextStyleToSkStyle(const TextStyle& txt)
{
    skt::TextStyle skStyle;

    skStyle.setColor(txt.color);
    skStyle.setDecoration(static_cast<skt::TextDecoration>(txt.decoration));
    skStyle.setDecorationColor(txt.decorationColor);
    skStyle.setDecorationStyle(static_cast<skt::TextDecorationStyle>(txt.decorationStyle));
    skStyle.setDecorationThicknessMultiplier(SkDoubleToScalar(txt.decorationThicknessMultiplier));
    skStyle.setFontStyle(MakeSkFontStyle(txt.fontWeight, txt.fontStyle));
    skStyle.setTextBaseline(static_cast<skt::TextBaseline>(txt.baseline));

    std::vector<SkString> fonts;
    std::transform(txt.fontFamilies.begin(), txt.fontFamilies.end(), std::back_inserter(fonts),
        [](const std::string& f) { return SkString(f.c_str()); });
    skStyle.setFontFamilies(fonts);

    skStyle.setFontSize(SkDoubleToScalar(txt.fontSize));
    skStyle.setLetterSpacing(SkDoubleToScalar(txt.letterSpacing));
    skStyle.setWordSpacing(SkDoubleToScalar(txt.wordSpacing));
    skStyle.setHeight(SkDoubleToScalar(txt.height));
    skStyle.setHeightOverride(txt.heightOverride);
    skStyle.setHalfLeading(txt.halfLeading);

    skStyle.setLocale(SkString(txt.locale.c_str()));
    if (txt.background.has_value()) {
        skStyle.setBackgroundPaintID(AllocPaintID(txt.background.value()));
    }
    if (txt.foreground.has_value()) {
        skStyle.setForegroundPaintID(AllocPaintID(txt.foreground.value()));
    } else {
        PaintRecord paint;
        paint.SetColor(txt.color);
        paint.isSymbolGlyph = txt.isSymbolGlyph;
        paint.symbol.SetRenderColor(txt.symbol.GetRenderColor());
        paint.symbol.SetRenderMode(txt.symbol.GetRenderMode());
        paint.symbol.SetSymbolEffect(txt.symbol.GetEffectStrategy());
        skStyle.setForegroundPaintID(AllocPaintID(paint));
    }

    skStyle.resetFontFeatures();
    for (const auto& ff : txt.fontFeatures.GetFontFeatures()) {
        skStyle.addFontFeature(SkString(ff.first.c_str()), ff.second);
    }

    if (!txt.fontVariations.GetAxisValues().empty()) {
        std::vector<SkFontArguments::VariationPosition::Coordinate> coordinates;
        for (const auto& [axis, value] : txt.fontVariations.GetAxisValues()) {
            if (axis.length() == axisLen) {
                coordinates.push_back({
                    SkSetFourByteTag(axis[0], axis[1], axis[2], axis[3]),
                    value,
                });
            }
        }
        SkFontArguments::VariationPosition position = { coordinates.data(), static_cast<int>(coordinates.size()) };
        skStyle.setFontArguments(SkFontArguments().setVariationDesignPosition(position));
    }

    skStyle.resetShadows();
    for (const TextShadow& txtShadow : txt.textShadows) {
        skt::TextShadow shadow;
        shadow.fOffset = txtShadow.offset;
        shadow.fBlurSigma = txtShadow.blurSigma;
        shadow.fColor = txtShadow.color;
        skStyle.addShadow(shadow);
    }

    return skStyle;
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
