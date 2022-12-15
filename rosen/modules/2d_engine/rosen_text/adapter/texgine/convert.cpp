/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
namespace AdapterTexgine {
std::shared_ptr<OHOS::Rosen::AdapterTexgine::FontCollection> Convert(
        const std::shared_ptr<OHOS::Rosen::FontCollection> &fontCollection)
{
    return std::static_pointer_cast<OHOS::Rosen::AdapterTexgine::FontCollection>(fontCollection);
}

Texgine::TypographyStyle Convert(const TypographyStyle &style)
{
    Texgine::TypographyStyle ys = {
        .fontWeight_ = Convert(style.fontWeight_),
        .fontStyle_ = Convert(style.fontStyle_),
        .fontFamilies_ = { style.fontFamily_ },
        .fontSize_ = style.fontSize_,
        .heightScale_ = style.heightScale_,
        .heightOnly_ = style.heightOnly_,
        .locale_ = style.locale_,
        .maxLines_ = style.maxLines_,
        .ellipsis_ = style.ellipsis_,
        .breakStrategy_ = Convert(style.breakStrategy_),
        .wordBreakType_ = Convert(style.wordBreakType_),
        .align_ = Convert(style.textAlign_),
        .direction_ = Convert(style.textDirection_),
        .useLineStyle_ = style.useLineStyle_,
        .lineStyle_ = {
            .only_ = style.lineStyleOnly_,
            .fontWeight_ = Convert(style.lineStyleFontWeight_),
            .fontStyle_ = Convert(style.lineStyleFontStyle_),
            .fontFamilies_ = style.lineStyleFontFamilies_,
            .heightOnly_ = style.lineStyleHeightOnly_,
            .fontSize_ = style.lineStyleFontSize_,
            .heightScale_ = style.lineStyleHeightScale_,
        },
    };
    if (style.lineStyleSpacingScale_ >= 0) {
        ys.lineStyle_.spacingScale_ = style.lineStyleSpacingScale_;
    }
    return ys;
}

Texgine::TextStyle Convert(const TextStyle &style)
{
    auto color = SkColorSetARGB(style.color_.GetAlpha(),
                                style.color_.GetRed(),
                                style.color_.GetGreen(),
                                style.color_.GetBlue());
    auto decorationColor = SkColorSetARGB(style.decorationColor_.GetAlpha(),
                                          style.decorationColor_.GetRed(),
                                          style.decorationColor_.GetGreen(),
                                          style.decorationColor_.GetBlue());
    Texgine::TextStyle xs = {
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
        .heightOnly_ = style.heightOnly_,
        .heightScale_ = style.heightScale_,
        .letterSpacing_ = style.letterSpacing_,
        .wordSpacing_ = style.wordSpacing_,
        .foreground_ = style.foreground_,
        .background_ = style.background_,
    };

    for (const auto &[tag, value] : style.fontFeatures_.GetFontFeatures()) {
        xs.fontFeature_.SetFeature(tag, value);
    }

    for (const auto &[color, offset, radius] : style.shadows_) {
        auto shadowColor = (color.GetAlpha() << 24) | (color.GetRed() << 16) |
            (color.GetGreen() << 8) | (color.GetBlue() << 0);
        Texgine::TextShadow shadow = {
            .offsetX_ = offset.GetX(),
            .offsetY_ = offset.GetY(),
            .color_ = shadowColor,
            .blurLeave_ = radius,
        };
        xs.shadows_.emplace_back(shadow);
    }
    return xs;
}

IndexAndAffinity Convert(const Texgine::IndexAndAffinity &pos)
{
    return { pos.index_, Convert(pos.affinity_) };
}

Boundary Convert(const Texgine::Boundary &range)
{
    return { range.leftIndex_, range.rightIndex_ };
}

TextRect Convert(const Texgine::TextRect &box)
{
    Drawing::RectF rect(box.rect_.fLeft, box.rect_.fTop, box.rect_.fRight, box.rect_.fBottom);
    return { rect, Convert(box.direction_) };
}

Affinity Convert(const Texgine::Affinity &affinity)
{
    switch (affinity) {
        case Texgine::Affinity::Prev:
            return Affinity::Prev;
        case Texgine::Affinity::Next:
            return Affinity::Next;
    }
    return Affinity::Prev;
}

TextDirection Convert(const Texgine::TextDirection &direction)
{
    switch (direction) {
        case Texgine::TextDirection::LTR:
            return TextDirection::LTR;
        case Texgine::TextDirection::RTL:
            return TextDirection::RTL;
    }
    return TextDirection::LTR;
}

Texgine::TextRectHeightStyle Convert(const TextRectHeightStyle &style)
{
    switch (style) {
        case TextRectHeightStyle::Tight:
            return Texgine::TextRectHeightStyle::Tight;
        case TextRectHeightStyle::CoverTopAndBottom:
            return Texgine::TextRectHeightStyle::CoverTopAndBottom;
        case TextRectHeightStyle::CoverHalfTopAndBottom:
            return Texgine::TextRectHeightStyle::CoverHalfTopAndBottom;
        case TextRectHeightStyle::CoverTop:
            return Texgine::TextRectHeightStyle::CoverTop;
        case TextRectHeightStyle::CoverBottom:
            return Texgine::TextRectHeightStyle::CoverBottom;
        case TextRectHeightStyle::FollowByStrut:
            return Texgine::TextRectHeightStyle::FollowByLineStyle;
    }
    return Texgine::TextRectHeightStyle::Tight;
}

Texgine::TextRectWidthStyle Convert(const TextRectWidthStyle &style)
{
    switch (style) {
        case TextRectWidthStyle::Tight:
            return Texgine::TextRectWidthStyle::Tight;
        case TextRectWidthStyle::Max:
            return Texgine::TextRectWidthStyle::MaxWidth;
    }
    return Texgine::TextRectWidthStyle::Tight;
}

Texgine::WordBreakType Convert(const WordBreakType &style)
{
    switch (style) {
        case WordBreakType::Normal:
            return Texgine::WordBreakType::Normal;
        case WordBreakType::BreakAll:
            return Texgine::WordBreakType::BreakAll;
        case WordBreakType::BreakWord:
            return Texgine::WordBreakType::BreakWord;
    }
    return Texgine::WordBreakType::Normal;
}

Texgine::BreakStrategy Convert(const BreakStrategy &style)
{
    switch (style) {
        case BreakStrategy::Greedy:
            return Texgine::BreakStrategy::Greedy;
        case BreakStrategy::HighQuality:
            return Texgine::BreakStrategy::HighQuality;
        case BreakStrategy::Balanced:
            return Texgine::BreakStrategy::Balanced;
    }
    return Texgine::BreakStrategy::Greedy;
}

Texgine::FontWeight Convert(const FontWeight &style)
{
    switch (style) {
        case FontWeight::W100:
            return Texgine::FontWeight::W100;
        case FontWeight::W200:
            return Texgine::FontWeight::W200;
        case FontWeight::W300:
            return Texgine::FontWeight::W300;
        case FontWeight::W400:
            return Texgine::FontWeight::W400;
        case FontWeight::W500:
            return Texgine::FontWeight::W500;
        case FontWeight::W600:
            return Texgine::FontWeight::W600;
        case FontWeight::W700:
            return Texgine::FontWeight::W700;
        case FontWeight::W800:
            return Texgine::FontWeight::W800;
        case FontWeight::W900:
            return Texgine::FontWeight::W900;
    }
    return Texgine::FontWeight::W400;
}

Texgine::FontStyle Convert(const FontStyle &style)
{
    switch (style) {
        case FontStyle::Normal:
            return Texgine::FontStyle::Normal;
        case FontStyle::Italic:
            return Texgine::FontStyle::Italic;
    }
    return Texgine::FontStyle::Normal;
}

Texgine::TextAlign Convert(const TextAlign &align)
{
    switch (align) {
        case TextAlign::Left:
            return Texgine::TextAlign::Left;
        case TextAlign::Right:
            return Texgine::TextAlign::Right;
        case TextAlign::Center:
            return Texgine::TextAlign::Center;
        case TextAlign::Justify:
            return Texgine::TextAlign::Justify;
        case TextAlign::Start:
            return Texgine::TextAlign::Start;
        case TextAlign::End:
            return Texgine::TextAlign::End;
    }
    return Texgine::TextAlign::Start;
}

Texgine::TextBaseline Convert(const TextBaseline &baseline)
{
    switch (baseline) {
        case TextBaseline::Alphabetic:
            return Texgine::TextBaseline::Alphabetic;
        case TextBaseline::Ideographic:
            return Texgine::TextBaseline::Ideographic;
    }
    return Texgine::TextBaseline::Alphabetic;
}

Texgine::TextDirection Convert(const TextDirection &direction)
{
    switch (direction) {
        case TextDirection::LTR:
            return Texgine::TextDirection::LTR;
        case TextDirection::RTL:
            return Texgine::TextDirection::RTL;
    }
    return Texgine::TextDirection::LTR;
}

Texgine::TextDecorationStyle Convert(const TextDecorationStyle &style)
{
    switch (style) {
        case TextDecorationStyle::Solid:
            return Texgine::TextDecorationStyle::Solid;
        case TextDecorationStyle::Double:
            return Texgine::TextDecorationStyle::Double;
        case TextDecorationStyle::Dotted:
            return Texgine::TextDecorationStyle::Dotted;
        case TextDecorationStyle::Dashed:
            return Texgine::TextDecorationStyle::Dashed;
        case TextDecorationStyle::Wavy:
            return Texgine::TextDecorationStyle::Wavy;
    }
    return Texgine::TextDecorationStyle::Solid;
}

Texgine::TextDecoration Convert(const TextDecoration &decoration)
{
    Texgine::TextDecoration retval = Texgine::TextDecoration::None;
    if (decoration & TextDecoration::UNDERLINE) {
        retval |= Texgine::TextDecoration::Underline;
    }
    if (decoration & TextDecoration::OVERLINE) {
        retval |= Texgine::TextDecoration::Overline;
    }
    if (decoration & TextDecoration::LINETHROUGH) {
        retval |= Texgine::TextDecoration::Linethrough;
    }
    return retval;
}

Texgine::AnySpanAlignment Convert(const PlaceholderVerticalAlignment &alignment)
{
    switch (alignment) {
        case PlaceholderVerticalAlignment::OffsetAtBaseline:
            return Texgine::AnySpanAlignment::OffsetAtBaseline;
        case PlaceholderVerticalAlignment::AboveBaseline:
            return Texgine::AnySpanAlignment::AboveBaseline;
        case PlaceholderVerticalAlignment::BelowBaseline:
            return Texgine::AnySpanAlignment::BelowBaseline;
        case PlaceholderVerticalAlignment::TopOfRowBox:
            return Texgine::AnySpanAlignment::TopOfRowBox;
        case PlaceholderVerticalAlignment::BottomOfRowBox:
            return Texgine::AnySpanAlignment::BottomOfRowBox;
        case PlaceholderVerticalAlignment::CenterOfRowBox:
            return Texgine::AnySpanAlignment::CenterOfRowBox;
    }
    return Texgine::AnySpanAlignment::OffsetAtBaseline;
}
} // namespace AdapterTexgine
} // namespace Rosen
} // namespace OHOS
