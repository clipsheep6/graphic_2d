/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef USE_ROSEN_DRAWING

#include "pipeline/rs_draw_cmd.h"
#ifdef ROSEN_OHOS
#include "buffer_utils.h"
#endif
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#include "message_parcel.h"
#include "rs_trace.h"
#include "securec.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t CORNER_SIZE = 4;
void SimplifyPaint(uint32_t color, SkPaint* paint)
{
    paint->setColor(color);
    paint->setShader(nullptr);
    paint->setColorFilter(nullptr);
    paint->setStrokeWidth(1.04); // 1.04 is empirical value
    paint->setStrokeJoin(SkPaint::kRound_Join);
}
} // namespace

OpItemTasks& OpItemTasks::Instance()
{
    static OpItemTasks instance;
    return instance;
}

void OpItemTasks::AddTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.push_back(task);
}

void OpItemTasks::ProcessTask()
{
    std::vector<std::function<void()>> tasks = {};
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::swap(tasks, tasks_);
    }
    for (auto& task : tasks) {
        task();
    }
}

std::unique_ptr<OpItem> OpItemWithPaint::GenerateCachedOpItem(
    const RSPaintFilterCanvas* canvas, const SkRect* rect) const
{
    // check if this opItem can be cached
    auto optionalBounds = GetCacheBounds();
    if (!optionalBounds.has_value() || optionalBounds.value().isEmpty()) {
        return nullptr;
    }
    auto& bounds = optionalBounds.value();

    // create surface & canvas to draw onto
    sk_sp<SkSurface> offscreenSurface = nullptr;
    if (auto surface = canvas != nullptr ? canvas->GetSurface() : nullptr) {
        // create GPU accelerated surface if possible
        offscreenSurface = surface->makeSurface(bounds.width(), bounds.height());
    } else {
        // create CPU raster surface
        auto offscreenInfo =
            SkImageInfo::Make(bounds.width(), bounds.height(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
        offscreenSurface = SkSurface::MakeRaster(offscreenInfo);
    }
    // check if surface is created successfully
    if (offscreenSurface == nullptr) {
        RS_LOGW("OpItem::GenerateCachedOpItem Failed to create offscreen surface, abort caching");
        return nullptr;
    }
    // create offscreen canvas and copy configuration from current canvas
    auto offscreenCanvas = std::make_unique<RSPaintFilterCanvas>(offscreenSurface.get());
    if (canvas) {
        offscreenCanvas->CopyConfiguration(*canvas);
    }

    // align draw op to [0, 0]
    if (bounds.left() != 0 || bounds.top() != 0) {
        offscreenCanvas->translate(-bounds.left(), -bounds.top());
    }

    // draw on the bitmap.
    Draw(*offscreenCanvas, rect);

    // generate BitmapOpItem with correct offset
    SkPaint paint;
    paint.setAntiAlias(true);
    if (paint_.getColor() == 0x00000001) {
#ifdef NEW_SKIA
        return std::make_unique<BitmapOpItem>(offscreenSurface->makeImageSnapshot(), bounds.x(), bounds.y(),
            SkSamplingOptions(), &paint);
#else
        return std::make_unique<BitmapOpItem>(offscreenSurface->makeImageSnapshot(), bounds.x(), bounds.y(), &paint);
#endif
    } else {
#ifdef NEW_SKIA
        return std::make_unique<ColorFilterBitmapOpItem>(
            offscreenSurface->makeImageSnapshot(), bounds.x(), bounds.y(), SkSamplingOptions(), &paint);
#else
        return std::make_unique<ColorFilterBitmapOpItem>(
            offscreenSurface->makeImageSnapshot(), bounds.x(), bounds.y(), &paint);
#endif
    }
}

void OpItemWithRSImage::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (rsImage_) {
#ifdef NEW_SKIA
        rsImage_->DrawImage(canvas, samplingOptions_, paint_);
#else
        rsImage_->DrawImage(canvas, paint_);
#endif
    }
}

RectOpItem::RectOpItem(SkRect rect, const SkPaint& paint) : OpItemWithPaint(sizeof(RectOpItem)), rect_(rect)
{
    paint_ = paint;
}

void RectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawRect(rect_, paint_);
}

RoundRectOpItem::RoundRectOpItem(const SkRRect& rrect, const SkPaint& paint)
    : OpItemWithPaint(sizeof(RoundRectOpItem)), rrect_(rrect)
{
    paint_ = paint;
}

void RoundRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawRRect(rrect_, paint_);
}

DRRectOpItem::DRRectOpItem(const SkRRect& outer, const SkRRect& inner, const SkPaint& paint)
    : OpItemWithPaint(sizeof(DRRectOpItem))
{
    outer_ = outer;
    inner_ = inner;
    paint_ = paint;
}

void DRRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawDRRect(outer_, inner_, paint_);
}

OvalOpItem::OvalOpItem(SkRect rect, const SkPaint& paint) : OpItemWithPaint(sizeof(OvalOpItem)), rect_(rect)
{
    paint_ = paint;
}

void OvalOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawOval(rect_, paint_);
}

RegionOpItem::RegionOpItem(SkRegion region, const SkPaint& paint) : OpItemWithPaint(sizeof(RegionOpItem))
{
    region_ = region;
    paint_ = paint;
}

void RegionOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawRegion(region_, paint_);
}

ArcOpItem::ArcOpItem(const SkRect& rect, float startAngle, float sweepAngle, bool useCenter, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ArcOpItem)), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle),
      useCenter_(useCenter)
{
    paint_ = paint;
}

void ArcOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawArc(rect_, startAngle_, sweepAngle_, useCenter_, paint_);
}

SaveOpItem::SaveOpItem() : OpItem(sizeof(SaveOpItem)) {}

void SaveOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.save();
}

RestoreOpItem::RestoreOpItem() : OpItem(sizeof(RestoreOpItem)) {}

void RestoreOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.restore();
}

FlushOpItem::FlushOpItem() : OpItem(sizeof(FlushOpItem)) {}

void FlushOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.flush();
}

#ifdef NEW_SKIA
MatrixOpItem::MatrixOpItem(const SkM44& matrix) : OpItem(sizeof(MatrixOpItem)), matrix_(matrix) {}
#else
MatrixOpItem::MatrixOpItem(const SkMatrix& matrix) : OpItem(sizeof(MatrixOpItem)), matrix_(matrix) {}
#endif

void MatrixOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.setMatrix(matrix_);
}

ClipRectOpItem::ClipRectOpItem(const SkRect& rect, SkClipOp op, bool doAA)
    : OpItem(sizeof(ClipRectOpItem)), rect_(rect), clipOp_(op), doAA_(doAA)
{}

void ClipRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.clipRect(rect_, clipOp_, doAA_);
}

ClipRRectOpItem::ClipRRectOpItem(const SkRRect& rrect, SkClipOp op, bool doAA)
    : OpItem(sizeof(ClipRRectOpItem)), rrect_(rrect), clipOp_(op), doAA_(doAA)
{}

void ClipRRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.clipRRect(rrect_, clipOp_, doAA_);
}

ClipRegionOpItem::ClipRegionOpItem(const SkRegion& region, SkClipOp op)
    : OpItem(sizeof(ClipRegionOpItem)), region_(region), clipOp_(op)
{}

void ClipRegionOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.clipRegion(region_, clipOp_);
}

TranslateOpItem::TranslateOpItem(float distanceX, float distanceY)
    : OpItem(sizeof(TranslateOpItem)), distanceX_(distanceX), distanceY_(distanceY)
{}

void TranslateOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.translate(distanceX_, distanceY_);
}

ScaleOpItem::ScaleOpItem(float scaleX, float scaleY)
    : OpItem(sizeof(ScaleOpItem)), scaleX_(scaleX), scaleY_(scaleY)
{}

void ScaleOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.scale(scaleX_, scaleY_);
}

