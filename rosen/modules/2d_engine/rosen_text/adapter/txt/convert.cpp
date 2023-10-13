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
namespace AdapterTxt {
std::shared_ptr<OHOS::Rosen::AdapterTxt::FontCollection> Convert(
    const std::shared_ptr<OHOS::Rosen::FontCollection> &fontCollection)
{
    return std::static_pointer_cast<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection);
}

IndexAndAffinity Convert(const txt::Paragraph::PositionWithAffinity &pos)
{
    return { pos.position, static_cast<Affinity>(pos.affinity) };
}

Boundary Convert(const txt::Paragraph::Range<size_t> &range)
{
    return { range.start, range.end };
}

TextRect Convert(const txt::Paragraph::TextBox &box)
{
    Drawing::RectF rect(box.rect.fLeft, box.rect.fTop, box.rect.fRight, box.rect.fBottom);
    return { rect, static_cast<TextDirection>(box.direction) };
}

txt::Paragraph::RectHeightStyle Convert(const TextRectHeightStyle &style)
{
    return static_cast<txt::Paragraph::RectHeightStyle>(style);
}

txt::Paragraph::RectWidthStyle Convert(const TextRectWidthStyle &style)
{
    return static_cast<txt::Paragraph::RectWidthStyle>(style);
}

txt::ParagraphStyle Convert(const TypographyStyle &style)
{
    return {
#ifndef USE_GRAPHIC_TEXT_GINE
        .font_weight = static_cast<txt::FontWeight>(style.fontWeight_),
        .font_style = static_cast<txt::FontStyle>(style.fontStyle_),
        .font_family = style.fontFamily_,
        .font_size = style.fontSize_,
        .height = style.heightScale_,
        .has_height_override = style.heightOnly_,
        .strut_enabled = style.useLineStyle_,
        .strut_font_weight = static_cast<txt::FontWeight>(style.lineStyleFontWeight_),
        .strut_font_style = static_cast<txt::FontStyle>(style.lineStyleFontStyle_),
        .strut_font_families = style.lineStyleFontFamilies_,
        .strut_font_size = style.lineStyleFontSize_,
        .strut_height = style.lineStyleHeightScale_,
        .strut_has_height_override = style.lineStyleHeightOnly_,
        .strut_leading = style.lineStyleSpacingScale_,
        .force_strut_height = style.lineStyleOnly_,
        .text_align = static_cast<txt::TextAlign>(style.textAlign_),
        .text_direction = static_cast<txt::TextDirection>(style.textDirection_),
        .max_lines = style.maxLines_,
        .ellipsis = style.ellipsis_,
        .ellipsisModal = static_cast<txt::EllipsisModal>(style.ellipsisModal_),
        .locale = style.locale_,
        .break_strategy = static_cast<minikin::BreakStrategy>(style.breakStrategy_),
        .word_break_type = static_cast<minikin::WordBreakType>(style.wordBreakType_),
#else
        .font_weight = static_cast<txt::FontWeight>(style.fontWeight),
        .font_style = static_cast<txt::FontStyle>(style.fontStyle),
        .font_family = style.fontFamily,
        .font_size = style.fontSize,
        .height = style.heightScale,
        .has_height_override = style.heightOnly,
        .strut_enabled = style.useLineStyle,
        .strut_font_weight = static_cast<txt::FontWeight>(style.lineStyleFontWeight),
        .strut_font_style = static_cast<txt::FontStyle>(style.lineStyleFontStyle),
        .strut_font_families = style.lineStyleFontFamilies,
        .strut_font_size = style.lineStyleFontSize,
        .strut_height = style.lineStyleHeightScale,
        .strut_has_height_override = style.lineStyleHeightOnly,
        .strut_half_leading = style.lineStyleHalfLeading,
        .strut_leading = style.lineStyleSpacingScale,
        .force_strut_height = style.lineStyleOnly,
        .text_align = static_cast<txt::TextAlign>(style.textAlign),
        .text_direction = static_cast<txt::TextDirection>(style.textDirection),
        .max_lines = style.maxLines,
        .ellipsis = style.ellipsis,
        .ellipsis_modal = static_cast<txt::EllipsisModal>(style.ellipsisModal),
        .locale = style.locale,
        .break_strategy = static_cast<minikin::BreakStrategy>(style.breakStrategy),
        .word_break_type = static_cast<minikin::WordBreakType>(style.wordBreakType),
#endif
    };
}

