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

#include "property/rs_filter_cache_manager.h"

#ifndef USE_ROSEN_DRAWING
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_skia_filter.h"
#include "src/image/SkImage_Base.h"

#ifdef RS_ENABLE_GL
#include "include/gpu/GrBackendSurface.h"
#endif

namespace OHOS {
namespace Rosen {
inline bool IsLargeArea(int width, int height)
{
    // Use configurable threshold to determine if the area is large, and apply different cache policy.
    // [PLANNING]: dynamically adjust the cache policy (e.g. update interval and cache area expansion) according to the
    // cache size / dirty region percentage / current frame rate / filter radius.
    static auto threshold = RSSystemProperties::GetFilterCacheSizeThreshold();
    return width > threshold && height > threshold;
}

void RSFilterCacheManager::UpdateCacheStateWithFilterHash(uint32_t filterHash)
{
    if (cachedFilteredSnapshot_ == nullptr) {
        return;
    }
    // If we are caching a filtered snapshot, we need to check if the filter hash matches.
    if (filterHash == cachedFilterHash_) {
        return;
    }

    RS_OPTIONAL_TRACE_FUNC_BEGIN();
    ROSEN_LOGD(
        "RSFilterCacheManager::UpdateCacheStateWithFilterHash Cache expired. Reason: Cached filtered snapshot"
        "%{public}X does not match filter hash %{public}X.",
        cachedFilterHash_, filterHash);
    InvalidateCache(CacheType::CACHE_TYPE_FILTERED_SNAPSHOT);
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSFilterCacheManager::UpdateCacheStateWithFilterRegion(const RectI& filterRegion)
{
    if (!IsCacheValid()) {
        return;
    }

    // Test if the filter region is contained in the cached region.
    auto skFilterRegion = SkIRect::MakeLTRB(
        filterRegion.GetLeft(), filterRegion.GetTop(), filterRegion.GetRight(), filterRegion.GetBottom());
    if (snapshotRegion_.contains(skFilterRegion)) {
        snapshotRegion_ = skFilterRegion;
        return;
    }
    RS_OPTIONAL_TRACE_FUNC_BEGIN();
    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterRegion Cache expired. Reason: Filter region is not "
               "within the cached region.");
    InvalidateCache();
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSFilterCacheManager::UpdateCacheStateWithFilterRegion(bool isCachedRegionCannotCoverFilterRegion)
{
    if (isCachedRegionCannotCoverFilterRegion == false || !IsCacheValid()) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC_BEGIN();

    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterRegion Cache expired. Reason: Filter region is not "
               "within the cached region.");
    InvalidateCache();
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSFilterCacheManager::UpdateCacheStateWithDirtyRegion(const RectI& dirtyRegion)
{
    if (!IsCacheValid()) {
        return;
    }

    // Use the dirty region to determine if the cache is valid.
    auto SkDirtyRegion =
        SkIRect::MakeLTRB(dirtyRegion.GetLeft(), dirtyRegion.GetTop(), dirtyRegion.GetRight(), dirtyRegion.GetBottom());
    // The underlying image is not affected by the dirty region, cache is valid.
    if (!SkDirtyRegion.intersect(snapshotRegion_)) {
        return;
    }

    RS_OPTIONAL_TRACE_FUNC_BEGIN();
    // The underlying image is affected by the dirty region, determine if the cache should be invalidated by cache  age.
    // [PLANNING]: also take into account the filter radius / cache size / percentage of intersected area.
    if (cacheUpdateInterval_ > 0) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache"
            "invalidation for %{public}d frames.", cacheUpdateInterval_);
    } else {
        InvalidateCache();
    }
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSFilterCacheManager::UpdateCacheStateWithDirtyRegion(bool isIntersectedWithDirtyRegion)
{
    if (isIntersectedWithDirtyRegion == false || !IsCacheValid()) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC_BEGIN();

    // The underlying image is affected by the dirty region, determine if the cache should be invalidated by cache age.
    // [PLANNING]: also take into account the filter radius / cache size / percentage of intersected area.
    if (cacheUpdateInterval_ > 0) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache "
            "invalidation for %{public}d frames.", cacheUpdateInterval_);
    } else {
        InvalidateCache();
    }
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter,
    const std::optional<SkIRect>& srcRect, const std::optional<SkIRect>& dstRect)
{
    // Filter validation is not needed, since it's already done in RSPropertiesPainter::DrawFilter.
    auto clipIBounds = canvas.getDeviceClipBounds();
    if (clipIBounds.isEmpty()) {
        // clipIBounds is empty, no need to draw filter.
        return;
    }
    RS_OPTIONAL_TRACE_FUNC_BEGIN();

    SkAutoCanvasRestore autoRestore(&canvas, true);
    canvas.resetMatrix();

    ReattachCachedImage(canvas);

    // if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
    //     // The cache is expired, take a snapshot again.
    //     TakeSnapshot(canvas, filter);
    //     ClipVisibleRect(canvas);
    //     DrawCachedSnapshot(canvas, filter);
    //     RS_OPTIONAL_TRACE_FUNC_END();
    //     return;
    // }

    // // Update the cache age, this will ensure that an old cache will be invalidated immediately when intersecting with
    // // dirty region.
    // if (cacheUpdateInterval_ > 0) {
    //     --cacheUpdateInterval_;
    // }
    // if (cacheType_ == CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
    //     // We are caching a filtered snapshot, draw the cached filtered image directly.
    //     ClipVisibleRect(canvas);
    //     DrawCachedFilteredSnapshot(canvas);
    //     RS_OPTIONAL_TRACE_FUNC_END();
    //     return;
    // }

    // // cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT
    // // We are caching a snapshot, check if we should convert it to a filtered snapshot.
    // auto filterHash = filter->Hash();
    // if (filterHash == cachedFilterHash_ && snapshotRegion_ == clipIBounds) {
    //     // Both filter and filterRegion have not changed, increase the counter.
    //     frameSinceLastFilterChange_++;
    // } else {
    //     // Filter or filterRegion changed, reset the counter.
    //     frameSinceLastFilterChange_ = 0;
    //     snapshotRegion_ = clipIBounds;
    //     if (filterHash != cachedFilterHash_) {
    //         filter->PreProcess(cachedImage_);
    //         cachedFilterHash_ = filterHash;
    //     }
    // }
    // // filter has not changed for more than 3 frames, convert the cache image to a filtered image.
    // if (frameSinceLastFilterChange_ >= 3) {
    //     ROSEN_LOGD(
    //         "RSFilterCacheManager::DrawFilter The filter filter and region have not changed in the last 3 frames, "
    //         "generating a filtered image cache.");
    //     GenerateFilteredSnapshot(canvas, filter);
    //     ClipVisibleRect(canvas);
    //     DrawCachedFilteredSnapshot(canvas);
    //     RS_OPTIONAL_TRACE_FUNC_END();
    //     return;
    // }

    // ClipVisibleRect(canvas);
    // DrawCachedSnapshot(canvas, filter);
    // RS_OPTIONAL_TRACE_FUNC_END();
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const std::optional<SkIRect>& srcRect,
        const std::optional<SkIRect>& dstRect)
{
    // // This function is similar to RSFilterCacheManager::DrawFilter, but does not draw anything on the canvas. Instead,
    // // it directly returns the cached image and region. Filter validation is not needed, since it's already done in
    // // RSPropertiesPainter::GenerateCachedEffectData.
    // RS_OPTIONAL_TRACE_FUNC_BEGIN();

    // ReattachCachedImage(canvas);

    // if (cacheType_ == CacheType::CACHE_TYPE_NONE) {
    //     // The cache is expired, so take an image snapshot again and cache it.
    //     ROSEN_LOGD("RSFilterCacheManager::GeneratedCachedEffectData Cache expired, taking snapshot.");
    //     TakeSnapshot(canvas, filter);
    // }

    // // The GeneratedCachedEffectData function generates a filtered image cache, but it does not use any cache policies
    // // like DrawFilter.
    // if (cacheType_ == CacheType::CACHE_TYPE_SNAPSHOT) {
    //     ROSEN_LOGD(
    //         "RSFilterCacheManager::GeneratedCachedEffectData Cache is snapshot, generating filtered image cache.");
    //     snapshotRegion_ = cachedImageRegion_;
    //     GenerateFilteredSnapshot(canvas, filter);
    // }

    // if (cacheType_ != CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
    //     ROSEN_LOGE("RSFilterCacheManager::GeneratedCachedEffectData Cache generation failed.");
    //     RS_OPTIONAL_TRACE_FUNC_END();
    //     return {};
    // }

    // RS_OPTIONAL_TRACE_FUNC_END();
    // return { cachedImage_, cachedImageRegion_ };
    return nullptr;
}

void RSFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& srcRect)
{
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC_BEGIN();

    auto clipIBounds = srcRect;
    // shrink the clipIBounds by 1px to avoid edge artifacts.
    auto snapshotIBounds = clipIBounds.makeOutset(-1, -1);

    // Take a screenshot.
    auto snapshot = skSurface->makeImageSnapshot(snapshotIBounds);
    if (snapshot == nullptr) {
        ROSEN_LOGE("RSFilterCacheManager::TakeSnapshot failed to make an image snapshot.");
        RS_OPTIONAL_TRACE_FUNC_END();
        return;
    }
#ifdef NEW_SKIA
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(snapshot->width(), snapshot->height())) {
        ROSEN_LOGD("TakeSnapshot cache image resource(width:%{public}d, height:%{public}d).",
            snapshot->width(), snapshot->height());
        as_IB(snapshot)->hintCacheGpuResource();
    }
#endif
    filter->PreProcess(snapshot);

    // Update the cache state.
    snapshotRegion_ = clipIBounds;
    cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(snapshot, snapshotIBounds);

    // If the cached image is larger than threshold, we will increase the cache update interval, which is configurable
    // by `hdc shell param set persist.sys.graphic.filterCacheUpdateInterval <interval>`, the default value is 1.
    // Update: we also considered the filter parameters, only enable skip-frame if the blur radius is large enough.
    // Note: the cache will be invalidated immediately if the cached region cannot fully cover the filter region.
    bool isLargeArea = IsLargeArea(clipIBounds.width(), clipIBounds.height());
    cacheUpdateInterval_ =
        isLargeArea && filter->CanSkipFrame() ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0;
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSFilterCacheManager::GenerateFilteredSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSSkiaFilter>& filter, const SkIRect& dstRect)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr || cachedSnapshot_ == nullptr) {
        return;
    }
    // The cache type has been validated, so filterRegion_ and cachedImage_ should be valid. There is no need to check
    // them again.
    RS_OPTIONAL_TRACE_FUNC_BEGIN();

    // Create an offscreen canvas with the same size as the filter region.
    auto offscreenRect = dstRect;
    auto offscreenSurface = surface->makeSurface(offscreenRect.width(), offscreenRect.height());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    // Align the offscreen canvas coordinate system to the filter region.
    // offscreenCanvas.translate(-SkIntToScalar(offscreenRect.x()), -SkIntToScalar(offscreenRect.y()));
    auto src = SkRect::MakeSize(SkSize::Make(cachedSnapshot_->cachedRect_.size()));
    auto dst = SkRect::MakeSize(SkSize::Make(offscreenRect.size()));

    // Draw the cached snapshot onto the offscreen canvas, applying the filter.
    // DrawCachedSnapshot(offscreenCanvas, filter);
    filter->DrawImageRect(offscreenCanvas, cachedSnapshot_->cachedImage_, src, dst);
    filter->PostProcess(canvas);

    // Update the cache state with the filtered snapshot.
    auto filteredSnapshot = offscreenSurface->makeImageSnapshot();