TextBlobOpItem::TextBlobOpItem(const sk_sp<SkTextBlob> textBlob, float x, float y, const SkPaint& paint)
    : OpItemWithPaint(sizeof(TextBlobOpItem)), textBlob_(textBlob), x_(x), y_(y)
{
    paint_ = paint;
}

void TextBlobOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    bool isHighContrastEnabled = canvas.isHighContrastEnabled();
    if (isHighContrastEnabled) {
        ROSEN_LOGD("TextBlobOpItem::Draw highContrastEnabled");
        uint32_t color = paint_.getColor();
        uint32_t channelSum = SkColorGetR(color) + SkColorGetG(color) + SkColorGetB(color);
        bool flag = channelSum < 594; // 594 is empirical value

        SkPaint outlinePaint(paint_);
        SimplifyPaint(flag ? SK_ColorWHITE : SK_ColorBLACK, &outlinePaint);
        outlinePaint.setStyle(SkPaint::kStrokeAndFill_Style);
        canvas.drawTextBlob(textBlob_, x_, y_, outlinePaint);

        SkPaint innerPaint(paint_);
        SimplifyPaint(flag ? SK_ColorBLACK : SK_ColorWHITE, &innerPaint);
        innerPaint.setStyle(SkPaint::kFill_Style);
        canvas.drawTextBlob(textBlob_, x_, y_, innerPaint);
    } else {
        canvas.drawTextBlob(textBlob_, x_, y_, paint_);
    }
}

#ifdef NEW_SKIA
BitmapOpItem::BitmapOpItem(const sk_sp<SkImage> bitmapInfo, float left, float top,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint)
    : OpItemWithRSImage(samplingOptions, sizeof(BitmapOpItem)), samplingOptions_(samplingOptions)
{
    if (bitmapInfo) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetImage(bitmapInfo);
        rsImage_->SetSrcRect(RectF(0, 0, bitmapInfo->width(), bitmapInfo->height()));
        rsImage_->SetDstRect(RectF(left, top, bitmapInfo->width(), bitmapInfo->height()));
    }
    if (paint) {
        paint_ = *paint;
    }
}

BitmapOpItem::BitmapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkSamplingOptions& samplingOptions,
    const SkPaint& paint)
    : OpItemWithRSImage(rsImage, samplingOptions, paint, sizeof(BitmapOpItem)), samplingOptions_(samplingOptions)
{}

ColorFilterBitmapOpItem::ColorFilterBitmapOpItem(
    const sk_sp<SkImage> bitmapInfo, float left, float top,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint)
    : BitmapOpItem(bitmapInfo, left, top, SkSamplingOptions(), paint)
{}

ColorFilterBitmapOpItem::ColorFilterBitmapOpItem(std::shared_ptr<RSImageBase> rsImage,
    const SkSamplingOptions& samplingOptions, const SkPaint& paint)
    : BitmapOpItem(rsImage, SkSamplingOptions(), paint)
{}
#else
BitmapOpItem::BitmapOpItem(const sk_sp<SkImage> bitmapInfo, float left, float top, const SkPaint* paint)
    : OpItemWithRSImage(sizeof(BitmapOpItem))
{
    if (bitmapInfo) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetImage(bitmapInfo);
        rsImage_->SetSrcRect(RectF(0, 0, bitmapInfo->width(), bitmapInfo->height()));
        rsImage_->SetDstRect(RectF(left, top, bitmapInfo->width(), bitmapInfo->height()));
    }
    if (paint) {
        paint_ = *paint;
    }
}

BitmapOpItem::BitmapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint)
    : OpItemWithRSImage(rsImage, paint, sizeof(BitmapOpItem))
{}

ColorFilterBitmapOpItem::ColorFilterBitmapOpItem(
    const sk_sp<SkImage> bitmapInfo, float left, float top, const SkPaint* paint)
    : BitmapOpItem(bitmapInfo, left, top, paint)
{}

ColorFilterBitmapOpItem::ColorFilterBitmapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint)
    : BitmapOpItem(rsImage, paint)
{}
#endif

void ColorFilterBitmapOpItem::Draw(RSPaintFilterCanvas &canvas, const SkRect *) const
{
    auto colorFilterCanvas = std::make_shared<RSColorFilterCanvas>(&canvas);
    BitmapOpItem::Draw(*colorFilterCanvas, nullptr);
}

#ifdef NEW_SKIA
BitmapRectOpItem::BitmapRectOpItem(
    const sk_sp<SkImage> bitmapInfo, const SkRect* rectSrc, const SkRect& rectDst,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint, SkCanvas::SrcRectConstraint constraint)
    : OpItemWithRSImage(samplingOptions, sizeof(BitmapRectOpItem)),
    samplingOptions_(samplingOptions), constraint_(constraint)
{
    if (bitmapInfo) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetImage(bitmapInfo);
        rsImage_->SetSrcRect(rectSrc == nullptr ? RectF(0, 0, bitmapInfo->width(), bitmapInfo->height()) :
                             RectF(rectSrc->left(), rectSrc->top(), rectSrc->width(), rectSrc->height()));
        rsImage_->SetDstRect(RectF(rectDst.left(), rectDst.top(), rectDst.width(), rectDst.height()));
    }
    if (paint) {
        paint_ = *paint;
    }
}

BitmapRectOpItem::BitmapRectOpItem(std::shared_ptr<RSImageBase> rsImage, const SkSamplingOptions& samplingOptions,
    const SkPaint& paint, SkCanvas::SrcRectConstraint constraint)
    : OpItemWithRSImage(rsImage, samplingOptions, paint, sizeof(BitmapRectOpItem)),
    samplingOptions_(samplingOptions), constraint_(constraint)
{}
#else
BitmapRectOpItem::BitmapRectOpItem(
    const sk_sp<SkImage> bitmapInfo, const SkRect* rectSrc, const SkRect& rectDst, const SkPaint* paint)
    : OpItemWithRSImage(sizeof(BitmapRectOpItem))
{
    if (bitmapInfo) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetImage(bitmapInfo);
        rsImage_->SetSrcRect(rectSrc == nullptr ? RectF(0, 0, bitmapInfo->width(), bitmapInfo->height()) :
                             RectF(rectSrc->left(), rectSrc->top(), rectSrc->width(), rectSrc->height()));
        rsImage_->SetDstRect(RectF(rectDst.left(), rectDst.top(), rectDst.width(), rectDst.height()));
    }
    if (paint) {
        paint_ = *paint;
    }
}

BitmapRectOpItem::BitmapRectOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint)
    : OpItemWithRSImage(rsImage, paint, sizeof(BitmapRectOpItem))
{}
#endif

#ifdef NEW_SKIA
PixelMapOpItem::PixelMapOpItem(
    const std::shared_ptr<Media::PixelMap>& pixelmap, float left, float top,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint)
    : OpItemWithRSImage(samplingOptions, sizeof(PixelMapOpItem)), samplingOptions_(samplingOptions)
{
    if (pixelmap) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetPixelMap(pixelmap);
        rsImage_->SetSrcRect(RectF(0, 0, pixelmap->GetWidth(), pixelmap->GetHeight()));
        rsImage_->SetDstRect(RectF(left, top, pixelmap->GetWidth(), pixelmap->GetHeight()));
    }
    if (paint) {
        paint_ = *paint;
    }
}

PixelMapOpItem::PixelMapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkSamplingOptions& samplingOptions,
    const SkPaint& paint)
    : OpItemWithRSImage(rsImage, samplingOptions, paint, sizeof(PixelMapOpItem)), samplingOptions_(samplingOptions)
{}
#else
PixelMapOpItem::PixelMapOpItem(
    const std::shared_ptr<Media::PixelMap>& pixelmap, float left, float top, const SkPaint* paint)
    : OpItemWithRSImage(sizeof(PixelMapOpItem))
{
    if (pixelmap) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetPixelMap(pixelmap);
        rsImage_->SetSrcRect(RectF(0, 0, pixelmap->GetWidth(), pixelmap->GetHeight()));
        rsImage_->SetDstRect(RectF(left, top, pixelmap->GetWidth(), pixelmap->GetHeight()));
    }
    if (paint) {
        paint_ = *paint;
    }
}

