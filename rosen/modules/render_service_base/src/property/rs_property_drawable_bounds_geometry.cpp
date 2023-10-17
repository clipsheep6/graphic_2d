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
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

namespace {
constexpr int PARAM_DOUBLE = 2;
} // namespace
namespace OHOS::Rosen {
bool RSBackgroundDrawable::forceBgAntiAlias_ = true;
// ============================================================================
// Bounds geometry
void RSBoundsGeometryDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->concat(boundsMatrix_);
}
RSPropertyDrawable::DrawablePtr RSBoundsGeometryDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    return std::make_unique<RSBoundsGeometryDrawable>();
}
void RSBoundsGeometryDrawable::OnGeometryChange(const RSProperties& properties)
{
    boundsMatrix_ = properties.GetBoundsGeometry()->GetMatrix();
}

void RSClipBoundsDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    // Planning: Generate() should calculate the draw op and cache it
    auto& properties = context.properties_;
    if (properties.GetClipBounds() != nullptr) {
        context.canvas_->clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else if (properties.GetClipToRRect()) {
        context.canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetClipRRect()));
    } else if (!properties.GetCornerRadius().IsZero()) {
        context.canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()), true);
    } else {
        context.canvas_->clipRect(RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect()));
    }
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
}
RSBorderDRRectDrawable::RSBorderDRRectDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
{}
void RSBorderDRRectDrawable::OnGeometryChange(const RSProperties& properties)
{
    inner_ = RSPropertiesPainter::RRect2SkRRect(properties.GetInnerRRect());
    outer_ = RSPropertiesPainter::RRect2SkRRect(properties.GetRRect());
}
void RSBorderDRRectDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->drawDRRect(outer_, inner_, paint_);
}

RSBorderFourLineDrawable::RSBorderFourLineDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
{}
void RSBorderFourLineDrawable::OnGeometryChange(const RSProperties& properties)
{
    rect_ = properties.GetBoundsRect();
}
void RSBorderFourLineDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.properties_.GetBorder()->PaintFourLine(*context.canvas_, paint_, rect_);
}

RSBorderPathDrawable::RSBorderPathDrawable(SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
{}
void RSBorderPathDrawable::OnGeometryChange(const RSProperties& properties)
{
    auto borderWidth = properties.GetBorder()->GetWidth();
    RRect rrect = properties.GetRRect();
    rrect.rect_.width_ -= borderWidth;
    rrect.rect_.height_ -= borderWidth;
    rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
    borderPath_.reset();
    borderPath_.addRRect(RSPropertiesPainter::RRect2SkRRect(rrect));
}
void RSBorderPathDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->drawPath(borderPath_, paint_);
}

RSBorderFourLineRoundCornerDrawable::RSBorderFourLineRoundCornerDrawable(
    SkPaint&& paint, const RSProperties& properties)
    : RSBorderDrawable(std::move(paint))
{
    paint_.setStyle(SkPaint::Style::kStroke_Style);
}
void RSBorderFourLineRoundCornerDrawable::OnGeometryChange(const RSProperties& properties)
{
    innerRrect_ = RSPropertiesPainter::RRect2SkRRect(properties.GetInnerRRect());
    rrect_ = RSPropertiesPainter::RRect2SkRRect(properties.GetRRect());
}
void RSBorderFourLineRoundCornerDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    SkAutoCanvasRestore acr(context.canvas_, true);
    auto& properties = context.properties_;
    context.canvas_->clipRRect(innerRrect_, SkClipOp::kDifference, true);
    properties.GetBorder()->PaintTopPath(*context.canvas_, paint_, rrect_);
    properties.GetBorder()->PaintRightPath(*context.canvas_, paint_, rrect_);
    properties.GetBorder()->PaintBottomPath(*context.canvas_, paint_, rrect_);
    properties.GetBorder()->PaintLeftPath(*context.canvas_, paint_, rrect_);
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
    maskBounds_ = RSPropertiesPainter::Rect2SkRect(bounds);
    maskPaint_.setBlendMode(SkBlendMode::kSrcIn);
    auto filter = SkColorFilters::Compose(SkLumaColorFilter::Make(), SkColorFilters::SRGBToLinearGamma());
    maskFilter_.setColorFilter(filter);
}

