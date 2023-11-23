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
#include "include/core/SkPaint.h"
#include "include/core/SkPoint3.h"
#include "include/effects/SkImageFilters.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/utils/SkShadowUtils.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties_painter.h"

namespace {
constexpr int PARAM_DOUBLE = 2;
} // namespace
namespace OHOS::Rosen {
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
RSPropertyDrawable::DrawablePtr RSBoundsGeometryDrawable::Generate(const RSRenderNode& node)
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
        canvas.ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(properties.GetClipRRect()), Drawing::ClipOp::INTERSECT, false);
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(
            RSPropertiesPainter::RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(
            RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, false);
    }
#endif
}

RSPropertyDrawable::DrawablePtr RSClipBoundsDrawable::Generate(const RSRenderNode& node)
{
    return std::make_unique<RSClipBoundsDrawable>();
}

// ============================================================================
// PointLight
RSPropertyDrawable::DrawablePtr RSPointLightDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    const auto& properties = context.properties_;
    const auto& illuminatedPtr = properties.GetIlluminated();
    if (illuminatedPtr && illuminatedPtr->IsIlluminatedValid()) {
        return std::make_unique<RSPointLightDrawable>();
    }
    return nullptr;
}

void RSPointLightDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetMutableRenderProperties();
    const auto& illuminatedPtr = properties.GetIlluminated();
    if (illuminatedPtr && illuminatedPtr->IsIlluminated() && ROSEN_EQ(properties.GetBloom(), 0.f)) {
        RSPropertiesPainter::DrawLight(properties, canvas);
    }
}

// ============================================================================
// Border
RSPropertyDrawable::DrawablePtr RSBorderDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
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
RSBorderDRRectDrawable::RSBorderDRRectDrawable(
    Drawing::Brush&& brush, Drawing::Pen&& pen, const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{
    OnBoundsChange(properties);
}

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
RSBorderFourLineDrawable::RSBorderFourLineDrawable(
    Drawing::Brush&& brush, Drawing::Pen&& pen, const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{
    OnBoundsChange(properties);
}

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
RSBorderPathDrawable::RSBorderPathDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen, const RSProperties& properties)
    : RSBorderDrawable(std::move(brush), std::move(pen))
#endif
{
    OnBoundsChange(properties);
}

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
    canvas.DrawPath(borderPath_);
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
    OnBoundsChange(properties);
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
    Drawing::AutoCanvasRestore acr(canvas, true);
    auto& properties = node.GetMutableRenderProperties();
    canvas.ClipRoundRect(innerRrect_, Drawing::ClipOp::DIFFERENCE, true);
    properties.GetBorder()->PaintTopPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintRightPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintBottomPath(canvas, pen_, rrect_);
    properties.GetBorder()->PaintLeftPath(canvas, pen_, rrect_);
#endif
}

// ============================================================================
// Mask
std::unique_ptr<RSPropertyDrawable> RSMaskDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        return nullptr;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        return nullptr;
    }
    if (mask->IsSvgMask()) {
        if (mask->GetSvgDom()) {
            return std::make_unique<RSSvgDomMaskDrawable>(mask);
        } else if (mask->GetSvgPicture()) {
            return std::make_unique<RSSvgPictureMaskDrawable>(mask);
        }
    } else if (mask->IsGradientMask()) {
        return std::make_unique<RSGradientMaskDrawable>(mask);
    } else if (mask->IsPathMask()) {
        return std::make_unique<RSPathMaskDrawable>(mask);
    }
    return nullptr;
}

RSMaskDrawable::RSMaskDrawable(std::shared_ptr<RSMask> mask) : mask_(std::move(mask))
{
#ifndef USE_ROSEN_DRAWING
    maskPaint_.setBlendMode(SkBlendMode::kSrcIn);
    auto filter = SkColorFilters::Compose(SkLumaColorFilter::Make(), SkColorFilters::SRGBToLinearGamma());
    maskFilter_.setColorFilter(filter);
#else
    maskBrush_.SetBlendMode(Drawing::BlendMode::SRC_IN);
    Drawing::Filter filter;
    filter.SetColorFilter(Drawing::ColorFilter::CreateComposeColorFilter(
        *(Drawing::ColorFilter::CreateLumaColorFilter()), *(Drawing::ColorFilter::CreateSrgbGammaToLinear())));
    maskFilterBrush_.SetFilter(filter);
#endif
}

