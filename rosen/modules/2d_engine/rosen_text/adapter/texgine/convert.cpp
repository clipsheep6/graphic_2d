/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "convert.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTextEngine {
std::shared_ptr<OHOS::Rosen::AdapterTextEngine::FontCollection> Convert(
    const std::shared_ptr<OHOS::Rosen::FontCollection> &fontCollection)
{
    return std::static_pointer_cast<OHOS::Rosen::AdapterTextEngine::FontCollection>(fontCollection);
}

TextEngine::TypographyStyle Convert(const TypographyStyle &style)
{
    TextEngine::TypographyStyle ys = {
#ifndef USE_GRAPHIC_TEXT_GINE
        .fontWeight_ = Convert(style.fontWeight_),
        .fontStyle_ = Convert(style.fontStyle_),
        .fontFamilies_ = { style.fontFamily_ },
        .fontSize_ = style.fontSize_,
        .heightScale_ = style.heightScale_,
        .halfLeading_ = style.halfLeading_,
        .heightOnly_ = style.heightOnly_,
        .locale_ = style.locale_,
        .maxLines_ = style.maxLines_,
        .ellipsis_ = style.ellipsis_,
        .breakStrategy_ = Convert(style.breakStrategy_),
        .wordBreakType_ = Convert(style.wordBreakType_),
        .align_ = Convert(style.textAlign_),
        .direction_ = Convert(style.textDirection_),
        .ellipsisModal_ = Convert(style.ellipsisModal_),
        .useLineStyle_ = style.useLineStyle_,
        .lineStyle_ = {
            .only_ = style.lineStyleOnly_,
            .fontWeight_ = Convert(style.lineStyleFontWeight_),
            .fontStyle_ = Convert(style.lineStyleFontStyle_),
            .fontFamilies_ = style.lineStyleFontFamilies_,
            .halfLeading_ = style.lineStyleHalfLeading_,
            .heightOnly_ = style.lineStyleHeightOnly_,
            .fontSize_ = style.lineStyleFontSize_,
            .heightScale_ = style.lineStyleHeightScale_,
#else
        .fontWeight = Convert(style.fontWeight),
        .fontStyle = Convert(style.fontStyle),
        .fontFamilies = { style.fontFamily },
        .fontSize = style.fontSize,
        .heightScale = style.heightScale,
        .halfLeading = style.halfLeading,
        .heightOnly = style.heightOnly,
        .locale = style.locale,
        .maxLines = style.maxLines,
        .ellipsis = style.ellipsis,
        .breakStrategy = Convert(style.breakStrategy),
        .wordBreakType = Convert(style.wordBreakType),
        .align = Convert(style.textAlign),
        .direction = Convert(style.textDirection),
        .ellipsisModal = Convert(style.ellipsisModal),
        .useLineStyle = style.useLineStyle,
        .lineStyle = {
            .only = style.lineStyleOnly,
            .fontWeight = Convert(style.lineStyleFontWeight),
            .fontStyle = Convert(style.lineStyleFontStyle),
            .fontFamilies = style.lineStyleFontFamilies,
            .halfLeading = style.lineStyleHalfLeading,
            .heightOnly = style.lineStyleHeightOnly,
            .fontSize = style.lineStyleFontSize,
            .heightScale = style.lineStyleHeightScale,
#endif
        },
    };
#ifndef USE_GRAPHIC_TEXT_GINE
    if (style.lineStyleSpacingScale_ >= 0) {
        ys.lineStyle_.spacingScale_ = style.lineStyleSpacingScale_;
#else
    if (style.lineStyleSpacingScale >= 0) {
        ys.lineStyle.spacingScale = style.lineStyleSpacingScale;
#endif
    }
    return ys;
}

TextEngine::TextStyle Convert(const TextStyle &style)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    auto color = SkColorSetARGB(style.color_.GetAlpha(),
                                style.color_.GetRed(),
                                style.color_.GetGreen(),
                                style.color_.GetBlue());
    auto decorationColor = SkColorSetARGB(style.decorationColor_.GetAlpha(),
                                          style.decorationColor_.GetRed(),
                                          style.decorationColor_.GetGreen(),
                                          style.decorationColor_.GetBlue());
#else
    auto color = SkColorSetARGB(
        style.color.GetAlpha(), style.color.GetRed(), style.color.GetGreen(), style.color.GetBlue());
    auto decorationColor = SkColorSetARGB(style.decorationColor.GetAlpha(),
        style.decorationColor.GetRed(), style.decorationColor.GetGreen(), style.decorationColor.GetBlue());
#endif
    std::optional<TextEngine::TexginePaint> foreground = std::nullopt;
#ifndef USE_GRAPHIC_TEXT_GINE
    if (style.foreground.has_value()) {
        foreground = TextEngine::TexginePaint();
        foreground.value().SetPaint(style.foreground.value());
    }
#else
    if (style.foreground.has_value()) {
        foreground = TextEngine::TexginePaint();
        foreground.value().SetPaint(style.foreground.value());
    }
#endif
    std::optional<TextEngine::TexginePaint> background = std::nullopt;
#ifndef USE_GRAPHIC_TEXT_GINE
    if (style.background.has_value()) {
        background = TextEngine::TexginePaint();
        background.value().SetPaint(style.background_.value());
    }
#else
    if (style.background.has_value()) {
        background = TextEngine::TexginePaint();
        background.value().SetPaint(style.background.value());
    }
#endif
    TextEngine::TextStyle xs = {
#ifndef USE_GRAPHIC_TEXT_GINE
        .fontWeight_ = Convert(style.fontWeight_),
        .fontStyle_ = Convert(style.fontStyle_),
        .fontFamilies_ = style.fontFamilies_,
        .fontSize_ = style.fontSize_,
        .decoration_ = Convert(style.decoration_),
        .decorationColor_ = decorationColor,
        .decorationStyle_ = Convert(style.decorationStyle_),
        .decorationThicknessScale_ = style.decorationThicknessScale_,
        .color_ = color,
        .baseline_ = Convert(style.baseline_),
        .locale_ = style.locale_,
        .halfLeading_ = style.halfLeading_,
        .heightOnly_ = style.heightOnly_,
        .heightScale_ = style.heightScale_,
        .letterSpacing_ = style.letterSpacing_,
        .wordSpacing_ = style.wordSpacing_,
        .foreground_ = foreground,
        .background_ = background,
        .isSymbolGlyph = style.isSymbolGlyph,
#else
        .fontWeight = Convert(style.fontWeight),
        .fontStyle = Convert(style.fontStyle),
        .fontFamilies = style.fontFamilies,
        .fontSize = style.fontSize,
        .decoration = Convert(style.decoration),
        .decorationColor = decorationColor,
        .decorationStyle = Convert(style.decorationStyle),
        .decorationThicknessScale = style.decorationThicknessScale,
        .color = color,
        .baseline = Convert(style.baseline),
        .locale = style.locale,
        .halfLeading = style.halfLeading,
        .heightOnly = style.heightOnly,
        .heightScale = style.heightScale,
        .letterSpacing = style.letterSpacing,
        .wordSpacing = style.wordSpacing,
        .foreground = foreground,
        .background = background,
        .isSymbolGlyph = style.isSymbolGlyph,
#endif
    };

    xs.symbol.colorList_ = style.symbol.GetRenderColor();
    xs.symbol.renderMode_ = style.symbol.GetRenderMode();
    xs.symbol.effectStratey_ = style.symbol.GetEffectStrategy();

#ifndef USE_GRAPHIC_TEXT_GINE
    for (const auto &[tag, value] : style.fontFeatures_.GetFontFeatures()) {
        xs.fontFeature_.SetFeature(tag, value);
#else
    for (const auto &[tag, value] : style.fontFeatures.GetFontFeatures()) {
        xs.fontFeature.SetFeature(tag, value);
#endif
    }

#ifndef USE_GRAPHIC_TEXT_GINE
    for (const auto &[color, offset, radius] : style.shadows_) {
#else
    for (const auto &[color, offset, radius] : style.shadows) {
#endif
        // 24, 16, 8, 0: How many bits are moved to the right
        auto shadowColor = (color.GetAlpha() << 24) | (color.GetRed() << 16) |
            (color.GetGreen() << 8) | (color.GetBlue() << 0);
        TextEngine::TextShadow shadow = {
#ifndef USE_GRAPHIC_TEXT_GINE
            .offsetX_ = offset.GetX(),
            .offsetY_ = offset.GetY(),
            .color_ = shadowColor,
            .blurLeave_ = radius,
#else
            .offsetX = offset.GetX(),
            .offsetY = offset.GetY(),
            .color = shadowColor,
            .blurLeave = radius,
#endif
        };
#ifndef USE_GRAPHIC_TEXT_GINE
        xs.shadows_.emplace_back(shadow);
#else
        xs.shadows.emplace_back(shadow);
#endif
    }
    return xs;
}

IndexAndAffinity Convert(const TextEngine::IndexAndAffinity &pos)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return { pos.index_, Convert(pos.affinity_) };
#else
    return { pos.index, Convert(pos.affinity) };
#endif
}

Boundary Convert(const TextEngine::Boundary &range)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return { range.leftIndex_, range.rightIndex_ };
#else
    return { range.leftIndex, range.rightIndex };
#endif
}

TextRect Convert(const TextEngine::TextRect &box)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    Drawing::RectF rect(*box.rect_.fLeft_, *box.rect_.fTop_, *box.rect_.fRight_, *box.rect_.fBottom_);
    return { rect, Convert(box.direction_) };
#else
    Drawing::RectF rect(*box.rect.fLeft_, *box.rect.fTop_, *box.rect.fRight_, *box.rect.fBottom_);
    return { rect, Convert(box.direction) };
#endif
}

