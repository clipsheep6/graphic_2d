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

#include "draw/path.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int PARAM_DOUBLE = 2;
constexpr int32_t DASHED_LINE_LENGTH = 3;
} // namespace

RSBorder::RSBorder(const bool& isOutline)
{
    if (isOutline) {
        SetStyle(BorderStyle::SOLID);
        SetColor(Color(0, 0, 0));
    }
}

void RSBorder::SetColor(Color color)
{
    colors_.clear();
    colors_.push_back(color);
}

void RSBorder::SetWidth(float width)
{
    widths_.clear();
    widths_.push_back(width);
}

void RSBorder::SetStyle(BorderStyle style)
{
    styles_.clear();
    styles_.push_back(style);
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
        return BorderStyle::NONE;
    } else if (styles_.size() == 1) {
        return styles_.front();
    } else {
        return styles_.at(idx);
    }
}

void RSBorder::SetColorFour(const Vector4<Color>& color)
{
    if (color.x_ == color.y_ && color.x_ == color.z_ && color.x_ == color.w_) {
        return SetColor(color.x_);
    }
    colors_ = { color.x_, color.y_, color.z_, color.w_ };
}

void RSBorder::SetWidthFour(const Vector4f& width)
{
    if (width.x_ == width.y_ && width.x_ == width.z_ && width.x_ == width.w_) {
        return SetWidth(width.x_);
    }
    widths_ = { width.x_, width.y_, width.z_, width.w_ };
}

void RSBorder::SetStyleFour(const Vector4<uint32_t>& style)
{
    if (style.x_ == style.y_ && style.x_ == style.z_ && style.x_ == style.w_) {
        return SetStyle(static_cast<BorderStyle>(style.x_));
    }
    styles_ = { static_cast<BorderStyle>(style.x_), static_cast<BorderStyle>(style.y_),
                static_cast<BorderStyle>(style.z_), static_cast<BorderStyle>(style.w_) };
}

