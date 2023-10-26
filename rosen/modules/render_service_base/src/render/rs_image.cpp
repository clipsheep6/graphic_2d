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

#include "render/rs_image.h"

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
#include <include/gpu/GrDirectContext.h>
#else
#include <include/gpu/GrContext.h>
#endif
#include "include/core/SkPaint.h"
#include "include/core/SkRRect.h"
#endif
#include "pipeline/sk_resource_manager.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_image_cache.h"
#include "render/rs_pixel_map_util.h"
#include "rs_trace.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t CORNER_SIZE = 4;
}

RSImage::~RSImage()
{}

bool RSImage::IsEqual(const RSImage& other) const
{
    bool radiusEq = true;
    for (auto i = 0; i < CORNER_SIZE; i++) {
        radiusEq &= (radius_[i] == other.radius_[i]);
    }
    return (image_ == other.image_) && (pixelMap_ == other.pixelMap_) &&
           (imageFit_ == other.imageFit_) && (imageRepeat_ == other.imageRepeat_) &&
           (scale_ == other.scale_) && radiusEq && (compressData_ == other.compressData_);
}
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
void RSImage::CanvasDrawImage(SkCanvas& canvas, const SkRect& rect, const SkSamplingOptions& samplingOptions,
    const SkPaint& paint, bool isBackground)
