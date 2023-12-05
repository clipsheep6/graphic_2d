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

#include "render/rs_image_base.h"

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkImage.h"
#include "src/core/SkImagePriv.h"
#else
#include "image/image.h"
#endif
#include "common/rs_background_thread.h"
#ifdef RS_ENABLE_PARALLEL_UPLOAD
#include "common/rs_upload_texture_thread.h"
#endif
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/sk_resource_manager.h"
#include "property/rs_properties_painter.h"
#include "render/rs_image_cache.h"
#include "render/rs_pixel_map_util.h"
#include "memory/rs_memory_track.h"
#include "rs_trace.h"
#include "sandbox_utils.h"

namespace OHOS::Rosen {
RSImageBase::~RSImageBase()
{
    if (pixelMap_) {
        pixelMap_ = nullptr;
        if (uniqueId_ > 0) {
            if (renderServiceImage_) {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
                if (!RSSystemProperties::GetRsVulkanEnabled() && isPinImage_) {
                    RSUploadTextureThread::Instance().RemoveTask(std::to_string(uniqueId_));
                    auto unpinTask = [image = image_]() {
                        auto grContext = RSUploadTextureThread::Instance().GetShareGrContext().get();
                        if (grContext && image) {
                            SkImage_unpinAsTexture(image.get(), grContext);
                        }
                    };
                    RSUploadTextureThread::Instance().PostSyncTask(unpinTask);
                }
#endif
#endif
                auto task = [uniqueId = uniqueId_]() {
                    RSImageCache::Instance().ReleasePixelMapCache(uniqueId);
                };
                RSBackgroundThread::Instance().PostTask(task);
            } else {
                RSImageCache::Instance().ReleasePixelMapCache(uniqueId_);
            }
        }
    } else { // if pixelMap_ not nullptr, do not release skImage cache
        if (image_) {
            image_ = nullptr;
            if (uniqueId_ > 0) {
                // if image_ is obtained by RSPixelMapUtil::ExtractSkImage, uniqueId_ here is related to pixelMap,
                // image_ is not in SkiaImageCache, but still check it here
                // in this case, the cached image_ will be removed when pixelMap cache is removed
#ifndef USE_ROSEN_DRAWING
                RSImageCache::Instance().ReleaseSkiaImageCache(uniqueId_);
#else
                RSImageCache::Instance().ReleaseDrawingImageCache(uniqueId_);
#endif
            }
        }
    }
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
void RSImageBase::DrawImage(RSPaintFilterCanvas& canvas, const SkSamplingOptions& samplingOptions, const SkPaint& paint)
#else
void RSImageBase::DrawImage(RSPaintFilterCanvas& canvas, const SkPaint& paint)
#endif
{
    ConvertPixelMapToSkImage();
    auto src = RSPropertiesPainter::Rect2SkRect(srcRect_);
    auto dst = RSPropertiesPainter::Rect2SkRect(dstRect_);
    if (image_ == nullptr) {
        RS_LOGE("RSImageBase::DrawImage image_ is nullptr");
        return;
    }
#ifdef NEW_SKIA
    canvas.drawImageRect(image_, src, dst, samplingOptions, &paint, SkCanvas::kStrict_SrcRectConstraint);
#else
    canvas.drawImageRect(image_, src, dst, &paint);
#endif
}
#else
void RSImageBase::DrawImage(Drawing::Canvas& canvas, const Drawing::SamplingOptions& samplingOptions)
{
    ConvertPixelMapToDrawingImage();
    auto src = RSPropertiesPainter::Rect2DrawingRect(srcRect_);
    auto dst = RSPropertiesPainter::Rect2DrawingRect(dstRect_);
    if (image_ == nullptr) {
        RS_LOGE("RSImageBase::DrawImage image_ is nullptr");
        return;
    }
    canvas.DrawImageRect(*image_, src, dst, samplingOptions);
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSImageBase::SetImage(const sk_sp<SkImage> image)
#else
void RSImageBase::SetImage(const std::shared_ptr<Drawing::Image> image)
#endif
{
    isDrawn_ = false;
    image_ = image;
    if (image_) {
        SKResourceManager::Instance().HoldResource(image);
#ifndef USE_ROSEN_DRAWING
        srcRect_.SetAll(0.0, 0.0, image_->width(), image_->height());
        GenUniqueId(image_->uniqueID());
#else
        srcRect_.SetAll(0.0, 0.0, image_->GetWidth(), image_->GetHeight());
        GenUniqueId(image_->GetUniqueID());
#endif
    }
}

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifndef USE_ROSEN_DRAWING
void RSImageBase::SetDmaImage(const sk_sp<SkImage> image)
#else
void RSImageBase::SetDmaImage(const std::shared_ptr<Drawing::Image> image)
#endif
{
    isDrawn_ = false;
    image_ = image;
    SKResourceManager::Instance().HoldResource(image);
}
#endif

void RSImageBase::SetPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap)
{
    pixelMap_ = pixelmap;
    if (pixelMap_) {
        srcRect_.SetAll(0.0, 0.0, pixelMap_->GetWidth(), pixelMap_->GetHeight());
        image_ = nullptr;
        GenUniqueId(pixelMap_->GetUniqueId());
    }
}

void RSImageBase::DumpPicture(DfxString& info) const
{
    if (!pixelMap_) {
        return;
    }
    info.AppendFormat("%d    [%d * %d]  %p\n", pixelMap_->GetByteCount(), pixelMap_->GetWidth(), pixelMap_->GetHeight(),
        pixelMap_.get());
}

void RSImageBase::SetSrcRect(const RectF& srcRect)
{
    srcRect_ = srcRect;
}

void RSImageBase::SetDstRect(const RectF& dstRect)
{
    if (dstRect_ != dstRect) {
        isDrawn_ = false;
    }
    dstRect_ = dstRect;
}

void RSImageBase::SetImagePixelAddr(void* addr)
{
    imagePixelAddr_ = addr;
}

void RSImageBase::UpdateNodeIdToPicture(NodeId nodeId)
{
    if (!nodeId) {
        return;
    }
    if (pixelMap_) {
        MemoryTrack::Instance().UpdatePictureInfo(pixelMap_->GetPixels(), nodeId, ExtractPid(nodeId));
    }
    if (image_ || imagePixelAddr_) {
        MemoryTrack::Instance().UpdatePictureInfo(imagePixelAddr_, nodeId, ExtractPid(nodeId));
    }
}

void RSImageBase::MarkRenderServiceImage()
{
    renderServiceImage_ = true;
}

#ifdef ROSEN_OHOS
#ifndef USE_ROSEN_DRAWING
static bool UnmarshallingAndCacheSkImage(Parcel& parcel, sk_sp<SkImage>& img, uint64_t uniqueId, void*& imagepixelAddr)
{
    if (img != nullptr) {
        // match a cached SkImage
        if (!RSMarshallingHelper::SkipSkImage(parcel)) {
            RS_LOGE("UnmarshalAndCacheSkImage SkipSkImage fail");
            return false;
        }
    } else if (RSMarshallingHelper::Unmarshalling(parcel, img, imagepixelAddr)) {
        // unmarshalling the SkImage and cache it
        RSImageCache::Instance().CacheSkiaImage(uniqueId, img);
    } else {
        RS_LOGE("UnmarshalAndCacheSkImage fail");
        return false;
    }
    return true;
}
#else
static bool UnmarshallingAndCacheDrawingImage(
    Parcel& parcel, std::shared_ptr<Drawing::Image>& img, uint64_t uniqueId, void*& imagepixelAddr)
{
    if (img != nullptr) {
        // match a cached SkImage
        if (!RSMarshallingHelper::SkipImage(parcel)) {
            RS_LOGE("UnmarshalAndCacheSkImage SkipSkImage fail");
            return false;
        }
    } else if (RSMarshallingHelper::Unmarshalling(parcel, img, imagepixelAddr)) {
        // unmarshalling the SkImage and cache it
        RSImageCache::Instance().CacheDrawingImage(uniqueId, img);
    } else {
        RS_LOGE("UnmarshalAndCacheSkImage fail");
        return false;
    }
    return true;
}
#endif


static bool UnmarshallingAndCachePixelMap(Parcel& parcel, std::shared_ptr<Media::PixelMap>& pixelMap, uint64_t uniqueId)
{
    if (pixelMap != nullptr) {
        // match a cached pixelMap
        if (!RSMarshallingHelper::SkipPixelMap(parcel)) {
            return false;
        }
    } else if (RSMarshallingHelper::Unmarshalling(parcel, pixelMap)) {
        if (pixelMap && !pixelMap->IsEditable()) {
            // unmarshalling the pixelMap and cache it
            RSImageCache::Instance().CachePixelMap(uniqueId, pixelMap);
        }
    } else {
        return false;
    }
    return true;
}

static bool UnmarshallingIdAndRect(Parcel& parcel, uint64_t& uniqueId, RectF& srcRect, RectF& dstRect)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, uniqueId)) {
        RS_LOGE("RSImage::Unmarshalling uniqueId fail");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, srcRect)) {
        RS_LOGE("RSImage::Unmarshalling srcRect fail");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, dstRect)) {
        RS_LOGE("RSImage::Unmarshalling dstRect fail");
        return false;
    }
    return true;
}

