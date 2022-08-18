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

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"
#include "egl_manager.h"
#include "sk_image_chain.h"
#include "include/gpu/GrContext.h"
#include "tools/gpu/GrContextFactory.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
SKImageChain::SKImageChain(SkCanvas* canvas, sk_sp<SkImage> image)
{
    if (canvas != nullptr && image != nullptr) {
        canvas_ = canvas;
        image_ =  image;
    }
}
SKImageChain::SKImageChain(std::shared_ptr<Media::PixelMap> srcPixelMap) :srcPixelMap_(srcPixelMap)
{
    initWithoutCanvas_ = true;
}

SKImageChain::~SKImageChain()
{
    if (initWithoutCanvas_ && forceCPU_) {
            delete canvas_;
    }
}

void SKImageChain::InitWithoutCanvas()
{
    if (srcPixelMap_ == nullptr) {
        LOGE("The srcPixelMap_ is nullptr!!!");
        return;
    }
    auto rowBytes = srcPixelMap_->GetRowBytes();
    auto width = srcPixelMap_->GetWidth();
    auto height = srcPixelMap_->GetHeight();
    SkImageInfo imageInfo = SkImageInfo::Make(width, height,
    PixelFormatConvert(srcPixelMap_->GetPixelFormat()), static_cast<SkAlphaType>(srcPixelMap_->GetAlphaType()));
    SkPixmap srcPixmap(imageInfo, srcPixelMap_->GetPixels(), rowBytes);
    SkBitmap srcBitmap;
    srcBitmap.installPixels(srcPixmap);
    image_ = SkImage::MakeFromBitmap(srcBitmap);
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.editable = true;
    auto dstPixelMap = Media::PixelMap::Create(opts);
    dstPixmap_ = std::make_shared<SkPixmap>(imageInfo, dstPixelMap->GetPixels(), rowBytes);
    // dstPixmap_ = new SkPixmap(imageInfo, dstPixelMap->GetPixels(), rowBytes);
    dstPixelMap_ = std::shared_ptr<Media::PixelMap>(dstPixelMap.release());
    LOGE("imagechain InitWithoutCanvas start");
#ifdef ACE_ENABLE_GL
    LOGE("imagechain ACE_ENABLE_GL enter");
    if (forceCPU_) {
        // CPU render
        SkBitmap dstBitmap;
        dstBitmap.installPixels(*dstPixmap_.get());
        canvas_ = new SkCanvas(dstBitmap);
    } else {
        // GPU render
        EglManager::GetInstance().Init();
        sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
        sk_sp<GrContext> grContext(GrContext::MakeGL(std::move(glInterface)));
        gpuSurface_ = SkSurface::MakeRenderTarget(grContext.get(), SkBudgeted::kNo, imageInfo);
        if (!gpuSurface_) {
            LOGE("imagechain SkSurface::MakeRenderTarget returned null\n");
            return;
        }
        canvas_ = gpuSurface_->getCanvas();
    }
#else
    LOGE("imagechain ACE_ENABLE_GL no enter");
    // CPU render
    SkBitmap dstBitmap;
    dstBitmap.installPixels(*dstPixmap_.get());
    canvas_ = new SkCanvas(dstBitmap);
#endif
}

void SKImageChain::SetForceCPU(bool forceCPU)
{
    forceCPU_ = forceCPU;
}

void SKImageChain::SetFilters(sk_sp<SkImageFilter> filter)
{
    if (filters_ == nullptr) {
        filters_ = filter;
    } else {
        filters_ = SkImageFilters::Compose(filter, filters_);
    }
}

void SKImageChain::SetClipRect(SkRect* rect)
{
    rect_ = rect;
}

void SKImageChain::SetClipPath(SkPath* path)
{
    path_ = path;
}

void SKImageChain::SetClipRRect(SkRRect* rRect)
{
    rRect_ = rRect;
}

std::shared_ptr<Media::PixelMap> SKImageChain::GetPixelMap()
{
    return dstPixelMap_;
}

void SKImageChain::Draw()
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "SKImageChain::Draw");
    if (initWithoutCanvas_) {
        InitWithoutCanvas();
    }
    if (canvas_ == nullptr) {
        LOGE("imagechain The canvas_ is nullptr!!!");
        return;
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setBlendMode(SkBlendMode::kSrc);
    paint.setImageFilter(filters_);
    if (rect_ != nullptr) {
        canvas_->clipRect((*rect_), true);
    } else if (path_ != nullptr) {
        canvas_->clipPath(*path_, true);
    } else if (rRect_ != nullptr) {
        canvas_->clipRRect(*rRect_, true);
    }
    canvas_->save();
    canvas_->resetMatrix();
    canvas_->drawImage(image_.get(), 0, 0, &paint);
    if (!forceCPU_) {
        if (!canvas_->readPixels(*dstPixmap_.get(), 0, 0)) {
            LOGE("imagechain readPixels Faild");
        }
    }
    canvas_->restore();
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

SkColorType SKImageChain::PixelFormatConvert(const Media::PixelFormat& pixelFormat)
{
    SkColorType colorType;
    switch (pixelFormat) {
        case Media::PixelFormat::BGRA_8888:
            colorType = SkColorType::kBGRA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGBA_8888:
            colorType = SkColorType::kRGBA_8888_SkColorType;
            break;
        case Media::PixelFormat::RGB_565:
            colorType = SkColorType::kRGB_565_SkColorType;
            break;
        case Media::PixelFormat::ALPHA_8:
            colorType = SkColorType::kAlpha_8_SkColorType;
            break;
        default:
            colorType = SkColorType::kUnknown_SkColorType;
            break;
    }
    return colorType;
}
} // namespcae Rosen
} // namespace OHOS