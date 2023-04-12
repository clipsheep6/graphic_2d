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

#include "skia_paint.h"

#include "skia_color_filter.h"
#include "skia_color_space.h"
#include "skia_image_filter.h"
#include "skia_mask_filter.h"
#include "skia_path.h"
#include "skia_path_effect.h"
#include "skia_shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPaint::SkiaPaint() noexcept
    : stroke_(std::make_shared<PaintData>()), fill_(std::make_shared<PaintData>()), isStrokeFirst_(false)
{}

void SkiaPaint::ApplyBrushToFill(const Brush& brush)
{
    fill_->paint.reset();
    fill_->isEnabled = true;
    BrushToSkPaint(brush, fill_->paint);
}

void SkiaPaint::ApplyPenToStroke(const Pen& pen)
{
    stroke_->paint.reset();
    stroke_->isEnabled = true;
    PenToSkPaint(pen, stroke_->paint);
}

void SkiaPaint::BrushToSkPaint(const Brush& brush, SkPaint& paint) const
{
    paint.setColor(brush.GetColor().CastToColorQuad());

    auto cs = brush.GetColorSpace();
    if (cs != nullptr) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        paint.setColor(paint.getColor4f(), colorSpace.get());
    }

    paint.setAlpha(brush.GetAlpha());
    paint.setBlendMode(static_cast<SkBlendMode>(brush.GetBlendMode()));
    paint.setAntiAlias(brush.IsAntiAlias());

    auto s = brush.GetShaderEffect();
    if (s != nullptr) {
        auto skShaderImpl = s->GetImpl<SkiaShaderEffect>();
        sk_sp<SkShader> skShader = (skShaderImpl != nullptr) ? skShaderImpl->GetShader() : nullptr;
        paint.setShader(skShader);
    } else {
        paint.setShader(nullptr);
    }

    auto filter = brush.GetFilter();
    ApplyFilter(paint, filter);
    paint.setStyle(SkPaint::kFill_Style);
}

void SkiaPaint::PenToSkPaint(const Pen& pen, SkPaint& paint) const
{
    paint.setColor(pen.GetColor().CastToColorQuad());

    auto cs = pen.GetColorSpace();
    if (cs != nullptr) {
        auto skColorSpaceImpl = cs->GetImpl<SkiaColorSpace>();
        sk_sp<SkColorSpace> colorSpace = (skColorSpaceImpl != nullptr) ? skColorSpaceImpl->GetColorSpace() : nullptr;
        paint.setColor(paint.getColor4f(), colorSpace.get());
    }

    paint.setStrokeMiter(pen.GetMiterLimit());
    paint.setStrokeWidth(pen.GetWidth());
    paint.setAntiAlias(pen.IsAntiAlias());
    paint.setAlpha(pen.GetAlpha());
    paint.setBlendMode(static_cast<SkBlendMode>(pen.GetBlendMode()));

    switch (pen.GetCapStyle()) {
        case Pen::CapStyle::FLAT_CAP:
            paint.setStrokeCap(SkPaint::kButt_Cap);
            break;
        case Pen::CapStyle::SQUARE_CAP:
            paint.setStrokeCap(SkPaint::kSquare_Cap);
            break;
        case Pen::CapStyle::ROUND_CAP:
            paint.setStrokeCap(SkPaint::kRound_Cap);
            break;
        default:
            break;
    }

    switch (pen.GetJoinStyle()) {
        case Pen::JoinStyle::MITER_JOIN:
            paint.setStrokeJoin(SkPaint::kMiter_Join);
            break;
        case Pen::JoinStyle::ROUND_JOIN:
            paint.setStrokeJoin(SkPaint::kRound_Join);
            break;
        case Pen::JoinStyle::BEVEL_JOIN:
            paint.setStrokeJoin(SkPaint::kBevel_Join);
            break;
        default:
            break;
    }

    auto p = pen.GetPathEffect();
    if (p != nullptr) {
        auto skPathEffectImpl = p->GetImpl<SkiaPathEffect>();
        sk_sp<SkPathEffect> skPathEffect = (skPathEffectImpl != nullptr) ? skPathEffectImpl->GetPathEffect() : nullptr;
        paint.setPathEffect(skPathEffect);
    } else {
        paint.setPathEffect(nullptr);
    }

    auto s = pen.GetShaderEffect();
    if (s != nullptr) {
        auto skShaderImpl = s->GetImpl<SkiaShaderEffect>();
        sk_sp<SkShader> skShader = (skShaderImpl != nullptr) ? skShaderImpl->GetShader() : nullptr;
        paint.setShader(skShader);
    } else {
        paint.setShader(nullptr);
    }

    auto filter = pen.GetFilter();
    ApplyFilter(paint, filter);
    paint.setStyle(SkPaint::kStroke_Style);
}