PixelMapOpItem::PixelMapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint)
    : OpItemWithRSImage(rsImage, paint, sizeof(PixelMapOpItem))
{}
#endif

#ifdef NEW_SKIA
PixelMapRectOpItem::PixelMapRectOpItem(
    const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& src, const SkRect& dst,
    const SkSamplingOptions& samplingOptions, const SkPaint* paint, SkCanvas::SrcRectConstraint constraint)
    : OpItemWithRSImage(samplingOptions, sizeof(PixelMapRectOpItem)),
    samplingOptions_(samplingOptions), constraint_(constraint)
{
    if (pixelmap) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetPixelMap(pixelmap);
        rsImage_->SetSrcRect(RectF(src.left(), src.top(), src.width(), src.height()));
        rsImage_->SetDstRect(RectF(dst.left(), dst.top(), dst.width(), dst.height()));
    }
    if (paint) {
        paint_ = *paint;
    }
}

PixelMapRectOpItem::PixelMapRectOpItem(std::shared_ptr<RSImageBase> rsImage, const SkSamplingOptions& samplingOptions,
    const SkPaint& paint, SkCanvas::SrcRectConstraint constraint)
    : OpItemWithRSImage(rsImage, samplingOptions, paint, sizeof(PixelMapRectOpItem)),
    samplingOptions_(samplingOptions), constraint_(constraint)
{}
#else
PixelMapRectOpItem::PixelMapRectOpItem(
    const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& src, const SkRect& dst, const SkPaint* paint)
    : OpItemWithRSImage(sizeof(PixelMapRectOpItem))
{
    if (pixelmap) {
        rsImage_ = std::make_shared<RSImageBase>();
        rsImage_->SetPixelMap(pixelmap);
        rsImage_->SetSrcRect(RectF(src.left(), src.top(), src.width(), src.height()));
        rsImage_->SetDstRect(RectF(dst.left(), dst.top(), dst.width(), dst.height()));
    }
    if (paint) {
        paint_ = *paint;
    }
}

PixelMapRectOpItem::PixelMapRectOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint)
    : OpItemWithRSImage(rsImage, paint, sizeof(PixelMapRectOpItem))
{}
#endif

#ifdef NEW_SKIA
BitmapNineOpItem::BitmapNineOpItem(const sk_sp<SkImage> bitmapInfo, const SkIRect& center, const SkRect& rectDst,
    SkFilterMode filter, const SkPaint* paint)
    : OpItemWithPaint(sizeof(BitmapNineOpItem)), center_(center), rectDst_(rectDst), filter_(filter)
{
    if (bitmapInfo) {
        bitmapInfo_ = bitmapInfo;
    }
    if (paint) {
        paint_ = *paint;
    }
}

void BitmapNineOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawImageNine(bitmapInfo_.get(), center_, rectDst_, filter_, &paint_);
}
#else
BitmapNineOpItem::BitmapNineOpItem(
    const sk_sp<SkImage> bitmapInfo, const SkIRect& center, const SkRect& rectDst, const SkPaint* paint)
    : OpItemWithPaint(sizeof(BitmapNineOpItem)), center_(center), rectDst_(rectDst)
{
    if (bitmapInfo) {
        bitmapInfo_ = bitmapInfo;
    }
    if (paint) {
        paint_ = *paint;
    }
}

void BitmapNineOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawImageNine(bitmapInfo_, center_, rectDst_, &paint_);
}
#endif

AdaptiveRRectOpItem::AdaptiveRRectOpItem(float radius, const SkPaint& paint)
    : OpItemWithPaint(sizeof(AdaptiveRRectOpItem)), radius_(radius), paint_(paint)
{}

void AdaptiveRRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (!rect) {
        ROSEN_LOGE("AdaptiveRRectOpItem::Draw, skrect is null");
        return;
    }
    SkRRect rrect = SkRRect::MakeRectXY(*rect, radius_, radius_);
    canvas.drawRRect(rrect, paint_);
}

AdaptiveRRectScaleOpItem::AdaptiveRRectScaleOpItem(float radiusRatio, const SkPaint& paint)
    : OpItemWithPaint(sizeof(AdaptiveRRectScaleOpItem)), radiusRatio_(radiusRatio), paint_(paint)
{}

void AdaptiveRRectScaleOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (!rect) {
        ROSEN_LOGE("AdaptiveRRectScaleOpItem::Draw, skrect is null");
        return;
    }
    SkRRect rrect = SkRRect::MakeRectXY(*rect, radiusRatio_ * (*rect).height(), radiusRatio_ * (*rect).height());
    canvas.drawRRect(rrect, paint_);
}

ClipAdaptiveRRectOpItem::ClipAdaptiveRRectOpItem(const SkVector radius[])
    : OpItem(sizeof(ClipAdaptiveRRectOpItem))
{
    errno_t ret = memcpy_s(radius_, CORNER_SIZE * sizeof(SkVector), radius, CORNER_SIZE * sizeof(SkVector));
    if (ret != EOK) {
        ROSEN_LOGE("ClipAdaptiveRRectOpItem: memcpy failed!");
    }
}

void ClipAdaptiveRRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (!rect) {
        ROSEN_LOGE("ClipAdaptiveRRectOpItem::Draw skrect is null");
        return;
    }
    SkRRect rrect = SkRRect::MakeEmpty();
    rrect.setRectRadii(*rect, radius_);
    canvas.clipRRect(rrect, true);
}

ClipOutsetRectOpItem::ClipOutsetRectOpItem(float dx, float dy)
    : OpItem(sizeof(ClipOutsetRectOpItem)), dx_(dx), dy_(dy)
{}

void ClipOutsetRectOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    auto clipRect = canvas.getLocalClipBounds().makeOutset(dx_, dy_);
#ifdef NEW_SKIA
    canvas.clipRect(clipRect, SkClipOp::kIntersect, true);
#else
    canvas.clipRect(clipRect, SkClipOp::kExtraEnumNeedInternallyPleaseIgnoreWillGoAway5, true);
#endif
}

PathOpItem::PathOpItem(const SkPath& path, const SkPaint& paint) : OpItemWithPaint(sizeof(PathOpItem))
{
    path_ = path;
    paint_ = paint;
}

void PathOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawPath(path_, paint_);
}

ClipPathOpItem::ClipPathOpItem(const SkPath& path, SkClipOp clipOp, bool doAA)
    : OpItem(sizeof(ClipPathOpItem)), path_(path), clipOp_(clipOp), doAA_(doAA)
{}

void ClipPathOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.clipPath(path_, clipOp_, doAA_);
}

PaintOpItem::PaintOpItem(const SkPaint& paint) : OpItemWithPaint(sizeof(PaintOpItem))
{
    paint_ = paint;
}

void PaintOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawPaint(paint_);
}

#ifdef NEW_SKIA
ImageWithParmOpItem::ImageWithParmOpItem(const sk_sp<SkImage> img, const sk_sp<SkData> data,
    const RsImageInfo& rsimageInfo, const SkSamplingOptions& samplingOptions, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ImageWithParmOpItem)), samplingOptions_(samplingOptions)
#else
ImageWithParmOpItem::ImageWithParmOpItem(const sk_sp<SkImage> img, const sk_sp<SkData> data,
    const RsImageInfo& rsimageInfo, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ImageWithParmOpItem))