RSSvgDomMaskDrawable::RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds) : RSMaskDrawable(mask, bounds) {}

void RSSvgDomMaskDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->save();
    int tmpLayer = context.canvas_->saveLayer(maskBounds_, nullptr);
    context.canvas_->saveLayer(maskBounds_, &maskFilter_);
    {
        SkAutoCanvasRestore acr(context.canvas_, true);
        context.canvas_->translate(maskBounds_.fLeft + mask_->GetSvgX(), maskBounds_.fTop + mask_->GetSvgY());
        context.canvas_->scale(mask_->GetScaleX(), mask_->GetScaleY());
        mask_->GetSvgDom()->render(context.canvas_);
    }
    context.canvas_->restoreToCount(tmpLayer);
    context.canvas_->saveLayer(maskBounds_, &maskPaint_);
    context.canvas_->clipRect(maskBounds_, true);
}

RSSvgPictureMaskDrawable::RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    : RSMaskDrawable(mask, bounds)
{}

void RSSvgPictureMaskDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->save();
    int tmpLayer = context.canvas_->saveLayer(maskBounds_, nullptr);
    context.canvas_->saveLayer(maskBounds_, &maskFilter_);
    {
        SkAutoCanvasRestore acr(context.canvas_, true);
        context.canvas_->translate(maskBounds_.fLeft + mask_->GetSvgX(), maskBounds_.fTop + mask_->GetSvgY());
        context.canvas_->scale(mask_->GetScaleX(), mask_->GetScaleY());
        context.canvas_->drawPicture(mask_->GetSvgPicture());
    }
    context.canvas_->restoreToCount(tmpLayer);
    context.canvas_->saveLayer(maskBounds_, &maskPaint_);
    context.canvas_->clipRect(maskBounds_, true);
}

RSGradientMaskDrawable::RSGradientMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds)
    : RSMaskDrawable(mask, bounds)
{}

void RSGradientMaskDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->Save();
    int tmpLayer = context.canvas_->saveLayer(maskBounds_, nullptr);
    context.canvas_->saveLayer(maskBounds_, &maskFilter_);
    {
        SkAutoCanvasRestore acr(context.canvas_, true);
        context.canvas_->translate(maskBounds_.fLeft, maskBounds_.fTop);
        SkRect rect = SkRect::MakeIWH(maskBounds_.fRight - maskBounds_.fLeft, maskBounds_.fBottom - maskBounds_.fTop);
        context.canvas_->drawRect(rect, mask_->GetMaskPaint());
    }
    context.canvas_->restoreToCount(tmpLayer);
    context.canvas_->saveLayer(maskBounds_, &maskPaint_);
    context.canvas_->clipRect(maskBounds_, true);
}

RSPathMaskDrawable::RSPathMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds) : RSMaskDrawable(mask, bounds) {}

void RSPathMaskDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->save();
    int tmpLayer = context.canvas_->saveLayer(maskBounds_, nullptr);
    context.canvas_->saveLayer(maskBounds_, &maskFilter_);
    {
        SkAutoCanvasRestore acr(context.canvas_, true);
        context.canvas_->translate(maskBounds_.fLeft, maskBounds_.fTop);
        context.canvas_->drawPath(mask_->GetMaskPath(), mask_->GetMaskPaint());
    }
    context.canvas_->restoreToCount(tmpLayer);
    context.canvas_->saveLayer(maskBounds_, &maskPaint_);
    context.canvas_->clipRect(maskBounds_, true);
}