#ifdef NEW_SKIA
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(filteredSnapshot->width(), filteredSnapshot->height())) {
        ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
            filteredSnapshot->width(), filteredSnapshot->height());
        as_IB(filteredSnapshot)->hintCacheGpuResource();
    }
#endif
    cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(filteredSnapshot, offscreenRect);
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSFilterCacheManager::DrawCachedFilteredSnapshot(
    RSPaintFilterCanvas& canvas, const SkIRect& dstRect) const
{
    if (cachedSnapshot_ == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC_BEGIN();

    // The cache type has been validated, so filterRegion_ and cachedImage_ should be valid. There is no need to check
    // them again.
    auto src = SkRect::MakeSize(SkSize::Make(cachedFilteredSnapshot_->cachedRect_.size()));
    auto dst = SkRect::MakeSize(SkSize::Make(dstRect.size()));

    SkPaint paint;
    paint.setAntiAlias(true);
#ifdef NEW_SKIA
    canvas.drawImageRect(
        cachedSnapshot_->cachedImage_, src, dst, SkSamplingOptions(), &paint, SkCanvas::kFast_SrcRectConstraint);
#endif
    RS_OPTIONAL_TRACE_FUNC_END();
}

void RSFilterCacheManager::InvalidateCache(CacheType cacheType)
{
    if (cacheType & CacheType::CACHE_TYPE_SNAPSHOT) {
        cachedSnapshot_.reset();
    }
    if (cacheType & CacheType::CACHE_TYPE_FILTERED_SNAPSHOT) {
        cachedFilteredSnapshot_.reset();
        cachedFilterHash_ = 0;
    }
    cacheUpdateInterval_ = 0;
}

void RSFilterCacheManager::ClipVisibleRect(RSPaintFilterCanvas& canvas) const
{
    auto visibleIRect = canvas.GetVisibleRect().round();
    auto deviceClipRect = canvas.getDeviceClipBounds();
    if (!visibleIRect.isEmpty() && deviceClipRect.intersect(visibleIRect)) {
#ifdef NEW_SKIA
        canvas.clipIRect(visibleIRect);
#endif
    }
}

const SkIRect& RSFilterCacheManager::GetCachedImageRegion() const
{
    static const auto emptyRect = SkIRect::MakeEmpty();
    return IsCacheValid() ? snapshotRegion_ : emptyRect;
}

void RSFilterCacheManager::ReattachCachedImage(RSPaintFilterCanvas& canvas)
{
    if (cachedSnapshot_ && !ReattachCachedImageImpl(canvas, cachedSnapshot_)) {
        InvalidateCache(CacheType::CACHE_TYPE_SNAPSHOT);
    }
    if (cachedFilteredSnapshot_ && !ReattachCachedImageImpl(canvas, cachedFilteredSnapshot_)) {
        InvalidateCache(CacheType::CACHE_TYPE_FILTERED_SNAPSHOT);
    }
}

bool RSFilterCacheManager::ReattachCachedImageImpl(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData>& effectData)
{
#if defined(NEW_SKIA)
    if (effectData == nullptr || effectData->cachedImage_->isValid(canvas.recordingContext())) {
#else
    if (cachedEffectData == nullptr || cachedEffectData->cachedImage_->isValid(canvas.getGrContext())) {
#endif
        return true;
    }
    RS_OPTIONAL_TRACE_FUNC_BEGIN();

#ifdef RS_ENABLE_GL
    auto sharedBackendTexture = effectData->cachedImage_->getBackendTexture(false);
    if (!sharedBackendTexture.isValid()) {
        ROSEN_LOGE("RSFilterCacheManager::ReattachCachedImage failed to get backend texture.");
        RS_OPTIONAL_TRACE_FUNC_END();
        return false;
    }
#if defined(NEW_SKIA)
    auto reattachedCachedImage = SkImage::MakeFromTexture(canvas.recordingContext(), sharedBackendTexture,
#else
    auto reattachedCachedImage = SkImage::MakeFromTexture(canvas.getGrContext(), sharedBackendTexture,
#endif
        kBottomLeft_GrSurfaceOrigin, effectData->cachedImage_->colorType(),
        effectData->cachedImage_->alphaType(), nullptr);
#if defined(NEW_SKIA)
    if (reattachedCachedImage == nullptr || !reattachedCachedImage->isValid(canvas.recordingContext())) {
#else
    if (reattachedCachedImage == nullptr || !reattachedCachedImage->isValid(canvas.getGrContext())) {
#endif
        ROSEN_LOGE("RSFilterCacheManager::ReattachCachedImage failed to create SkImage from backend texture.");
        RS_OPTIONAL_TRACE_FUNC_END();
        return false;
    }
    effectData->cachedImage_ = reattachedCachedImage;
#endif
    RS_OPTIONAL_TRACE_FUNC_END();
    return false;
}
} // namespace Rosen
} // namespace OHOS
#endif