Affinity Convert(const TextEngine::Affinity &affinity)
{
    switch (affinity) {
        case TextEngine::Affinity::PREV:
            return Affinity::PREV;
        case TextEngine::Affinity::NEXT:
            return Affinity::NEXT;
    }
    return Affinity::PREV;
}

TextDirection Convert(const TextEngine::TextDirection &direction)
{
    switch (direction) {
        case TextEngine::TextDirection::LTR:
            return TextDirection::LTR;
        case TextEngine::TextDirection::RTL:
            return TextDirection::RTL;
    }
    return TextDirection::LTR;
}

TextEngine::TextRectHeightStyle Convert(const TextRectHeightStyle &style)
{
    switch (style) {
        case TextRectHeightStyle::TIGHT:
            return TextEngine::TextRectHeightStyle::TIGHT;
        case TextRectHeightStyle::COVER_TOP_AND_BOTTOM:
            return TextEngine::TextRectHeightStyle::COVER_TOP_AND_BOTTOM;
        case TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM:
            return TextEngine::TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM;
        case TextRectHeightStyle::COVER_TOP:
            return TextEngine::TextRectHeightStyle::COVER_TOP;
        case TextRectHeightStyle::COVER_BOTTOM:
            return TextEngine::TextRectHeightStyle::COVER_BOTTOM;
        case TextRectHeightStyle::FOLLOW_BY_STRUT:
            return TextEngine::TextRectHeightStyle::FOLLOW_BY_LINE_STYLE;
    }
    return TextEngine::TextRectHeightStyle::TIGHT;
}

