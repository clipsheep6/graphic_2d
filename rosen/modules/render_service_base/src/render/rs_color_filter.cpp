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
#include "render/rs_color_filter.h"

#include "src/core/SkOpts.h"

#include "platform/common/rs_log.h"
#ifdef USE_ROSEN_DRAWING
#include "effect/color_matrix.h"
#endif

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
RSColorFilter::RSColorFilter(sk_sp<SkColorFilter> colorFilter)
    : colorFilter_(colorFilter) {};
#else
RSColorFilter::RSColorFilter(std::shared_ptr<Drawing::ColorFilter> colorFilter)
    : colorFilter_(colorFilter) {};
#endif
RSColorFilter::~RSColorFilter() = default;

sk_sp<SkColorFilter> RSColorFilter::GetSkColorFilter()
{
    return colorFilter_;
}

void RSColorFilter::PreProcess(sk_sp<SkImage> image)
{
    return;
}

void RSColorFilter::PostProcess(RSPaintFilterCanvas& canvas)
{
    return;
}

sk_sp<SkImage> RSColorFilter::ProcessImage(SkCanvas& canvas, const sk_sp<SkImage>& image) const
{
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColorFilter(colorFilter);
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter skSurface is null");
        return;
    }
    auto clipBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter image is null");
        return;
    }
    as_IB(imageSnapshot)->hintCacheGpuResource();
    canvas.resetMatrix();
    SkSamplingOptions options(SkFilterMode::kNearest, SkMipmapMode::kNone);
    canvas.drawImageRect(imageSnapshot, SkRect::Make(clipBounds), options, &paint);
#else
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter surface is null");
        return;
    }
    auto clipBounds = canvas.GetDeviceClipBounds();
    auto imageSnapshot = surface->GetImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter image is null");
        return;
    }
    as_IB(imageSnapshot->ExportSkImage().get())->hintCacheGpuResource();
    canvas.ResetMatrix();
    Drawing::SamplingOptions options(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*imageSnapshot, clipBounds, options);
    canvas.DetachBrush();
#endif
}
} // namespace Rosen
} // namespace OHOS