void SkiaPaint::SkPaintToBrush(Brush& brush, const SkPaint& paint) const
{
    brush.SetColor(Color(paint.getColor()));
    brush.SetAlphaF(paint.getAlphaf());
    brush.SetBlendMode(static_cast<BlendMode>(paint.getBlendMode()));

    auto skShader = paint.refShader();
    if (skShader) {
        std::shared_ptr<ShaderEffect> shaderEffect = std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::NO_TYPE);
        auto skiaShaderImpl = shaderEffect->GetImpl<SkiaShaderEffect>();
        skiaShaderImpl->SetShader(skShader);
        brush.SetShaderEffect(shaderEffect);
    } else {
        brush.SetShaderEffect(nullptr);
    }

    Filter filter;
    GetFilter(paint, filter);

    brush.SetFilter(filter);
}

void SkiaPaint::SkPaintToPen(Pen& pen, const SkPaint& paint) const
{
    pen.SetColor(paint.getColor());
    pen.SetWidth(paint.getStrokeWidth());
    pen.SetAntiAlias(paint.isAntiAlias());
    pen.SetAlpha(paint.getAlpha());
    pen.SetBlendMode(static_cast<BlendMode>(paint.getBlendMode()));

    switch (paint.getStrokeCap()) {
        case SkPaint::kButt_Cap:
            pen.SetCapStyle(Pen::CapStyle::FLAT_CAP);
            break;
        case SkPaint::kSquare_Cap:
            pen.SetCapStyle(Pen::CapStyle::SQUARE_CAP);
            break;
        case SkPaint::kRound_Cap:
            pen.SetCapStyle(Pen::CapStyle::ROUND_CAP);
            break;
        default:
            break;
    }

    switch (paint.getStrokeJoin()) {
        case SkPaint::kMiter_Join:
            pen.SetJoinStyle(Pen::JoinStyle::MITER_JOIN);
            break;
        case SkPaint::kRound_Join:
            pen.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);
            break;
        case SkPaint::kBevel_Join:
            pen.SetJoinStyle(Pen::JoinStyle::BEVEL_JOIN);
            break;
        default:
            break;
    }

    auto skPathEffect = paint.refPathEffect();
    if (skPathEffect) {
        auto pathEffect = std::make_shared<PathEffect>(PathEffect::PathEffectType::NO_TYPE);
        auto skiaPathEffect = pathEffect->GetImpl<SkiaPathEffect>();
        skiaPathEffect->SetSkPathEffect(skPathEffect);
        pen.SetPathEffect(pathEffect);
    } else {
        pen.SetPathEffect(nullptr);
    }

    auto skShader = paint.refShader();
    if (skShader) {
        auto shaderEffect = std::make_shared<ShaderEffect>(ShaderEffect::ShaderEffectType::NO_TYPE);
        auto skiaShaderImpl = shaderEffect->GetImpl<SkiaShaderEffect>();
        skiaShaderImpl->SetSkShader(skShader);
        pen.SetShaderEffect(shaderEffect);
    } else {
        pen.SetShaderEffect(nullptr);
    }

    Filter filter;
    GetFilter(paint, filter);

    pen.SetFilter(filter);
}

void SkiaPaint::DisableStroke()
{
    stroke_->isEnabled = false;
}

void SkiaPaint::DisableFill()
{
    fill_->isEnabled = false;
}