TextEngine::TextRectWidthStyle Convert(const TextRectWidthStyle &style)
{
    switch (style) {
        case TextRectWidthStyle::TIGHT:
            return TextEngine::TextRectWidthStyle::TIGHT;
        case TextRectWidthStyle::MAX:
            return TextEngine::TextRectWidthStyle::MAX_WIDTH;
    }
    return TextEngine::TextRectWidthStyle::TIGHT;
}

TextEngine::WordBreakType Convert(const WordBreakType &style)
{
    switch (style) {
        case WordBreakType::NORMAL:
            return TextEngine::WordBreakType::NORMAL;
        case WordBreakType::BREAK_ALL:
            return TextEngine::WordBreakType::BREAK_ALL;
        case WordBreakType::BREAK_WORD:
            return TextEngine::WordBreakType::BREAK_WORD;
    }
    return TextEngine::WordBreakType::NORMAL;
}

TextEngine::BreakStrategy Convert(const BreakStrategy &style)
{
    switch (style) {
        case BreakStrategy::GREEDY:
            return TextEngine::BreakStrategy::GREEDY;
        case BreakStrategy::HIGH_QUALITY:
            return TextEngine::BreakStrategy::HIGH_QUALITY;
        case BreakStrategy::BALANCED:
            return TextEngine::BreakStrategy::BALANCED;
    }
    return TextEngine::BreakStrategy::GREEDY;
}

TextEngine::FontWeight Convert(const FontWeight &style)
{
    switch (style) {
        case FontWeight::W100:
            return TextEngine::FontWeight::W100;
        case FontWeight::W200:
            return TextEngine::FontWeight::W200;
        case FontWeight::W300:
            return TextEngine::FontWeight::W300;
        case FontWeight::W400:
            return TextEngine::FontWeight::W400;
        case FontWeight::W500:
            return TextEngine::FontWeight::W500;
        case FontWeight::W600:
            return TextEngine::FontWeight::W600;
        case FontWeight::W700:
            return TextEngine::FontWeight::W700;
        case FontWeight::W800:
            return TextEngine::FontWeight::W800;
        case FontWeight::W900:
            return TextEngine::FontWeight::W900;
    }
    return TextEngine::FontWeight::W400;
}

TextEngine::FontStyle Convert(const FontStyle &style)
{
    switch (style) {
        case FontStyle::NORMAL:
            return TextEngine::FontStyle::NORMAL;
        case FontStyle::ITALIC:
            return TextEngine::FontStyle::ITALIC;
    }
    return TextEngine::FontStyle::NORMAL;
}