txt::PlaceholderRun Convert(const PlaceholderSpan &run)
{
    return {
        run.width, run.height,
        static_cast<txt::PlaceholderAlignment>(run.alignment),
        static_cast<txt::TextBaseline>(run.baseline),
        run.baselineOffset,
    };
}

txt::TextStyle Convert(const TextStyle &style)
{
    txt::TextStyle textStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    auto color = SkColorSetARGB(style.color_.GetAlpha(),
                                style.color_.GetRed(),
                                style.color_.GetGreen(),
                                style.color_.GetBlue());
#else
    auto color = SkColorSetARGB(
        style.color.GetAlpha(), style.color.GetRed(), style.color.GetGreen(), style.color.GetBlue());
#endif
    textStyle.color = color;
#ifndef USE_GRAPHIC_TEXT_GINE
    textStyle.decoration = style.decoration_;
    auto decorationColor = SkColorSetARGB(style.decorationColor_.GetAlpha(),
                                          style.decorationColor_.GetRed(),
                                          style.decorationColor_.GetGreen(),
                                          style.decorationColor_.GetBlue());
#else
    textStyle.decoration = style.decoration;
    auto decorationColor = SkColorSetARGB(style.decorationColor.GetAlpha(),
        style.decorationColor.GetRed(), style.decorationColor.GetGreen(), style.decorationColor.GetBlue());
#endif
    textStyle.decoration_color = decorationColor;
#ifndef USE_GRAPHIC_TEXT_GINE
    textStyle.decoration_style = static_cast<txt::TextDecorationStyle>(style.decorationStyle_);
    textStyle.decoration_thickness_multiplier = style.decorationThicknessScale_;
    textStyle.font_weight = static_cast<txt::FontWeight>(style.fontWeight_);
    textStyle.font_style = static_cast<txt::FontStyle>(style.fontStyle_);
    textStyle.text_baseline = static_cast<txt::TextBaseline>(style.baseline_);
    textStyle.font_families = style.fontFamilies_;
    textStyle.font_size = style.fontSize_;
    textStyle.letter_spacing = style.letterSpacing_;
    textStyle.word_spacing = style.wordSpacing_;
    textStyle.height = style.heightScale_;
    textStyle.has_height_override = style.heightOnly_;
    textStyle.locale = style.locale_;
    textStyle.has_background = style.background_.has_value();
    textStyle.background = style.background_.value_or(SkPaint());
    textStyle.has_foreground = style.foreground_.has_value();
    textStyle.foreground = style.foreground_.value_or(SkPaint());
#else
    textStyle.decoration_style = static_cast<txt::TextDecorationStyle>(style.decorationStyle);
    textStyle.decoration_thickness_multiplier = style.decorationThicknessScale;
    textStyle.font_weight = static_cast<txt::FontWeight>(style.fontWeight);
    textStyle.font_style = static_cast<txt::FontStyle>(style.fontStyle);
    textStyle.text_baseline = static_cast<txt::TextBaseline>(style.baseline);
    textStyle.half_leading = style.halfLeading;
    textStyle.font_families = style.fontFamilies;
    textStyle.font_size = style.fontSize;
    textStyle.letter_spacing = style.letterSpacing;
    textStyle.word_spacing = style.wordSpacing;
    textStyle.height = style.heightScale;
    textStyle.has_height_override = style.heightOnly;
    textStyle.locale = style.locale;
    textStyle.has_background = style.background.has_value();
    textStyle.background = style.background.value_or(SkPaint());
    textStyle.has_foreground = style.foreground.has_value();
    textStyle.foreground = style.foreground.value_or(SkPaint());
#endif

#ifndef USE_GRAPHIC_TEXT_GINE
    for (const auto &[color, offset, radius] : style.shadows_) {
#else
    for (const auto &[color, offset, radius] : style.shadows) {
#endif
        auto shadowColor = SkColorSetARGB(color.GetAlpha(), color.GetRed(),
                                          color.GetGreen(), color.GetBlue());
        auto shadowOffset = SkPoint::Make(offset.GetX(), offset.GetY());
        textStyle.text_shadows.emplace_back(shadowColor, shadowOffset, radius);
    }

#ifndef USE_GRAPHIC_TEXT_GINE
    for (const auto &[tag, value] : style.fontFeatures_.GetFontFeatures()) {
#else
    for (const auto &[tag, value] : style.fontFeatures.GetFontFeatures()) {
#endif
        textStyle.font_features.SetFeature(tag, value);
    }
    return textStyle;
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