RSSvgDomMaskDrawable::RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSSvgDomMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());

    canvas.save();
    canvas.saveLayer(bounds, nullptr);
    int tmpLayer = canvas.getSaveCount();
    canvas.saveLayer(bounds, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(bounds.fLeft + mask_->GetSvgX(), bounds.fTop + mask_->GetSvgY());
        canvas.scale(mask_->GetScaleX(), mask_->GetScaleY());
        mask_->GetSvgDom()->render(&canvas);
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(bounds, &maskPaint_);
    canvas.clipRect(bounds, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft() + mask_->GetSvgX(), bounds.GetTop() + mask_->GetSvgY());
        canvas.Scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.DrawSVGDOM(mask_->GetSvgDom());
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSSvgPictureMaskDrawable::RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSSvgPictureMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    canvas.save();
    canvas.saveLayer(bounds, nullptr);
    int tmpLayer = canvas.getSaveCount();
    canvas.saveLayer(bounds, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(bounds.fLeft + mask_->GetSvgX(), bounds.fTop + mask_->GetSvgY());
        canvas.scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.drawPicture(mask_->GetSvgPicture());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(bounds, &maskPaint_);
    canvas.clipRect(bounds, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft() + mask_->GetSvgX(), bounds.GetTop() + mask_->GetSvgY());
        canvas.Scale(mask_->GetScaleX(), mask_->GetScaleY());
        canvas.DrawPicture(*mask_->GetSvgPicture());
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSGradientMaskDrawable::RSGradientMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSGradientMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    canvas.save();
    canvas.saveLayer(bounds, nullptr);
    int tmpLayer = canvas.getSaveCount();
    canvas.saveLayer(bounds, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(bounds.fLeft, bounds.fTop);
        SkRect rect = SkRect::MakeIWH(bounds.fRight - bounds.fLeft, bounds.fBottom - bounds.fTop);
        canvas.drawRect(rect, mask_->GetMaskPaint());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(bounds, &maskPaint_);
    canvas.clipRect(bounds, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft(), bounds.GetTop());
        Drawing::Rect rect =
            Drawing::Rect(0, 0, bounds.GetRight() - bounds.GetLeft(), bounds.GetBottom() - bounds.GetTop());
        canvas.AttachBrush(mask_->GetMaskBrush());
        canvas.DrawRect(rect);
        canvas.DetachBrush();
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
}

RSPathMaskDrawable::RSPathMaskDrawable(std::shared_ptr<RSMask> mask) : RSMaskDrawable(mask) {}

void RSPathMaskDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    auto& properties = node.GetRenderProperties();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    canvas.save();
    canvas.saveLayer(bounds, nullptr);
    int tmpLayer = canvas.getSaveCount();
    canvas.saveLayer(bounds, &maskFilter_);
    {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.translate(bounds.fLeft, bounds.fTop);
        canvas.drawPath(mask_->GetMaskPath(), mask_->GetMaskPaint());
    }
    canvas.restoreToCount(tmpLayer);
    canvas.saveLayer(bounds, &maskPaint_);
    canvas.clipRect(bounds, true);
#else
    canvas.Save();
    Drawing::SaveLayerOps slr(&bounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();
    Drawing::SaveLayerOps slrMask(&bounds, &maskFilterBrush_);
    canvas.SaveLayer(slrMask);
    {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.Translate(bounds.GetLeft(), bounds.GetTop());
        canvas.AttachBrush(mask_->GetMaskBrush());
        canvas.DrawPath(*mask_->GetMaskPath());
        canvas.DetachBrush();
    }
    canvas.RestoreToCount(tmpLayer);
    Drawing::SaveLayerOps slrContent(&bounds, &maskBrush_);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, true);
#endif
}

// ============================================================================
// Shadow
std::unique_ptr<RSPropertyDrawable> RSShadowBaseDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    if (properties.IsSpherizeValid() || !properties.IsShadowValid()) {
        return nullptr;
    }
    if (properties.GetShadowMask()) {
        return std::make_unique<RSColorfulShadowDrawable>(properties);
    } else {
        if (properties.GetShadow()->GetHardwareAcceleration()) {
            if (properties.GetShadowElevation() <= 0.f) {
                return nullptr;
            }
            return std::make_unique<RSHardwareAccelerationShadowDrawable>(properties);
        } else {
            return std::make_unique<RSShadowDrawable>(properties);
        }
    }
}