void RSBorder::SetRadiusFour(const Vector4f& radius)
{
    radius_ = { radius.x_, radius.y_, radius.z_, radius.w_ };
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

Vector4<uint32_t> RSBorder::GetStyleFour() const
{
    if (styles_.size() == 4) {
        return Vector4<uint32_t>(static_cast<uint32_t>(styles_[0]), static_cast<uint32_t>(styles_[1]),
                                 static_cast<uint32_t>(styles_[2]), static_cast<uint32_t>(styles_[3]));
    } else {
        return Vector4<uint32_t>(static_cast<uint32_t>(GetStyle()));
    }
}

Vector4f RSBorder::GetRadiusFour() const
{
    return radius_;
}

void SetBorderEffect(Drawing::Pen& pen, BorderStyle style, float width, float spaceBetweenDot, float borderLength)
{
    if (ROSEN_EQ(width, 0.f)) {
        return;
    }
    if (style == BorderStyle::DOTTED) {
        Drawing::Path dotPath;
        if (ROSEN_EQ(spaceBetweenDot, 0.f)) {
            spaceBetweenDot = width * PARAM_DOUBLE;
        }
        dotPath.AddCircle(0.0f, 0.0f, width / PARAM_DOUBLE);
        pen.SetPathEffect(Drawing::PathEffect::CreatePathDashEffect(dotPath, spaceBetweenDot, 0.0,
            Drawing::PathDashStyle::ROTATE));
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
        pen.SetPathEffect(Drawing::PathEffect::CreateDashPathEffect(intervals, sizeof(intervals)/sizeof(float), 0.0));
    } else {
        pen.SetPathEffect(nullptr);
    }
}

bool RSBorder::ApplyFillStyle(Drawing::Brush& brush) const
{
    if (colors_.size() != 1) {
        return false;
    }
    if (styles_.size() != 1 || GetStyle() != BorderStyle::SOLID) {
        return false;
    }
    for (const float& width : widths_) {
        if (ROSEN_LE(width, 0.f)) {
            return false;
        }
    }
    brush.SetColor(GetColor().AsArgbInt());
    return true;
}

bool RSBorder::ApplyPathStyle(Drawing::Pen& pen) const
{
    if (colors_.size() != 1 || widths_.size() != 1 || styles_.size() != 1) {
        return false;
    }
    pen.SetWidth(widths_.front());
    pen.SetColor(colors_.front().AsArgbInt());
    SetBorderEffect(pen, GetStyle(), widths_.front(), 0.f, 0.f);
    return true;
}

bool RSBorder::ApplyFourLine(Drawing::Pen& pen) const
{
    if (colors_.size() != 1 || styles_.size() != 1) {
        return false;
    }
    return true;
}

bool RSBorder::ApplyLineStyle(Drawing::Pen& pen, int borderIdx, float length) const
{
    if (GetWidth(borderIdx) <= 0.0f) {
        return false;
    }
    float borderWidth = GetWidth(borderIdx);
    BorderStyle borderStyle = GetStyle(borderIdx);
    float addLen = (borderStyle != BorderStyle::DOTTED) ? 0.0f : 0.5f;
    auto borderLength = length - borderWidth * addLen * PARAM_DOUBLE;
    int32_t rawNumber = borderLength / (PARAM_DOUBLE * borderWidth);
    if (borderStyle == BorderStyle::DOTTED && rawNumber == 0) {
        return false;
    }

    pen.SetWidth(GetWidth(borderIdx));
    Color color = GetColor(borderIdx);
    pen.SetColor(color.AsArgbInt());
    SetBorderEffect(pen, GetStyle(borderIdx), borderWidth, borderLength / rawNumber, borderLength);
    return true;
}

void RSBorder::PaintFourLine(Drawing::Canvas& canvas, Drawing::Pen& pen, RectF rect) const
{
    float borderLeftWidth = GetWidth(RSBorder::LEFT);
    float borderRightWidth = GetWidth(RSBorder::RIGHT);
    float borderTopWidth = GetWidth(RSBorder::TOP);
    float borderBottomWidth = GetWidth(RSBorder::BOTTOM);
    if (ApplyLineStyle(pen, RSBorder::LEFT, rect.height_)) {
        float addLen = (GetStyle(RSBorder::LEFT) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.AttachPen(pen);
        canvas.DrawLine(
            Drawing::Point(rect.left_ + borderLeftWidth / PARAM_DOUBLE, rect.top_ + addLen * borderTopWidth),
            Drawing::Point(rect.left_ + borderLeftWidth / PARAM_DOUBLE, rect.GetBottom() - borderBottomWidth));
        canvas.DetachPen();
    }
    if (ApplyLineStyle(pen, RSBorder::RIGHT, rect.height_)) {
        float addLen = (GetStyle(RSBorder::RIGHT) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.AttachPen(pen);
        canvas.DrawLine(
            Drawing::Point(rect.GetRight() - borderRightWidth / PARAM_DOUBLE,
                rect.GetBottom() - addLen * borderBottomWidth),
            Drawing::Point(rect.GetRight() - borderRightWidth / PARAM_DOUBLE, rect.top_ + borderTopWidth));
        canvas.DetachPen();
    }
    if (ApplyLineStyle(pen, RSBorder::TOP, rect.width_)) {
        float addLen = (GetStyle(RSBorder::TOP) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.AttachPen(pen);
        canvas.DrawLine(
            Drawing::Point(rect.GetRight() - addLen * borderRightWidth, rect.top_ + borderTopWidth / PARAM_DOUBLE),
            Drawing::Point(rect.left_ + borderLeftWidth, rect.top_ + borderTopWidth / PARAM_DOUBLE));
        canvas.DetachPen();
    }
    if (ApplyLineStyle(pen, RSBorder::BOTTOM, rect.width_)) {
        float addLen = (GetStyle(RSBorder::BOTTOM) != BorderStyle::DOTTED) ? 0.0f : 0.5f;
        canvas.AttachPen(pen);
        canvas.DrawLine(
            Drawing::Point(rect.left_ + addLen * borderLeftWidth, rect.GetBottom() - borderBottomWidth / PARAM_DOUBLE),
            Drawing::Point(rect.GetRight() - borderRightWidth, rect.GetBottom() - borderBottomWidth / PARAM_DOUBLE));
        canvas.DetachPen();
    }
}

void RSBorder::PaintTopPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const RRect inrrect) const
{
    float topW = GetWidth(RSBorder::TOP);
    if (topW > 0.f) {
        float offsetX = rrect.GetRect().GetLeft();
        float offsetY = rrect.GetRect().GetTop();
        float width = rrect.GetRect().GetWidth();
        float leftW = GetWidth(RSBorder::LEFT);
        float rightW = GetWidth(RSBorder::RIGHT);
        float bottomW = GetWidth(RSBorder::BOTTOM);

        ApplyLineStyle(pen, RSBorder::TOP, width);
        auto style = GetStyle(RSBorder::TOP);
        if (style != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        Drawing::scalar centerY = rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2;
        Drawing::Point tlip(offsetX, centerY);
        if (leftW > 0.f) {
            tlip = getIntersectionPoint(Drawing::RoundRect::TOP_LEFT_POS, rrect);
        }
        // top right intersection point with rrect center
        Drawing::Point trip(offsetX + width, centerY);
        if (rightW > 0.f) {
            trip = getIntersectionPoint(Drawing::RoundRect::TOP_RIGHT_POS, rrect);
        }
        Drawing::Path topClipPath;
        topClipPath.MoveTo(offsetX, offsetY);
        topClipPath.LineTo(offsetX + tlip.GetX(), offsetY + tlip.GetY());
        topClipPath.LineTo(offsetX + trip.GetX(), offsetY + trip.GetY());
        topClipPath.LineTo(offsetX + width, offsetY);
        topClipPath.Close();
        canvas.ClipPath(topClipPath, Drawing::ClipOp::INTERSECT, true);

        canvas.AttachPen(pen);
        canvas.DrawRoundRect(GetDrawingRRect(inrrect));
        canvas.DetachPen();
    }
}

void RSBorder::PaintRightPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const RRect inrrect) const
{
    float rightW = GetWidth(RSBorder::RIGHT);
    if (rightW > 0.f) {
        float offsetX = rrect.GetRect().GetLeft();
        float offsetY = rrect.GetRect().GetTop();
        float width = rrect.GetRect().GetWidth();
        float height = rrect.GetRect().GetHeight();
        float leftW = GetWidth(RSBorder::LEFT);
        float topW = GetWidth(RSBorder::TOP);
        float bottomW = GetWidth(RSBorder::BOTTOM);

        ApplyLineStyle(pen, RSBorder::RIGHT, height);
        auto style = GetStyle(RSBorder::RIGHT);
        if (style != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        Drawing::Path rightClipPath;
        // top rigth intersection point with rrect center
        Drawing::Point trip = getIntersectionPoint(Drawing::RoundRect::TOP_RIGHT_POS, rrect);
        // bottom right intersection point with rrect center
        Drawing::Point brip = getIntersectionPoint(Drawing::RoundRect::BOTTOM_RIGHT_POS, rrect);
        rightClipPath.MoveTo(offsetX + width, offsetY);
        rightClipPath.LineTo(offsetX + trip.GetX(), offsetY + trip.GetY());
        rightClipPath.LineTo(offsetX + brip.GetX(), offsetY + brip.GetY());
        rightClipPath.LineTo(offsetX + width, offsetY + height);
        rightClipPath.Close();
        canvas.ClipPath(rightClipPath, Drawing::ClipOp::INTERSECT, true);

        canvas.AttachPen(pen);
        canvas.DrawRoundRect(GetDrawingRRect(inrrect));
        canvas.DetachPen();
    }
}

void RSBorder::PaintBottomPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const RRect inrrect) const
{
    float bottomW = GetWidth(RSBorder::BOTTOM);
    if (bottomW > 0.f) {
        float offsetX = rrect.GetRect().GetLeft();
        float offsetY = rrect.GetRect().GetTop();
        float width = rrect.GetRect().GetWidth();
        float height = rrect.GetRect().GetHeight();
        float leftW = GetWidth(RSBorder::LEFT);
        float topW = GetWidth(RSBorder::TOP);
        float rightW = GetWidth(RSBorder::RIGHT);

        ApplyLineStyle(pen, RSBorder::BOTTOM, width);
        auto style = GetStyle(RSBorder::BOTTOM);
        if (style != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        Drawing::scalar centerY = rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2;
        // bottom left intersection point with rrect center
        Drawing::Point blip(offsetX, centerY);
        if (leftW > 0.f) {
            blip = getIntersectionPoint(Drawing::RoundRect::BOTTOM_LEFT_POS, rrect);
        }
        // bottom right intersection point with rrect center
        Drawing::Point brip(offsetX + width, centerY);
        if (rightW > 0.f) {
            brip = getIntersectionPoint(Drawing::RoundRect::BOTTOM_RIGHT_POS, rrect);
        }
        Drawing::Path bottomClipPath;
        bottomClipPath.MoveTo(offsetX, offsetY + height);
        bottomClipPath.LineTo(offsetX + blip.GetX(), offsetY + blip.GetY());
        bottomClipPath.LineTo(offsetX + brip.GetX(), offsetY + brip.GetY());
        bottomClipPath.LineTo(offsetX + width, offsetY + height);
        bottomClipPath.Close();
        canvas.ClipPath(bottomClipPath, Drawing::ClipOp::INTERSECT, true);

        canvas.AttachPen(pen);
        canvas.DrawRoundRect(GetDrawingRRect(inrrect));
        canvas.DetachPen();
    }
}

void RSBorder::PaintLeftPath(Drawing::Canvas& canvas, Drawing::Pen& pen, const Drawing::RoundRect& rrect,
    const RRect inrrect) const
{
    float leftW = GetWidth(RSBorder::LEFT);
    if (leftW > 0.f) {
        float offsetX = rrect.GetRect().GetLeft();
        float offsetY = rrect.GetRect().GetTop();
        float height = rrect.GetRect().GetHeight();
        float topW = GetWidth(RSBorder::TOP);
        float rightW = GetWidth(RSBorder::RIGHT);
        float bottomW = GetWidth(RSBorder::BOTTOM);

        ApplyLineStyle(pen, RSBorder::LEFT, height);
        auto style = GetStyle(RSBorder::LEFT);
        if (style != BorderStyle::DOTTED) {
            pen.SetWidth(std::max(std::max(leftW, topW), std::max(rightW, bottomW)));
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        Drawing::Path leftClipPath;
        // top left intersection point with rrect center
        Drawing::Point tlip = getIntersectionPoint(Drawing::RoundRect::TOP_LEFT_POS, rrect);
        // bottom left intersection point with rrect center
        Drawing::Point blip = getIntersectionPoint(Drawing::RoundRect::BOTTOM_LEFT_POS, rrect);
        leftClipPath.MoveTo(offsetX, offsetY);
        leftClipPath.LineTo(offsetX + tlip.GetX(), offsetY + tlip.GetY());
        leftClipPath.LineTo(offsetX + blip.GetX(), offsetY + blip.GetY());
        leftClipPath.LineTo(offsetX, offsetY + height);
        leftClipPath.Close();
        canvas.ClipPath(leftClipPath, Drawing::ClipOp::INTERSECT, true);

        canvas.AttachPen(pen);
        canvas.DrawRoundRect(GetDrawingRRect(inrrect));
        canvas.DetachPen();
    }
}

Drawing::RoundRect RSBorder::GetDrawingRRect(const RRect& inrrect) const
{
    // get Rect for drawing border path
    float leftW = GetWidth(RSBorder::LEFT) / 2.f;
    float rightW = GetWidth(RSBorder::RIGHT) / 2.f;
    float topW = GetWidth(RSBorder::TOP) / 2.f;
    float bottomW = GetWidth(RSBorder::BOTTOM) / 2.f;
    Drawing::Rect drawingRect = Drawing::Rect(
        leftW, topW,
        inrrect.rect_.width_ - rightW, inrrect.rect_.height_ - bottomW);
    // set radius for all 4 corner of RRect
    constexpr uint32_t NUM_OF_CORNERS_IN_RECT = 4;
    std::vector<Drawing::Point> radiuses(NUM_OF_CORNERS_IN_RECT);
    std::vector<Drawing::Point> drads {{leftW, topW}, {rightW, topW}, {leftW, bottomW}, {rightW, bottomW}};
    for (uint32_t i = 0; i < NUM_OF_CORNERS_IN_RECT; i++) {
        radiuses.at(i).SetX(std::max(0.f, inrrect.radius_[i].x_ - drads[i].GetX()));
        radiuses.at(i).SetY(std::max(0.f, inrrect.radius_[i].y_ - drads[i].GetY()));
    }
    return Drawing::RoundRect(drawingRect, radiuses);
}

Drawing::Point RSBorder::getIntersectionPoint(Drawing::RoundRect::CornerPos cornerPos,
    const Drawing::RoundRect& rrect) const
{
    float x = rrect.GetRect().GetLeft() + rrect.GetRect().GetWidth() / 2.f;
    float y = rrect.GetRect().GetTop() + rrect.GetRect().GetHeight() / 2.f;
    float leftW = GetWidth(RSBorder::LEFT);
    float rightW = GetWidth(RSBorder::RIGHT);
    float kc = y / x;
    float k = 1.f;

    switch (cornerPos) {
        case Drawing::RoundRect::TOP_RIGHT_POS:
            if (rightW != 0) {
                k = GetWidth(RSBorder::TOP) / rightW;
                (k < kc) ? (y = x * k) : (x = rrect.GetRect().GetWidth() - y / k);
            }
            break;
        case Drawing::RoundRect::BOTTOM_RIGHT_POS:
            if (rightW != 0) {
                k = GetWidth(RSBorder::BOTTOM) / rightW;
                (k < kc) ? (y = rrect.GetRect().GetHeight() - x * k) : (x = rrect.GetRect().GetWidth() - y / k);
            }
            break;
        case Drawing::RoundRect::BOTTOM_LEFT_POS:
            if (leftW != 0) {
                k = GetWidth(RSBorder::BOTTOM) / leftW;
                (k < kc) ? (y = rrect.GetRect().GetHeight() - x * k) : (x = y / k);
            }
            break;
        // return left top pos by default
        default:
            if (leftW != 0) {
                k = GetWidth(RSBorder::TOP) / leftW;
                (k < kc) ? (y = x * k) : (x = y / k);
            }
            break;
    }
    return Drawing::Point(x, y);
}

std::string RSBorder::ToString() const
{
    std::stringstream ss;
    if (colors_.size() > 0) {
        ss << "colors: ";
    }
    for (auto color : colors_) {
        ss << color.AsArgbInt() << ", ";
    }
    if (widths_.size() > 0) {
        ss << "widths: ";
    }
    for (auto width : widths_) {
        ss << width << ", ";
    }
    if (styles_.size() > 0) {
        ss << "styles: ";
    }
    for (auto style : styles_) {
        ss << static_cast<uint32_t>(style) << ", ";
    }
    std::string output = ss.str();
    return output;
}

bool RSBorder::HasBorder() const
{
    return !colors_.empty() && !widths_.empty() && !styles_.empty() &&
        !std::all_of(colors_.begin(), colors_.end(), [](const Color& color) { return color.GetAlpha() == 0; }) &&
        !std::all_of(widths_.begin(), widths_.end(), [](const float& width) { return width <= 0.f; }) &&
        !std::all_of(
            styles_.begin(), styles_.end(), [](const BorderStyle& style) { return style == BorderStyle::NONE; });
}
} // namespace Rosen
} // namespace OHOS