#else
void RSImage::CanvasDrawImage(SkCanvas& canvas, const SkRect& rect, const SkPaint& paint, bool isBackground)
#endif
{
#ifdef NEW_SKIA
    if (!isDrawn_ || rect != lastRect_) {
#endif
        UpdateNodeIdToPicture(nodeId_);
        SkAutoCanvasRestore acr(&canvas, HasRadius());
        frameRect_.SetAll(rect.left(), rect.top(), rect.width(), rect.height());
#else
void RSImage::CanvasDrawImage(Drawing::Canvas& canvas, const Drawing::Rect& rect, bool isBackground)
{
    canvas.Save();
    frameRect_.SetAll(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
#endif
        if (!isBackground) {
            ApplyImageFit();
            ApplyCanvasClip(canvas);
        }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
        DrawImageRepeatRect(samplingOptions, paint, canvas);
    } else {
        SkAutoCanvasRestore acr(&canvas, HasRadius());
        canvas.drawImageRect(image_, src_, dst_, samplingOptions, &paint, SkCanvas::kFast_SrcRectConstraint);
    }
    lastRect_ = rect;
#else
    DrawImageRepeatRect(paint, canvas);
#endif
#else
    DrawImageRepeatRect(canvas);
    canvas.Restore();
#endif
}

struct ImageParameter
{
    float ratio;
    float srcW;
    float srcH;
    float frameW;
    float frameH;
    float dstW;
    float dstH;
};

RectF ApplyImageFitSwitch(ImageParameter &imageParameter, ImageFit imageFit_, RectF tempRectF)
{
    switch (imageFit_) {
        case ImageFit::TOP_LEFT:
            tempRectF.SetAll(0.f, 0.f, imageParameter.srcW, imageParameter.srcH);
            return tempRectF;
        case ImageFit::FILL:
            break;
        case ImageFit::NONE:
            imageParameter.dstW = imageParameter.srcW;
            imageParameter.dstH = imageParameter.srcH;
            break;
        case ImageFit::COVER:
            imageParameter.dstW = std::max(imageParameter.frameW, imageParameter.frameH * imageParameter.ratio);
            imageParameter.dstH = std::max(imageParameter.frameH, imageParameter.frameW / imageParameter.ratio);
            break;
        case ImageFit::FIT_WIDTH:
            imageParameter.dstH = imageParameter.frameW / imageParameter.ratio;
            break;
        case ImageFit::FIT_HEIGHT:
            imageParameter.dstW = imageParameter.frameH * imageParameter.ratio;
            break;
        case ImageFit::SCALE_DOWN:
            if (imageParameter.srcW < imageParameter.frameW && imageParameter.srcH < imageParameter.frameH) {
                imageParameter.dstW = imageParameter.srcW;
                imageParameter.dstH = imageParameter.srcH;
            } else {
                imageParameter.dstW = std::min(imageParameter.frameW, imageParameter.frameH * imageParameter.ratio);
                imageParameter.dstH = std::min(imageParameter.frameH, imageParameter.frameW / imageParameter.ratio);
            }
            break;
        case ImageFit::CONTAIN:
        default:
            imageParameter.dstW = std::min(imageParameter.frameW, imageParameter.frameH * imageParameter.ratio);
            imageParameter.dstH = std::min(imageParameter.frameH, imageParameter.frameW / imageParameter.ratio);
            break;
    }
    tempRectF.SetAll((imageParameter.frameW - imageParameter.dstW) / 2,
        (imageParameter.frameH - imageParameter.dstH) / 2, imageParameter.dstW, imageParameter.dstH);
    return tempRectF;
}

void RSImage::ApplyImageFit()
{
    if (scale_ == 0) {
        RS_LOGE("RSImage::ApplyImageFit failed, scale_ is zero ");
        return;
    }
    const float srcW = srcRect_.width_ / scale_;
    const float srcH = srcRect_.height_ / scale_;
    const float frameW = frameRect_.width_;
    const float frameH = frameRect_.height_;
    float dstW = frameW;
    float dstH = frameH;
    if (srcH == 0) {
        RS_LOGE("RSImage::ApplyImageFit failed, srcH is zero ");
        return;
    }
    float ratio = srcW / srcH;
    if (ratio == 0) {
        RS_LOGE("RSImage::ApplyImageFit failed, ratio is zero ");
        return;
    }
    ImageParameter imageParameter;
    imageParameter.ratio = ratio;
    imageParameter.srcW = srcW;
    imageParameter.srcH = srcH;
    imageParameter.frameW = frameW;
    imageParameter.frameH = frameH;
    imageParameter.dstW = dstW;
    imageParameter.dstH = dstH;
    RectF tempRectF = dstRect_; 
    dstRect_ = ApplyImageFitSwitch(imageParameter, imageFit_, tempRectF);
}

bool RSImage::HasRadius() const
{
    return hasRadius_;
}

#ifndef USE_ROSEN_DRAWING
void RSImage::ApplyCanvasClip(SkCanvas& canvas)
{
    if (!HasRadius()) {
        return;
    }
    auto rect = (imageRepeat_ == ImageRepeat::NO_REPEAT) ? dstRect_.IntersectRect(frameRect_) : frameRect_;
    SkRRect rrect = SkRRect::MakeEmpty();
    rrect.setRectRadii(RSPropertiesPainter::Rect2SkRect(rect), radius_);
    canvas.clipRRect(rrect, true);
}
#else
void RSImage::ApplyCanvasClip(Drawing::Canvas& canvas)
{
    auto rect = (imageRepeat_ == ImageRepeat::NO_REPEAT) ? dstRect_.IntersectRect(frameRect_) : frameRect_;
    Drawing::RoundRect rrect(RSPropertiesPainter::Rect2DrawingRect(rect), radius_);
    canvas.ClipRoundRect(rrect, Drawing::ClipOp::INTERSECT, true);
}
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
static SkImage::CompressionType PixelFormatToCompressionType(Media::PixelFormat pixelFormat) {
    switch (pixelFormat) {
        case Media::PixelFormat::ASTC_4x4: return SkImage::CompressionType::kASTC_RGBA8_4x4;
        case Media::PixelFormat::ASTC_6x6: return SkImage::CompressionType::kASTC_RGBA8_6x6;
        case Media::PixelFormat::ASTC_8x8: return SkImage::CompressionType::kASTC_RGBA8_8x8;
        case Media::PixelFormat::UNKNOWN:
        default: return SkImage::CompressionType::kNone;
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSImage::UploadGpu(SkCanvas& canvas)
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
    if (compressData_) {
        auto cache = RSImageCache::Instance().GetRenderSkiaImageCacheByPixelMapId(uniqueId_, gettid());
        std::lock_guard<std::mutex> lock(mutex_);
        if (cache) {
            image_ = cache;
        } else {
#ifdef NEW_SKIA
            if (canvas.recordingContext() == nullptr) {
#else
            if (canvas.getGrContext() == nullptr) {
#endif
                return;
            }
            RS_TRACE_NAME("make compress img");
#ifdef NEW_SKIA
            // [planning] new skia remove enum kASTC_CompressionType
            // Need to confirm if kBC1_RGBA8_UNORM and kASTC_CompressionType are the same
            Media::ImageInfo imageInfo;
            pixelMap_->GetImageInfo(imageInfo);
            auto image = SkImage::MakeTextureFromCompressed(GrAsDirectContext(canvas.recordingContext()), compressData_,
                static_cast<int>(srcRect_.width_), static_cast<int>(srcRect_.height_),
                PixelFormatToCompressionType(imageInfo.pixelFormat));
#else
            auto image = SkImage::MakeFromCompressed(canvas.getGrContext(), compressData_,
                static_cast<int>(srcRect_.width_), static_cast<int>(srcRect_.height_), SkImage::kASTC_CompressionType);
#endif
            if (image) {
                image_ = image;
                SKResourceManager::Instance().HoldResource(image);
                RSImageCache::Instance().CacheRenderSkiaImageByPixelMapId(uniqueId_, image, gettid());
            } else {
                RS_LOGE("make astc image %{public}" PRIu64 " (%{public}d, %{public}d) failed",
                    uniqueId_, (int)srcRect_.width_, (int)srcRect_.height_);
            }
            compressData_ = nullptr;
        }
    }
#endif
}
#else
void RSImage::UploadGpu(Drawing::Canvas& canvas)
{
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
    if (compressData_) {
        auto cache = RSImageCache::Instance().GetDrawingImageCache(uniqueId_, gettid());
        std::lock_guard<std::mutex> lock(mutex_);
        if (cache) {
            image_ = cache;
        } else {
            if (canvas.GetGPUContext() == nullptr) {
                return;
            }
            RS_TRACE_NAME("make compress img");
            auto image = std::make_shared<Drawing::Image>();
            image->BuildFromCompressed(*canvas.GetGPUContext(), compressData_, static_cast<int>(srcRect_.width_),
                static_cast<int>(srcRect_.height_), Drawing::CompressedType::ASTC);
            if (image) {
                image_ = image;
                RSImageCache::Instance().CacheDrawingImage(uniqueId_, image, gettid());
            } else {
                RS_LOGE("make astc image %{public}d (%{public}d, %{public}d) failed",
                    uniqueId_, (int)srcRect_.width_, (int)srcRect_.height_);
            }
            compressData_ = nullptr;
        }
    }
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
void RSImage::DrawImageRepeatRect(const SkSamplingOptions& samplingOptions, const SkPaint& paint, SkCanvas& canvas)
#else
void RSImage::DrawImageRepeatRect(const SkPaint& paint, SkCanvas& canvas)
#endif
#else
void RSImage::DrawImageRepeatRect(Drawing::Canvas& canvas)
#endif
{
    int minX = 0;
    int minY = 0;
    int maxX = 0;
    int maxY = 0;
    float left = frameRect_.left_;
    float right = frameRect_.GetRight();
    float top = frameRect_.top_;
    float bottom = frameRect_.GetBottom();
    // calculate REPEAT_XY
    float eps = 0.01; // set epsilon
    if (ImageRepeat::REPEAT_X == imageRepeat_ || ImageRepeat::REPEAT == imageRepeat_) {
        while (dstRect_.left_ + minX * dstRect_.width_ > left + eps) {
            --minX;
        }
        while (dstRect_.left_ + maxX * dstRect_.width_ < right - eps) {
            ++maxX;
        }
    }
    if (ImageRepeat::REPEAT_Y == imageRepeat_ || ImageRepeat::REPEAT == imageRepeat_) {
        while (dstRect_.top_ + minY * dstRect_.height_ > top + eps) {
            --minY;
        }
        while (dstRect_.top_ + maxY * dstRect_.height_ < bottom - eps) {
            ++maxY;
        }
    }

    // draw repeat rect
#ifndef USE_ROSEN_DRAWING
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
#ifndef RS_ENABLE_PARALLEL_UPLOAD
    if (pixelMap_ != nullptr && image_ == nullptr) {
        ConvertPixelMapToSkImage();
    }
#endif
#else
    ConvertPixelMapToSkImage();
#endif
#else
    ConvertPixelMapToDrawingImage();
#endif
    UploadGpu(canvas);
#ifndef USE_ROSEN_DRAWING
    src_ = RSPropertiesPainter::Rect2SkRect(srcRect_);
#else
    if (image_ == nullptr) {
        RS_LOGE("RSImage::DrawImageRepeatRect failed, image is nullptr");
        return;
    }
    auto src = RSPropertiesPainter::Rect2DrawingRect(srcRect_);
#endif
    for (int i = minX; i <= maxX; ++i) {
        for (int j = minY; j <= maxY; ++j) {
#ifndef USE_ROSEN_DRAWING
            dst_ = SkRect::MakeXYWH(dstRect_.left_ + i * dstRect_.width_, dstRect_.top_ + j * dstRect_.height_,
                dstRect_.width_, dstRect_.height_);
#ifdef NEW_SKIA
            canvas.drawImageRect(image_, src_, dst_, samplingOptions, &paint, SkCanvas::kFast_SrcRectConstraint);
#else
            canvas.drawImageRect(image_, src, dst, &paint, SkCanvas::kFast_SrcRectConstraint);
#endif
#else
            auto dst = Drawing::Rect(dstRect_.left_ + i * dstRect_.width_, dstRect_.top_ + j * dstRect_.height_,
                dstRect_.left_ + (i + 1) * dstRect_.width_, dstRect_.top_ + (j + 1) * dstRect_.height_);
            Drawing::SamplingOptions samplingOptions;
            canvas.DrawImageRect(*image_, src, dst, samplingOptions,
                Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
#endif
        }
    }
    if (imageRepeat_ == ImageRepeat::NO_REPEAT) {
        isDrawn_ = true;
    }
}

#ifndef USE_ROSEN_DRAWING
void RSImage::SetCompressData(const sk_sp<SkData> data, const uint32_t id, const int width, const int height)
#else
void RSImage::SetCompressData(
    const std::shared_ptr<Drawing::Data> data, const uint32_t id, const int width, const int height)
#endif
{
#ifdef RS_ENABLE_GL
    compressData_ = data;
    if (compressData_) {
        srcRect_.SetAll(0.0, 0.0, width, height);
#ifndef USE_ROSEN_DRAWING
        GenUniqueId(image_ ? image_->uniqueID() : id);
#else
        GenUniqueId(image_ ? image_->GetUniqueID() : id);
#endif
        image_ = nullptr;
    }
#endif
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
void RSImage::SetCompressData(const sk_sp<SkData> compressData)
{
    isDrawn_ = false;
    compressData_ = compressData;
}
#endif

void RSImage::SetImageFit(int fitNum)
{
    imageFit_ = static_cast<ImageFit>(fitNum);
}

void RSImage::SetImageRepeat(int repeatNum)
{
    imageRepeat_ = static_cast<ImageRepeat>(repeatNum);
}

#ifndef USE_ROSEN_DRAWING
void RSImage::SetRadius(const SkVector radius[])
#else
void RSImage::SetRadius(const std::vector<Drawing::Point>& radius)
#endif
{
    hasRadius_ = false;
    for (auto i = 0; i < CORNER_SIZE; i++) {
        radius_[i] = radius[i];
#ifndef USE_ROSEN_DRAWING
        hasRadius_ = hasRadius_ || !radius_[i].isZero();
#else
        hasRadius_ = hasRadius_ || radius_[i].GetX() != 0 || radius_[i].GetY() != 0;
#endif
    }
}

void RSImage::SetScale(double scale)
{
    if (scale > 0.0) {
        scale_ = scale;
    }
}

void RSImage::SetNodeId(NodeId nodeId)
{
    nodeId_ = nodeId;
}

#ifdef ROSEN_OHOS
static bool UnmarshallingIdAndSize(Parcel& parcel, uint64_t& uniqueId, int& width, int& height)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, uniqueId)) {
        RS_LOGE("RSImage::Unmarshalling uniqueId fail");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, width)) {
        RS_LOGE("RSImage::Unmarshalling width fail");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, height)) {
        RS_LOGE("RSImage::Unmarshalling height fail");
        return false;
    }
    return true;
}

#ifndef USE_ROSEN_DRAWING
static bool UnmarshallingCompressData(Parcel& parcel, bool skipData, sk_sp<SkData>& compressData)
{
    if (skipData) {
        if (!RSMarshallingHelper::SkipSkData(parcel)) {
            RS_LOGE("RSImage::Unmarshalling SkipSkData fail");
            return false;
        }
    } else {
        if (!RSMarshallingHelper::UnmarshallingWithCopy(parcel, compressData)) {
            RS_LOGE("RSImage::Unmarshalling UnmarshallingWithCopy SkData fail");
            return false;
        }
    }
    return true;
}
#else
static bool UnmarshallingCompressData(Parcel& parcel, bool skipData, std::shared_ptr<Drawing::Data>& compressData)
{
    if (skipData) {
        if (!RSMarshallingHelper::SkipData(parcel)) {
            RS_LOGE("RSImage::Unmarshalling SkipData fail");
            return false;
        }
    } else {
        if (!RSMarshallingHelper::UnmarshallingWithCopy(parcel, compressData)) {
            RS_LOGE("RSImage::Unmarshalling UnmarshallingWithCopy Data fail");
            return false;
        }
    }
    return true;
}
#endif

bool RSImage::Marshalling(Parcel& parcel) const
{
    int imageFit = static_cast<int>(imageFit_);
    int imageRepeat = static_cast<int>(imageRepeat_);

    std::lock_guard<std::mutex> lock(mutex_);
    auto image = image_;
#ifndef USE_ROSEN_DRAWING
    if (image && image->isTextureBacked()) {
#else
    if (image && image->IsTextureBacked()) {
#endif
        image = nullptr;
        ROSEN_LOGE("RSImage::Marshalling skip texture image");
    }
    bool success = RSMarshallingHelper::Marshalling(parcel, uniqueId_) &&
                   RSMarshallingHelper::Marshalling(parcel, static_cast<int>(srcRect_.width_)) &&
                   RSMarshallingHelper::Marshalling(parcel, static_cast<int>(srcRect_.height_)) &&
                   RSMarshallingHelper::Marshalling(parcel, nodeId_) &&
                   parcel.WriteBool(pixelMap_ == nullptr) &&
#ifndef USE_ROSEN_DRAWING
                   RSMarshallingHelper::Marshalling(parcel, image) &&
#else
                   RSMarshallingHelper::Marshalling(parcel, image) &&
#endif
                   RSMarshallingHelper::Marshalling(parcel, pixelMap_) &&
                   RSMarshallingHelper::Marshalling(parcel, compressData_) &&
                   RSMarshallingHelper::Marshalling(parcel, imageFit) &&
                   RSMarshallingHelper::Marshalling(parcel, imageRepeat) &&
                   RSMarshallingHelper::Marshalling(parcel, radius_) &&
                   RSMarshallingHelper::Marshalling(parcel, scale_);
    return success;
}

RSImage* RSImage::Unmarshalling(Parcel& parcel)
{
    uint64_t uniqueId;
    int width;
    int height;
    if (!UnmarshallingIdAndSize(parcel, uniqueId, width, height)) {
        RS_LOGE("RSImage::Unmarshalling UnmarshallingIdAndSize fail");
        return nullptr;
    }
    NodeId nodeId;
    if (!RSMarshallingHelper::Unmarshalling(parcel, nodeId)) {
        RS_LOGE("RSImage::Unmarshalling nodeId fail");
        return nullptr;
    }

#ifndef USE_ROSEN_DRAWING
    bool useSkImage;
    sk_sp<SkImage> img;
    std::shared_ptr<Media::PixelMap> pixelMap;
    void* imagepixelAddr = nullptr;
    if (!UnmarshallingSkImageAndPixelMap(parcel, uniqueId, useSkImage, img, pixelMap, imagepixelAddr)) {
        return nullptr;
    }
    sk_sp<SkData> compressData;
    bool skipData = img != nullptr || !useSkImage;
    if (!UnmarshallingCompressData(parcel, skipData, compressData)) {
        return nullptr;
    }
#else
    bool useSkImage;
    std::shared_ptr<Drawing::Image> img;
    std::shared_ptr<Media::PixelMap> pixelMap;
    void* imagepixelAddr = nullptr;
    if (!UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSkImage, img, pixelMap, imagepixelAddr)) {
        return nullptr;
    }
    std::shared_ptr<Drawing::Data> compressData;
    bool skipData = img != nullptr || !useSkImage;
    if (!UnmarshallingCompressData(parcel, skipData, compressData)) {
        return nullptr;
    }
#endif

    int fitNum;
    if (!RSMarshallingHelper::Unmarshalling(parcel, fitNum)) {
        RS_LOGE("RSImage::Unmarshalling fitNum fail");
        return nullptr;
    }

    int repeatNum;
    if (!RSMarshallingHelper::Unmarshalling(parcel, repeatNum)) {
        RS_LOGE("RSImage::Unmarshalling repeatNum fail");
        return nullptr;
    }

#ifndef USE_ROSEN_DRAWING
    SkVector radius[CORNER_SIZE];
    for (auto i = 0; i < CORNER_SIZE; i++) {
        if (!RSMarshallingHelper::Unmarshalling(parcel, radius[i])) {
            RS_LOGE("RSImage::Unmarshalling radius fail");
            return nullptr;
        }
    }
#else
    std::vector<Drawing::Point> radius(CORNER_SIZE);
    if (!RSMarshallingHelper::Unmarshalling(parcel, radius)) {
        RS_LOGE("RSImage::Unmarshalling radius fail");
        return nullptr;
    }
#endif

    double scale;
    if (!RSMarshallingHelper::Unmarshalling(parcel, scale)) {
        RS_LOGE("RSImage::Unmarshalling scale fail");
        return nullptr;
    }

    RSImage* rsImage = new RSImage();
    rsImage->SetImage(img);
    rsImage->SetImagePixelAddr(imagepixelAddr);
    rsImage->SetCompressData(compressData, uniqueId, width, height);
    rsImage->SetPixelMap(pixelMap);
    rsImage->SetImageFit(fitNum);
    rsImage->SetImageRepeat(repeatNum);
    rsImage->SetRadius(radius);
    rsImage->SetScale(scale);
    rsImage->SetNodeId(nodeId);
    rsImage->uniqueId_ = uniqueId;
    rsImage->MarkRenderServiceImage();
    RSImageBase::IncreaseCacheRefCount(uniqueId, useSkImage, pixelMap);
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)  && !defined(USE_ROSEN_DRAWING) && defined(RS_ENABLE_UNI_RENDER)
    if (pixelMap != nullptr && pixelMap->GetAllocatorType() != Media::AllocatorType::DMA_ALLOC) {
        rsImage->ConvertPixelMapToSkImage();
    }
#endif
    return rsImage;
}
#endif
} // namespace Rosen
} // namespace OHOS