RSShadowBaseDrawable::RSShadowBaseDrawable(const RSProperties& properties)
    : offsetX_(properties.GetShadowOffsetX()), offsetY_(properties.GetShadowOffsetY()),
      color_(properties.GetShadowColor())
{}

RSShadowDrawable::RSShadowDrawable(const RSProperties& properties)
    : RSShadowBaseDrawable(properties), radius_(properties.GetShadowRadius())
{}

#ifndef USE_ROSEN_DRAWING
void RSShadowBaseDrawable::ClipShadowPath(RSRenderNode& node, RSPaintFilterCanvas& canvas, SkPath& skPath)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetSkiaPath().isEmpty()) {
        skPath = properties.GetShadowPath()->GetSkiaPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        }
    } else if (properties.GetClipBounds()) {
        skPath = properties.GetClipBounds()->GetSkiaPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        }
    } else {
        skPath.addRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()));
        if (!properties.GetShadowIsFilled()) {
            canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()), SkClipOp::kDifference, true);
        }
    }
}
#else
void RSShadowBaseDrawable::ClipShadowPath(RSRenderNode& node, RSPaintFilterCanvas& canvas, Drawing::Path& path)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetDrawingPath().IsValid()) {
        path = properties.GetShadowPath()->GetDrawingPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else if (properties.GetClipBounds()) {
        path = properties.GetClipBounds()->GetDrawingPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else {
        path.AddRoundRect(RRect2DrawingRRect(properties.GetRRect()));
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::DIFFERENCE, true);
        }
    }
}
#endif

RSColor RSShadowDrawable::GetColorForShadow(RSRenderNode& node, RSPaintFilterCanvas& canvas,
    SkPath& skPath, SkMatrix& matrix, SkIRect& deviceClipBounds)
{
    RSColor colorPicked;
    const RSProperties& properties = node.GetRenderProperties();
    // color shadow alpha deault is 255, if need to be changed, should add a arkui interface
    auto shadowAlpha = UINT8_MAX;
    auto& colorPickerTask = properties.GetColorPickerCacheTaskShadow();
    if (colorPickerTask != nullptr && properties.GetShadowColorStrategy()) {
        RSPropertiesPainter::PickColor(properties, canvas, skPath, matrix, deviceClipBounds, colorPicked);
        RSPropertiesPainter::GetDarkColor(colorPicked);
        if (!colorPickerTask->GetFirstGetColorFinished()) {
            shadowAlpha = 0;
        }
    } else {
        colorPicked = color_;
    }
    return RSColor(colorPicked.GetRed(), colorPicked.GetGreen(), colorPicked.GetBlue(), shadowAlpha);
}

void RSShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    auto deviceClipBounds = canvas.getDeviceClipBounds();
    SkAutoCanvasRestore acr(&canvas, true);
    SkPath skPath;
    ClipShadowPath(node, canvas, skPath);
    skPath.offset(offsetX_, offsetY_);
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas.setMatrix(matrix);
    RSColor colorPicked = GetColorForShadow(node, canvas, skPath, matrix, deviceClipBounds);
    SkPaint paint;
    paint.setColor(
        SkColorSetARGB(colorPicked.GetAlpha(), colorPicked.GetRed(), colorPicked.GetGreen(), colorPicked.GetBlue()));
    paint.setAntiAlias(true);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, radius_));
    canvas.drawPath(skPath, paint);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(node, canvas, path);
    path.Offset(offsetX_, offsetY_);
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
    canvas.DrawPath(path);
    canvas.DetachBrush();
#endif
}

RSHardwareAccelerationShadowDrawable::RSHardwareAccelerationShadowDrawable(const RSProperties& properties)
    : RSShadowBaseDrawable(properties), shadowElevation_(properties.GetShadowElevation())
{}

void RSHardwareAccelerationShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    SkPath skPath;
    ClipShadowPath(node, canvas, skPath);
    skPath.offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas.setMatrix(matrix);
    SkPoint3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    SkPoint3 lightPos = { canvas.getTotalMatrix().getTranslateX() + skPath.getBounds().centerX(),
        canvas.getTotalMatrix().getTranslateY() + skPath.getBounds().centerY(), DEFAULT_LIGHT_HEIGHT };
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas.GetAlpha());
    Color spotColor = color_;
    spotColor.MultiplyAlpha(canvas.GetAlpha());
    SkShadowUtils::DrawShadow(&canvas, skPath, planeParams, lightPos, DEFAULT_LIGHT_RADIUS, ambientColor.AsArgbInt(),
        spotColor.AsArgbInt(), SkShadowFlags::kTransparentOccluder_ShadowFlag);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(node, canvas, path);
    path.Offset(offsetX_, offsetY_);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::SCALE_X, std::ceil(matrix.Get(Drawing::Matrix::SCALE_X)));
    matrix.Set(Drawing::Matrix::SCALE_Y, std::ceil(matrix.Get(Drawing::Matrix::SCALE_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Point3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    Drawing::scalar centerX = path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2;
    Drawing::scalar centerY = path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2;
    Drawing::Point3 lightPos = { canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_X) + centerX,
        canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_Y) + centerY, DEFAULT_LIGHT_HEIGHT };
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas.GetAlpha());
    Color spotColor = color_;
    spotColor.MultiplyAlpha(canvas.GetAlpha());
    canvas.DrawShadow(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS, Drawing::Color(ambientColor.AsArgbInt()),
        Drawing::Color(spotColor.AsArgbInt()), Drawing::ShadowFlags::TRANSPARENT_OCCLUDER);
#endif
}

RSColorfulShadowDrawable::RSColorfulShadowDrawable(const RSProperties& properties) : RSShadowBaseDrawable(properties)
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
    filter.SetImageFilter(
        Drawing::ImageFilter::CreateBlurImageFilter(blurRadius, blurRadius, Drawing::TileMode::DECAL, nullptr));
    blurBrush_.SetFilter(filter);
#endif
    node_ = properties.backref_;
}

void RSColorfulShadowDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    SkPath skPath;
    ClipShadowPath(node, canvas, skPath);
    // save layer, draw image with clipPath, blur and draw back
    canvas.saveLayer(nullptr, &blurPaint_);
    canvas.translate(offsetX_, offsetY_);
    canvas.clipPath(skPath);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    ClipShadowPath(node, canvas, path);
    // save layer, draw image with clipPath, blur and draw back
    Drawing::SaveLayerOps slr(nullptr, &blurBrush_);
    canvas.SaveLayer(slr);
    canvas.Translate(offsetX_, offsetY_);
    canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, false);
#endif
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(node_.lock())) {
        node->InternalDrawContent(canvas);
    }
}

// ============================================================================
// DynamicLightUp
std::unique_ptr<RSPropertyDrawable> RSDynamicLightUpDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    if (!properties.IsDynamicLightUpValid()) {
        return nullptr;
    }
    return std::make_unique<RSDynamicLightUpDrawable>();
}

bool RSDynamicLightUpDrawable::Update(const RSRenderNode& node)
{
    return node.GetRenderProperties().IsDynamicLightUpValid();
}

void RSDynamicLightUpDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawDynamicLightUp(node.GetRenderProperties(), canvas);
}

// ============================================================================
// LightUpEffect
std::unique_ptr<RSPropertyDrawable> RSLightUpEffectDrawable::Generate(const RSRenderNode& node)
{
    if (!node.GetRenderProperties().IsLightUpEffectValid()) {
        return nullptr;
    }
    return std::make_unique<RSLightUpEffectDrawable>();
}

bool RSLightUpEffectDrawable::Update(const RSRenderNode& node)
{
    return node.GetRenderProperties().IsLightUpEffectValid();
}

// ============================================================================
// Binarization
void RSBinarizationDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawBinarizationShader(node.GetRenderProperties(), canvas);
}

std::unique_ptr<RSPropertyDrawable> RSBinarizationDrawable::Generate(const RSRenderNode& node)
{
    auto& aiInvert = node.GetRenderProperties().GetAiInvert();
    if (!aiInvert.has_value()) {
        return nullptr;
    }
    return std::make_unique<RSBinarizationDrawable>();
}

// ============================================================================
// LightUpEffect
void RSLightUpEffectDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawLightUpEffect(node.GetRenderProperties(), canvas);
}

// ============================================================================
// background filter
std::unique_ptr<RSPropertyDrawable> RSBackgroundFilterDrawable::Generate(const RSRenderNode& node)
{
    if (!RSPropertiesPainter::BLUR_ENABLED) {
        ROSEN_LOGD("RSBackgroundFilterDrawable::Generate close blur.");
        return nullptr;
    }
    auto& filter = node.GetRenderProperties().GetBackgroundFilter();
    if (filter == nullptr) {
        return nullptr;
    }
    if (node.IsInstanceOf<RSEffectRenderNode>()) {
        // for RSEffectRenderNode, we just use generates effect data, instead of draw filter
        return std::make_unique<RSEffectDataGenerateDrawable>();
    }
    return std::make_unique<RSBackgroundFilterDrawable>();
}

void RSBackgroundFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    // fix compatibility with Node Group, copied from RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildren
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        return;
    }
    RSPropertiesPainter::DrawFilter(node.GetRenderProperties(), canvas, FilterType::BACKGROUND_FILTER);
}

// foreground filter
std::unique_ptr<RSPropertyDrawable> RSForegroundFilterDrawable::Generate(const RSRenderNode& node)
{
    if (!RSPropertiesPainter::BLUR_ENABLED) {
        ROSEN_LOGD("RSForegroundFilterDrawable::Generate close blur.");
        return nullptr;
    }
    auto& filter = node.GetRenderProperties().GetFilter();
    if (filter == nullptr) {
        return nullptr;
    }
    return std::make_unique<RSForegroundFilterDrawable>();
}

bool RSForegroundFilterDrawable::Update(const RSRenderNode& node)
{
    return node.GetRenderProperties().GetFilter() != nullptr;
}

void RSForegroundFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawFilter(node.GetRenderProperties(), canvas, FilterType::FOREGROUND_FILTER);
}

// effect data
bool RSEffectDataGenerateDrawable::Update(const RSRenderNode& node)
{
    return node.GetRenderProperties().GetBackgroundFilter() != nullptr;
}

void RSEffectDataGenerateDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (auto effectNode = node.ReinterpretCastTo<RSEffectRenderNode>()) {
        if (auto& region = effectNode->effectRegion_) {
            RSPropertiesPainter::DrawBackgroundEffect(node.GetRenderProperties(), canvas, region->getBounds());
        }
    }
}

// effect data apply
std::unique_ptr<RSPropertyDrawable> RSEffectDataApplyDrawable::Generate(const RSRenderNode& node)
{
    if (node.GetRenderProperties().GetUseEffect() == false) {
        return nullptr;
    }
    return std::make_unique<RSEffectDataApplyDrawable>();
}

bool RSEffectDataApplyDrawable::Update(const RSRenderNode& node)
{
    return node.GetRenderProperties().GetUseEffect();
}

bool RSBackgroundFilterDrawable::Update(const RSRenderNode& node)
{
    return node.GetRenderProperties().GetFilter() != nullptr;
}

void RSEffectDataApplyDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    // fix compatibility with Node Group, copied from RSCanvasRenderNode::ProcessAnimatePropertyBeforeChildren
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        return;
    }
    RSPropertiesPainter::ApplyBackgroundEffect(node.GetRenderProperties(), canvas);
}

// ============================================================================
// LinearGradientBlurFilter
std::unique_ptr<RSPropertyDrawable> RSLinearGradientBlurFilterDrawable::Generate(const RSRenderNode& node)
{
    const auto& para = node.GetRenderProperties().GetLinearGradientBlurPara();
    if (para == nullptr || para->blurRadius_ <= 0) {
        return nullptr;
    }
    return std::make_unique<RSLinearGradientBlurFilterDrawable>();
}

bool RSLinearGradientBlurFilterDrawable::Update(const RSRenderNode& node)
{
    return node.GetRenderProperties().GetLinearGradientBlurPara() != nullptr;
}

void RSLinearGradientBlurFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawLinearGradientBlurFilter(node.GetRenderProperties(), canvas);
}

// ============================================================================
// ForegroundColor
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

bool RSForegroundColorDrawable::Update(const RSRenderNode& node)
{
    auto fgColor = node.GetRenderProperties().GetForegroundColor();
#ifndef USE_ROSEN_DRAWING
    paint_.setColor(fgColor.AsArgbInt());
#else
    brush_.SetColor(fgColor.AsArgbInt());
#endif
    return fgColor != RgbPalette::Transparent();
}

std::unique_ptr<RSPropertyDrawable> RSForegroundColorDrawable::Generate(const RSRenderNode& node)
{
    auto fgColor = node.GetRenderProperties().GetForegroundColor();
    if (fgColor == RgbPalette::Transparent()) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setColor(fgColor.AsArgbInt());
    paint.setAntiAlias(true);
    return std::make_unique<RSForegroundColorDrawable>(std::move(paint));
#else
    Drawing::Brush brush;
    brush.SetColor(bgColor.AsArgbInt());
    brush.SetAntiAlias(true);
    return std::make_unique<RSForegroundColorDrawable>(std::move(brush));
#endif
}

