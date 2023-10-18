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

#include "render/rs_image_cache.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
// modify the RSImageCache instance as global to extend life cycle, fix destructor crash
static RSImageCache gRSImageCacheInstance;

RSImageCache& RSImageCache::Instance()
{
    return gRSImageCacheInstance;
}

#ifndef USE_ROSEN_DRAWING
void RSImageCache::CacheSkiaImage(uint64_t uniqueId, sk_sp<SkImage> img)
{
    if (img && uniqueId > 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        skiaImageCache_.emplace(uniqueId, std::make_pair(img, 0));
    }
}
#else
void RSImageCache::CacheDrawingImage(uint64_t uniqueId, std::shared_ptr<Drawing::Image> img)
{
    if (img && uniqueId > 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        drawingImageCache_.emplace(uniqueId, std::make_pair(img, 0));
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> RSImageCache::GetSkiaImageCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        return it->second.first;
    }
    return nullptr;
}
#else
std::shared_ptr<Drawing::Image> RSImageCache::GetDrawingImageCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = drawingImageCache_.find(uniqueId);
    if (it != drawingImageCache_.end()) {
        return it->second.first;
    }
    return nullptr;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSImageCache::IncreaseSkiaImageCacheRefCount(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        it->second.second++;
    }
}
#else
void RSImageCache::IncreaseDrawingImageCacheRefCount(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = drawingImageCache_.find(uniqueId);
    if (it != drawingImageCache_.end()) {
        it->second.second++;
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSImageCache::ReleaseSkiaImageCache(uint64_t uniqueId)
{
    // release the skImage if no RSImage holds it
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        it->second.second--;
        if (it->second.first == nullptr || it->second.second == 0) {
            skiaImageCache_.erase(it);
        }
    }
}
#else
void RSImageCache::ReleaseDrawingImageCache(uint64_t uniqueId)
{
    // release the Drawing::Image if no RSImage holds it
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = drawingImageCache_.find(uniqueId);
    if (it != drawingImageCache_.end()) {
        it->second.second--;
        if (it->second.first == nullptr || it->second.second == 0) {
            drawingImageCache_.erase(it);
        }
    }
}
#endif

void RSImageCache::CachePixelMap(uint64_t uniqueId, std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (pixelMap && uniqueId > 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        pixelMapCache_.emplace(uniqueId, std::make_pair(pixelMap, 0));
    }
}

std::shared_ptr<Media::PixelMap> RSImageCache::GetPixelMapCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = pixelMapCache_.find(uniqueId);
    if (it != pixelMapCache_.end()) {
        return it->second.first;
    }
    return nullptr;
}

void RSImageCache::IncreasePixelMapCacheRefCount(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = pixelMapCache_.find(uniqueId);
    if (it != pixelMapCache_.end()) {
        it->second.second++;
    }
}

void RSImageCache::ReleasePixelMapCache(uint64_t uniqueId)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    {
        // release the pixelMap if no RSImage holds it
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = pixelMapCache_.find(uniqueId);
        if (it != pixelMapCache_.end()) {
            it->second.second--;
            if (it->second.first == nullptr || it->second.second == 0) {
                pixelMap = it->second.first;
                pixelMapCache_.erase(it);
#ifndef USE_ROSEN_DRAWING
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
                ReleaseMultiSkiaImageCacheByPixelMapId(uniqueId);
#else
                ReleaseSkiaImageCacheByPixelMapId(uniqueId);
#endif
#else
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
                ReleaseMultiDrawingImageCacheByPixelMapId(uniqueId);
#else
                ReleaseDrawingImageCacheByPixelMapId(uniqueId);
#endif
#endif
            }
        }
#ifndef USE_ROSEN_DRAWING
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
        auto itr = pixelMapIdRelatedMultiSkiaImageCache_.find(uniqueId);
        if (itr != pixelMapIdRelatedMultiSkiaImageCache_.end()) {
            pixelMapIdRelatedMultiSkiaImageCache_.erase(itr);
        }
#else
        auto itr = pixelMapIdRelatedSkiaImageCache_.find(uniqueId);
        if (itr != pixelMapIdRelatedSkiaImageCache_.end()) {
            pixelMapIdRelatedSkiaImageCache_.erase(itr);
        }
#endif
#else
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
        auto itr = pixelMapIdRelatedMultiDrawingImageCache_.find(uniqueId);
        if (itr != pixelMapIdRelatedMultiDrawingImageCache_.end()) {
            pixelMapIdRelatedMultiDrawingImageCache_.erase(itr);
        }
#else
        auto itr = pixelMapIdRelatedDrawingImageCache_.find(uniqueId);
        if (itr != pixelMapIdRelatedDrawingImageCache_.end()) {
            pixelMapIdRelatedDrawingImageCache_.erase(itr);
        }
#endif
#endif
    }
}

#ifndef USE_ROSEN_DRAWING
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
void RSImageCache::CacheRenderMultiSkiaImageByPixelMapId(uint64_t uniqueId, pid_t tid, sk_sp<SkImage> img)
{
    if (uniqueId > 0 && img) {
        std::lock_guard<std::mutex> lock(mapMutex_);
        pixelMapIdRelatedMultiSkiaImageCache_[uniqueId][tid] = img;
    }
}
#else
void RSImageCache::CacheRenderSkiaImageByPixelMapId(uint64_t uniqueId, sk_sp<SkImage> img)
{
    if (uniqueId > 0 && img) {
        std::lock_guard<std::mutex> lock(mapMutex_);
        pixelMapIdRelatedSkiaImageCache_.emplace(std::make_pair(uniqueId, img));
    }
}
#endif
#else
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
void RSImageCache::CacheRenderMultiDrawingImageByPixelMapId(uint64_t uniqueId, pid_t tid, std::shared_ptr<Drawing::Image> img)
{
    if (uniqueId > 0 && img) {
        std::lock_guard<std::mutex> lock(mapMutex_);
        pixelMapIdRelatedMultiDrawingImageCache_[uniqueId][tid] = img;
    }
}
#else
void RSImageCache::CacheRenderDrawingImageByPixelMapId(uint64_t uniqueId, std::shared_ptr<Drawing::Image> img)
{
    if (uniqueId > 0 && img) {
        std::lock_guard<std::mutex> lock(mapMutex_);
        pixelMapIdRelatedDrawingImageCache_.emplace(std::make_pair(uniqueId, img));
    }
}
#endif
#endif

#ifndef USE_ROSEN_DRAWING
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
sk_sp<SkImage> RSImageCache::GetRenderMultiSkiaImageCacheByPixelMapId(uint64_t uniqueId, pid_t tid) const
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedMultiSkiaImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedMultiSkiaImageCache_.end()) {
        auto innerIt = it->second.find(tid);
        if (innerIt != it->second.end()) {
            return innerIt->second;
        }
    }
    return nullptr;
}
#else
sk_sp<SkImage> RSImageCache::GetRenderSkiaImageCacheByPixelMapId(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedSkiaImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedSkiaImageCache_.end()) {
        return it->second;
    }
    return nullptr;
}
#endif
#else
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
std::shared_ptr<Drawing::Image> RSImageCache::GetRenderMultiDrawingImageCacheByPixelMapId(uint64_t uniqueId, pid_t tid) const
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedDrawingImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedDrawingImageCache_.end()) {
        auto innerIt = it->second.find(tid);
        if (innerIt != it->second.end()) {
            return innerIt->second;
        }
    }
    return nullptr;
}
#else
std::shared_ptr<Drawing::Image> RSImageCache::GetRenderDrawingImageCacheByPixelMapId(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedDrawingImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedDrawingImageCache_.end()) {
        return it->second;
    }
    return nullptr;
}
#endif
#endif

#ifndef USE_ROSEN_DRAWING
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
void RSImageCache::ReleaseMultiSkiaImageCacheByPixelMapId(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedMultiSkiaImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedMultiSkiaImageCache_.end()) {
        pixelMapIdRelatedMultiSkiaImageCache_.erase(it);
    }
}
#else
void RSImageCache::ReleaseSkiaImageCacheByPixelMapId(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedSkiaImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedSkiaImageCache_.end()) {
        pixelMapIdRelatedSkiaImageCache_.erase(it);
    }
}
#endif
#else
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
void RSImageCache::ReleaseMultiDrawingImageCacheByPixelMapId(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedMultiDrawingImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedMultiDrawingImageCache_.end()) {
        pixelMapIdRelatedMultiDrawingImageCache_.erase(it);
    }
}
#else
void RSImageCache::ReleaseDrawingImageCacheByPixelMapId(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = pixelMapIdRelatedDrawingImageCache_.find(uniqueId);
    if (it != pixelMapIdRelatedDrawingImageCache_.end()) {
        pixelMapIdRelatedDrawingImageCache_.erase(it);
    }
}
#endif
#endif
} // namespace Rosen
} // namespace OHOS
