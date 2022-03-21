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

#include "render/rs_border.h"

#include "include/core/SkPaint.h"
#include "include/effects/Sk1DPathEffect.h"
#include "include/effects/SkDashPathEffect.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr int PARAM_DOUBLE = 2;
constexpr int32_t DASHED_LINE_LENGTH = 3;
} // namespace

void RSBorder::SetColor(Color color)
{
    colors_.resize(1, color);
}

void RSBorder::SetWidth(float width)
{
    widths_.resize(1, width);
}

void RSBorder::SetStyle(BorderStyle style)
{
    styles_.resize(1, style);
}

Color RSBorder::GetColor(int idx) const
{
    if (colors_.empty()) {
        return RgbPalette::Transparent();
    } else if (colors_.size() == 1) {
        return colors_.front();
    } else {
        return colors_.at(idx);
    }
}

float RSBorder::GetWidth(int idx) const
{
    if (widths_.empty()) {
        return 0.f;
    } else if (widths_.size() == 1) {
        return widths_.front();
    } else {
        return widths_.at(idx);
    }
}

BorderStyle RSBorder::GetStyle(int idx) const
{
    if (styles_.empty()) {
        return BorderStyle::SOLID;
    } else if (styles_.size() == 1) {
        return styles_.front();
    } else {
        return styles_.at(idx);
    }
}

void RSBorder::SetColorFour(Vector4<Color> color)
{
    if (color.x_ == color.y_ && color.x_ == color.z_ && color.x_ == color.w_) {
        return SetColor(color.x_);
    }
    colors_ = { color.x_, color.y_, color.z_, color.w_ };
}

void RSBorder::SetWidthFour(Vector4f width)
{
    if (width.x_ == width.y_ && width.x_ == width.z_ && width.x_ == width.w_) {
        return SetWidth(width.x_);
    }
    widths_ = { width.x_, width.y_, width.z_, width.w_ };
}

void RSBorder::SetStyleFour(Vector4<BorderStyle> style)
{
    if (style.x_ == style.y_ && style.x_ == style.z_ && style.x_ == style.w_) {
        return SetStyle(style.x_);
    }
    styles_ = { style.x_, style.y_, style.z_, style.w_ };
}

Vector4<Color> RSBorder::GetColorFour() const
{
    if (colors_.size() == 4) {
        return Vector4<Color>(colors_[0], colors_[1], colors_[2], colors_[3]);
    } else {
        return Vector4<Color>(GetColor());
    }
}

Vector4f RSBorder::GetWidthFour() const
{
    if (widths_.size() == 4) {
        return Vector4f(widths_[0], widths_[1], widths_[2], widths_[3]);
    } else {
        return Vector4f(GetWidth());
    }
}

Vector4<BorderStyle> RSBorder::GetStyleFour() const
{
    if (styles_.size() == 4) {
        return Vector4<BorderStyle>(styles_[0], styles_[1], styles_[2], styles_[3]);
    } else {
        return Vector4<BorderStyle>(GetStyle());
    }
}

void SetBorderEffect(SkPaint& paint, BorderStyle style, float width, float spaceBetweenDot, float borderLength)
{
    if (style == BorderStyle::DOTTED) {
        SkPath dotPath;
        if (ROSEN_EQ(spaceBetweenDot, 0.f)) {
            spaceBetweenDot = width * PARAM_DOUBLE;
        }
        dotPath.addCircle(0.0f, 0.0f, width / PARAM_DOUBLE);
        paint.setPathEffect(SkPath1DPathEffect::Make(dotPath, spaceBetweenDot, 0.0, SkPath1DPathEffect::kRotate_Style));
    } else if (style == BorderStyle::DASHED) {
        double addLen = 0.0; // When left < 2 * gap, splits left to gaps.
        double delLen = 0.0; // When left > 2 * gap, add one dash and shortening them.
        if (!ROSEN_EQ(borderLength, 0.f)) {
            float count = borderLength / width;
            float leftLen = fmod((count - DASHED_LINE_LENGTH), (DASHED_LINE_LENGTH + 1));
            if (leftLen > DASHED_LINE_LENGTH - 1) {
                delLen = (DASHED_LINE_LENGTH + 1 - leftLen) * width /
                         static_cast<int>((count - DASHED_LINE_LENGTH) / (DASHED_LINE_LENGTH + 1) + 2);
            } else {
                addLen = leftLen * width / static_cast<int>((count - DASHED_LINE_LENGTH) / (DASHED_LINE_LENGTH + 1));
            }
        }
        const float intervals[] = { width * DASHED_LINE_LENGTH - delLen, width + addLen };
        paint.setPathEffect(SkDashPathEffect::Make(intervals, SK_ARRAY_COUNT(intervals), 0.0));
    } else {
        paint.setPathEffect(nullptr);
    }
}

bool RSBorder::ApplyFillStyle(SkPaint& paint) const
{
    if (colors_.size() != 1) {
        return false;
    }
    if (styles_.size() != 1 || styles_.front() != BorderStyle::SOLID) {
        return false;
    }
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setColor(GetColor().AsArgbInt());
    return true;
}

bool RSBorder::ApplyPathStyle(SkPaint& paint) const
{
    if (colors_.size() != 1 || widths_.size() != 1 || styles_.size() != 1) {
        return false;
    }
    paint.setStrokeWidth(widths_.front());
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setColor(colors_.front().AsArgbInt());
    SetBorderEffect(paint, styles_.front(), widths_.front(), 0.f, 0.f);
    return true;
}

bool RSBorder::ApplyFourLine(SkPaint& paint) const
{
    if (styles_.size() != 1 || colors_.size() != 1) {
        return false;
    }
    paint.setStyle(SkPaint::Style::kStroke_Style);
    return true;
}

bool RSBorder::ApplyLineStyle(SkPaint& paint, int borderIdx, float length) const
{
    if (GetWidth(borderIdx) > 0.f) {
        return false;
    }
    float borderWidth = GetWidth(borderIdx);
    float addLen = (GetStyle(borderIdx) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
    auto borderLength = length - borderWidth * addLen * PARAM_DOUBLE;
    int32_t rawNumber = borderLength / (PARAM_DOUBLE * borderWidth);
    if (rawNumber == 0) {
        return false;
    }
    SetBorderEffect(paint, GetStyle(borderIdx), borderWidth, borderLength / rawNumber, borderLength);
    return true;
}

} // namespace Rosen
} // namespace OHOS