TextEngine::TextAlign Convert(const TextAlign &align)
{
    switch (align) {
        case TextAlign::LEFT:
            return TextEngine::TextAlign::LEFT;
        case TextAlign::RIGHT:
            return TextEngine::TextAlign::RIGHT;
        case TextAlign::CENTER:
            return TextEngine::TextAlign::CENTER;
        case TextAlign::JUSTIFY:
            return TextEngine::TextAlign::JUSTIFY;
        case TextAlign::START:
            return TextEngine::TextAlign::START;
        case TextAlign::END:
            return TextEngine::TextAlign::END;
    }
    return TextEngine::TextAlign::START;
}

TextEngine::TextBaseline Convert(const TextBaseline &baseline)
{
    switch (baseline) {
        case TextBaseline::ALPHABETIC:
            return TextEngine::TextBaseline::ALPHABETIC;
        case TextBaseline::IDEOGRAPHIC:
            return TextEngine::TextBaseline::IDEOGRAPHIC;
    }
    return TextEngine::TextBaseline::ALPHABETIC;
}

TextEngine::TextDirection Convert(const TextDirection &direction)
{
    switch (direction) {
        case TextDirection::LTR:
            return TextEngine::TextDirection::LTR;
        case TextDirection::RTL:
            return TextEngine::TextDirection::RTL;
    }
    return TextEngine::TextDirection::LTR;
}

TextEngine::TextDecorationStyle Convert(const TextDecorationStyle &style)
{
    switch (style) {
        case TextDecorationStyle::SOLID:
            return TextEngine::TextDecorationStyle::SOLID;
        case TextDecorationStyle::DOUBLE:
            return TextEngine::TextDecorationStyle::DOUBLE;
        case TextDecorationStyle::DOTTED:
            return TextEngine::TextDecorationStyle::DOTTED;
        case TextDecorationStyle::DASHED:
            return TextEngine::TextDecorationStyle::DASHED;
        case TextDecorationStyle::WAVY:
            return TextEngine::TextDecorationStyle::WAVY;
    }
    return TextEngine::TextDecorationStyle::SOLID;
}

TextEngine::TextDecoration Convert(const TextDecoration &decoration)
{
    TextEngine::TextDecoration retval = TextEngine::TextDecoration::NONE;
    if (decoration & TextDecoration::UNDERLINE) {
        retval |= TextEngine::TextDecoration::UNDERLINE;
    }
    if (decoration & TextDecoration::OVERLINE) {
        retval |= TextEngine::TextDecoration::OVERLINE;
    }
    if (decoration & TextDecoration::LINE_THROUGH) {
        retval |= TextEngine::TextDecoration::LINE_THROUGH;
    }
    return retval;
}

TextEngine::AnySpanAlignment Convert(const PlaceholderVerticalAlignment &alignment)
{
    switch (alignment) {
        case PlaceholderVerticalAlignment::OFFSET_AT_BASELINE:
            return TextEngine::AnySpanAlignment::OFFSET_AT_BASELINE;
        case PlaceholderVerticalAlignment::ABOVE_BASELINE:
            return TextEngine::AnySpanAlignment::ABOVE_BASELINE;
        case PlaceholderVerticalAlignment::BELOW_BASELINE:
            return TextEngine::AnySpanAlignment::BELOW_BASELINE;
        case PlaceholderVerticalAlignment::TOP_OF_ROW_BOX:
            return TextEngine::AnySpanAlignment::TOP_OF_ROW_BOX;
        case PlaceholderVerticalAlignment::BOTTOM_OF_ROW_BOX:
            return TextEngine::AnySpanAlignment::BOTTOM_OF_ROW_BOX;
        case PlaceholderVerticalAlignment::CENTER_OF_ROW_BOX:
            return TextEngine::AnySpanAlignment::CENTER_OF_ROW_BOX;
    }
    return TextEngine::AnySpanAlignment::OFFSET_AT_BASELINE;
}

TextEngine::EllipsisModal Convert(const EllipsisModal &ellipsisModal)
{
    switch (ellipsisModal) {
        case EllipsisModal::HEAD:
            return TextEngine::EllipsisModal::HEAD;
        case EllipsisModal::MIDDLE:
            return TextEngine::EllipsisModal::MIDDLE;
        case EllipsisModal::TAIL:
        default:
            return TextEngine::EllipsisModal::TAIL;
    }
}
} // namespace AdapterTextEngine
} // namespace Rosen
} // namespace OHOS
