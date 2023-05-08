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

#ifdef NEW_SKIA
#include <include/gpu/GrDirectContext.h>
#endif
#include "include/core/SkPaint.h"
#include "include/core/SkRRect.h"
#include "platform/common/rs_log.h"
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
#ifdef NEW_SKIA
void RSImage::CanvasDrawImage(SkCanvas& canvas, const SkRect& rect, const SkSamplingOptions& samplingOptions,
    const SkPaint& paint, bool isBackground)
#else
void RSImage::CanvasDrawImage(SkCanvas& canvas, const SkRect& rect, const SkPaint& paint, bool isBackground)
#endif
{
    UpdateNodeIdToPicture(nodeId_);
    canvas.save();
    frameRect_.SetAll(rect.left(), rect.top(), rect.width(), rect.height());
    if (!isBackground) {
        ApplyImageFit();
        ApplyCanvasClip(canvas);
    }
#ifdef NEW_SKIA
    DrawImageRepeatRect(samplingOptions, paint, canvas);
#else
    DrawImageRepeatRect(paint, canvas);
#endif
    canvas.restore();
}

void RSImage::ApplyImageFit()
{
    const float srcW = srcRect_.width_ / scale_;
    const float srcH = srcRect_.height_ / scale_;
    const float frameW = frameRect_.width_;
    const float frameH = frameRect_.height_;
    float dstW = frameW;
    float dstH = frameH;
    float ratio = srcW / srcH;
    switch (imageFit_) {
        case ImageFit::TOP_LEFT:
            dstRect_.SetAll(0.f, 0.f, srcW, srcH);
            return;
        case ImageFit::FILL:
            break;
        case ImageFit::NONE:
            dstW = srcW;
            dstH = srcH;
            break;
        case ImageFit::COVER:
            dstW = std::max(frameW, frameH * ratio);
            dstH = std::max(frameH, frameW / ratio);
            break;
        case ImageFit::FIT_WIDTH:
            dstH = frameW / ratio;
            break;
        case ImageFit::FIT_HEIGHT:
            dstW = frameH * ratio;
            break;
        case ImageFit::SCALE_DOWN:
            if (srcW < frameW && srcH < frameH) {
                dstW = srcW;
                dstH = srcH;
            } else {
                dstW = std::min(frameW, frameH * ratio);
                dstH = std::min(frameH, frameW / ratio);
            }
            break;
        case ImageFit::CONTAIN:
        default:
            dstW = std::min(frameW, frameH * ratio);
            dstH = std::min(frameH, frameW / ratio);
            break;
    }
    dstRect_.SetAll((frameW - dstW) / 2, (frameH - dstH) / 2, dstW, dstH);
}

void RSImage::ApplyCanvasClip(SkCanvas& canvas)
{
    auto rect = (imageRepeat_ == ImageRepeat::NO_REPEAT) ? dstRect_.IntersectRect(frameRect_) : frameRect_;
    SkRRect rrect = SkRRect::MakeEmpty();
    rrect.setRectRadii(RSPropertiesPainter::Rect2SkRect(rect), radius_);
    canvas.clipRRect(rrect, true);
}

void RSImage::UploadGpu(SkCanvas& canvas)
{
#ifdef RS_ENABLE_GL
    if (compressData_) {
        auto cache = RSImageCache::Instance().GetSkiaImageCache(uniqueId_);
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
            auto image = SkImage::MakeTextureFromCompressed(GrAsDirectContext(canvas.recordingContext()), compressData_,
                static_cast<int>(srcRect_.width_), static_cast<int>(srcRect_.height_),
                SkImage::CompressionType::kBC1_RGBA8_UNORM);
#else
            auto image = SkImage::MakeFromCompressed(canvas.getGrContext(), compressData_,
                static_cast<int>(srcRect_.width_), static_cast<int>(srcRect_.height_), SkImage::kASTC_CompressionType);
#endif
            if (image) {
                image_ = image;
                RSImageCache::Instance().CacheSkiaImage(uniqueId_, image);
            } else {
                RS_LOGE("make astc image %d (%d, %d) failed, size:%d", uniqueId_,
                    (int)srcRect_.width_, (int)srcRect_.height_, compressData_->size());
            }
            compressData_ = nullptr;
        }
    }