// ============================================================================
// Shadow
std::unique_ptr<RSPropertyDrawable> RSShadowBaseDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    if (properties.IsSpherizeValid() || !properties.IsShadowValid()) {
        return nullptr;
    }
    SkPath skPath;
    // PLANNING: Generate SkPath and deal with LeashWindow

    if (properties.GetShadowMask()) {
        return std::make_unique<RSColorfulShadowDrawable>(skPath, properties);
    } else {
        if (properties.GetShadow()->GetHardwareAcceleration()) {
            if (properties.GetShadowElevation() <= 0.f) {
                return nullptr;
            }
            return std::make_unique<RSHardwareAccelerationShadowDrawable>(skPath, properties);
        } else {
            return std::make_unique<RSShadowDrawable>(skPath, properties);
        }
    }
}

RSShadowBaseDrawable::RSShadowBaseDrawable(SkPath skPath, const RSProperties& properties) : skPath_(skPath)
{
    offsetX_ = properties.GetShadowOffsetX();
    offsetY_ = properties.GetShadowOffsetY();
    color_ = properties.GetShadowColor();
}

RSShadowDrawable::RSShadowDrawable(SkPath skPath, const RSProperties& properties)
    : RSShadowBaseDrawable(skPath, properties)
{
    radius_ = properties.GetShadowRadius();
}

void RSShadowDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    skPath_.offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(context.canvas_);
    auto matrix = context.canvas_->getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    context.canvas_->setMatrix(matrix);
    SkPaint paint;
    paint.setColor(color_.AsArgbInt());
    paint.setAntiAlias(true);
    paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, radius_));
    context.canvas_->drawPath(skPath_, paint);
}

RSHardwareAccelerationShadowDrawable::RSHardwareAccelerationShadowDrawable(
    SkPath skPath, const RSProperties& properties)
    : RSShadowBaseDrawable(skPath, properties)
{
    shadowElevation_ = properties.GetShadowElevation();
}

void RSHardwareAccelerationShadowDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    skPath_.offset(offsetX_, offsetY_);
    RSAutoCanvasRestore rst(context.canvas_);
    auto matrix = context.canvas_->getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    context.canvas_->setMatrix(matrix);
    SkPoint3 planeParams = { 0.0f, 0.0f, shadowElevation_ };
    SkPoint3 lightPos = { context.canvas_->getTotalMatrix().getTranslateX() + skPath_.getBounds().centerX(),
        context.canvas_->getTotalMatrix().getTranslateY() + skPath_.getBounds().centerY(), DEFAULT_LIGHT_HEIGHT };
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(context.canvas_->GetAlpha());
    color_.MultiplyAlpha(context.canvas_->GetAlpha());
    SkShadowUtils::DrawShadow(context.canvas_, skPath_, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
        ambientColor.AsArgbInt(), color_.AsArgbInt(), SkShadowFlags::kTransparentOccluder_ShadowFlag);
}

RSColorfulShadowDrawable::RSColorfulShadowDrawable(SkPath skPath, const RSProperties& properties)
    : RSShadowBaseDrawable(skPath, properties)
{
    const SkScalar blurRadius =
        properties.GetShadow()->GetHardwareAcceleration()
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();
    blurPaint_.setImageFilter(SkImageFilters::Blur(blurRadius, blurRadius, SkTileMode::kDecal, nullptr));
    node_ = properties.backref_;
}

void RSColorfulShadowDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    SkAutoCanvasRestore acr(context.canvas_, true);
    // save layer, draw image with clipPath, blur and draw back
    context.canvas_->saveLayer(nullptr, &blurPaint_);
    context.canvas_->translate(offsetX_, offsetY_);
    context.canvas_->clipPath(skPath_);
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(node_.lock())) {
        node->InternalDrawContent(*context.canvas_);
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

void RSDynamicLightUpDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    SkSurface* skSurface = context.canvas_->GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    auto clipBounds = context.canvas_->getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = RSPropertiesPainter::MakeDynamicLightUpShader(rate_, degree_, imageShader);
    SkPaint paint;
    paint.setShader(shader);
    SkAutoCanvasRestore acr(context.canvas_, true);
    context.canvas_->translate(clipBounds.left(), clipBounds.top());
    context.canvas_->drawPaint(paint);
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

void RSEffectDataApplyDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    const auto& effectData = context.canvas_->GetEffectData();
    if (effectData == nullptr || effectData->cachedImage_ == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(context.canvas_, true);
    context.canvas_->resetMatrix();
    auto visibleIRect = context.canvas_->GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        context.canvas_->clipIRect(visibleIRect);
    }

    // dstRect: canvas clip region
    auto dstRect = SkRect::Make(context.canvas_->getDeviceClipBounds());
    // srcRect: map dstRect onto cache coordinate
    auto srcRect = dstRect.makeOffset(-effectData->cachedRect_.left(), -effectData->cachedRect_.top());
    SkPaint defaultPaint;
    context.canvas_->drawImageRect(effectData->cachedImage_, srcRect, dstRect, SkSamplingOptions(), &defaultPaint,
        SkCanvas::kFast_SrcRectConstraint);
}

