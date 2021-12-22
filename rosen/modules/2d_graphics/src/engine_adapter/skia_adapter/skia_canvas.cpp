/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "skia_canvas.h"

#include "image/bitmap.h"

namespace OHOS {
namespace Rosen {
SkiaCanvas::SkiaCanvas() : skiaCanvas_(nullptr), skiaPaint_() {}

SkiaCanvas::~SkiaCanvas() {}

static inline const SkMatrix& GetSkMatrix(const Matrix& matrix)
{
    const SkiaMatrix* m = static_cast<const SkiaMatrix*>(matrix.GetMatrixImpl());
    return m->ExportSkiaMatrix();
}

static inline const SkBitmap& GetSkBitmap(std::shared_ptr<BitmapImpl> bitmap)
{
    const SkiaBitmap* bmp = static_cast<const SkiaBitmap*>(bitmap.get());
    return (bmp->ExportSkiaBitmap());
}

void SkiaCanvas::Bind(const Bitmap& bitmap)
{
    skiaCanvas_ = std::make_shared<SkCanvas>(GetSkBitmap(bitmap.GetBitmapData()));
}

void SkiaCanvas::DrawPoint(const Point& point)
{
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawPoint(SkPoint::Make(point.GetX(), point.GetY()), d->paint);
        }
    }
}

void SkiaCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawLine(SkPoint::Make(startPt.GetX(), startPt.GetY()),
                SkPoint::Make(endPt.GetX(), endPt.GetY()), d->paint);
        }
    }
}

void SkiaCanvas::DrawRect(const Rect& rect)
{
    SkRect r = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawRect(r, d->paint);
        }
    }
}

void SkiaCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    SkRRect rRect;
    RoundRectCastToSkRRect(roundRect, rRect);
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawRRect(rRect, d->paint);
        }
    }
}

void SkiaCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    SkRRect outerRRect;
    RoundRectCastToSkRRect(outer, outerRRect);

    SkRRect innerRRect;
    RoundRectCastToSkRRect(inner, innerRRect);

    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawDRRect(outerRRect, innerRRect, d->paint);
        }
    }
}

void SkiaCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    SkRect arcRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawArc(arcRect, startAngle, sweepAngle, false, d->paint);
        }
    }
}

void SkiaCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    SkRect pieRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawArc(pieRect, startAngle, sweepAngle, true, d->paint);
        }
    }
}

void SkiaCanvas::DrawOval(const Rect& oval)
{
    SkRect ovalRect = SkRect::MakeLTRB(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawOval(ovalRect, d->paint);
        }
    }
}

void SkiaCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawCircle(centerPt.GetX(), centerPt.GetY(), radius, d->paint);
        }
    }
}

void SkiaCanvas::DrawPath(const Path& path)
{
    SkPath* p = static_cast<SkPath*>(path.GetPathData()->GetPath());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (p != nullptr && d != nullptr) {
            skiaCanvas_->drawPath(*p, d->paint);
        }
    }
}

void SkiaCanvas::DrawBackground(const Brush& brush)
{
    SkPaint paint;
    skiaPaint_.BrushToSkPaint(brush, paint);
    skiaCanvas_->drawPaint(paint);
}

void SkiaCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
                            scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    SkPath* p = static_cast<SkPath*>(path.GetPathData()->GetPath());
    SkPoint3 point1 =  SkPoint3::Make(planeParams.GetX(), planeParams.GetY(), planeParams.GetZ());
    SkPoint3 point2 =  SkPoint3::Make(devLightPos.GetX(), devLightPos.GetY(), devLightPos.GetZ());
    SkColor color1 = ambientColor.CastToColorQuad();
    SkColor color2 = spotColor.CastToColorQuad();
    SkShadowFlags flags = static_cast<SkShadowFlags>(flag);
    if (p != nullptr) {
        SkShadowUtils::DrawShadow(skiaCanvas_.get(), *p, point1, point2, color1, color2, flags);
    }
}

void SkiaCanvas::DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py)
{
    SkBitmap bmp = GetSkBitmap(bitmap.GetBitmapData());
    for (auto d : skiaPaint_.GetSortedPaints()) {
        if (d != nullptr) {
            skiaCanvas_->drawBitmap(bmp, px, py, &d->paint);
        }
    }
}

