/*
 * Copyright 2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "drawing_painter_impl.h"

#include "include/core/SkBlurTypes.h"
#include "include/core/SkMaskFilter.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkDiscretePathEffect.h"

#include "skia_adapter/skia_paint.h"
#include "skia_adapter/skia_convert_utils.h"
#include "skia_adapter/skia_text_blob.h"
#include "skia_adapter/skia_path.h"

#include <array>

using namespace OHOS::Rosen;

namespace skia {
namespace textlayout {

static Drawing::Paint ConvertDecorStyle(const ParagraphPainter::DecorationStyle& decorStyle,
    Drawing::Paint::PaintStyle drawStyle = Drawing::Paint::PAINT_STROKE) {
    Drawing::Paint paint;
    paint.SetStyle(drawStyle);
    paint.SetAntiAlias(true);
    paint.SetColor(txt::PaintRecord::ToRSColor(decorStyle.getColor()));
    paint.SetWidth(decorStyle.getStrokeWidth());
    return paint;
}

RSCanvasParagraphPainter::RSCanvasParagraphPainter(RSCanvas* canvas,
    const std::vector<txt::PaintRecord>& paints)
    : fCanvas(canvas), paints(paints) {}

void RSCanvasParagraphPainter::drawTextBlob(const sk_sp<SkTextBlob>& blob,
    SkScalar x, SkScalar y, const SkPaintOrID& paint) {
    SkASSERT(!std::holds_alternative<SkPaint>(paint));
    const txt::PaintRecord& pr = paints[std::get<PaintID>(paint)];
    
    auto textBlobImpl = std::make_shared<Drawing::SkiaTextBlob>(blob);
    auto textBlob = std::make_shared<Drawing::TextBlob>(textBlobImpl);

    fCanvas->AttachBrush(pr.brush);
    fCanvas->AttachPen(pr.pen);
    fCanvas->DrawTextBlob(textBlob.get(), x, y);
    fCanvas->DetachPen();
    fCanvas->DetachBrush();
}

void RSCanvasParagraphPainter::drawTextShadow(const sk_sp<SkTextBlob>& blob,
    SkScalar x, SkScalar y, SkColor color, SkScalar blurSigma) {
    Drawing::Filter filter;
    filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(
        Drawing::BlurType::NORMAL, blurSigma, false
    ));

    Drawing::Brush brush;
    brush.SetColor(txt::PaintRecord::ToRSColor(color));
    brush.SetAntiAlias(true);
    brush.SetFilter(filter);

    auto textBlobImpl = std::make_shared<Drawing::SkiaTextBlob>(blob);
    auto textBlob = std::make_shared<Drawing::TextBlob>(textBlobImpl);

    fCanvas->AttachBrush(brush);
    fCanvas->DrawTextBlob(textBlob.get(), x, y);
    fCanvas->DetachBrush();
}

void RSCanvasParagraphPainter::drawRect(const SkRect& rect, const SkPaintOrID& paint) {
    SkASSERT(!std::holds_alternative<SkPaint>(paint));
    const txt::PaintRecord& pr = paints[std::get<PaintID>(paint)];
    Drawing::Rect rsRect;
    Drawing::SkiaConvertUtils::SkRectCastToDrawingRect(rect, rsRect);

    fCanvas->AttachBrush(pr.brush);
    fCanvas->AttachPen(pr.pen);
    fCanvas->DrawRect(rsRect);
    fCanvas->DetachPen();
    fCanvas->DetachBrush();
}

void RSCanvasParagraphPainter::drawFilledRect(const SkRect& rect, const DecorationStyle& decorStyle) {
    Drawing::Paint paint = ConvertDecorStyle(decorStyle, Drawing::Paint::PAINT_FILL);
    Drawing::Rect rsRect;
    Drawing::SkiaConvertUtils::SkRectCastToDrawingRect(rect, rsRect);

    fCanvas->AttachPaint(paint);
    fCanvas->DrawRect(rsRect);
    fCanvas->DetachPaint();
}

void RSCanvasParagraphPainter::drawPath(const SkPath& path, const DecorationStyle& decorStyle) {
    Drawing::Paint paint = ConvertDecorStyle(decorStyle);
    Drawing::Path rsPath;
    rsPath.GetImpl<Drawing::SkiaPath>()->SetPath(path);

    fCanvas->AttachPaint(paint);
    fCanvas->DrawPath(rsPath);
    fCanvas->DetachPaint();
}

void RSCanvasParagraphPainter::drawLine(SkScalar x0, SkScalar y0, SkScalar x1, SkScalar y1,
    const DecorationStyle& decorStyle) {
    Drawing::Paint paint = ConvertDecorStyle(decorStyle);
    Drawing::Point point0(x0, y0);
    Drawing::Point point1(x1, y1);

    fCanvas->AttachPaint(paint);
    fCanvas->DrawLine(point0, point1);
    fCanvas->DetachPaint();
}

void RSCanvasParagraphPainter::clipRect(const SkRect& rect) {
    Drawing::Rect rsRect;
    Drawing::SkiaConvertUtils::SkRectCastToDrawingRect(rect, rsRect);
    fCanvas->ClipRect(rsRect);
}

void RSCanvasParagraphPainter::translate(SkScalar dx, SkScalar dy) {
    fCanvas->Translate(dx, dy);
}

void RSCanvasParagraphPainter::save() {
    fCanvas->Save();
}

void RSCanvasParagraphPainter::restore() {
    fCanvas->Restore();
}

}  // namespace textlayout
}  // namespace skia