#ifndef USE_ROSEN_DRAWING
bool RSImageBase::UnmarshallingSkImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useSkImage,
    sk_sp<SkImage>& img, std::shared_ptr<Media::PixelMap>& pixelMap, void*& imagepixelAddr)
#else
bool RSImageBase::UnmarshallingDrawingImageAndPixelMap(Parcel& parcel, uint64_t uniqueId, bool& useSkImage,
    std::shared_ptr<Drawing::Image>& img, std::shared_ptr<Media::PixelMap>& pixelMap, void*& imagepixelAddr)
#endif
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, useSkImage)) {
        return false;
    }
    if (useSkImage) {
#ifndef USE_ROSEN_DRAWING
        img = RSImageCache::Instance().GetSkiaImageCache(uniqueId);
        RS_TRACE_NAME_FMT("RSImageBase::Unmarshalling skImage uniqueId:%lu, size:[%d %d], cached:%d",
            uniqueId, img ? img->width() : 0, img ? img->height() : 0, img != nullptr);
        if (!UnmarshallingAndCacheSkImage(parcel, img, uniqueId, imagepixelAddr)) {
#else
        img = RSImageCache::Instance().GetDrawingImageCache(uniqueId);
        RS_TRACE_NAME_FMT("RSImageBase::Unmarshalling Image uniqueId:%lu, size:[%d %d], cached:%d",
            uniqueId, img ? img->GetWidth() : 0, img ? img->GetHeight() : 0, img != nullptr);
        if (!UnmarshallingAndCacheDrawingImage(parcel, img, uniqueId, imagepixelAddr)) {
#endif
            RS_LOGE("RSImageBase::Unmarshalling UnmarshalAndCacheSkImage fail");
            return false;
        }
        RSMarshallingHelper::SkipPixelMap(parcel);
    } else {
#ifndef USE_ROSEN_DRAWING
        if (!RSMarshallingHelper::SkipSkImage(parcel)) {
#else
        if (!RSMarshallingHelper::SkipImage(parcel)) {
#endif
            return false;
        }
        pixelMap = RSImageCache::Instance().GetPixelMapCache(uniqueId);
        RS_TRACE_NAME_FMT("RSImageBase::Unmarshalling pixelMap uniqueId:%lu, size:[%d %d], cached:%d",
            uniqueId, pixelMap ? pixelMap->GetWidth() : 0, pixelMap ? pixelMap->GetHeight() : 0, pixelMap != nullptr);
        if (!UnmarshallingAndCachePixelMap(parcel, pixelMap, uniqueId)) {
            RS_LOGE("RSImageBase::Unmarshalling UnmarshalAndCachePixelMap fail");
            return false;
        }
    }
    return true;
}

void RSImageBase::IncreaseCacheRefCount(uint64_t uniqueId, bool useSkImage, std::shared_ptr<Media::PixelMap>
    pixelMap)
{
    if (useSkImage) {
#ifndef USE_ROSEN_DRAWING
        RSImageCache::Instance().IncreaseSkiaImageCacheRefCount(uniqueId);
#else
        RSImageCache::Instance().IncreaseDrawingImageCacheRefCount(uniqueId);
#endif
    } else if (pixelMap && !pixelMap->IsEditable()) {
        RSImageCache::Instance().IncreasePixelMapCacheRefCount(uniqueId);
    }
}

bool RSImageBase::Marshalling(Parcel& parcel) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = RSMarshallingHelper::Marshalling(parcel, uniqueId_) &&
                   RSMarshallingHelper::Marshalling(parcel, srcRect_) &&
                   RSMarshallingHelper::Marshalling(parcel, dstRect_) &&
                   parcel.WriteBool(pixelMap_ == nullptr) &&
                   RSMarshallingHelper::Marshalling(parcel, image_) &&
                   RSMarshallingHelper::Marshalling(parcel, pixelMap_);
    return success;
}