void SkiaCanvas::DrawImage() {} // TODO...

void SkiaCanvas::DrawText(const Text& text) {} // TODO...

void SkiaCanvas::ClipRect(const Rect& rect, ClipOp op)
{
    SkRect clipRect = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    SkClipOp clipOp = static_cast<SkClipOp>(op);
    skiaCanvas_->clipRect(clipRect, clipOp);
}

void SkiaCanvas::ClipRoundRect(const RoundRect& roundRect, ClipOp op)
{
    SkRRect rRect;
    RoundRectCastToSkRRect(roundRect, rRect);
    SkClipOp clipOp = static_cast<SkClipOp>(op);
    skiaCanvas_->clipRRect(rRect, clipOp);
}

void SkiaCanvas::ClipPath(const Path& path, ClipOp op)
{
    SkPath* p = static_cast<SkPath*>(path.GetPathData()->GetPath());
    if (p != nullptr) {
        SkClipOp clipOp = static_cast<SkClipOp>(op);
        skiaCanvas_->clipPath(*p, clipOp);
    }
}

void SkiaCanvas::SetMatrix(const Matrix& matrix)
{
    const SkMatrix m = GetSkMatrix(matrix);
    skiaCanvas_->setMatrix(m);
}

void SkiaCanvas::ResetMatrix()
{
    skiaCanvas_->resetMatrix();
}

void SkiaCanvas::ConcatMatrix(const Matrix& matrix)
{
    const SkMatrix m = GetSkMatrix(matrix);
    skiaCanvas_->concat(m);
}

void SkiaCanvas::Translate(scalar dx, scalar dy)
{
    skiaCanvas_->translate(dx, dy);
}

void SkiaCanvas::Scale(scalar sx, scalar sy)
{
    skiaCanvas_->scale(sx, sy);
}

void SkiaCanvas::Rotate(scalar deg)
{
    skiaCanvas_->rotate(deg);
}

void SkiaCanvas::Shear(scalar sx, scalar sy)
{
    skiaCanvas_->skew(sx, sy);
}

void SkiaCanvas::Flush()
{
    skiaCanvas_->flush();
}

void SkiaCanvas::Clear(ColorQuad color)
{
    skiaCanvas_->clear(color);
}

void SkiaCanvas::Save()
{
    skiaCanvas_->save();
}

void SkiaCanvas::SaveLayer(const Rect& rect, const Brush& brush)
{
    SkRect bounds = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    SkPaint paint;
    skiaPaint_.BrushToSkPaint(brush, paint);
    skiaCanvas_->saveLayer(bounds, &paint);
}

void SkiaCanvas::Restore()
{
    skiaCanvas_->restore();
}

void SkiaCanvas::AttachPen(const Pen& pen)
{
    skiaPaint_.ApplyPenToStroke(pen);
}

void SkiaCanvas::AttachBrush(const Brush& brush)
{
    skiaPaint_.ApplyBrushToFill(brush);
}

void SkiaCanvas::DetachPen()
{
    skiaPaint_.DisableStroke();
}

void SkiaCanvas::DetachBrush()
{
    skiaPaint_.DisableFill();
}

void SkiaCanvas::RoundRectCastToSkRRect(const RoundRect& roundRect, SkRRect& skRRect)
{
    Rect rect = roundRect.GetRect();
    SkRect outer = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());

    SkVector radii[4];
    Point p;

    p = roundRect.GetCornerRadius(RoundRect::TOP_LEFT_POS);
    radii[SkRRect::kUpperLeft_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::TOP_RIGHT_POS);
    radii[SkRRect::kUpperRight_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::BOTTOM_RIGHT_POS);
    radii[SkRRect::kLowerRight_Corner] = { p.GetX(), p.GetY() };
    p = roundRect.GetCornerRadius(RoundRect::BOTTOM_LEFT_POS);
    radii[SkRRect::kLowerLeft_Corner] = { p.GetX(), p.GetY() };

    skRRect.setRectRadii(outer, radii);
}
}
}
