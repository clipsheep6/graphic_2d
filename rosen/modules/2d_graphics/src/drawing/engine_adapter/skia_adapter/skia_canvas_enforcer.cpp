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

#include "utils/log.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkImageFilter.h"

#include "skia_canvas_enforcer.h"
#include "skia_path.h"
#include "skia_matrix.h"
#include "skia_image_filter.h"

namespace OHOS{
namespace Rosen {
namespace Drawing {
SkiaCanvasEnforcer::SkiaCanvasEnforcer(CoreCanvas& canvas)
    : SkNoDrawCanvas(canvas.GetLocalClipBounds().GetWidth(), canvas.GetLocalClipBounds().GetHeight())
{
    coreCanvas_ = &canvas;
}

SkiaCanvasEnforcer::~SkiaCanvasEnforcer() {}

void SkiaCanvasEnforcer::willSave()
{
    coreCanvas_->Save();
}

void SkiaCanvasEnforcer::willRestore()
{
    coreCanvas_->Restore();
}

void SkiaCanvasEnforcer::onFlush()
{
    coreCanvas_->Flush();
}

SkCanvas::SaveLayerStrategy SkiaCanvasEnforcer::getSaveLayerStrategy(const SaveLayerRec& rec)
{
    std::shared_ptr<Rect> drRect = nullptr;
    auto skRect = rec.fBounds;
    if (skRect) {
        drRect = std::make_shared<Rect>(skRect->left(), skRect->top(), skRect->right(), skRect->bottom());
    }

    std::shared_ptr<Brush> brush = nullptr;
    if (rec.fPaint) {
        skiaPaint_.SkPaintToBrush(*brush, *(rec.fPaint));
    }

    std::shared_ptr<ImageFilter> drImageFilter = nullptr;
    if (rec.fBackdrop) {
        drImageFilter = std::make_shared<ImageFilter>(ImageFilter::FilterType::NO_TYPE);
        auto skiaImageFilter = drImageFilter->GetImpl<SkiaImageFilter>();
        skiaImageFilter->SetSkImageFilter(sk_ref_sp(rec.fBackdrop));
    }

    Drawing::SaveLayerRec drSaveLayerRec(drRect.get(), brush.get(), drImageFilter.get(), rec.fSaveLayerFlags);
    coreCanvas_->SaveLayer(drSaveLayerRec);
    return SkCanvas::kNoLayer_SaveLayerStrategy;
}

void SkiaCanvasEnforcer::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
    Rect drRect(rect.left(), rect.top(), rect.right(), rect.bottom());

    AttachPaint(coreCanvas_, paint);
    coreCanvas_->DrawOval(drRect);
    DetachPaint(coreCanvas_, paint);
}

void SkiaCanvasEnforcer::onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[], const SkPaint& paint)
{
    AttachPaint(coreCanvas_, paint);

    if (mode == SkCanvas::kPoints_PointMode) {
        for (size_t i = 0; i < count; i++) {
            coreCanvas_->DrawPoint(Point(pts[i].x(), pts[i].y()));
        }
    } else if (mode == kLines_PointMode) {
        for (size_t i = 0; i + 1 < count; i = i + 2) {
            coreCanvas_->DrawLine(Point(pts[i].x(), pts[i].y()), Point(pts[i + 1].x(), pts[i + 1].y()));
        }
    } else if (mode == kPolygon_PointMode) {
        LOGE("TODO: PointMode is kPolygon_PointMode.");
    }

    DetachPaint(coreCanvas_, paint);
}

void SkiaCanvasEnforcer::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    Path drPath;
    auto skiaPathImpl = drPath.GetImpl<SkiaPath>();
    skiaPathImpl->SetPath(path);

    AttachPaint(coreCanvas_, paint);
    coreCanvas_->DrawPath(drPath);
    DetachPaint(coreCanvas_, paint);
}

void SkiaCanvasEnforcer::onDrawRRect(const SkRRect& rrect, const SkPaint& paint)
{
    Rect drRect(rrect.rect().left(), rrect.rect().top(), rrect.rect().right(), rrect.rect().bottom());

    std::vector<Point> radiusXY;
    radiusXY.emplace_back(rrect.radii(SkRRect::kUpperLeft_Corner).x(), rrect.radii(SkRRect::kUpperLeft_Corner).y());
    radiusXY.emplace_back(rrect.radii(SkRRect::kUpperRight_Corner).x(), rrect.radii(SkRRect::kUpperRight_Corner).y());
    radiusXY.emplace_back(rrect.radii(SkRRect::kLowerRight_Corner).x(), rrect.radii(SkRRect::kLowerRight_Corner).y());
    radiusXY.emplace_back(rrect.radii(SkRRect::kLowerLeft_Corner).x(), rrect.radii(SkRRect::kLowerLeft_Corner).y());

    RoundRect drRRect(drRect, radiusXY);

    AttachPaint(coreCanvas_, paint);
    coreCanvas_->DrawRoundRect(drRRect);
    DetachPaint(coreCanvas_, paint);
}

void SkiaCanvasEnforcer::didConcat(const SkMatrix& matrix)
{
    Matrix drMatrix;
    auto skiaMatrixImpl = drMatrix.GetImpl<SkiaMatrix>();
    skiaMatrixImpl->ImportMatrix(matrix);

    coreCanvas_->ConcatMatrix(drMatrix);
}

void SkiaCanvasEnforcer::didSetMatrix(const SkMatrix& matrix)
{
    Matrix drMatrix;
    auto skiaMatrixImpl = drMatrix.GetImpl<SkiaMatrix>();
    skiaMatrixImpl->ImportMatrix(matrix);

    coreCanvas_->SetMatrix(drMatrix);
}

void SkiaCanvasEnforcer::didTranslate(SkScalar dx, SkScalar dy)
{
    coreCanvas_->Translate(dx, dy);
}

void SkiaCanvasEnforcer::AttachPaint(CoreCanvas* canvas, const SkPaint& paint)
{
    if (paint.getStyle() == SkPaint::kFill_Style || paint.getStyle() == SkPaint::kStrokeAndFill_Style) {
        Brush brush;
        skiaPaint_.SkPaintToBrush(brush, paint);
        coreCanvas_->AttachBrush(brush);
    }

    if (paint.getStyle() == SkPaint::kStroke_Style || paint.getStyle() == SkPaint::kStrokeAndFill_Style) {
        Pen pen;
        skiaPaint_.SkPaintToPen(pen, paint);
        coreCanvas_->AttachPen(pen);
    }
}

void SkiaCanvasEnforcer::DetachPaint(CoreCanvas* canvas, const SkPaint& paint)
{
    if (paint.getStyle() == SkPaint::kFill_Style || paint.getStyle() == SkPaint::kStrokeAndFill_Style) {
        coreCanvas_->DetachBrush();
    }

    if (paint.getStyle() == SkPaint::kStroke_Style || paint.getStyle() == SkPaint::kStrokeAndFill_Style) {
        coreCanvas_->DetachPen();
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
