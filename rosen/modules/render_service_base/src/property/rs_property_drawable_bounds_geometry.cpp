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

#include "property/rs_property_drawable_bounds_geometry.h"

#include <utility>

#include "include/core/SkMaskFilter.h"
#include "include/core/SkPoint3.h"
#include "include/effects/SkImageFilters.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/utils/SkShadowUtils.h"
#include "src/image/SkImage_Base.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

namespace {
constexpr int PARAM_DOUBLE = 2;
} // namespace
namespace OHOS::Rosen {
bool RSBackgroundDrawable::forceBgAntiAlias_ = true;
// ============================================================================
// Bounds geometry
void RSBoundsGeometryDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.concat(node.GetRenderProperties().GetBoundsGeometry()->GetMatrix());
#else
    canvas.ConcatMatrix(node.GetRenderProperties().GetBoundsGeometry()->GetMatrix());
#endif
}
RSPropertyDrawable::DrawablePtr RSBoundsGeometryDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    return std::make_unique<RSBoundsGeometryDrawable>();
}

void RSClipBoundsDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    // Planning: Generate() should calculate the draw op and cache it
    auto& properties = node.GetMutableRenderProperties();
#ifndef USE_ROSEN_DRAWING
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else if (properties.GetClipToRRect()) {
        canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetClipRRect()));
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()), true);
    } else {
        canvas.clipRect(RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect()));
    }
#else
    if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipToRRect()) {
        canvas.ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetClipRRect()),
            Drawing::ClipOp::INTERSECT, false);
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()),
            Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect()),
            Drawing::ClipOp::INTERSECT, false);
    }
#endif
}

RSPropertyDrawable::DrawablePtr RSClipBoundsDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    return std::make_unique<RSClipBoundsDrawable>();
}

// ============================================================================
// Border
RSPropertyDrawable::DrawablePtr RSBorderDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto& border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setAntiAlias(true);
    if (border->ApplyFillStyle(paint)) {
        return std::make_unique<RSBorderDRRectDrawable>(std::move(paint), properties);
    } else if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(paint)) {
        return std::make_unique<RSBorderFourLineDrawable>(std::move(paint), properties);
    } else if (border->ApplyPathStyle(paint)) {
        return std::make_unique<RSBorderPathDrawable>(std::move(paint), properties);
    } else {
        return std::make_unique<RSBorderFourLineRoundCornerDrawable>(std::move(paint), properties);
    }
#else
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        return std::make_unique<RSBorderDRRectDrawable>(std::move(brush), std::move(pen), properties);
    } else if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(pen)) {
        return std::make_unique<RSBorderFourLineDrawable>(std::move(brush), std::move(pen), properties);
    } else if (border->ApplyPathStyle(pen)) {
        return std::make_unique<RSBorderPathDrawable>(std::move(brush), std::move(pen), properties);
    } else {
        return std::make_unique<RSBorderFourLineRoundCornerDrawable>(std::move(brush), std::move(pen), properties);
    }
#endif
}
#ifndef USE_ROSEN_DRAWING
RSBorderDRRectDrawable::RSBorderDRRectDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
#else
RSBorderDRRectDrawable::RSBorderDRRectDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{}
void RSBorderDRRectDrawable::OnBoundsChange(const RSProperties& properties)
{
#ifndef USE_ROSEN_DRAWING
    inner_ = RSPropertiesPainter::RRect2SkRRect(properties.GetInnerRRect());
    outer_ = RSPropertiesPainter::RRect2SkRRect(properties.GetRRect());
#else
    inner_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetInnerRRect());
    outer_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect());
#endif
}
void RSBorderDRRectDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.drawDRRect(outer_, inner_, paint_);
#else
    canvas.AttachPen(pen_);
    canvas.AttachBrush(brush_);
    canvas.DrawNestedRoundRect(outer_, inner_);
    canvas.DetachPen();
    canvas.DetachBrush();
#endif
}

#ifndef USE_ROSEN_DRAWING
RSBorderFourLineDrawable::RSBorderFourLineDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
#else
RSBorderFourLineDrawable::RSBorderFourLineDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{}
void RSBorderFourLineDrawable::OnBoundsChange(const RSProperties& properties)
{
    rect_ = properties.GetBoundsRect();
}
void RSBorderFourLineDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    node.GetMutableRenderProperties().GetBorder()->PaintFourLine(canvas, paint_, rect_);
#else
    node.GetMutableRenderProperties().GetBorder()->PaintFourLine(canvas, pen_, rect_);
#endif
}

#ifndef USE_ROSEN_DRAWING
RSBorderPathDrawable::RSBorderPathDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
#else
RSBorderPathDrawable::RSBorderPathDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
    const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{}
void RSBorderPathDrawable::OnBoundsChange(const RSProperties& properties)
{
    auto borderWidth = properties.GetBorder()->GetWidth();
    RRect rrect = properties.GetRRect();
    rrect.rect_.width_ -= borderWidth;
    rrect.rect_.height_ -= borderWidth;
    rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
#ifndef USE_ROSEN_DRAWING
    borderPath_.reset();
    borderPath_.addRRect(RSPropertiesPainter::RRect2SkRRect(rrect));
#else
    borderPath_.Reset();
    borderPath_.AddRoundRect(RSPropertiesPainter::RRect2DrawingRRect(rrect));
#endif
}
void RSBorderPathDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.drawPath(borderPath_, paint_);
#else
    canvas.AttachPen(pen_);
    canvas.AttachBrush(brush_);
    canvas.DrawPath(borderPath_, paint_);
    canvas.DetachPen();
    canvas.DetachBrush();
#endif
}

#ifndef USE_ROSEN_DRAWING
RSBorderFourLineRoundCornerDrawable::RSBorderFourLineRoundCornerDrawable(
    SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
#else
RSBorderFourLineRoundCornerDrawable::RSBorderFourLineRoundCornerDrawable(
    Drawing::Brush&& brush, Drawing::Pen&& pen, const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{
#ifndef USE_ROSEN_DRAWING
    paint_.setStyle(SkPaint::Style::kStroke_Style);
#endif
}
void RSBorderFourLineRoundCornerDrawable::OnBoundsChange(const RSProperties& properties)
{
#ifndef USE_ROSEN_DRAWING
    innerRrect_ = RSPropertiesPainter::RRect2SkRRect(properties.GetInnerRRect());
    rrect_ = RSPropertiesPainter::RRect2SkRRect(properties.GetRRect());
#else
    innerRrect_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetInnerRRect());
    rrect_ = RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect());
#endif
}
void RSBorderFourLineRoundCornerDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    auto& properties = node.GetMutableRenderProperties();
    canvas.clipRRect(innerRrect_, SkClipOp::kDifference, true);
    properties.GetBorder()->PaintTopPath(canvas, paint_, rrect_);
    properties.GetBorder()->PaintRightPath(canvas, paint_, rrect_);
    properties.GetBorder()->PaintBottomPath(canvas, paint_, rrect_);
    properties.GetBorder()->PaintLeftPath(canvas, paint_, rrect_);
#else
    Drawing::AutoCanvasRestore acr(&canvas, true);
    auto& properties = node.GetMutableRenderProperties();
    canvas.ClipRRect(innerRrect_, Drawing::ClipOp::DIFFERENCE, true);
    properties.GetBorder()->PaintTopPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintRightPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintBottomPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintLeftPath(canvas, pen_, rrect_);
#endif
}

// ============================================================================
// Mask
std::unique_ptr<RSPropertyDrawable> RSMaskDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        return nullptr;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        return nullptr;
    }
    if (mask->IsSvgMask()) {
        if (mask->GetSvgDom()) {
            return std::make_unique<RSSvgDomMaskDrawable>(mask, properties.GetBoundsRect());
        } else if (mask->GetSvgPicture()) {
            return std::make_unique<RSSvgPictureMaskDrawable>(mask, properties.GetBoundsRect());
        }
    } else if (mask->IsGradientMask()) {
        return std::make_unique<RSGradientMaskDrawable>(mask, properties.GetBoundsRect());
    } else if (mask->IsPathMask()) {
        return std::make_unique<RSPathMaskDrawable>(mask, properties.GetBoundsRect());
    }
    return nullptr;
}

RSMaskDrawable::RSMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds) : mask_(std::move(mask))
{
#ifndef USE_ROSEN_DRAWING
    maskBounds_ = RSPropertiesPainter::Rect2SkRect(bounds);
    maskPaint_.setBlendMode(SkBlendMode::kSrcIn);
    auto filter = SkColorFilters::Compose(SkLumaColorFilter::Make(), SkColorFilters::SRGBToLinearGamma());
    maskFilter_.setColorFilter(filter);
#else
    maskBounds_ = RSPropertiesPainter::Rect2DrawingRect(bounds);
    maskBrush_.SetBlendMode(Drawing::BlendMode::SRC_IN);
    Drawing::Filter filter;
    filter.SetColorFilter(Drawing::ColorFilter::CreateComposeColorFilter(
        *(Drawing::ColorFilter::CreateLumaColorFilter()), *(Drawing::ColorFilter::CreateSrgbGammaToLinear())
    ))
    maskFilterBrush_.setColorFilter(filter);
#endif
}

RSSvgDomMaskDrawable::RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds) : RSMaskDrawable(mask, bounds) {}

void RSSvgDomMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.save();
    int tmpLayer = canvas.saveLayer(maskBounds_, nullptr);
    canvas.saveLayer(maskBounds_, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(maskBounds_.fLeft + mask_->GetSvgX(), maskBounds_.fTop + mask_->GetSvgY());
        canvas.scale(mask_->GetScaleX(), mask_->GetScaleY());
        mask_->GetSvgDom()->render(&canvas);
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(maskBounds_, &maskPaint_);
    canvas.clipRect(maskBounds_, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&maskBounds_, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&maskBounds_, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(&canvas, true);
        canvas.Translate(maskBounds_.GetLeft() + mask_->GetSvgX(), maskBounds_.GetTop() + mask_->GetSvgY());
        canvas.Scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.DrawSVGDOM(mask_->GetSvgDom());
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&maskBounds_, &maskPaint_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(maskBounds_, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSSvgPictureMaskDrawable::RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    : RSMaskDrawable(mask, bounds)
{}

void RSSvgPictureMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.save();
    int tmpLayer = canvas.saveLayer(maskBounds_, nullptr);
    canvas.saveLayer(maskBounds_, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(maskBounds_.fLeft + mask_->GetSvgX(), maskBounds_.fTop + mask_->GetSvgY());
        canvas.scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.drawPicture(mask_->GetSvgPicture());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(maskBounds_, &maskPaint_);
    canvas.clipRect(maskBounds_, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&maskBounds_, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&maskBounds_, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(&canvas, true);
        canvas.Translate(maskBounds_.GetLeft() + mask_->GetSvgX(), maskBounds_.GetTop() + mask_->GetSvgY());
        canvas.Scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.DrawPicture(mask_->GetSvgPicture());
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&maskBounds_, &maskPaint_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(maskBounds_, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSGradientMaskDrawable::RSGradientMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    : RSMaskDrawable(mask, bounds)
{}

void RSGradientMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.save();
    int tmpLayer = canvas.saveLayer(maskBounds_, nullptr);
    canvas.saveLayer(maskBounds_, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(maskBounds_.fLeft, maskBounds_.fTop);
        SkRect rect = SkRect::MakeIWH(maskBounds_.fRight - maskBounds_.fLeft, maskBounds_.fBottom - maskBounds_.fTop);
        canvas.drawRect(rect, mask_->GetMaskPaint());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(maskBounds_, &maskPaint_);
    canvas.clipRect(maskBounds_, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&maskBounds_, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&maskBounds_, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(&canvas, true);
        canvas.translate(maskBounds_.GetLeft(), maskBounds_.GetTop());
        Drawing::Rect rect = Drawing::Rect(0, 0, maskBounds_.GetRight() - maskBounds_.GetLeft(),
            maskBounds_.GetBottom() - maskBounds_.GetTop());
        canvas.AttachBrush(mask_->GetMaskPaint());
        canvas.DrawRect(rect);
        canvas.DetachBrush();
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&maskBounds_, &maskPaint_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(maskBounds_, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSPathMaskDrawable::RSPathMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds) : RSMaskDrawable(mask, bounds) {}

void RSPathMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.save();
    int tmpLayer = canvas.saveLayer(maskBounds_, nullptr);
    canvas.saveLayer(maskBounds_, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(maskBounds_.fLeft, maskBounds_.fTop);
        canvas.drawPath(mask_->GetMaskPath(), mask_->GetMaskPaint());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(maskBounds_, &maskPaint_);
    canvas.clipRect(maskBounds_, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&maskBounds_, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&maskBounds_, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(&canvas, true);
        canvas.translate(maskBounds_.GetLeft(), maskBounds_.GetTop());
        canvas.AttachBrush(mask_->GetMaskPaint());
        canvas.DrawPath(*mask_->GetMaskPath());
        canvas.DetachBrush();
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&maskBounds_, &maskPaint_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(maskBounds_, Drawing::ClipOp::INTERSECT, true);
#endif
}

// ============================================================================
// Shadow
std::unique_ptr<RSPropertyDrawable> RSShadowBaseDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    if (properties.IsSpherizeValid() || !properties.IsShadowValid()) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkPath skPath;
#else
    Drawing::Path path;
#endif
    // PLANNING: Generate SkPath and deal with LeashWindow

    if (properties.GetShadowMask()) {
#ifndef USE_ROSEN_DRAWING
        return std::make_unique<RSColorfulShadowDrawable>(skPath, properties);
#else
        return std::make_unique<RSColorfulShadowDrawable>(path, properties);
#endif
    } else {
        if (properties.GetShadow()->GetHardwareAcceleration()) {
            if (properties.GetShadowElevation() <= 0.f) {
                return nullptr;
            }
#ifndef USE_ROSEN_DRAWING
            return std::make_unique<RSHardwareAccelerationShadowDrawable>(skPath, properties);
#else
            return std::make_unique<RSHardwareAccelerationShadowDrawable>(path, properties);
#endif
        } else {
#ifndef USE_ROSEN_DRAWING
            return std::make_unique<RSShadowDrawable>(skPath, properties);
#else
            return std::make_unique<RSShadowDrawable>(path, properties);
#endif
        }
    }
}

#ifndef USE_ROSEN_DRAWING
RSShadowBaseDrawable::RSShadowBaseDrawable(SkPath skPath, const RSProperties& properties) : skPath_(skPath)
#else
RSShadowBaseDrawable::RSShadowBaseDrawable(Drawing::Path path, const RSProperties& properties) : path_(path)
#endif
{
    offsetX_ = properties.GetShadowOffsetX();
    offsetY_ = properties.GetShadowOffsetY();
    color_ = properties.GetShadowColor();
}

#ifndef USE_ROSEN_DRAWING
RSShadowDrawable::RSShadowDrawable(SkPath skPath, const RSProperties& properties)
    : RSShadowBaseDrawable(skPath, properties)
#else
RSShadowDrawable::RSShadowDrawable(Drawing::Path path, const RSProperties& properties)
    : RSShadowBaseDrawable(path, properties)
#endif
{
    radius_ = properties.GetShadowRadius();
}

void RSShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    skPath_.offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas.setMatrix(matrix);
    SkPaint paint;
    paint.setColor(color_.AsArgbInt());
    paint.setAntiAlias(true);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, radius_));
    canvas.drawPath(skPath_, paint);
#else
    path_.Offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::SCALE_X, std::ceil(matrix.Get(Drawing::Matrix::SCALE_X)));
    matrix.Set(Drawing::Matrix::SCALE_Y, std::ceil(matrix.Get(Drawing::Matrix::SCALE_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Brush brush;
    brush.SetColor(color_.AsArgbInt());
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, radius_));
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    canvas.DrawPath(path_);
    canvas.DetachBrush();
#endif
}

#ifndef USE_ROSEN_DRAWING
RSHardwareAccelerationShadowDrawable::RSHardwareAccelerationShadowDrawable(
    SkPath skPath, const RSProperties& properties)
    : RSShadowBaseDrawable(skPath, properties)
#else
RSHardwareAccelerationShadowDrawable::RSHardwareAccelerationShadowDrawable(
    Drawing::Path path, const RSProperties& properties)
    : RSShadowBaseDrawable(path, properties)
#endif
{
    shadowElevation_ = properties.GetShadowElevation();
}

void RSHardwareAccelerationShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    skPath_.offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas.setMatrix(matrix);
    SkPoint3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    SkPoint3 lightPos = { canvas.getTotalMatrix().getTranslateX() + skPath_.getBounds().centerX(),
        canvas.getTotalMatrix().getTranslateY() + skPath_.getBounds().centerY(), DEFAULT_LIGHT_HEIGHT };
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas.GetAlpha());
    color_.MultiplyAlpha(canvas.GetAlpha());
    SkShadowUtils::DrawShadow(&canvas, skPath_, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
        ambientColor.AsArgbInt(), color_.AsArgbInt(), SkShadowFlags::kTransparentOccluder_ShadowFlag);
#else
    path_.Offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::SCALE_X, std::ceil(matrix.Get(Drawing::Matrix::SCALE_X)));
    matrix.Set(Drawing::Matrix::SCALE_Y, std::ceil(matrix.Get(Drawing::Matrix::SCALE_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Point3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    Drawing::scalar centerX = path_.GetBounds().GetLeft() + path_.GetBounds().GetWidth() / 2;
    Drawing::scalar centerY = path_.GetBounds().GetTop() + path_.GetBounds().GetHeight() / 2;
    Drawing::Point3 lightPos = { canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_X) + centerX,
        canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_Y) + centerY, DEFAULT_LIGHT_HEIGHT };
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas.GetAlpha());
    color_.MultiplyAlpha(canvas.GetAlpha());
    canvas.DrawShadow(path_, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
        Drawing::Color(ambientColor.AsArgbInt()), Drawing::Color(color_.AsArgbInt()),
        Drawing::ShadowFlags::TRANSPARENT_OCCLUDER);
#endif
}

#ifndef USE_ROSEN_DRAWING
RSColorfulShadowDrawable::RSColorfulShadowDrawable(SkPath skPath, const RSProperties& properties)
    : RSShadowBaseDrawable(skPath, properties)
#else
RSColorfulShadowDrawable::RSColorfulShadowDrawable(Drawing::Path path, const RSProperties& properties)
    : RSShadowBaseDrawable(path, properties)
#endif
{
#ifndef USE_ROSEN_DRAWING
    const SkScalar blurRadius =
        properties.GetShadow()->GetHardwareAcceleration()
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();
    blurPaint_.setImageFilter(SkImageFilters::Blur(blurRadius, blurRadius, SkTileMode::kDecal, nullptr));
#else
    const Drawing::scalar blurRadius =
        properties.GetShadow()->GetHardwareAcceleration()
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();
    Drawing::Filter filter;
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        blurRadius, blurRadius, Drawing::TileMode::DECAL, nullptr
    ))
    blurBrush_.SetFilter(filter);
#endif
    node_ = properties.backref_;
}

void RSColorfulShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    // save layer, draw image with clipPath, blur and draw back
    canvas.saveLayer(nullptr, &blurPaint_);
    canvas.translate(offsetX_, offsetY_);
    canvas.clipPath(skPath_);
#else
    Drawing::AutoCanvasRestore acr(&canvas, true);
    // save layer, draw image with clipPath, blur and draw back
    Drawing::SaveLayerOps slr(nullptr, &blurBrush_);
    canvas.SaveLayer(slr);
    canvas.Translate(offsetX_, offsetY_);
    canvas.ClipPath(path_, Drawing::ClipOp::INTERSECT, false);
#endif
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(node_.lock())) {
        node->InternalDrawContent(canvas);
    }
}

// ============================================================================
// DynamicLightUp
std::unique_ptr<RSPropertyDrawable> RSDynamicLightUpDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    if (!properties.IsDynamicLightUpValid()) {
        return nullptr;
    }
    return std::make_unique<RSDynamicLightUpDrawable>(
        properties.GetDynamicLightUpRate().value(), properties.GetDynamicLightUpDegree().value());
}

RSDynamicLightUpDrawable::RSDynamicLightUpDrawable(float rate, float degree) : rate_(rate), degree_(degree) {}

void RSDynamicLightUpDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    auto clipBounds = canvas.getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = RSPropertiesPainter::MakeDynamicLightUpShader(rate_, degree_, imageShader);
    SkPaint paint;
    paint.setShader(shader);
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
}

