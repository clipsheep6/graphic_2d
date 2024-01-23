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

#include "drawing_painter_impl.h"

#include <array>

#include "include/core/SkBlurTypes.h"
#include "include/core/SkMaskFilter.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkDiscretePathEffect.h"
#include "skia_adapter/skia_paint.h"
#include "skia_adapter/skia_path.h"
#include "skia_adapter/skia_text_blob.h"

#ifdef HM_SYMBOL_TXT_ENABLE
#include <parameters.h>

const bool G_IS_HM_SYMBOL_TXT_ENABLE =
    (std::atoi(OHOS::system::GetParameter("persist.sys.graphic.hmsymboltxt.enable", "1").c_str()) != 0);
#else
const bool G_IS_HM_SYMBOL_TXT_ENABLE = true;
#endif

namespace OHOS {
namespace Rosen {
namespace SPText {
static Drawing::Paint ConvertDecorStyle(const ParagraphPainter::DecorationStyle& decorStyle,
    Drawing::Paint::PaintStyle drawStyle = Drawing::Paint::PAINT_STROKE)
{
    Drawing::Paint paint;
    paint.SetStyle(drawStyle);
    paint.SetAntiAlias(true);
    paint.SetColor(PaintRecord::ToRSColor(decorStyle.getColor()));
    paint.SetWidth(decorStyle.getStrokeWidth());
    return paint;
}

static Drawing::Rect ToDrawingRect(const SkRect& skRect)
{
    Drawing::Rect rect;
    rect.SetLeft(skRect.fLeft);
    rect.SetTop(skRect.fTop);
    rect.SetRight(skRect.fRight);
    rect.SetBottom(skRect.fBottom);
    return rect;
}

RSCanvasParagraphPainter::RSCanvasParagraphPainter(Drawing::Canvas* canvas, const std::vector<PaintRecord>& paints)
    : canvas_(canvas), paints_(paints)
{}

void RSCanvasParagraphPainter::DrawSymbolSkiaTxt(RSTextBlob* blob, const RSPoint& offset,
    const PaintRecord &pr)
{
    if (pr.pen.has_value() && pr.brush.has_value()) {
        canvas_->AttachBrush(pr.brush.value());
        canvas_->AttachPen(pr.pen.value());
        HMSymbolRun::DrawSymbol(canvas_, blob, offset, pr.symbol);
        canvas_->DetachPen();
        canvas_->DetachBrush();
    } else if (pr.pen.has_value() && !pr.brush.has_value()) {
        canvas_->AttachPen(pr.pen.value());
        HMSymbolRun::DrawSymbol(canvas_, blob, offset,  pr.symbol);
        canvas_->DetachPen();
    } else if (!pr.pen.has_value() && pr.brush.has_value()) {
        canvas_->AttachBrush(pr.brush.value());
        HMSymbolRun::DrawSymbol(canvas_, blob, offset,  pr.symbol);
        canvas_->DetachBrush();
    } else {
        Drawing::Brush brush;
        brush.SetColor(pr.color);
        canvas_->AttachBrush(brush);
        HMSymbolRun::DrawSymbol(canvas_, blob, offset,  pr.symbol);
        canvas_->DetachBrush();
    }
}

void RSCanvasParagraphPainter::drawTextBlob(
    const sk_sp<SkTextBlob>& blob, SkScalar x, SkScalar y, const SkPaintOrID& paint)
{
    SkASSERT(!std::holds_alternative<SkPaint>(paint));
    const PaintRecord& pr = paints_[std::get<PaintID>(paint)];

    auto textBlobImpl = std::make_shared<Drawing::SkiaTextBlob>(blob);
    auto textBlob = std::make_shared<Drawing::TextBlob>(textBlobImpl);

    if (pr.isSymbolGlyph && G_IS_HM_SYMBOL_TXT_ENABLE) {
        std::vector<SkPoint> points;
        GetPointsForTextBlob(blob.get(), points);
        RSPoint offset;
        if (points.size() > 0) {
            offset = RSPoint{ x + points[0].x(), y + points[0].y() };
        } else {
            offset = RSPoint{ x, y };
        }
        SymbolAnimation(pr);
        DrawSymbolSkiaTxt(textBlob.get(), offset, pr);
    } else if (pr.pen.has_value() && pr.brush.has_value()) {
        canvas_->AttachPen(pr.pen.value());
        canvas_->DrawTextBlob(textBlob.get(), x, y);
        canvas_->DetachPen();
        canvas_->AttachBrush(pr.brush.value());
        canvas_->DrawTextBlob(textBlob.get(), x, y);
        canvas_->DetachBrush();
    } else if (pr.pen.has_value() && !pr.brush.has_value()) {
        canvas_->AttachPen(pr.pen.value());
        canvas_->DrawTextBlob(textBlob.get(), x, y);
        canvas_->DetachPen();
    } else if (!pr.pen.has_value() && pr.brush.has_value()) {
        canvas_->AttachBrush(pr.brush.value());
        canvas_->DrawTextBlob(textBlob.get(), x, y);
        canvas_->DetachBrush();
    } else {
        Drawing::Brush brush;
        brush.SetColor(pr.color);
        canvas_->AttachBrush(brush);
        canvas_->DrawTextBlob(textBlob.get(), x, y);
        canvas_->DetachBrush();
    }
}

void RSCanvasParagraphPainter::SymbolAnimation(const PaintRecord &pr)
{
    auto painterSymbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    if (painterSymbolAnimationConfig == nullptr) {
        return;
    }

    painterSymbolAnimationConfig->effectStrategy = TextEngine::SymbolAnimationEffectStrategy(
        pr.symbol.GetEffectStrategy());
    if (animationFunc_ != nullptr) {
        animationFunc_(painterSymbolAnimationConfig);
    }
}

void RSCanvasParagraphPainter::drawTextShadow(
    const sk_sp<SkTextBlob>& blob, SkScalar x, SkScalar y, SkColor color, SkScalar blurSigma)
{
    Drawing::Filter filter;
    filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, blurSigma, false));

    Drawing::Brush brush;
    brush.SetColor(PaintRecord::ToRSColor(color));
    brush.SetAntiAlias(true);
    brush.SetFilter(filter);

    auto textBlobImpl = std::make_shared<Drawing::SkiaTextBlob>(blob);
    auto textBlob = std::make_shared<Drawing::TextBlob>(textBlobImpl);

    canvas_->AttachBrush(brush);
    canvas_->DrawTextBlob(textBlob.get(), x, y);
    canvas_->DetachBrush();
}

void RSCanvasParagraphPainter::drawRect(const SkRect& rect, const SkPaintOrID& paint)
{
    SkASSERT(!std::holds_alternative<SkPaint>(paint));
    const PaintRecord& pr = paints_[std::get<PaintID>(paint)];
    Drawing::Rect rsRect = ToDrawingRect(rect);

    if (pr.pen.has_value()) {
        canvas_->AttachPen(pr.pen.value());
        canvas_->DrawRect(rsRect);
        canvas_->DetachPen();
    }
    if (pr.brush.has_value()) {
        canvas_->AttachBrush(pr.brush.value());
        canvas_->DrawRect(rsRect);
        canvas_->DetachBrush();
    }
}

void RSCanvasParagraphPainter::drawFilledRect(const SkRect& rect, const DecorationStyle& decorStyle)
{
    Drawing::Paint paint = ConvertDecorStyle(decorStyle, Drawing::Paint::PAINT_FILL);
    Drawing::Rect rsRect = ToDrawingRect(rect);

    canvas_->AttachPaint(paint);
    canvas_->DrawRect(rsRect);
    canvas_->DetachPaint();
}

void RSCanvasParagraphPainter::drawPath(const SkPath& path, const DecorationStyle& decorStyle)
{
    Drawing::Paint paint = ConvertDecorStyle(decorStyle);
    Drawing::Path rsPath;
    rsPath.GetImpl<Drawing::SkiaPath>()->SetPath(path);

    canvas_->AttachPaint(paint);
    canvas_->DrawPath(rsPath);
    canvas_->DetachPaint();
}

void RSCanvasParagraphPainter::drawLine(
    SkScalar x0, SkScalar y0, SkScalar x1, SkScalar y1, const DecorationStyle& decorStyle)
{
    Drawing::Paint paint = ConvertDecorStyle(decorStyle);
    Drawing::Point point0(x0, y0);
    Drawing::Point point1(x1, y1);

    canvas_->AttachPaint(paint);
    canvas_->DrawLine(point0, point1);
    canvas_->DetachPaint();
}

void RSCanvasParagraphPainter::clipRect(const SkRect& rect)
{
    Drawing::Rect rsRect = ToDrawingRect(rect);
    canvas_->ClipRect(rsRect);
}

void RSCanvasParagraphPainter::translate(SkScalar dx, SkScalar dy)
{
    canvas_->Translate(dx, dy);
}

void RSCanvasParagraphPainter::save()
{
    canvas_->Save();
}

void RSCanvasParagraphPainter::restore()
{
    canvas_->Restore();
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
