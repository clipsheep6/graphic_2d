/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "property/rs_properties_painter.h"

#include "common/rs_obj_abs_geometry.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRRect.h"
#include "include/effects/Sk1DPathEffect.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/utils/SkShadowUtils.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_transition_properties.h"
#include "render/rs_blur_filter.h"
#include "render/rs_image.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_mask.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int PARAM_DOUBLE = 2;
constexpr int32_t DASHED_LINE_LENGTH = 3;
} // namespace

SkRect Rect2SkRect(const RectF& r)
{
    return SkRect::MakeXYWH(r.left_, r.top_, r.width_, r.height_);
}

SkRRect RRect2SkRRect(const RRect& rr)
{
    SkRect rect = SkRect::MakeXYWH(rr.rect_.left_, rr.rect_.top_, rr.rect_.width_, rr.rect_.height_);
    SkRRect rrect = SkRRect::MakeEmpty();
    SkVector vec[4];
    for (int i = 0; i < 4; i++) {
        vec[i].set(rr.radius_[i].x_, rr.radius_[i].y_);
    }
    rrect.setRectRadii(rect, vec);
    return rrect;
}

bool RSPropertiesPainter::GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, SkMatrix& mat)
{
    if (w == rect.width_ && h == rect.height_) {
        return false;
    }
    mat.reset();
    switch (gravity) {
        case Gravity::CENTER: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, 0);
            return true;
        }
        case Gravity::BOTTOM: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, rect.height_ - h);
            return true;
        }
        case Gravity::LEFT: {
            mat.preTranslate(0, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RIGHT: {
            mat.preTranslate(rect.width_ - w, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP_LEFT: {
            return false;
        }
        case Gravity::TOP_RIGHT: {
            mat.preTranslate(rect.width_ - w, 0);
            return true;
        }
        case Gravity::BOTTOM_LEFT: {
            mat.preTranslate(0, rect.height_ - h);
            return true;
        }
        case Gravity::BOTTOM_RIGHT: {
            mat.preTranslate(rect.width_ - w, rect.height_ - h);
            return true;
        }
        case Gravity::RESIZE: {
            mat.preScale(rect.width_ / w, rect.height_ / h);
            return true;
        }
        case Gravity::RESIZE_ASPECT: {
            float scale = std::min(rect.width_ / w, rect.height_ / h);
            mat.preScale(scale, scale);
            mat.preTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL: {
            float scale = std::max(rect.width_ / w, rect.height_ / h);
            mat.preScale(scale, scale);
            mat.preTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
            return true;
        }
        default: {
            ROSEN_LOGE("GetGravityMatrix unknow gravity=[%d]", gravity);
            return false;
        }
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

void RSPropertiesPainter::Clip(SkCanvas& canvas, RectF rect)
{
    canvas.clipRect(Rect2SkRect(rect), true);
}

void RSPropertiesPainter::DrawShadow(const RSProperties& properties, SkCanvas& canvas)
{
    if (properties.shadow_ && properties.shadow_->IsValid()) {
        canvas.save();
        SkPath skPath;
        if (properties.GetShadowPath()) {
            skPath = properties.GetShadowPath()->GetSkiaPath();
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        } else if (properties.GetClipBounds()) {
            skPath = properties.GetClipBounds()->GetSkiaPath();
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        } else {
            skPath.addRRect(RRect2SkRRect(properties.GetRRect()));
            canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), SkClipOp::kDifference, true);
        }
        skPath.offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
        SkColor spotColor = properties.GetShadowColor().AsArgbInt();
        if (properties.shadow_->GetHardwareAcceleration()) {
            SkPoint3 planeParams = { 0.0f, 0.0f, properties.GetShadowElevation() };
            SkPoint3 lightPos = { DEFAULT_LIGHT_POSITION_X, DEFAULT_LIGHT_POSITION_Y, DEFAULT_LIGHT_HEIGHT };
            SkColor ambientColor = DEFAULT_AMBIENT_COLOR;
            SkShadowUtils::DrawShadow(&canvas, skPath, planeParams, lightPos, DEFAULT_LIGHT_RADIUS, ambientColor,
                spotColor, SkShadowFlags::kTransparentOccluder_ShadowFlag);
        } else {
            SkPaint paint;
            paint.setColor(spotColor);
            paint.setAntiAlias(true);
            paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, properties.GetShadowRadius()));
            canvas.drawPath(skPath, paint);
        }
        canvas.restore();
    }
}

void RSPropertiesPainter::SaveLayerForFilter(const RSProperties& properties, SkCanvas& canvas,
    std::shared_ptr<RSSkiaFilter>& filter, const std::unique_ptr<SkRect>& rect)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    filter->ApplyTo(paint);
    if (rect != nullptr) {
        canvas.clipRect((*rect), true);
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }
    SkCanvas::SaveLayerRec slr(nullptr, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
    canvas.saveLayer(slr);
    RSRootRenderNode::MarkForceRaster();
}

void RSPropertiesPainter::RestoreForFilter(SkCanvas& canvas)
{
    canvas.restore();
}

void RSPropertiesPainter::DrawBackground(const RSProperties& properties, SkCanvas& canvas)
{
    auto filter = std::static_pointer_cast<RSSkiaFilter>(properties.GetBackgroundFilter());
    DrawShadow(properties, canvas);
    // clip
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else if (properties.GetClipToBounds()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }
    // paint backgroundColor
    if (filter != nullptr) {
        SaveLayerForFilter(properties, canvas, filter);
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    canvas.save();
    auto bgColor = properties.GetBackgroundColor();
    if (bgColor != RgbPalette::Transparent()) {
        paint.setColor(bgColor.AsArgbInt());
        canvas.drawRRect(RRect2SkRRect(properties.GetRRect()), paint);
    } else if (const auto& bgImage = properties.GetBgImage()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
        auto boundsRect = Rect2SkRect(properties.GetBoundsRect());
        bgImage->SetDstRect(properties.GetBgImageRect());
        bgImage->CanvasDrawImage(canvas, boundsRect, paint, true);
    } else if (const auto& bgShader = properties.GetBackgroundShader()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
        paint.setShader(bgShader->GetSkShader());
        canvas.drawPaint(paint);
    }
    canvas.restore();
    if (filter != nullptr) {
        RestoreForFilter(canvas);
    }
}

void RSPropertiesPainter::DrawFrame(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, std::shared_ptr<DrawCmdList>& cmds)
{
    if (cmds != nullptr) {
        SkMatrix mat;
        if (GetGravityMatrix(
                properties.GetFrameGravity(), properties.GetFrameRect(), cmds->GetWidth(), cmds->GetHeight(), mat)) {
            canvas.concat(mat);
        }
        auto frameRect = Rect2SkRect(properties.GetFrameRect());

        cmds->Playback(canvas, &frameRect);
    }
}

void RSPropertiesPainter::DrawBorder(const RSProperties& properties, SkCanvas& canvas)
{
    if (properties.GetBorderWidth() > 0.f) {
        auto borderWidth = properties.GetBorderWidth();
        auto borderStyle = properties.GetBorderStyle();
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(properties.GetBorderColor().AsArgbInt());
        if (BorderStyle::SOLID == borderStyle || BorderStyle::NONE == borderStyle) {
            paint.setStyle(SkPaint::Style::kFill_Style);
            canvas.drawDRRect(RRect2SkRRect(properties.GetRRect()), RRect2SkRRect(properties.GetInnerRRect()), paint);
            return;
        }
        paint.setStrokeWidth(borderWidth);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        if (properties.GetCornerRadius() > 0.f) {
            RRect rrect = properties.GetRRect();
            rrect.rect_.width_ -= borderWidth;
            rrect.rect_.height_ -= borderWidth;
            rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
            SkPath borderPath;
            borderPath.addRRect(RRect2SkRRect(rrect));
            SetBorderEffect(paint, borderStyle, borderWidth, 0.f, 0.f);
            canvas.drawPath(borderPath, paint);
        } else {
            float addLen = (borderStyle != BorderStyle::DOTTED) ? 0.0f : 0.5f;
            RectF rect = properties.GetBoundsRect();
            auto borderLengthVert = properties.GetBoundsHeight() - borderWidth * addLen * PARAM_DOUBLE;
            int32_t rawNumberVert = borderLengthVert / (PARAM_DOUBLE * borderWidth);
            auto borderLengthHoriz = properties.GetBoundsWidth() - borderWidth * addLen * PARAM_DOUBLE;
            int32_t rawNumberHoriz = borderLengthHoriz / (PARAM_DOUBLE * borderWidth);
            if (rawNumberVert == 0 || rawNumberHoriz == 0) {
                ROSEN_LOGE("number of dot is zero");
                return;
            }
            // draw left and right border
            SetBorderEffect(paint, borderStyle, borderWidth, borderLengthVert / rawNumberVert, borderLengthVert);
            canvas.drawLine(rect.left_ + borderWidth / PARAM_DOUBLE, rect.top_ + addLen * borderWidth,
                rect.left_ + borderWidth / PARAM_DOUBLE, rect.GetBottom() - borderWidth, paint);
            canvas.drawLine(rect.GetRight() - borderWidth / PARAM_DOUBLE, rect.GetBottom() - addLen * borderWidth,
                rect.GetRight() - borderWidth / PARAM_DOUBLE, rect.top_ + borderWidth, paint);
            // draw top and bottom border
            SetBorderEffect(paint, borderStyle, borderWidth, borderLengthHoriz / rawNumberHoriz, borderLengthHoriz);
            canvas.drawLine(rect.GetRight() - addLen * borderWidth, rect.top_ + borderWidth / PARAM_DOUBLE,
                rect.left_ + borderWidth, rect.top_ + borderWidth / PARAM_DOUBLE, paint);
            canvas.drawLine(rect.left_ + addLen * borderWidth, rect.GetBottom() - borderWidth / PARAM_DOUBLE,
                rect.GetRight() - borderWidth, rect.GetBottom() - borderWidth / PARAM_DOUBLE, paint);
        }
    }
}

void RSPropertiesPainter::DrawForegroundColor(const RSProperties& properties, SkCanvas& canvas)
{
    auto bgColor = properties.GetForegroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return;
    }
    // clip
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else if (properties.GetClipToBounds()) {
        canvas.clipRect(Rect2SkRect(properties.GetBoundsRect()), true);
    }

    SkPaint paint;
    paint.setColor(bgColor.AsArgbInt());
    paint.setAntiAlias(true);
    canvas.drawRRect(RRect2SkRRect(properties.GetRRect()), paint);
}

void RSPropertiesPainter::DrawTransitionProperties(const std::unique_ptr<RSTransitionProperties>& transitionProperties,
    const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    if (transitionProperties == nullptr) {
        return;
    }
    // alpha
    canvas.MultiplyAlpha(transitionProperties->GetAlpha());

    // translate, currently translateZ is not used
    auto translate = transitionProperties->GetTranslate();
    canvas.translate(translate.x_, translate.y_);

    // scale and rotate about the center of node, currently scaleZ is not used
    auto center = properties.GetBoundsSize() * 0.5f;
    auto scale = transitionProperties->GetScale();
    canvas.translate(center.x_, center.y_);
    canvas.scale(scale.x_, scale.y_);
    canvas.concat(transitionProperties->GetRotate());
    canvas.translate(-center.x_, -center.y_);
}

void RSPropertiesPainter::DrawMask(const RSProperties& properties, SkCanvas& canvas)
{
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawMask not has mask property");
        return;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom()) {
        ROSEN_LOGD("RSPropertiesPainter::DrawMask not has Svg Mask property");
        return;
    }

    canvas.save();
    SkRect maskBounds = Rect2SkRect(properties.GetBoundsRect());
    canvas.saveLayer(maskBounds, nullptr);
    int tmpLayer = canvas.getSaveCount();

    SkPaint maskfilter;
    auto filter = SkColorFilters::Compose(SkLumaColorFilter::Make(), SkColorFilters::SRGBToLinearGamma());
    maskfilter.setColorFilter(filter);
    canvas.saveLayer(maskBounds, &maskfilter);
    if (mask->IsSvgMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft + mask->GetSvgX(), maskBounds.fTop + mask->GetSvgY());
        canvas.scale(mask->GetScaleX(), mask->GetScaleY());
        mask->GetSvgDom()->render(&canvas);
    } else if (mask->IsGradientMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft, maskBounds.fTop);
        SkRect skRect = SkRect::MakeIWH(maskBounds.fRight - maskBounds.fLeft, maskBounds.fBottom - maskBounds.fTop);
        canvas.drawRect(skRect, mask->GetMaskPaint());
    } else if (mask->IsPathMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft, maskBounds.fTop);
        canvas.drawPath(mask->GetMaskPath(), mask->GetMaskPaint());
    }

    // back to mask layer
    canvas.restoreToCount(tmpLayer);
    // create content layer
    SkPaint maskPaint;
    maskPaint.setBlendMode(SkBlendMode::kSrcIn);
    canvas.saveLayer(maskBounds, &maskPaint);
    canvas.clipRect(maskBounds, true);
}
} // namespace Rosen
} // namespace OHOS