RSImageBase* RSImageBase::Unmarshalling(Parcel& parcel)
{
    uint64_t uniqueId;
    RectF srcRect;
    RectF dstRect;
    if (!UnmarshallingIdAndRect(parcel, uniqueId, srcRect, dstRect)) {
        RS_LOGE("RSImage::Unmarshalling UnmarshalIdAndSize fail");
        return nullptr;
    }

    bool useSkImage;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> img;
    std::shared_ptr<Media::PixelMap> pixelMap;
    void* imagepixelAddr = nullptr;
    if (!UnmarshallingSkImageAndPixelMap(parcel, uniqueId, useSkImage, img, pixelMap, imagepixelAddr)) {
        return nullptr;
    }
#else
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    std::shared_ptr<Media::PixelMap> pixelMap;
    void* imagepixelAddr = nullptr;
    if (!UnmarshallingDrawingImageAndPixelMap(parcel, uniqueId, useSkImage, img, pixelMap, imagepixelAddr)) {
        return nullptr;
    }
#endif

    RSImageBase* rsImage = new RSImageBase();
    rsImage->SetImage(img);
    rsImage->SetImagePixelAddr(imagepixelAddr);
    rsImage->SetPixelMap(pixelMap);
    rsImage->SetSrcRect(srcRect);
    rsImage->SetDstRect(dstRect);
    rsImage->uniqueId_ = uniqueId;
    rsImage->MarkRenderServiceImage();
    IncreaseCacheRefCount(uniqueId, useSkImage, pixelMap);
    return rsImage;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSImageBase::ConvertPixelMapToSkImage()
{
    if (!image_ && pixelMap_) {
        if (!pixelMap_->IsEditable()) {
#if defined(ROSEN_OHOS)
            image_ = RSImageCache::Instance().GetRenderSkiaImageCacheByPixelMapId(uniqueId_, gettid());
#else
            image_ = RSImageCache::Instance().GetRenderSkiaImageCacheByPixelMapId(uniqueId_);
#endif
        }
        if (!image_) {
            image_ = RSPixelMapUtil::ExtractSkImage(pixelMap_);
            if (image_) {
                SKResourceManager::Instance().HoldResource(image_);
            }
            if (!pixelMap_->IsEditable()) {
#if defined(ROSEN_OHOS)
                RSImageCache::Instance().CacheRenderSkiaImageByPixelMapId(uniqueId_, image_, gettid());
#else
                RSImageCache::Instance().CacheRenderSkiaImageByPixelMapId(uniqueId_, image_);
#endif
            }
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL) && defined(RS_ENABLE_PARALLEL_UPLOAD)
#if !defined(USE_ROSEN_DRAWING) && defined(NEW_SKIA) && defined(RS_ENABLE_UNI_RENDER)
            if (!RSSystemProperties::GetRsVulkanEnabled() && renderServiceImage_) {
                auto image = image_;
                auto pixelMap = pixelMap_;
                std::function<void()> uploadTexturetask = [image, pixelMap]() -> void {
                    auto grContext = RSUploadTextureThread::Instance().GetShareGrContext().get();
                    if (grContext && image && pixelMap) {
                        SkImage_pinAsTexture(image.get(), grContext);
                    }
                };
                RSUploadTextureThread::Instance().PostTask(uploadTexturetask, std::to_string(uniqueId_));
                isPinImage_ = true;
            }
#endif
#endif
        }
    }
}
#else
void RSImageBase::ConvertPixelMapToDrawingImage()
{
    if (!image_ && pixelMap_) {
        if (!pixelMap_->IsEditable()) {
#if defined(ROSEN_OHOS)
            image_ = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(uniqueId_, gettid());
#else
            image_ = RSImageCache::Instance().GetRenderDrawingImageCacheByPixelMapId(uniqueId_);
#endif
        }
        if (!image_) {
            image_ = RSPixelMapUtil::ExtractDrawingImage(pixelMap_);
            if (image_) {
                SKResourceManager::Instance().HoldResource(image_);
            }
            if (!pixelMap_->IsEditable()) {
#if defined(ROSEN_OHOS)
                RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(uniqueId_, image_, gettid());
#else
                RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(uniqueId_, image_);
#endif
            }
        }
    }
}
#endif

void RSImageBase::GenUniqueId(uint32_t id)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsignd number shift
    uniqueId_ = shiftedPid | id;
}

std::shared_ptr<Media::PixelMap> RSImageBase::GetPixelMap() const
{
    return pixelMap_;
}
}