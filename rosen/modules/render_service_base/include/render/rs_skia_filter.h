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

class RSShaderFilter {
public:
    RSShaderFilter() = default;
    RSShaderFilter(const RSShaderFilter&) = delete;
    virtual ~RSShaderFilter() = default;

    virtual void PreProcess(sk_sp<SkImage> image) {};
    virtual void PostProcess(RSPaintFilterCanvas& canvas) {};
    virtual sk_sp<SkImage> ProcessImage(SkCanvas& canvas, const sk_sp<SkImage>& image) const
    {
        return image;
    };
};

class RSSkiaFilter : public RSFilter {
public:
    RSSkiaFilter(sk_sp<SkImageFilter> imagefilter);
    RSSkiaFilter(std::shared_ptr<RSShaderFilter> shaderFilter);
    RSSkiaFilter(std::vector<std::shared_ptr<RSShaderFilter>>&& shaderFilters, sk_sp<SkImageFilter> imagefilter);
    RSSkiaFilter(const RSSkiaFilter&) = delete;
    ~RSSkiaFilter() override;
    virtual bool CanSkipFrame() const { return false; };

    std::shared_ptr<RSSkiaFilter> Compose(const std::shared_ptr<RSSkiaFilter>& other) const
    {
        auto shaderFilters = shaderFilters_;
        shaderFilters.insert(shaderFilters.end(), other->shaderFilters_.begin(), other->shaderFilters_.end());
        auto imageFilter = SkImageFilters::Compose(imageFilter_, other->imageFilter_);
        return std::make_shared<RSSkiaFilter>(std::move(shaderFilters), imageFilter);
    }

    // 画图上屏
    void DrawImageRect(
        SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const
    {
        // 级联RSShaderFilter，不需要关心src和dst，对整个图做处理就好
        std::for_each(shaderFilters_.begin(), shaderFilters_.end(),
            [&](auto& filter) { image = filter->ProcessImage(canvas, image); });
        SkPaint paint;
        paint.setImageFilter(imageFilter_);
        canvas.drawImageRect(image, src, dst, {}, &paint, SkCanvas::kStrict_SrcRectConstraint);
    }

    // 如取色等操作
    void PreProcess(sk_sp<SkImage> image) {
        std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
            image = filter->Preprocess(image);
        });
    };

    // 如绘制mask color等操作
    void PostProcess(RSPaintFilterCanvas& canvas) {
        std::for_each(shaderFilters_.begin(), shaderFilters_.end(), [&](auto& filter) {
            filter->PostProcess(canvas);
        });
    };

protected:
    sk_sp<SkImageFilter> imageFilter_ = nullptr;
    std::vector<std::shared_ptr<RSShaderFilter>> shaderFilters_;
};
#else
class RSDrawingFilter : public RSFilter {
public:
    RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imagefilter);
    ~RSDrawingFilter() override;
    Drawing::Brush GetBrush() const;
    virtual void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const;
    std::shared_ptr<Drawing::ImageFilter> GetImageFilter() const;
    virtual std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const = 0;
    virtual void PreProcess(std::shared_ptr<Drawing::Image> image) {};
    virtual void PostProcess(RSPaintFilterCanvas& canvas) {};
    virtual bool CanSkipFrame() const { return false; };

protected:
    std::shared_ptr<Drawing::ImageFilter> imageFilter_ = nullptr;
};
#endif
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_SKIA_RS_SKIA_FILTER_H