#endif
}

#ifdef NEW_SKIA
void RSImage::DrawImageRepeatRect(const SkSamplingOptions& samplingOptions, const SkPaint& paint, SkCanvas& canvas)
#else
void RSImage::DrawImageRepeatRect(const SkPaint& paint, SkCanvas& canvas)
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
    ConvertPixelMapToSkImage();
    UploadGpu(canvas);
    auto src = RSPropertiesPainter::Rect2SkRect(srcRect_);
    for (int i = minX; i <= maxX; ++i) {
        for (int j = minY; j <= maxY; ++j) {
            auto dst = SkRect::MakeXYWH(dstRect_.left_ + i * dstRect_.width_, dstRect_.top_ + j * dstRect_.height_,
                dstRect_.width_, dstRect_.height_);
#ifdef NEW_SKIA
            canvas.drawImageRect(image_, src, dst, samplingOptions, &paint, SkCanvas::kFast_SrcRectConstraint);
#else
            canvas.drawImageRect(image_, src, dst, &paint, SkCanvas::kFast_SrcRectConstraint);
#endif
        }
    }
}

void RSImage::SetCompressData(const sk_sp<SkData> data, const uint32_t id, const int width, const int height)
{
#ifdef RS_ENABLE_GL
    compressData_ = data;
    if (compressData_) {
        srcRect_.SetAll(0.0, 0.0, width, height);
        GenUniqueId(image_ ? image_->uniqueID() : id);
        image_ = nullptr;
    }
#endif
}

void RSImage::SetImageFit(int fitNum)
{
    imageFit_ = static_cast<ImageFit>(fitNum);
}

void RSImage::SetImageRepeat(int repeatNum)
{
    imageRepeat_ = static_cast<ImageRepeat>(repeatNum);
}

void RSImage::SetRadius(const SkVector radius[])
{
    for (auto i = 0; i < CORNER_SIZE; i++) {
        radius_[i] = radius[i];
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

bool RSImage::Marshalling(Parcel& parcel) const
{
    int imageFit = static_cast<int>(imageFit_);
    int imageRepeat = static_cast<int>(imageRepeat_);

    std::lock_guard<std::mutex> lock(mutex_);
    auto image = image_;
    if (image && image->isTextureBacked()) {
        image = nullptr;
        ROSEN_LOGE("RSImage::Marshalling skip texture image");
    }
    bool success = RSMarshallingHelper::Marshalling(parcel, uniqueId_) &&
                   RSMarshallingHelper::Marshalling(parcel, static_cast<int>(srcRect_.width_)) &&
                   RSMarshallingHelper::Marshalling(parcel, static_cast<int>(srcRect_.height_)) &&
                   RSMarshallingHelper::Marshalling(parcel, nodeId_) &&
                   parcel.WriteBool(pixelMap_ == nullptr) &&
                   RSMarshallingHelper::Marshalling(parcel, image) &&
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

    bool useSkImage;
    sk_sp<SkImage> img;
    std::shared_ptr<Media::PixelMap> pixelMap;
    void* imagepixelAddr = nullptr;
    if (!UnmarshallingSkImageAndPixelMap(parcel, uniqueId, useSkImage, img, imagepixelAddr, pixelMap)) {
        return nullptr;
    }
    sk_sp<SkData> compressData;
    bool skipData = img != nullptr || !useSkImage;
    if (!UnmarshallingCompressData(parcel, skipData, compressData)) {
        return nullptr;
    }

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

    SkVector radius[CORNER_SIZE];
    for (auto i = 0; i < CORNER_SIZE; i++) {
        if (!RSMarshallingHelper::Unmarshalling(parcel, radius[i])) {
            RS_LOGE("RSImage::Unmarshalling radius fail");
            return nullptr;
        }
    }

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

    RSImageBase::IncreaseCacheRefCount(uniqueId, useSkImage, pixelMap);
    return rsImage;
}
#endif
} // namespace Rosen
} // namespace OHOS