#endif
{
    rsImage_ = std::make_shared<RSImage>();
    rsImage_->SetImage(img);
    rsImage_->SetCompressData(data, rsimageInfo.uniqueId_, rsimageInfo.width_, rsimageInfo.height_);
    rsImage_->SetImageFit(rsimageInfo.fitNum_);
    rsImage_->SetImageRepeat(rsimageInfo.repeatNum_);
    rsImage_->SetRadius(rsimageInfo.radius_);
    rsImage_->SetScale(rsimageInfo.scale_);
    paint_ = paint;
}

#ifdef NEW_SKIA
ImageWithParmOpItem::ImageWithParmOpItem(
    const std::shared_ptr<Media::PixelMap>& pixelmap, const RsImageInfo& rsimageInfo,
    const SkSamplingOptions& samplingOptions, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ImageWithParmOpItem)), samplingOptions_(samplingOptions)
#else
ImageWithParmOpItem::ImageWithParmOpItem(
    const std::shared_ptr<Media::PixelMap>& pixelmap, const RsImageInfo& rsimageInfo, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ImageWithParmOpItem))
#endif
{
    rsImage_ = std::make_shared<RSImage>();
    rsImage_->SetPixelMap(pixelmap);
    rsImage_->SetImageFit(rsimageInfo.fitNum_);
    rsImage_->SetImageRepeat(rsimageInfo.repeatNum_);
    rsImage_->SetRadius(rsimageInfo.radius_);
    rsImage_->SetScale(rsimageInfo.scale_);
    paint_ = paint;
}

#ifdef NEW_SKIA
ImageWithParmOpItem::ImageWithParmOpItem(const std::shared_ptr<RSImage>& rsImage,
    const SkSamplingOptions& samplingOptions, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ImageWithParmOpItem)), rsImage_(rsImage), samplingOptions_(samplingOptions)
#else
ImageWithParmOpItem::ImageWithParmOpItem(const std::shared_ptr<RSImage>& rsImage, const SkPaint& paint)
    : OpItemWithPaint(sizeof(ImageWithParmOpItem)), rsImage_(rsImage)
#endif
{
    paint_ = paint;
}
ImageWithParmOpItem::~ImageWithParmOpItem()
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
    if (texId_ != 0U) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &texId_);
        texId_ = 0U;
    }
    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
    if (eglImage_ != EGL_NO_IMAGE_KHR) {
        auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglDestroyImageKHR(disp, eglImage_);
        eglImage_ = EGL_NO_IMAGE_KHR;
    }
#endif
#endif
}
void ImageWithParmOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (!rect) {
        ROSEN_LOGE("ImageWithParmOpItem: no rect");
        return;
    }

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<Media::PixelMap> pixelmap = rsImage_->GetPixelMap();
    if (pixelmap != nullptr && pixelmap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        sk_sp<SkImage> dmaImage = GetSkImageFromSurfaceBuffer(canvas,
            reinterpret_cast<SurfaceBuffer*> (pixelmap->GetFd()));
        rsImage_->SetImage(dmaImage);
    }
#endif
#endif

#ifdef NEW_SKIA
    rsImage_->CanvasDrawImage(canvas, *rect, samplingOptions_, paint_);
#else
    rsImage_->CanvasDrawImage(canvas, *rect, paint_);
#endif
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> ImageWithParmOpItem::GetSkImageFromSurfaceBuffer(SkCanvas& canvas, SurfaceBuffer* surfaceBuffer) const
{
    if (surfaceBuffer == nullptr) {
        RS_LOGE("GetSkImageFromSurfaceBuffer surfaceBuffer is nullptr");
        return nullptr;
    }
    if (nativeWindowBuffer_ == nullptr) {
        sptr<SurfaceBuffer> sfBuffer(surfaceBuffer);
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&sfBuffer);
        if (!nativeWindowBuffer_) {
            RS_LOGE("GetSkImageFromSurfaceBuffer create native window buffer fail");
            return nullptr;
        }
    }
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED,
        EGL_TRUE,
        EGL_NONE,
    };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
        if (eglImage_ == EGL_NO_IMAGE_KHR) {
            RS_LOGE("%s create egl image fail %d", __func__, eglGetError());
            return nullptr;
        }
    }

    // Create texture object
    if (texId_ == 0U) {
        glGenTextures(1, &texId_);
        glBindTexture(GL_TEXTURE_2D, texId_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, static_cast<GLeglImageOES>(eglImage_));
    }

    GrGLTextureInfo textureInfo = { GL_TEXTURE_2D, texId_, GL_RGBA8_OES };

    GrBackendTexture backendTexture(
        surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), GrMipMapped::kNo, textureInfo);
#ifdef NEW_SKIA
    auto skImage = SkImage::MakeFromTexture(canvas.recordingContext(), backendTexture, kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
#else
    auto skImage = SkImage::MakeFromTexture(canvas.getGrContext(), backendTexture, kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
#endif
    return skImage;
}
#endif // USE_ROSEN_DRAWING
#endif // ROSEN_OHOS & RS_ENABLE_GL

#ifdef NEW_SKIA
ConcatOpItem::ConcatOpItem(const SkM44& matrix) : OpItem(sizeof(ConcatOpItem)), matrix_(matrix) {}
#else
ConcatOpItem::ConcatOpItem(const SkMatrix& matrix) : OpItem(sizeof(ConcatOpItem)), matrix_(matrix) {}
#endif

void ConcatOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.concat(matrix_);
}

SaveLayerOpItem::SaveLayerOpItem(const SkCanvas::SaveLayerRec& rec) : OpItemWithPaint(sizeof(SaveLayerOpItem))
{
    if (rec.fBounds) {
        rect_ = *rec.fBounds;
        rectPtr_ = &rect_;
    }
    if (rec.fPaint) {
        paint_ = *rec.fPaint;
    }
    backdrop_ = sk_ref_sp(rec.fBackdrop);
    flags_ = rec.fSaveLayerFlags;
#ifndef NEW_SKIA
    mask_ = sk_ref_sp(rec.fClipMask);
    matrix_ = rec.fClipMatrix ? *(rec.fClipMatrix) : SkMatrix::I();
#endif
}

void SaveLayerOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
#ifdef NEW_SKIA
    canvas.saveLayer(
        { rectPtr_, &paint_, backdrop_.get(), flags_ });
#else
    canvas.saveLayer(
        { rectPtr_, &paint_, backdrop_.get(), mask_.get(), matrix_.isIdentity() ? nullptr : &matrix_, flags_ });
#endif
}

DrawableOpItem::DrawableOpItem(SkDrawable* drawable, const SkMatrix* matrix) : OpItem(sizeof(DrawableOpItem))
{
    drawable_ = sk_ref_sp(drawable);
    if (matrix) {
        matrix_ = *matrix;
    }
}

void DrawableOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawDrawable(drawable_.get(), &matrix_);
}

PictureOpItem::PictureOpItem(const sk_sp<SkPicture> picture, const SkMatrix* matrix, const SkPaint* paint)
    : OpItemWithPaint(sizeof(PictureOpItem)), picture_(picture)
{
    if (matrix) {
        matrix_ = *matrix;
    }
    if (paint) {
        paint_ = *paint;
    }
}

void PictureOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawPicture(picture_, &matrix_, &paint_);
}

PointsOpItem::PointsOpItem(SkCanvas::PointMode mode, int count, const SkPoint processedPoints[], const SkPaint& paint)
    : OpItemWithPaint(sizeof(PointsOpItem)), mode_(mode), count_(count), processedPoints_(new SkPoint[count])
{
    errno_t ret = memcpy_s(processedPoints_, count * sizeof(SkPoint), processedPoints, count * sizeof(SkPoint));
    if (ret != EOK) {
        ROSEN_LOGE("PointsOpItem: memcpy failed!");
    }
    paint_ = paint;
}

void PointsOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.drawPoints(mode_, count_, processedPoints_, paint_);
}

