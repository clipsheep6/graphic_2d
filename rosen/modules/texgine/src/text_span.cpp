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

#include "text_span.h"

#include <iomanip>
#include <stack>

#include <hb-icu.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkFont.h>
#include <include/core/SkMaskFilter.h>
#include <include/core/SkPath.h>
#include <include/effects/Sk1DPathEffect.h>
#include <include/effects/SkDashPathEffect.h>
#include <unicode/ubidi.h>

#include "font_collection.h"
#include "font_styles.h"
#include "measurer.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#include "texgine/utils/trace.h"
#include "text_converter.h"
#include "word_breaker.h"

namespace Texgine {
#define MAXRGB 255
#define OFFSETY 3
#define HALF 0.5f
#define WIDTHSCALAR 5.0f
#define HEIGHTSCALAR 5.0f
#define DOTTEDADVANCE 10.0f
#define WAVYADVANCE 6.0f
#define PHASE 0.0f
#define COUNT 2
#define MAXBLURRADIUS 64u
#define POINTX0 0
#define POINTX1 1
#define POINTX2 2
#define POINTX3 3
#define POINTX4 4
#define POINTX5 5
#define POINTX6 6
#define POINTY0 0
#define POINTY2 2
#define POINTY4 4
#define POINTY6 6

std::shared_ptr<TextSpan> TextSpan::MakeEmpty()
{
    return std::make_shared<TextSpan>();
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::string &text)
{
    auto span = MakeEmpty();
    auto decodeText = TextConverter::ToUTF16(text);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::u16string &text)
{
    auto span = MakeEmpty();
    std::vector<uint16_t> u16;
    for (const auto &t : text) {
        u16.push_back(t);
    }
    span->AddUTF16Text(u16);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::u32string &text)
{
    auto span = MakeEmpty();
    std::vector<uint32_t> u32;
    for (const auto &t : text) {
        u32.push_back(t);
    }
    auto decodeText = TextConverter::ToUTF16(u32);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::vector<uint16_t> &text)
{
    auto span = MakeEmpty();
    span->AddUTF16Text(text);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::vector<uint32_t> &text)
{
    auto span = MakeEmpty();
    auto decodeText = TextConverter::ToUTF16(text);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromCharGroups(const CharGroups &cgs)
{
    auto span = MakeEmpty();
    span->cgs_= cgs;
    return span;
}

void TextSpan::AddUTF16Text(const std::vector<uint16_t> &text)
{
    u16vect_.insert(u16vect_.end(), text.begin(), text.end());
}

std::shared_ptr<TextSpan> TextSpan::CloneWithCharGroups(const CharGroups &cgs)
{
    auto span = MakeEmpty();
    *span = *this;
    span->cgs_ = cgs;
    return span;
}

double TextSpan::GetHeight() const
{
    return smetrics_.fDescent - smetrics_.fAscent;
}

double TextSpan::GetWidth() const
{
    return width_;
}

double TextSpan::GetPreBreak() const
{
    return preBreak_;
}

double TextSpan::GetPostBreak() const
{
    return postBreak_;
}

bool TextSpan::IsRTL() const
{
    return rtl_;
}

void TextSpan::Paint(SkCanvas &canvas, double offsetx, double offsety, const TextStyle &xs)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setARGB(MAXRGB, MAXRGB, 0, 0);
    paint.setColor(xs.color_);
    if (xs.background_.has_value()) {
        auto rect = SkRect::MakeXYWH(offsetx, offsety + smetrics_.fAscent, width_,
            smetrics_.fDescent - smetrics_.fAscent);
        canvas.drawRect(rect, xs.background_.value());
    }

    if (xs.foreground_.has_value()) {
        paint = xs.foreground_.value();
    }

    canvas.drawTextBlob(textBlob_, offsetx, offsety, paint);
    PaintDecoration(canvas, offsetx, offsety, xs);
}

void TextSpan::PaintDecoration(SkCanvas &canvas, double offsetx, double offsety, const TextStyle &xs)
{
    double left = offsetx;
    double right = left + GetWidth();

    if ((xs.decoration_ & TextDecoration::Underline) == TextDecoration::Underline) {
        double y = offsety + smetrics_.fUnderlinePosition;
        PaintDecorationStyle(canvas, left, right, y, xs);
    }
    if ((xs.decoration_ & TextDecoration::Overline) == TextDecoration::Overline) {
        double y = offsety - abs(smetrics_.fAscent);
        PaintDecorationStyle(canvas, left, right, y, xs);
    }
    if ((xs.decoration_ & TextDecoration::Linethrough) == TextDecoration::Linethrough) {
        double y = offsety - (smetrics_.fCapHeight * HALF);
        PaintDecorationStyle(canvas, left, right, y, xs);
    }
    if ((xs.decoration_ & TextDecoration::Baseline) == TextDecoration::Baseline) {
        double y = offsety;
        PaintDecorationStyle(canvas, left, right, y, xs);
    }
}

void TextSpan::PaintDecorationStyle(SkCanvas &canvas, double left, double right, double y, const TextStyle &xs)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setARGB(MAXRGB, MAXRGB, 0, 0);
    paint.setColor(xs.decorationColor_.value_or(xs.color_));
    paint.setStrokeWidth(xs.decorationThicknessScale_);

    switch (xs.decorationStyle_) {
        case TextDecorationStyle::Solid:
            break;
        case TextDecorationStyle::Double:
            canvas.drawLine(left, y, right, y, paint);
            y += OFFSETY;
            break;
        case TextDecorationStyle::Dotted: {
            SkPath circle;
            circle.addOval(SkRect::MakeWH(WIDTHSCALAR, HEIGHTSCALAR));
            paint.setPathEffect(SkPath1DPathEffect::Make(circle, DOTTEDADVANCE, PHASE, SkPath1DPathEffect::kRotate_Style));
            break;
        }
        case TextDecorationStyle::Dashed: {
            const SkScalar intervals[2] = {WIDTHSCALAR, HEIGHTSCALAR};
            paint.setPathEffect(SkDashPathEffect::Make(intervals, COUNT, PHASE));
            paint.setStyle(SkPaint::kStroke_Style);
            break;
        }
        case TextDecorationStyle::Wavy: {
            SkPath wavy;
            float thickness = xs.decorationThicknessScale_;
            wavy.moveTo({POINTX0, POINTY2 - thickness});
            wavy.quadTo({POINTX1, POINTY0 - thickness}, {POINTX2, POINTY2 - thickness});
            wavy.lineTo({POINTX3, POINTY4 - thickness});
            wavy.quadTo({POINTX4, POINTY6 - thickness}, {POINTX5, POINTY4 - thickness});
            wavy.lineTo({POINTX6, POINTY2 - thickness});
            wavy.lineTo({POINTX6, POINTY2 + thickness});
            wavy.lineTo({POINTX5, POINTY4 + thickness});
            wavy.quadTo({POINTX4, POINTY6 + thickness}, {POINTX3, POINTY4 + thickness});
            wavy.lineTo({POINTX2, POINTY2 + thickness});
            wavy.quadTo({POINTX1, POINTY0 + thickness}, {POINTX0, POINTY2 + thickness});
            wavy.lineTo({POINTX0, POINTY2 - thickness});
            paint.setPathEffect(SkPath1DPathEffect::Make(wavy, WAVYADVANCE, PHASE, SkPath1DPathEffect::kRotate_Style));
            paint.setStyle(SkPaint::kStroke_Style);
            break;
        }
    }
    canvas.drawLine(left, y, right, y, paint);
}

void TextSpan::PaintShadow(SkCanvas &canvas, double offsetx, double offsety, const std::vector<TextShadow> &shadows)
{
    for (const auto &shadow : shadows) {
        auto x = offsetx + shadow.offsetX_;
        auto y = offsety + shadow.offsetY_;
        auto blurRadius = std::min(shadow.blurLeave_, MAXBLURRADIUS);

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(shadow.color_);
        paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, blurRadius));

        canvas.drawTextBlob(textBlob_, x, y, paint);
    }
}
} // namespace Texgine