// ============================================================================
// BackgroundEffect
std::unique_ptr<RSPropertyDrawable> RSEffectDataApplyDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    if (context.properties_.GetUseEffect() == false) {
        return nullptr;
    }
    return std::make_unique<RSEffectDataApplyDrawable>();
}

void RSEffectDataApplyDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    const auto& effectData = canvas.GetEffectData();
    if (effectData == nullptr || effectData->cachedImage_ == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.resetMatrix();
    auto visibleIRect = canvas.GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        canvas.clipIRect(visibleIRect);
    }

    // dstRect: canvas clip region
    auto dstRect = SkRect::Make(canvas.getDeviceClipBounds());
    // srcRect: map dstRect onto cache coordinate
    auto srcRect = dstRect.makeOffset(-effectData->cachedRect_.left(), -effectData->cachedRect_.top());
    SkPaint defaultPaint;
    canvas.drawImageRect(effectData->cachedImage_, srcRect, dstRect, SkSamplingOptions(), &defaultPaint,
        SkCanvas::kFast_SrcRectConstraint);
}

// ============================================================================
// LightUpEffect
void RSLightUpEffectDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    auto& properties = node.GetMutableRenderProperties();
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSLightUpEffectDrawable::Draw skSurface is null");
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    auto clipBounds = canvas.getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSLightUpEffectDrawable::Draw image is null");
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = RSPropertiesPainter::MakeLightUpEffectShader(properties.GetLightUpEffect(), imageShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.resetMatrix();
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
#endif
#endif
}

std::unique_ptr<RSPropertyDrawable> RSLightUpEffectDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    if (!context.properties_.IsLightUpEffectValid()) {
        return nullptr;
    }
    return std::make_unique<RSLightUpEffectDrawable>();
}

void RSBackgroundFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        return;
    }
    RSFilterDrawable::DrawFilter(node, canvas, BACKGROUND_FILTER, filter_);
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundFilterDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    if (!RSFilterDrawable::GetBlurEnabled()) {
        ROSEN_LOGD("RSBackgroundFilterDrawable::Generate close blur.");
        return nullptr;
    }
    auto& filter = context.properties_.GetBackgroundFilter();
    if (filter == nullptr) {
        return nullptr;
    }
    if (context.node_->IsInstanceOf<RSEffectRenderNode>()) {
        // for RSEffectRenderNode, we just use generates effect data, instead of draw filter
        return std::make_unique<RSEffectDataGenerateDrawable>(filter);
    }
    return std::make_unique<RSBackgroundFilterDrawable>(filter);
}

bool RSFilterDrawable::GetBlurEnabled()
{
    static bool BLUR_ENABLED = RSSystemProperties::GetBlurEnabled();
    return BLUR_ENABLED;
}

void RSFilterDrawable::DrawFilter(
    RSRenderNode& node, RSPaintFilterCanvas& canvas, FilterType filterType, const std::shared_ptr<RSFilter>& rsFilter)
{
    auto& properties = node.GetMutableRenderProperties();
    RS_OPTIONAL_TRACE_NAME("DrawFilter " + rsFilter->GetDescription());

    auto filter = std::static_pointer_cast<RSSkiaFilter>(rsFilter);
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSFilterDrawable::DrawFilter skSurface null");
        auto paint = filter->GetPaint();
        SkCanvas::SaveLayerRec slr(nullptr, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
        canvas.saveLayer(slr);
        filter->PostProcess(canvas);
        return;
    }

    // for foreground filter, when do online opacity, rendering result already applied opacity,
    // so drawImage should not apply opacity again
    RSAutoCanvasRestore autoCanvasRestore(&canvas,
        filterType == FilterType::FOREGROUND_FILTER ? RSPaintFilterCanvas::kAlpha : RSPaintFilterCanvas::kNone);
    if (filterType == FilterType::FOREGROUND_FILTER) {
        canvas.SetAlpha(1.0);
    }

#if defined(RS_ENABLE_GL)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(filterType == FilterType::FOREGROUND_FILTER);
        cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
        cacheManager->DrawFilter(canvas, filter);
        return;
    }
#endif
    auto clipIBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(clipIBounds.makeOutset(-1, -1));
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSFilterDrawable::DrawFilter image null");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->width(), imageSnapshot->height())) {
        ROSEN_LOGD("RSFilterDrawable cache image resource(width:%{public}d, height:%{public}d).",
            imageSnapshot->width(), imageSnapshot->height());
        as_IB(imageSnapshot)->hintCacheGpuResource();
    }

    filter->PreProcess(imageSnapshot);
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.resetMatrix();
    auto visibleIRect = canvas.GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        canvas.clipIRect(visibleIRect);
    }
    filter->DrawImageRect(canvas, imageSnapshot, SkRect::Make(imageSnapshot->bounds()), SkRect::Make(clipIBounds));
    filter->PostProcess(canvas);
}

void RSForegroundFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSFilterDrawable::DrawFilter(node, canvas, FOREGROUND_FILTER, filter_);
}