#ifdef NEW_SKIA
VerticesOpItem::VerticesOpItem(const SkVertices* vertices, SkBlendMode mode, const SkPaint& paint)
    : OpItemWithPaint(sizeof(VerticesOpItem)),
      vertices_(sk_ref_sp(const_cast<SkVertices*>(vertices))),
      mode_(mode)
{
    paint_ = paint;
}
#else
VerticesOpItem::VerticesOpItem(const SkVertices* vertices, const SkVertices::Bone bones[],
    int boneCount, SkBlendMode mode, const SkPaint& paint)
    : OpItemWithPaint(sizeof(VerticesOpItem)), vertices_(sk_ref_sp(const_cast<SkVertices*>(vertices))),
      bones_(new SkVertices::Bone[boneCount]), boneCount_(boneCount), mode_(mode)
{
    errno_t ret = memcpy_s(bones_, boneCount * sizeof(SkVertices::Bone), bones, boneCount * sizeof(SkVertices::Bone));
    if (ret != EOK) {
        ROSEN_LOGE("VerticesOpItem: memcpy failed!");
    }
    paint_ = paint;
}
#endif
VerticesOpItem::~VerticesOpItem()
{
#ifndef NEW_SKIA
    delete[] bones_;
#endif
}

void VerticesOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
#ifdef NEW_SKIA
    canvas.drawVertices(vertices_, mode_, paint_);
#else
    canvas.drawVertices(vertices_, bones_, boneCount_, mode_, paint_);
#endif
}

ShadowRecOpItem::ShadowRecOpItem(const SkPath& path, const SkDrawShadowRec& rec)
    : OpItem(sizeof(ShadowRecOpItem)), path_(path), rec_(rec)
{}

void ShadowRecOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    auto rec = rec_;
    if (canvas.GetAlpha() < 1.f) {
        rec.fAmbientColor = SkColorSetA(rec_.fAmbientColor,
            static_cast<unsigned>(canvas.GetAlpha() * SkColorGetA(rec_.fAmbientColor)));
        rec.fSpotColor = SkColorSetA(rec_.fSpotColor,
            static_cast<unsigned>(canvas.GetAlpha() * SkColorGetA(rec_.fSpotColor)));
    }
    canvas.private_draw_shadow_rec(path_, rec);
}

MultiplyAlphaOpItem::MultiplyAlphaOpItem(float alpha) : OpItem(sizeof(MultiplyAlphaOpItem)), alpha_(alpha) {}

void MultiplyAlphaOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.MultiplyAlpha(alpha_);
}

SaveAlphaOpItem::SaveAlphaOpItem() : OpItem(sizeof(SaveAlphaOpItem)) {}

void SaveAlphaOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.SaveAlpha();
}

RestoreAlphaOpItem::RestoreAlphaOpItem() : OpItem(sizeof(RestoreAlphaOpItem)) {}

void RestoreAlphaOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
    canvas.RestoreAlpha();
}

#ifdef ROSEN_OHOS
SurfaceBufferOpItem::SurfaceBufferOpItem(const RSSurfaceBufferInfo& surfaceBufferInfo)
    : OpItemWithPaint(sizeof(SurfaceBufferOpItem)), surfaceBufferInfo_(surfaceBufferInfo)
{}

SurfaceBufferOpItem::~SurfaceBufferOpItem()
{
#ifdef RS_ENABLE_GL
    if (eglImage_ != EGL_NO_IMAGE_KHR) {
        auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglDestroyImageKHR(disp, eglImage_);
    }
    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
    }
    if (texId_ != 0U) {
        glDeleteTextures(1, &texId_);
    }
#endif
}