// ============================================================================
// LightUpEffect
void RSLightUpEffectDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    auto& properties = context.properties_;
    auto& canvas = context.canvas_;
    SkSurface* skSurface = canvas->GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSLightUpEffectDrawable::Draw skSurface is null");
        return;
    }
    SkAutoCanvasRestore acr(canvas, true);
    auto clipBounds = canvas->getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSLightUpEffectDrawable::Draw image is null");
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = RSPropertiesPainter::MakeLightUpEffectShader(properties.GetLightUpEffect(), imageShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas->resetMatrix();
    canvas->translate(clipBounds.left(), clipBounds.top());
    canvas->drawPaint(paint);
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

void RSBackgroundFilterDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    if (context.canvas_->GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN) {
        return;
    }
    RSFilterDrawable::DrawFilter(context, BACKGROUND_FILTER, filter_);
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
    RSModifierContext& context, FilterType filterType, const std::shared_ptr<RSFilter>& rsFilter)
{
    auto& properties = context.properties_;
    auto& canvas = context.canvas_;
    RS_OPTIONAL_TRACE_NAME("DrawFilter " + rsFilter->GetDescription());

    auto filter = std::static_pointer_cast<RSSkiaFilter>(rsFilter);
    auto skSurface = canvas->GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSFilterDrawable::DrawFilter skSurface null");
        auto paint = filter->GetPaint();
        SkCanvas::SaveLayerRec slr(nullptr, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
        canvas->saveLayer(slr);
        filter->PostProcess(*canvas);
        return;
    }

    // for foreground filter, when do online opacity, rendering result already applied opacity,
    // so drawImage should not apply opacity again
    RSAutoCanvasRestore autoCanvasRestore(
        canvas, filterType == FilterType::FOREGROUND_FILTER ? RSPaintFilterCanvas::kAlpha : RSPaintFilterCanvas::kNone);
    if (filterType == FilterType::FOREGROUND_FILTER) {
        canvas->SetAlpha(1.0);
    }

#if defined(RS_ENABLE_GL)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(filterType == FilterType::FOREGROUND_FILTER);
        cacheManager != nullptr && !canvas->GetDisableFilterCache()) {
        cacheManager->DrawFilter(*canvas, filter);
        return;
    }
#endif
    auto clipIBounds = canvas->getDeviceClipBounds();
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
    SkAutoCanvasRestore acr(canvas, true);
    canvas->resetMatrix();
    auto visibleIRect = canvas->GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        canvas->clipIRect(visibleIRect);
    }
    filter->DrawImageRect(*canvas, imageSnapshot, SkRect::Make(imageSnapshot->bounds()), SkRect::Make(clipIBounds));
    filter->PostProcess(*canvas);
}

void RSForegroundFilterDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    RSFilterDrawable::DrawFilter(context, FOREGROUND_FILTER, filter_);
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

void RSLinearGradientBlurFilterDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    auto& canvas = context.canvas_;
#ifdef NEW_SKIA
    SkSurface* skSurface = canvas->GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(canvas, true);

    auto clipBounds = canvas->getDeviceClipBounds();
    auto clipIPadding = clipBounds.makeOutset(-1, -1);
    SkMatrix mat = canvas->getTotalMatrix();
    uint8_t directionBias = RSPropertiesPainter::CalcDirectionBias(mat);
    auto alphaGradientShader =
        RSPropertiesPainter::MakeAlphaGradientShader(SkRect::Make(clipIPadding), para_, directionBias);
    if (alphaGradientShader == nullptr) {
        ROSEN_LOGE("RSLinearGradientBlurFilterDrawable::Draw alphaGradientShader null");
        return;
    }
    float radius = para_->blurRadius_ / 2;
    canvas->resetMatrix();
    canvas->translate(clipIPadding.left(), clipIPadding.top());

    RSPropertiesPainter::DrawHorizontalLinearGradientBlur(
        skSurface, *canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawVerticalLinearGradientBlur(skSurface, *canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawHorizontalLinearGradientBlur(
        skSurface, *canvas, radius, alphaGradientShader, clipIPadding);
    RSPropertiesPainter::DrawVerticalLinearGradientBlur(skSurface, *canvas, radius, alphaGradientShader, clipIPadding);
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

void RSForegroundColorDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    auto& properties = context.properties_;
    auto& canvas = context.canvas_;
    canvas->drawRRect(RSPropertiesPainter::RRect2SkRRect(properties.GetRRect()), paint_);
}

std::unique_ptr<RSPropertyDrawable> RSForegroundColorDrawable::Generate(
    const RSPropertyDrawableGenerateContext& context)
{
    auto bgColor = context.properties_.GetForegroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return nullptr;
    }
    SkPaint paint;
    paint.setColor(bgColor.AsArgbInt());
    paint.setAntiAlias(true);
    return std::make_unique<RSForegroundColorDrawable>(std::move(paint));
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
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setAlphaf(opacity);
            auto clipBounds = RSPropertiesPainter::Rect2SkRect(bounds);
            if (particleType == ParticleType::POINTS) {
                Color color = particle->GetColor();
                auto alpha = color.GetAlpha();
                color.SetAlpha(alpha * opacity);
                paint.setColor(color.AsArgbInt());
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSPointParticleDrawable>(std::move(paint), particle, clipBounds));
            } else {
                uniParticleDrawable->AddPropertyDrawable(
                    std::make_shared<RSImageParticleDrawable>(std::move(paint), particle, clipBounds));
            }
        }
    }
    return std::move(uniParticleDrawable);
}

void RSParticleDrawable::AddPropertyDrawable(std::shared_ptr<RSPropertyDrawable> drawable)
{
    drawableVec_.emplace_back(drawable);
}

void RSParticleDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    for (auto& propertyDrawable : drawableVec_) {
        if (propertyDrawable != nullptr) {
            propertyDrawable->Draw(context);
        }
    }
}

void RSPointParticleDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    context.canvas_->clipRect(bounds_, true);
    auto radius = particles_->GetRadius();
    auto position = particles_->GetPosition();
    float scale = particles_->GetScale();
    context.canvas_->drawCircle(position.x_, position.y_, radius * scale, paint_);
}

void RSImageParticleDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    auto& canvas = context.canvas_;
    auto imageSize = particles_->GetImageSize();
    auto image = particles_->GetImage();
    canvas->clipRect(bounds_, true);
    auto position = particles_->GetPosition();
    float left = position.x_;
    float top = position.y_;
    float scale = particles_->GetScale();
    float right = position.x_ + imageSize.x_ * scale;
    float bottom = position.y_ + imageSize.y_ * scale;
    canvas->save();
    canvas->translate(position.x_, position.y_);
    canvas->rotate(particles_->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
    RectF destRect(left, top, right, bottom);
    image->SetDstRect(destRect);
    image->SetScale(scale);
    image->SetImageRepeat(0);
    SkRect rect { left, top, right, bottom };
#ifdef NEW_SKIA
    image->CanvasDrawImage(*canvas, rect, SkSamplingOptions(), paint_, false);
    canvas->restore();
#else
    image->CanvasDrawImage(*canvas, rect, paint_, false);
    canvas->restore();
#endif
}

void RSPixelStretchDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    auto& properties = context.properties_;
    auto& canvas = context.canvas_;
    auto skSurface = canvas->GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPixelStretchDrawable::Draw skSurface null");
        return;
    }

    auto clipBounds = canvas->getDeviceClipBounds();
    clipBounds.setXYWH(clipBounds.left(), clipBounds.top(), clipBounds.width() - 1, clipBounds.height() - 1);

    /* Calculates the relative coordinates of the clipbounds
        with respect to the origin of the current canvas coordinates */
    SkMatrix worldToLocalMat;
    if (!canvas->getTotalMatrix().invert(&worldToLocalMat)) {
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
        if (!canvas->getTotalMatrix().invert(&inverseMat)) {
            ROSEN_LOGE("RSPixelStretchDrawable::Draw get inverse matrix failed.");
        }
        scaleMat.setScale(inverseMat.getScaleX(), inverseMat.getScaleY());
    }

    canvas->save();
    canvas->translate(bounds_.x(), bounds_.y());
    if (pixelStretch_.x_ > 0) {
#ifdef NEW_SKIA
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(), &scaleMat));
#else
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, &scaleMat));
#endif
        canvas->drawRect(
            SkRect::MakeXYWH(-pixelStretch_.x_, -pixelStretch_.y_, scaledBounds.width(), scaledBounds.height()), paint);
    } else {
        scaleMat.setScale(scaledBounds.width() / bounds_.width() * scaleMat.getScaleX(),
            scaledBounds.height() / bounds_.height() * scaleMat.getScaleY());
#ifdef NEW_SKIA
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(), &scaleMat));
#else
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, &scaleMat));
#endif
        canvas->translate(-pixelStretch_.x_, -pixelStretch_.y_);
        canvas->drawRect(
            SkRect::MakeXYWH(pixelStretch_.x_, pixelStretch_.y_, bounds_.width(), bounds_.height()), paint);
    }
    canvas->restore();
}

std::unique_ptr<RSPropertyDrawable> RSPixelStretchDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& properties = context.properties_;
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return nullptr;
    }
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    return std::make_unique<RSPixelStretchDrawable>(pixelStretch.value(), bounds);
}

void RSBackgroundDrawable::setForceBgAntiAlias(bool forceBgAntiAlias)
{
    forceBgAntiAlias_ = forceBgAntiAlias;
}

void RSBackgroundDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    auto& canvas = context.canvas_;
    canvas->drawPaint(paint_);
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

void RSBackgroundImageDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    auto& canvas = context.canvas_;
    auto& properties = context.properties_;

    auto boundsRect = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
#ifdef NEW_SKIA
    image_->CanvasDrawImage(*canvas, boundsRect, SkSamplingOptions(), paint_, true);
#else
    image_->CanvasDrawImage(*canvas, boundsRect, paint_, true);
#endif
}

void RSEffectDataGenerateDrawable::Draw(RSPropertyDrawableRenderContext& context)
{
    RS_TRACE_NAME("DrawBackgroundEffect " + filter_->GetDescription());
    auto& canvas = context.canvas_;
    auto& properties = context.properties_;
    auto skSurface = canvas->GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSEffectDataGenerateDrawable::Draw skSurface null");
        return;
    }

    SkAutoCanvasRestore acr(canvas, true);
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSEffectRenderNode>(properties.backref_.lock())) {
        auto& rect = node->effectRegion_->getBounds();
        canvas->clipRect(rect);
    }

    auto filter = std::static_pointer_cast<RSSkiaFilter>(filter_);
#if defined(NEW_SKIA) && defined(RS_ENABLE_GL)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(false);
        cacheManager != nullptr && !canvas->GetDisableFilterCache()) {
        auto&& data = cacheManager->GeneratedCachedEffectData(*canvas, filter);
        canvas->SetEffectData(data);
        return;
    }
#endif

    auto imageRect = canvas->getDeviceClipBounds();
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
    canvas->SetEffectData(std::move(data));
}
} // namespace OHOS::Rosen