std::unique_ptr<RSPropertyDrawable> RSForegroundFilterDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    if (!RSFilterDrawable::GetBlurEnabled()) {
        ROSEN_LOGD("RSForegroundFilterDrawable::Generate close blur.");
        return nullptr;
    }
    auto& filter = context.properties_.GetFilter();
    if (filter == nullptr) {
        return nullptr;
    }
    return std::make_unique<RSForegroundFilterDrawable>(filter);
}

void RSLinearGradientBlurFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifdef NEW_SKIA
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);

    auto clipBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipBounds.makeOutset(-1, -1);
    SkMatrix mat = canvas.getTotalMatrix();
    uint8_t directionBias = RSPropertiesPainter::CalcDirectionBias(mat);
    auto alphaGradientShader =
        RSPropertiesPainter::MakeAlphaGradientShader(SkRect::Make(clipIPadding), para_, directionBias);
    if (alphaGradientShader == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilterDrawable::Draw alphaGradientShader null");
        return;
    }
    float radius = para_->blurRadius_ / 2;
    canvas.resetMatrix();
    canvas.translate(clipIPadding.left(), clipIPadding.top());

    RSPropertiesPainter::DrawHorizontalLinearGradientBlur(
        skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawVerticalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawHorizontalLinearGradientBlur(
        skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawVerticalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
#endif
}

std::unique_ptr<RSPropertyDrawable> RSLinearGradientBlurFilterDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    const auto& para = context.properties_.GetLinearGradientBlurPara();
    if (para == nullptr || para->blurRadius_ <= 0) {
        return nullptr;
    }
    return std::make_unique<RSLinearGradientBlurFilterDrawable>(para);
}

void RSForegroundColorDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetMutableRenderProperties();
#ifndef USE_ROSEN_DRAWING
    canvas.drawRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()), paint_);
#else
    canvas.AttachBrush(brush_);
    canvas.DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
#endif
}

std::unique_ptr<RSPropertyDrawable> RSForegroundColorDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto bgColor = context.properties_.GetForegroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setColor(bgColor.AsArgbInt());
    paint.setAntiAlias(true);
    return std::make_unique<RSForegroundColorDrawable>(std::move(paint));
#else
    Drawing::Brush brush;
    brush.SetColor(bgColor.AsArgbInt());
    brush.setAntiAlias(true);
    return std::make_unique<RSForegroundColorDrawable>(std::move(brush));
#endif
}

std::unique_ptr<RSPropertyDrawable> RSParticleDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto particleVector = properties.GetParticles();
    if (particleVector.GetParticleSize() == 0) {
        return nullptr;
    }
    auto uniParticleDrawable = std::make_unique<RSParticleDrawable>();
    const auto& particles = particleVector.GetParticleVector();
    auto bounds = properties.GetBoundsRect();
    for (auto& particle : particles) {
        if (particle != nullptr && particle->IsAlive()) {
            auto position = particle->GetPosition();
            if (!(bounds.Intersect(position.x_, position.y_))) {
                continue;
            }
            float opacity = particle->GetOpacity();
            float scale = particle->GetScale();
            if (opacity <= 0.f || scale <= 0.f) {
                continue;
            }
            auto particleType = particle->GetParticleType();
#ifndef USE_ROSEN_DRAWING
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setAlphaf(opacity);
            auto clipBounds = RSPropertiesPainter::Rect2SkRect(bounds);
#else
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            brush.SetAlphaf(opacity);
            auto clipBounds = RSPropertiesPainter::Rect2DrawingRect(bounds);
#endif
            if (particleType == ParticleType::POINTS) {
                Color color = particle->GetColor();
                auto alpha = color.GetAlpha();
                color.SetAlpha(alpha * opacity);
#ifndef USE_ROSEN_DRAWING
                paint.setColor(color.AsArgbInt());
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSPointParticleDrawable>(std::move(paint), particle, clipBounds));
#else
                brush.SetColor(color.AsArgbInt());
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSPointParticleDrawable>(std::move(brush), particle, clipBounds));
#endif
            } else {
#ifndef USE_ROSEN_DRAWING
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSImageParticleDrawable>(std::move(paint), particle, clipBounds));
#else
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSImageParticleDrawable>(std::move(brush), particle, clipBounds));
#endif
            }
        }
    }
    return std::move(uniParticleDrawable);
}

void RSParticleDrawable::AddPropertyDrawable(std::shared_ptr<RSPropertyDrawable> drawable)
{
    propertyDrawableVec_.emplace_back(drawable);
}

void RSParticleDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    for (auto& propertyDrawable : propertyDrawableVec_) {
        if (propertyDrawable != nullptr) {
            propertyDrawable->Draw(node, canvas);
        }
    }
}

void RSPointParticleDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto radius = particles_->GetRadius();
    auto position = particles_->GetPosition();
    float scale = particles_->GetScale();
#ifndef USE_ROSEN_DRAWING
    canvas.clipRect(bounds_, true);
    canvas.drawCircle(position.x_, position.y_, radius * scale, paint_);
#else
    canvas.ClipRect(bounds_, Drawing::ClipOp::INTERSECT, true);
    canvas.AttachBrush(brush_);
    canvas.DrawCircle(Drawing::Point(position.x_, position.y_), radius * scale);
    canvas.DetachBrush();
#endif
}

void RSImageParticleDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto imageSize = particles_->GetImageSize();
    auto image = particles_->GetImage();
#ifndef USE_ROSEN_DRAWING
    canvas.clipRect(bounds_, true);
#else
    canvas.ClipRect(bounds_, Drawing::ClipOp::INTERSECT, true);
#endif
    auto position = particles_->GetPosition();
    float left = position.x_;
    float top = position.y_;
    float scale = particles_->GetScale();
    float right = position.x_ + imageSize.x_ * scale;
    float bottom = position.y_ + imageSize.y_ * scale;
#ifndef USE_ROSEN_DRAWING
    canvas.save();
    canvas.translate(position.x_, position.y_);
    canvas.rotate(particles_->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
#else
    canvas.Save();
    canvas.Translate(position.x_, position.y_);
    canvas.Rotate(particles_->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
#endif
    RectF destRect(left, top, right, bottom);
    image->SetDstRect(destRect);
    image->SetScale(scale);
    image->SetImageRepeat(0);
#ifndef USE_ROSEN_DRAWING
    SkRect rect { left, top, right, bottom };
#ifdef NEW_SKIA
    image->CanvasDrawImage(canvas, rect, SkSamplingOptions(), paint_, false);
    canvas.restore();
#else
    image->CanvasDrawImage(canvas, rect, paint_, false);
    canvas.restore();
#endif
#else
    Drawing::Rect rect { left, top, right, bottom };
    canvas.AttachBrush(brush_);
    image->CanvasDrawImage(canvas, rect, false);
    canvas.DetachBrush();
    canvas.Restore();
#endif
}

void RSPixelStretchDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetMutableRenderProperties();
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPixelStretchDrawable::Draw skSurface null");
        return;
    }

    auto clipBounds = canvas.getDeviceClipBounds();
    clipBounds.setXYWH(clipBounds.left(), clipBounds.top(), clipBounds.width() - 1, clipBounds.height() - 1);

    /* Calculates the relative coordinates of the clipbounds
        with respect to the origin of the current canvas coordinates */
    SkMatrix worldToLocalMat;
    if (!canvas.getTotalMatrix().invert(&worldToLocalMat)) {
        ROSEN_LOGE("RSPixelStretchDrawable::Draw get invert matrix failed.");
    }
    SkRect localClipBounds;
    SkRect fClipBounds = SkRect::MakeXYWH(clipBounds.x(), clipBounds.y(), clipBounds.width(), clipBounds.height());
    if (!worldToLocalMat.mapRect(&localClipBounds, fClipBounds)) {
        ROSEN_LOGE("RSPixelStretchDrawable::Draw map rect failed.");
    }

    if (!bounds_.intersect(localClipBounds)) {
        ROSEN_LOGE("RSPixelStretchDrawable::Draw intersect clipbounds failed");
    }

    auto scaledBounds = SkRect::MakeLTRB(bounds_.left() - pixelStretch_.x_, bounds_.top() - pixelStretch_.y_,
        bounds_.right() + pixelStretch_.z_, bounds_.bottom() + pixelStretch_.w_);
    if (scaledBounds.isEmpty() || bounds_.isEmpty() || clipBounds.isEmpty()) {
        ROSEN_LOGE("RSPixelStretchDrawable::Draw invalid scaled bounds");
        return;
    }

    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSPixelStretchDrawable::Draw image null");
        return;
    }

    SkPaint paint;
    SkMatrix inverseMat, scaleMat;
    auto boundsGeo = (properties.GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        if (!canvas.getTotalMatrix().invert(&inverseMat)) {
            ROSEN_LOGE("RSPixelStretchDrawable::Draw get inverse matrix failed.");
        }
        scaleMat.setScale(inverseMat.getScaleX(), inverseMat.getScaleY());
    }

    canvas.save();
    canvas.translate(bounds_.x(), bounds_.y());
    if (pixelStretch_.x_ > 0) {
#ifdef NEW_SKIA
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(), &scaleMat));
#else
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, &scaleMat));
#endif
        canvas.drawRect(
            SkRect::MakeXYWH(-pixelStretch_.x_, -pixelStretch_.y_, scaledBounds.width(), scaledBounds.height()), paint);
    } else {
        scaleMat.setScale(scaledBounds.width() / bounds_.width() * scaleMat.getScaleX(),
            scaledBounds.height() / bounds_.height() * scaleMat.getScaleY());
#ifdef NEW_SKIA
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(), &scaleMat));
#else
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, &scaleMat));
#endif
        canvas.translate(-pixelStretch_.x_, -pixelStretch_.y_);
        canvas.drawRect(
            SkRect::MakeXYWH(pixelStretch_.x_, pixelStretch_.y_, bounds_.width(), bounds_.height()), paint);
    }
    canvas.restore();
}

std::unique_ptr<RSPropertyDrawable> RSPixelStretchDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
#else
    auto bounds = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
#endif
    return std::make_unique<RSPixelStretchDrawable>(pixelStretch.value(), bounds);
}

void RSBackgroundDrawable::setForceBgAntiAlias(bool forceBgAntiAlias)
{
    forceBgAntiAlias_ = forceBgAntiAlias;
}

void RSBackgroundDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.drawPaint(paint_);
#else
    canvas.DrawBackGround(brush_);
#endif
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundColorDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto& bgColor = properties.GetBackgroundColor();
    bool isTransparent = (bgColor == RgbPalette::Transparent());
    if (isTransparent) {
        return nullptr;
    }
    bool hasRoundedCorners = !properties.GetCornerRadius().IsZero();
    return std::make_unique<RSBackgroundColorDrawable>(hasRoundedCorners, bgColor.AsArgbInt());
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundShaderDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto bgShader = properties.GetBackgroundShader();
    if (!bgShader) {
        return nullptr;
    }
    bool hasRoundedCorners = !properties.GetCornerRadius().IsZero();
    return std::make_unique<RSBackgroundShaderDrawable>(hasRoundedCorners, bgShader->GetSkShader());
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundImageDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    const auto& bgImage = properties.GetBgImage();
    if (!bgImage) {
        return nullptr;
    }
    bool hasRoundedCorners = !properties.GetCornerRadius().IsZero();
    bgImage->SetDstRect(properties.GetBgImageRect());
    return std::make_unique<RSBackgroundImageDrawable>(hasRoundedCorners, bgImage);
}

void RSBackgroundImageDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetMutableRenderProperties();

#ifndef USE_ROSEN_DRAWING
    auto boundsRect = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
#ifdef NEW_SKIA
    image_->CanvasDrawImage(canvas, boundsRect, SkSamplingOptions(), paint_, true);
#else
    image_->CanvasDrawImage(canvas, boundsRect, paint_, true);
#endif
#else
    auto boundsRect = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.AttachBrush(brush_);
    image_->CanvasDrawImage(canvas, boundsRect, true);
    canvas->DetachBrush();
#endif
}

void RSEffectDataGenerateDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RS_TRACE_NAME("DrawBackgroundEffect " + filter_->GetDescription());
    auto& properties = node.GetMutableRenderProperties();
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSEffectDataGenerateDrawable::Draw skSurface null");
        return;
    }

    SkAutoCanvasRestore acr(&canvas, true);
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSEffectRenderNode>(properties.backref_.lock())) {
        auto& rect = node->effectRegion_->getBounds();
        canvas.clipRect(rect);
    }

    auto filter = std::static_pointer_cast<RSSkiaFilter>(filter_);
#if defined(NEW_SKIA) && defined(RS_ENABLE_GL)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(false);
        cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
        auto&& data = cacheManager->GeneratedCachedEffectData(canvas, filter);
        canvas.SetEffectData(data);
        return;
    }
#endif

    auto imageRect = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(imageRect);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSEffectDataGenerateDrawable::Draw image snapshot null");
        return;
    }

    filter->PreProcess(imageSnapshot);
    // create a offscreen skSurface
    sk_sp<SkSurface> offscreenSurface = skSurface->makeSurface(imageSnapshot->imageInfo());
    if (offscreenSurface == nullptr) {
        ROSEN_LOGE("RSEffectDataGenerateDrawable::Draw offscreenSurface null");
        return;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());
    auto clipBounds = SkRect::MakeIWH(imageRect.width(), imageRect.height());
    filter->DrawImageRect(offscreenCanvas, imageSnapshot, SkRect::Make(imageSnapshot->bounds()), clipBounds);
    filter->PostProcess(offscreenCanvas);

    auto imageCache = offscreenSurface->makeImageSnapshot();
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSEffectDataGenerateDrawable::DrawBackgroundEffect imageCache snapshot null");
        return;
    }
    auto data = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(imageCache), std::move(imageRect));
    canvas.SetEffectData(std::move(data));
}

// ============================================================================
// SavelayerBackground
std::unique_ptr<RSPropertyDrawable> RSSavelayerBackgroundDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    if (properties.GetColorBlendMode() == static_cast<int>(RSColorBlendModeType::NONE)) {
        return nullptr;
    }
    return std::make_unique<RSSavelayerBackgroundDrawable>();
}

void RSSavelayerBackgroundDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.saveLayer(nullptr, nullptr);
}

// ============================================================================
// SavelayerContent
std::unique_ptr<RSPropertyDrawable> RSSavelayerContentDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    if (properties.GetColorBlendMode() == static_cast<int>(RSColorBlendModeType::NONE)) {
        return nullptr;
    }
    static const std::vector<SkBlendMode> blendModeList = {
        SkBlendMode::kSrcIn, // RSColorBlendModeType::SRC_IN
        SkBlendMode::kDstIn, // RSColorBlendModeType::DST_IN
    };
    if (static_cast<unsigned long>(blendMode_) >= blendModeList.size()) {
        ROSEN_LOGE("color blendmode is set %d which is invalid.", blendMode_);
        return nullptr;
    }
    SkPaint blendPaint;
    blendPaint.setBlendMode(blendModeList[blendMode_]);
    return std::make_unique<RSSavelayerContentDrawable>(std::move(blendPaint));
}

void RSSavelayerContentDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.saveLayer(nullptr, &blendPaint);
}
} // namespace OHOS::Rosen