void SurfaceBufferOpItem::Draw(RSPaintFilterCanvas& canvas, const SkRect*) const
{
#ifdef RS_ENABLE_GL
    if (surfaceBufferInfo_.surfaceBuffer_ == nullptr) {
        ROSEN_LOGE("SurfaceBufferOpItem::Draw surfaceBuffer_ is nullptr");
        return;
    }
    nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&(surfaceBufferInfo_.surfaceBuffer_));
    if (!nativeWindowBuffer_) {
        ROSEN_LOGE("SurfaceBufferOpItem::Draw create native window buffer fail");
        return;
    }
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED,
        EGL_TRUE,
        EGL_NONE,
    };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        ROSEN_LOGE("%s create egl image fail %d", __func__, eglGetError());
        return;
    }

    // save
    GLuint originTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint *>(&originTexture));
    GLint minFilter;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
    GLint magFilter;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
    GLint wrapS;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
    GLint wrapT;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);

    // Create texture object
    texId_ = 0;
    glGenTextures(1, &texId_);
    glBindTexture(GL_TEXTURE_2D, texId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, static_cast<GLeglImageOES>(eglImage_));

    // restore
    glBindTexture(GL_TEXTURE_2D, originTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    GrGLTextureInfo textureInfo = { GL_TEXTURE_2D, texId_, GL_RGBA8_OES };

    GrBackendTexture backendTexture(
        surfaceBufferInfo_.width_, surfaceBufferInfo_.height_, GrMipMapped::kNo, textureInfo);
#ifdef NEW_SKIA
    auto skImage = SkImage::MakeFromTexture(canvas.recordingContext(), backendTexture, kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
#else
    auto skImage = SkImage::MakeFromTexture(canvas.getGrContext(), backendTexture, kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
#endif
    canvas.drawImage(skImage, surfaceBufferInfo_.offSetX_, surfaceBufferInfo_.offSetY_);
#endif // RS_ENABLE_GL
}
#endif // ROSEN_OHOS

// RectOpItem
bool RectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rect_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("RectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* RectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rect) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("RectOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new RectOpItem(rect, paint);
}

// RoundRectOpItem
bool RoundRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rrect_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("RoundRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* RoundRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRRect rrect;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rrect) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("RoundRectOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new RoundRectOpItem(rrect, paint);
}

// ImageWithParmOpItem
bool ImageWithParmOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rsImage_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
#ifdef NEW_SKIA
    success = success && RSMarshallingHelper::Marshalling(parcel, samplingOptions_);
#endif
    if (!success) {
        ROSEN_LOGE("ImageWithParmOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ImageWithParmOpItem::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<RSImage> rsImage;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rsImage) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
#ifdef NEW_SKIA
    SkSamplingOptions samplingOptions;
    success = success && RSMarshallingHelper::Unmarshalling(parcel, samplingOptions);
#endif
    if (!success) {
        ROSEN_LOGE("ImageWithParmOpItem::Unmarshalling failed!");
        return nullptr;
    }
#ifdef NEW_SKIA
    return new ImageWithParmOpItem(rsImage, samplingOptions, paint);
#else
    return new ImageWithParmOpItem(rsImage, paint);
#endif
}

// DRRectOpItem
bool DRRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, outer_) &&
                   RSMarshallingHelper::Marshalling(parcel, inner_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("DRRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* DRRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRRect outer;
    SkRRect inner;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, outer) &&
                   RSMarshallingHelper::Unmarshalling(parcel, inner) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("DRRectOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new DRRectOpItem(outer, inner, paint);
}

// OvalOpItem
bool OvalOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rect_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("OvalOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* OvalOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rect) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("OvalOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new OvalOpItem(rect, paint);
}

// RegionOpItem
bool RegionOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, region_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("RegionOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* RegionOpItem::Unmarshalling(Parcel& parcel)
{
    SkRegion region;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, region) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("RegionOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new RegionOpItem(region, paint);
}

// ArcOpItem
bool ArcOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rect_) &&
                   RSMarshallingHelper::Marshalling(parcel, startAngle_) &&
                   RSMarshallingHelper::Marshalling(parcel, sweepAngle_) &&
                   RSMarshallingHelper::Marshalling(parcel, useCenter_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("ArcOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ArcOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    float startAngle;
    float sweepAngle;
    bool useCenter;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rect) &&
                   RSMarshallingHelper::Unmarshalling(parcel, startAngle) &&
                   RSMarshallingHelper::Unmarshalling(parcel, sweepAngle) &&
                   RSMarshallingHelper::Unmarshalling(parcel, useCenter) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("ArcOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ArcOpItem(rect, startAngle, sweepAngle, useCenter, paint);
}

// SaveOpItem
OpItem* SaveOpItem::Unmarshalling(Parcel& parcel)
{
    return new SaveOpItem();
}

// RestoreOpItem
OpItem* RestoreOpItem::Unmarshalling(Parcel& parcel)
{
    return new RestoreOpItem();
}

// FlushOpItem
OpItem* FlushOpItem::Unmarshalling(Parcel& parcel)
{
    return new FlushOpItem();
}

// MatrixOpItem
bool MatrixOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, matrix_);
    if (!success) {
        ROSEN_LOGE("MatrixOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* MatrixOpItem::Unmarshalling(Parcel& parcel)
{
#ifdef NEW_SKIA
    SkM44 matrix;
#else
    SkMatrix matrix;
#endif
    bool success = RSMarshallingHelper::Unmarshalling(parcel, matrix);
    if (!success) {
        ROSEN_LOGE("MatrixOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new MatrixOpItem(matrix);
}

// ClipRectOpItem
bool ClipRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rect_) &&
                   RSMarshallingHelper::Marshalling(parcel, clipOp_) &&
                   RSMarshallingHelper::Marshalling(parcel, doAA_);
    if (!success) {
        ROSEN_LOGE("ClipRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ClipRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRect rect;
    SkClipOp clipOp;
    bool doAA;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rect) &&
                   RSMarshallingHelper::Unmarshalling(parcel, clipOp) &&
                   RSMarshallingHelper::Unmarshalling(parcel, doAA);
    if (!success) {
        ROSEN_LOGE("ClipRectOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ClipRectOpItem(rect, clipOp, doAA);
}

// ClipRRectOpItem
bool ClipRRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rrect_) &&
                   RSMarshallingHelper::Marshalling(parcel, clipOp_) &&
                   RSMarshallingHelper::Marshalling(parcel, doAA_);
    if (!success) {
        ROSEN_LOGE("ClipRRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ClipRRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkRRect rrect;
    SkClipOp clipOp;
    bool doAA;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rrect) &&
                   RSMarshallingHelper::Unmarshalling(parcel, clipOp) &&
                   RSMarshallingHelper::Unmarshalling(parcel, doAA);
    if (!success) {
        ROSEN_LOGE("ClipRRectOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ClipRRectOpItem(rrect, clipOp, doAA);
}

// ClipRegionOpItem
bool ClipRegionOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, region_) &&
                   RSMarshallingHelper::Marshalling(parcel, clipOp_);
    if (!success) {
        ROSEN_LOGE("ClipRegionOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ClipRegionOpItem::Unmarshalling(Parcel& parcel)
{
    SkRegion region;
    SkClipOp clipOp;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, region) &&
                   RSMarshallingHelper::Unmarshalling(parcel, clipOp);
    if (!success) {
        ROSEN_LOGE("ClipRegionOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ClipRegionOpItem(region, clipOp);
}

// TranslateOpItem
bool TranslateOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, distanceX_) &&
                   RSMarshallingHelper::Marshalling(parcel, distanceY_);
    if (!success) {
        ROSEN_LOGE("TranslateOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* TranslateOpItem::Unmarshalling(Parcel& parcel)
{
    float distanceX;
    float distanceY;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, distanceX) &&
                   RSMarshallingHelper::Unmarshalling(parcel, distanceY);
    if (!success) {
        ROSEN_LOGE("TranslateOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new TranslateOpItem(distanceX, distanceY);
}

// ScaleOpItem
bool ScaleOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, scaleX_) &&
                   RSMarshallingHelper::Marshalling(parcel, scaleY_);
    if (!success) {
        ROSEN_LOGE("ScaleOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ScaleOpItem::Unmarshalling(Parcel& parcel)
{
    float scaleX;
    float scaleY;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, scaleX) &&
                   RSMarshallingHelper::Unmarshalling(parcel, scaleY);
    if (!success) {
        ROSEN_LOGE("ScaleOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ScaleOpItem(scaleX, scaleY);
}

// TextBlobOpItem
bool TextBlobOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, textBlob_) &&
                   RSMarshallingHelper::Marshalling(parcel, x_) &&
                   RSMarshallingHelper::Marshalling(parcel, y_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("TextBlobOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* TextBlobOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkTextBlob> textBlob;
    float x;
    float y;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, textBlob) &&
                   RSMarshallingHelper::Unmarshalling(parcel, x) &&
                   RSMarshallingHelper::Unmarshalling(parcel, y) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("TextBlobOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new TextBlobOpItem(textBlob, x, y, paint);
}

// BitmapOpItem
bool BitmapOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rsImage_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
#ifdef NEW_SKIA
    success = success && RSMarshallingHelper::Marshalling(parcel, samplingOptions_);
#endif
    if (!success) {
        ROSEN_LOGE("BitmapOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* BitmapOpItem::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<RSImageBase> rsImage;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rsImage) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
#ifdef NEW_SKIA
    SkSamplingOptions samplingOptions;
    success = success && RSMarshallingHelper::Unmarshalling(parcel, samplingOptions);
#endif
    if (!success) {
        ROSEN_LOGE("BitmapOpItem::Unmarshalling failed!");
        return nullptr;
    }

#ifdef NEW_SKIA
    return new BitmapOpItem(rsImage, samplingOptions, paint);
#else
    return new BitmapOpItem(rsImage, paint);
#endif
}

// ColorFilterBitmapOpItem
bool ColorFilterBitmapOpItem::Marshalling(Parcel& parcel) const
{
    return BitmapOpItem::Marshalling(parcel);
}

OpItem* ColorFilterBitmapOpItem::Unmarshalling(Parcel &parcel)
{
    return BitmapOpItem::Unmarshalling(parcel);
}


// BitmapRectOpItem
bool BitmapRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rsImage_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
#ifdef NEW_SKIA
    success = success && RSMarshallingHelper::Marshalling(parcel, samplingOptions_) &&
        RSMarshallingHelper::Marshalling(parcel, static_cast<int32_t>(constraint_));
#endif
    if (!success) {
        ROSEN_LOGE("BitmapRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* BitmapRectOpItem::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<RSImageBase> rsImage;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rsImage) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
#ifdef NEW_SKIA
    SkSamplingOptions samplingOptions;
    int32_t constraint;
    success = success && RSMarshallingHelper::Unmarshalling(parcel, samplingOptions) &&
        RSMarshallingHelper::Unmarshalling(parcel, constraint);
#endif
    if (!success) {
        ROSEN_LOGE("BitmapRectOpItem::Unmarshalling failed!");
        return nullptr;
    }

#ifdef NEW_SKIA
    return new BitmapRectOpItem(rsImage, samplingOptions, paint, static_cast<SkCanvas::SrcRectConstraint>(constraint));
#else
    return new BitmapRectOpItem(rsImage, paint);
#endif
}

// PixelMapOpItem
bool PixelMapOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rsImage_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
#ifdef NEW_SKIA
    success = success && RSMarshallingHelper::Marshalling(parcel, samplingOptions_);
#endif
    if (!success) {
        ROSEN_LOGE("PixelMapOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* PixelMapOpItem::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<RSImageBase> rsImage;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rsImage) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
#ifdef NEW_SKIA
    SkSamplingOptions samplingOptions;
    success = success && RSMarshallingHelper::Unmarshalling(parcel, samplingOptions);
#endif
    if (!success) {
        ROSEN_LOGE("PixelMapOpItem::Unmarshalling failed!");
        return nullptr;
    }

#ifdef NEW_SKIA
    return new PixelMapOpItem(rsImage, samplingOptions, paint);
#else
    return new PixelMapOpItem(rsImage, paint);
#endif
}

// PixelMapRectOpItem
bool PixelMapRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, rsImage_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
#ifdef NEW_SKIA
    success = success && RSMarshallingHelper::Marshalling(parcel, samplingOptions_) &&
        RSMarshallingHelper::Marshalling(parcel, static_cast<int32_t>(constraint_));
#endif
    if (!success) {
        ROSEN_LOGE("PixelMapRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* PixelMapRectOpItem::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<RSImageBase> rsImage;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, rsImage) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
#ifdef NEW_SKIA
    SkSamplingOptions samplingOptions;
    int32_t constraint;
    success = success && RSMarshallingHelper::Unmarshalling(parcel, samplingOptions) &&
        RSMarshallingHelper::Unmarshalling(parcel, constraint);
#endif
    if (!success) {
        ROSEN_LOGE("PixelMapRectOpItem::Unmarshalling failed!");
        return nullptr;
    }
#ifdef NEW_SKIA
    return new PixelMapRectOpItem(rsImage, samplingOptions,
        paint, static_cast<SkCanvas::SrcRectConstraint>(constraint));
#else
    return new PixelMapRectOpItem(rsImage, paint);
#endif
}

// BitmapNineOpItem
bool BitmapNineOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, bitmapInfo_) &&
                   RSMarshallingHelper::Marshalling(parcel, center_) &&
                   RSMarshallingHelper::Marshalling(parcel, rectDst_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
#ifdef NEW_SKIA
    success = success && RSMarshallingHelper::Marshalling(parcel, static_cast<int32_t>(filter_));
#endif
    if (!success) {
        ROSEN_LOGE("BitmapNineOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* BitmapNineOpItem::Unmarshalling(Parcel& parcel)
{
    RS_TRACE_NAME("BitmapNineOpItem::Unmarshalling");
    sk_sp<SkImage> bitmapInfo;
    SkIRect center;
    SkRect rectDst;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, bitmapInfo) &&
                   RSMarshallingHelper::Unmarshalling(parcel, center) &&
                   RSMarshallingHelper::Unmarshalling(parcel, rectDst) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
#ifdef NEW_SKIA
    int32_t fliter;
    success = success && RSMarshallingHelper::Unmarshalling(parcel, fliter);
#endif
    if (!success) {
        ROSEN_LOGE("BitmapNineOpItem::Unmarshalling failed!");
        return nullptr;
    }
#ifdef NEW_SKIA
    return new BitmapNineOpItem(bitmapInfo, center, rectDst,
        static_cast<SkFilterMode>(fliter), &paint);
#else
    return new BitmapNineOpItem(bitmapInfo, center, rectDst, &paint);
#endif
}

// AdaptiveRRectOpItem
bool AdaptiveRRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, radius_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("AdaptiveRRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* AdaptiveRRectOpItem::Unmarshalling(Parcel& parcel)
{
    float radius;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, radius) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("AdaptiveRRectOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new AdaptiveRRectOpItem(radius, paint);
}

// AdaptiveRRectScaleOpItem
bool AdaptiveRRectScaleOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, radiusRatio_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("AdaptiveRRectScaleOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* AdaptiveRRectScaleOpItem::Unmarshalling(Parcel& parcel)
{
    float radiusRatio;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, radiusRatio) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("AdaptiveRRectScaleOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new AdaptiveRRectScaleOpItem(radiusRatio, paint);
}

// ClipAdaptiveRRectOpItem
bool ClipAdaptiveRRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, radius_);
    if (!success) {
        ROSEN_LOGE("ClipAdaptiveRRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ClipAdaptiveRRectOpItem::Unmarshalling(Parcel& parcel)
{
    SkVector radius[CORNER_SIZE];
    for (auto i = 0; i < CORNER_SIZE; i++) {
        if (!RSMarshallingHelper::Unmarshalling(parcel, radius[i])) {
            ROSEN_LOGE("ClipAdaptiveRRectOpItem::Unmarshalling failed!");
            return nullptr;
        }
    }
    return new ClipAdaptiveRRectOpItem(radius);
}

// ClipOutsetRectOpItem
bool ClipOutsetRectOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, dx_) &&
                   RSMarshallingHelper::Marshalling(parcel, dy_);
    if (!success) {
        ROSEN_LOGE("ClipOutsetRectOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ClipOutsetRectOpItem::Unmarshalling(Parcel& parcel)
{
    float dx;
    float dy;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, dx) &&
                   RSMarshallingHelper::Unmarshalling(parcel, dy);
    if (!success) {
        ROSEN_LOGE("ClipOutsetRectOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ClipOutsetRectOpItem(dx, dy);
}

// PathOpItem
bool PathOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, path_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("PathOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* PathOpItem::Unmarshalling(Parcel& parcel)
{
    SkPath path;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, path) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("PathOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new PathOpItem(path, paint);
}

// ClipPathOpItem
bool ClipPathOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, path_) &&
                   RSMarshallingHelper::Marshalling(parcel, clipOp_) &&
                   RSMarshallingHelper::Marshalling(parcel, doAA_);
    if (!success) {
        ROSEN_LOGE("ClipPathOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ClipPathOpItem::Unmarshalling(Parcel& parcel)
{
    SkPath path;
    SkClipOp clipOp;
    bool doAA;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, path) &&
                   RSMarshallingHelper::Unmarshalling(parcel, clipOp) &&
                   RSMarshallingHelper::Unmarshalling(parcel, doAA);
    if (!success) {
        ROSEN_LOGE("ClipPathOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ClipPathOpItem(path, clipOp, doAA);
}

// PaintOpItem
bool PaintOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("PaintOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* PaintOpItem::Unmarshalling(Parcel& parcel)
{
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("PaintOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new PaintOpItem(paint);
}

// ConcatOpItem
bool ConcatOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, matrix_);
    if (!success) {
        ROSEN_LOGE("ConcatOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ConcatOpItem::Unmarshalling(Parcel& parcel)
{
#ifdef NEW_SKIA
    SkM44 matrix;
#else
    SkMatrix matrix;
#endif
    bool success = RSMarshallingHelper::Unmarshalling(parcel, matrix);
    if (!success) {
        ROSEN_LOGE("ConcatOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ConcatOpItem(matrix);
}

// SaveLayerOpItem
bool SaveLayerOpItem::Marshalling(Parcel& parcel) const
{
    bool success = parcel.WriteBool(rectPtr_ != nullptr);
    if (rectPtr_) {
        success = success && RSMarshallingHelper::Marshalling(parcel, rect_);
    }
    success = success && RSMarshallingHelper::Marshalling(parcel, backdrop_) &&
               RSMarshallingHelper::Marshalling(parcel, flags_) &&
               RSMarshallingHelper::Marshalling(parcel, paint_);
#ifndef NEW_SKIA
    success = success && RSMarshallingHelper::Marshalling(parcel, mask_) &&
               RSMarshallingHelper::Marshalling(parcel, matrix_);
#endif
    if (!success) {
        ROSEN_LOGE("SaveLayerOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* SaveLayerOpItem::Unmarshalling(Parcel& parcel)
{
    bool isRectExist;
    SkRect rect;
    SkRect* rectPtr = nullptr;
    sk_sp<SkImageFilter> backdrop;
    SkCanvas::SaveLayerFlags flags;
    SkPaint paint;
    bool success = parcel.ReadBool(isRectExist);
    if (isRectExist) {
        success = success && RSMarshallingHelper::Unmarshalling(parcel, rect);
        rectPtr = &rect;
    }
    success = success && RSMarshallingHelper::Unmarshalling(parcel, backdrop) &&
               RSMarshallingHelper::Unmarshalling(parcel, flags) &&
               RSMarshallingHelper::Unmarshalling(parcel, paint);
#ifndef NEW_SKIA
    sk_sp<SkImage> mask;
    SkMatrix matrix;
    success = success && RSMarshallingHelper::Unmarshalling(parcel, mask) &&
               RSMarshallingHelper::Unmarshalling(parcel, matrix);
#endif
    if (!success) {
        ROSEN_LOGE("SaveLayerOpItem::Unmarshalling failed!");
        return nullptr;
    }
#ifdef NEW_SKIA
    SkCanvas::SaveLayerRec rec = { rectPtr, &paint, backdrop.get(), flags };
#else
    SkCanvas::SaveLayerRec rec = { rectPtr, &paint, backdrop.get(), mask.get(), &matrix, flags };
#endif
    return new SaveLayerOpItem(rec);
}

// DrawableOpItem
bool DrawableOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, drawable_) &&
                   RSMarshallingHelper::Marshalling(parcel, matrix_);
    if (!success) {
        ROSEN_LOGE("DrawableOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* DrawableOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkDrawable> drawable;
    SkMatrix matrix;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, drawable) &&
                   RSMarshallingHelper::Unmarshalling(parcel, matrix);
    if (!success) {
        ROSEN_LOGE("DrawableOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new DrawableOpItem(drawable.release(), &matrix);
}

// PictureOpItem
bool PictureOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, picture_) &&
                   RSMarshallingHelper::Marshalling(parcel, matrix_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("PictureOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* PictureOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkPicture> picture;
    SkMatrix matrix;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, picture) &&
                   RSMarshallingHelper::Unmarshalling(parcel, matrix) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("PictureOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new PictureOpItem(picture, &matrix, &paint);
}

// PointsOpItem
bool PointsOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, mode_) &&
                   RSMarshallingHelper::Marshalling(parcel, count_) &&
                   RSMarshallingHelper::MarshallingArray(parcel, processedPoints_, count_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
    if (!success) {
        ROSEN_LOGE("PointsOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* PointsOpItem::Unmarshalling(Parcel& parcel)
{
    SkCanvas::PointMode mode;
    int count;
    const SkPoint* processedPoints = nullptr;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, mode) &&
                   RSMarshallingHelper::Unmarshalling(parcel, count) &&
                   RSMarshallingHelper::UnmarshallingArray(parcel, processedPoints, count) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
    if (!success) {
        ROSEN_LOGE("PointsOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new PointsOpItem(mode, count, processedPoints, paint);
}

// VerticesOpItem
bool VerticesOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, vertices_) &&
                   RSMarshallingHelper::Marshalling(parcel, mode_) &&
                   RSMarshallingHelper::Marshalling(parcel, paint_);
#ifndef NEW_SKIA
    success = success && RSMarshallingHelper::Marshalling(parcel, boneCount_) &&
        RSMarshallingHelper::MarshallingArray(parcel, bones_, boneCount_);
#endif
    if (!success) {
        ROSEN_LOGE("VerticesOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* VerticesOpItem::Unmarshalling(Parcel& parcel)
{
    sk_sp<SkVertices> vertices;
    SkBlendMode mode;
    SkPaint paint;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, vertices) &&
                   RSMarshallingHelper::Unmarshalling(parcel, mode) &&
                   RSMarshallingHelper::Unmarshalling(parcel, paint);
#ifndef NEW_SKIA
    const SkVertices::Bone* bones = nullptr;
    int boneCount;
    success = success && RSMarshallingHelper::Unmarshalling(parcel, boneCount) &&
        RSMarshallingHelper::UnmarshallingArray(parcel, bones, boneCount);
#endif
    if (!success) {
        ROSEN_LOGE("VerticesOpItem::Unmarshalling failed!");
        return nullptr;
    }
#ifdef NEW_SKIA
    return new VerticesOpItem(vertices.get(), mode, paint);
#else
    return new VerticesOpItem(vertices.get(), bones, boneCount, mode, paint);
#endif
}

// ShadowRecOpItem
bool ShadowRecOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, path_) &&
                   RSMarshallingHelper::Marshalling(parcel, rec_);
    if (!success) {
        ROSEN_LOGE("ShadowRecOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* ShadowRecOpItem::Unmarshalling(Parcel& parcel)
{
    SkPath path;
    SkDrawShadowRec rec;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, path) &&
                   RSMarshallingHelper::Unmarshalling(parcel, rec);
    if (!success) {
        ROSEN_LOGE("ShadowRecOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new ShadowRecOpItem(path, rec);
}

// MultiplyAlphaOpItem
bool MultiplyAlphaOpItem::Marshalling(Parcel& parcel) const
{
    bool success = RSMarshallingHelper::Marshalling(parcel, alpha_);
    if (!success) {
        ROSEN_LOGE("MultiplyAlphaOpItem::Marshalling failed!");
        return false;
    }
    return success;
}

OpItem* MultiplyAlphaOpItem::Unmarshalling(Parcel& parcel)
{
    float alpha;
    bool success = RSMarshallingHelper::Unmarshalling(parcel, alpha);
    if (!success) {
        ROSEN_LOGE("MultiplyAlphaOpItem::Unmarshalling failed!");
        return nullptr;
    }
    return new MultiplyAlphaOpItem(alpha);
}

// SaveAlphaOpItem
OpItem* SaveAlphaOpItem::Unmarshalling(Parcel& parcel)
{
    return new SaveAlphaOpItem();
}

// RestoreAlphaOpItem
OpItem* RestoreAlphaOpItem::Unmarshalling(Parcel& parcel)
{
    return new RestoreAlphaOpItem();
}

// SurfaceBufferOpItem
#ifdef ROSEN_OHOS
bool SurfaceBufferOpItem::Marshalling(Parcel& parcel) const
{
    MessageParcel* parcelSurfaceBuffer = static_cast<MessageParcel*>(&parcel);
    WriteSurfaceBufferImpl(
        *parcelSurfaceBuffer, surfaceBufferInfo_.surfaceBuffer_->GetSeqNum(), surfaceBufferInfo_.surfaceBuffer_);
    bool success = RSMarshallingHelper::Marshalling(parcel, surfaceBufferInfo_.offSetX_) &&
                   RSMarshallingHelper::Marshalling(parcel, surfaceBufferInfo_.offSetY_) &&
                   RSMarshallingHelper::Marshalling(parcel, surfaceBufferInfo_.width_) &&
                   RSMarshallingHelper::Marshalling(parcel, surfaceBufferInfo_.height_);
    return success;
}

OpItem* SurfaceBufferOpItem::Unmarshalling(Parcel& parcel)
{
    RSSurfaceBufferInfo surfaceBufferInfo;
    MessageParcel *parcelSurfaceBuffer = static_cast<MessageParcel*>(&parcel);
    uint32_t sequence = 1U;
    ReadSurfaceBufferImpl(*parcelSurfaceBuffer, sequence, surfaceBufferInfo.surfaceBuffer_);
    bool success = RSMarshallingHelper::Unmarshalling(parcel, surfaceBufferInfo.offSetX_) &&
                   RSMarshallingHelper::Unmarshalling(parcel, surfaceBufferInfo.offSetY_) &&
                   RSMarshallingHelper::Unmarshalling(parcel, surfaceBufferInfo.width_) &&
                   RSMarshallingHelper::Unmarshalling(parcel, surfaceBufferInfo.height_);
    if (!success) {
        ROSEN_LOGE("SurfaceBufferOptItem::Unmarshalling failed");
        return nullptr;
    }
    return new SurfaceBufferOpItem(surfaceBufferInfo);
}
#endif
void OpItemWithRSImage::SetNodeId(NodeId id)
{
    if (rsImage_) {
        rsImage_->UpdateNodeIdToPicture(id);
    }
}

void ImageWithParmOpItem::SetNodeId(NodeId id)
{
    if (rsImage_) {
        rsImage_->UpdateNodeIdToPicture(id);
    }
}
} // namespace Rosen
} // namespace OHOS

#endif // USE_ROSEN_DRAWING