// ============================================================================
// Particle
std::unique_ptr<RSPropertyDrawable> RSParticleDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    const auto& particleVector = properties.GetParticles();
    if (particleVector.GetParticleSize() == 0) {
        return nullptr;
    }
    return std::make_unique<RSParticleDrawable>();
}

bool RSParticleDrawable::Update(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    return properties.GetPixelStretch().has_value();
}

void RSParticleDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawParticle(node.GetRenderProperties(), canvas);
}

void RSPixelStretchDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    RSPropertiesPainter::DrawPixelStretch(node.GetRenderProperties(), canvas);
}

bool RSPixelStretchDrawable::Update(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    return properties.GetPixelStretch().has_value();
}

std::unique_ptr<RSPropertyDrawable> RSPixelStretchDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return nullptr;
    }
    return std::make_unique<RSPixelStretchDrawable>();
}

// ============================================================================
// Background
void RSBackgroundDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.drawRRect(RSPropertiesPainter::RRect2SkRRect(node.GetMutableRenderProperties().GetInnerRRect()), paint_);
#else
    canvas.DrawBackground(brush_);
#endif
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundColorDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    auto& bgColor = properties.GetBackgroundColor();
    bool isTransparent = (bgColor == RgbPalette::Transparent());
    if (isTransparent) {
        return nullptr;
    }
    return std::make_unique<RSBackgroundColorDrawable>(bgColor.AsArgbInt());
}

bool RSBackgroundColorDrawable::Update(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    auto& bgColor = properties.GetBackgroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return false;
    }
#ifndef USE_ROSEN_DRAWING
    paint_.setColor(bgColor.AsArgbInt());
#else
    brush_.SetColor(bgColor.AsArgbInt());
#endif
    return true;
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundShaderDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    auto bgShader = properties.GetBackgroundShader();
    if (!bgShader) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    return std::make_unique<RSBackgroundShaderDrawable>(bgShader->GetSkShader());
#else
    return std::make_unique<RSBackgroundShaderDrawable>(bgShader->GetDrawingShader());
#endif
}

bool RSBackgroundShaderDrawable::Update(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    auto bgShader = properties.GetBackgroundShader();
    if (!bgShader) {
        return false;
    }
#ifndef USE_ROSEN_DRAWING
    paint_.setShader(bgShader->GetSkShader());
#else
    brush_.SetShaderEffect(bgShader->GetDrawingShader());
#endif
    return true;
}

std::unique_ptr<RSPropertyDrawable> RSBackgroundImageDrawable::Generate(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    const auto& bgImage = properties.GetBgImage();
    if (!bgImage) {
        return nullptr;
    }
    bgImage->SetDstRect(properties.GetBgImageRect());
    return std::make_unique<RSBackgroundImageDrawable>();
}

bool RSBackgroundImageDrawable::Update(const RSRenderNode& node)
{
    auto& properties = node.GetRenderProperties();
    const auto& bgImage = properties.GetBgImage();
    if (!bgImage) {
        return false;
    }
    bgImage->SetDstRect(properties.GetBgImageRect());
    return true;
}

void RSBackgroundImageDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto& properties = node.GetMutableRenderProperties();
    const auto& image = properties.GetBgImage();

#ifndef USE_ROSEN_DRAWING
    auto boundsRect = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
#ifdef NEW_SKIA
    image->CanvasDrawImage(canvas, boundsRect, SkSamplingOptions(), paint_, true);
#else
    image->CanvasDrawImage(canvas, boundsRect, paint_, true);
#endif
#else
    auto boundsRect = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.AttachBrush(brush_);
    image_->CanvasDrawImage(canvas, boundsRect, Drawing::SamplingOptions(), true);
    canvas.DetachBrush();
#endif
}

// ============================================================================
// SaveLayerBackground
void RSSaveLayerBackgroundDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    *content_ = canvas.saveLayer(nullptr, nullptr);
}

// SaveLayerContent
void RSSaveLayerContentDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    *content_ = canvas.saveLayer(nullptr, &blendPaint_);
}

} // namespace OHOS::Rosen