std::vector<std::shared_ptr<PaintData>> SkiaPaint::GetSortedPaints() const
{
    std::vector<std::shared_ptr<PaintData>> paints;
    if (IsStrokeFirst() && stroke_->isEnabled && fill_->isEnabled) {
        paints.push_back(stroke_);
        paints.push_back(fill_);
        return paints;
    }

    if (fill_->isEnabled) {
        paints.push_back(fill_);
    }
    if (stroke_->isEnabled) {
        paints.push_back(stroke_);
    }
    return paints;
}

void SkiaPaint::SetStrokeFirst(bool isStrokeFirst)
{
    isStrokeFirst_ = isStrokeFirst;
}

bool SkiaPaint::IsStrokeFirst() const
{
    return isStrokeFirst_;
}

void SkiaPaint::ApplyFilter(SkPaint& paint, const Filter& filter) const
{
#if !defined(USE_CANVASKIT0310_SKIA)
    switch (filter.GetFilterQuality()) {
        case Filter::FilterQuality::LOW:
            paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
            break;
        case Filter::FilterQuality::MEDIUM:
            paint.setFilterQuality(SkFilterQuality::kMedium_SkFilterQuality);
            break;
        case Filter::FilterQuality::HIGH:
            paint.setFilterQuality(SkFilterQuality::kHigh_SkFilterQuality);
            break;
        case Filter::FilterQuality::NONE:
            paint.setFilterQuality(SkFilterQuality::kNone_SkFilterQuality);
            break;
        default:
            break;
    }
#endif

    auto c = filter.GetColorFilter();
    if (c != nullptr) {
        auto skColorFilterImpl = c->GetImpl<SkiaColorFilter>();
        sk_sp<SkColorFilter> colorFilter =
            (skColorFilterImpl != nullptr) ? skColorFilterImpl->GetColorFilter() : nullptr;
        paint.setColorFilter(colorFilter);
    } else {
        paint.setColorFilter(nullptr);
    }

    auto i = filter.GetImageFilter();
    if (i != nullptr) {
        auto skImageFilterImpl = i->GetImpl<SkiaImageFilter>();
        sk_sp<SkImageFilter> imageFilter =
            (skImageFilterImpl != nullptr) ? skImageFilterImpl->GetImageFilter() : nullptr;
        paint.setImageFilter(imageFilter);
    } else {
        paint.setImageFilter(nullptr);
    }

    auto m = filter.GetMaskFilter();
    if (m != nullptr) {
        auto skMaskFilterImpl = m->GetImpl<SkiaMaskFilter>();
        sk_sp<SkMaskFilter> maskFilter = (skMaskFilterImpl != nullptr) ? skMaskFilterImpl->GetMaskFilter() : nullptr;
        paint.setMaskFilter(maskFilter);
    } else {
        paint.setMaskFilter(nullptr);
    }
}

void SkiaPaint::GetFilter(const SkPaint& paint, Filter& filter) const
{
    auto skColorFilter = paint.refColorFilter();
    if (skColorFilter) {
        auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::NO_TYPE);
        auto skiaColorFilter = colorFilter->GetImpl<SkiaColorFilter>();
        skiaColorFilter->SetColorFilter(skColorFilter);
        filter.SetColorFilter(colorFilter);
    } else {
        filter.SetColorFilter(nullptr);
    }

    auto skImageFilter = paint.refImageFilter();
    if (skImageFilter) {
        auto imageFilter = std::make_shared<ImageFilter>(ImageFilter::FilterType::NO_TYPE);
        auto skiaImageFilter = imageFilter->GetImpl<SkiaImageFilter>();
        skiaImageFilter->SetSkImageFilter(skImageFilter);
        filter.SetImageFilter(imageFilter);
    } else {
        filter.SetImageFilter(nullptr);
    }

    auto skMaskFilter = paint.refMaskFilter();
    if (skMaskFilter) {
        auto maskFilter = std::make_shared<MaskFilter>(MaskFilter::FilterType::NO_TYPE);
        auto skiaMaskFilter = maskFilter->GetImpl<SkiaMaskFilter>();
        skiaMaskFilter->SetSkMaskFilter(skMaskFilter);
        filter.SetMaskFilter(maskFilter);
    } else {
        filter.SetMaskFilter(nullptr);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS