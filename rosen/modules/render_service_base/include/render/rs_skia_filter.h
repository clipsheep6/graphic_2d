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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_SKIA_RS_SKIA_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_SKIA_RS_SKIA_FILTER_H

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkPaint.h"
#include "include/effects/SkImageFilters.h"
#else
#include "draw/brush.h"
#include "draw/canvas.h"
#include "effect/image_filter.h"
#endif

#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
#ifndef USE_ROSEN_DRAWING
class RSSkiaFilter : public RSFilter {
public:
    RSSkiaFilter(sk_sp<SkImageFilter> imagefilter);
    ~RSSkiaFilter() override;
    SkPaint GetPaint() const;
    sk_sp<SkImageFilter> GetImageFilter() const;
    virtual std::shared_ptr<RSSkiaFilter> Compose(const std::shared_ptr<RSSkiaFilter>& inner) = 0;
    virtual void PreProcess(sk_sp<SkImage> image) {};
    virtual void PostProcess(RSPaintFilterCanvas& canvas) {};

protected:
    sk_sp<SkImageFilter> imageFilter_ = nullptr;
};
#else
class RSDrawingFilter : public RSFilter {
public:
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imagefilter);
    ~RSDrawingFilter() override;
    Drawing::Brush GetBrush() const;
    std::shared_ptr<Drawing::ImageFilter> GetImageFilter() const;
    virtual std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& inner) = 0;
    virtual void PreProcess(std::shared_ptr<Drawing::Image> image) {};
    virtual void PostProcess(RSPaintFilterCanvas& canvas) {};

protected:
    std::shared_ptr<Drawing::ImageFilter> imageFilter_ = nullptr;
};
#endif
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_SKIA_RS_SKIA_FILTER_H