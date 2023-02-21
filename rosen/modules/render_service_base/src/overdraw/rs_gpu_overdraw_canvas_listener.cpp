/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "overdraw/rs_gpu_overdraw_canvas_listener.h"

#include <include/core/SkDrawable.h>
#include <include/core/SkOverdrawCanvas.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>
#include <include/effects/SkOverdrawColorFilter.h>

#include "overdraw/rs_overdraw_controller.h"

namespace OHOS {
namespace Rosen {
RSGPUOverdrawCanvasListener::RSGPUOverdrawCanvasListener(SkCanvas &canvas)
    : RSCanvasListener(canvas)
{
    listenedSurface_ = canvas.makeSurface(canvas.imageInfo());
    if (listenedSurface_ != nullptr) {
        overdrawCanvas_ = new SkOverdrawCanvas(listenedSurface_->getCanvas());
    }
}

RSGPUOverdrawCanvasListener::~RSGPUOverdrawCanvasListener()
{
    if (overdrawCanvas_ != nullptr) {
        delete overdrawCanvas_;
    }
}

void RSGPUOverdrawCanvasListener::Draw()
{
    auto image = listenedSurface_->makeImageSnapshot();
    SkPaint paint;
    auto overdrawColors = RSOverdrawController::GetInstance().GetColorArray();
#ifdef NEW_SKIA
    paint.setColorFilter(SkOverdrawColorFilter::MakeWithSkColors(overdrawColors.data()));
    canvas_.drawImage(image, 0, 0, SkSamplingOptions(), &paint);
#else
    paint.setColorFilter(SkOverdrawColorFilter::Make(overdrawColors.data()));
    canvas_.drawImage(image, 0, 0, &paint);
#endif
}

bool RSGPUOverdrawCanvasListener::IsValid() const
{
    return listenedSurface_ != nullptr;
}

void RSGPUOverdrawCanvasListener::onDrawRect(const SkRect& rect, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawRect(rect, paint);
}

void RSGPUOverdrawCanvasListener::onDrawRRect(const SkRRect& rect, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawRRect(rect, paint);
}

void RSGPUOverdrawCanvasListener::onDrawDRRect(const SkRRect& outer, const SkRRect& inner,
                                               const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawDRRect(outer, inner, paint);
}

void RSGPUOverdrawCanvasListener::onDrawOval(const SkRect& rect, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawOval(rect, paint);
}

void RSGPUOverdrawCanvasListener::onDrawArc(const SkRect& rect, SkScalar startAngle,
                                            SkScalar sweepAngle, bool useCenter,
                                            const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawArc(rect, startAngle, sweepAngle, useCenter, paint);
}

void RSGPUOverdrawCanvasListener::onDrawPath(const SkPath& path, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawPath(path, paint);
}

void RSGPUOverdrawCanvasListener::onDrawRegion(const SkRegion& region, const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawRegion(region, paint);
}

void RSGPUOverdrawCanvasListener::onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y,
                                                 const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawTextBlob(blob, x, y, paint);
}

void RSGPUOverdrawCanvasListener::onDrawPatch(const SkPoint cubics[12], const SkColor colors[4],
                                              const SkPoint texCoords[4], SkBlendMode mode,
                                              const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawPatch(cubics, colors, texCoords, mode, paint);
}

void RSGPUOverdrawCanvasListener::onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[],
                                               const SkPaint& paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawPoints(mode, count, pts, paint);
}

void RSGPUOverdrawCanvasListener::onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4],
                                                   SkCanvas::QuadAAFlags aaFlags,
                                                   const SkColor4f& color, SkBlendMode mode)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->experimental_DrawEdgeAAQuad(rect, clip, aaFlags, color.toSkColor(), mode);
}

void RSGPUOverdrawCanvasListener::onDrawAnnotation(const SkRect& rect, const char key[], SkData* value)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawAnnotation(rect, key, value);
}

void RSGPUOverdrawCanvasListener::onDrawShadowRec(const SkPath& path, const SkDrawShadowRec& rect)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->private_draw_shadow_rec(path, rect);
}

void RSGPUOverdrawCanvasListener::onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawDrawable(drawable, matrix);
}

void RSGPUOverdrawCanvasListener::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix,
                                                const SkPaint* paint)
{
    if (overdrawCanvas_ == nullptr) {
        ROSEN_LOGE("overdrawCanvas_ is nullptr");
        return;
    }
    overdrawCanvas_->drawPicture(picture, matrix, paint);
}
} // namespace Rosen
} // namespace OHOS
