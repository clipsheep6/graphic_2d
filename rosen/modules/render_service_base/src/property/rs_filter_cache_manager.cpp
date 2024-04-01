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
#include "rs_trace.h"
#include "render/rs_filter.h"

#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "include/gpu/GrBackendSurface.h"
#include "src/image/SkImage_Base.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
const char* RSFilterCacheManager::GetCacheState() const
{
    if (cachedFilteredSnapshot_ != nullptr) {
        return "Filtered image found in cache. Reusing cached result.";
    } else if (cachedSnapshot_ != nullptr) {
        return "Snapshot found in cache. Generating filtered image using cached data.";
    } else {
        return "No valid cache found.";
    }
}

inline static bool isEqualRect(const Drawing::RectI& src, const RectI& dst)
{
    return src.GetLeft() == dst.GetLeft() && src.GetTop() == dst.GetTop() && src.GetWidth() == dst.GetWidth() &&
           src.GetHeight() == dst.GetHeight();
}

void RSFilterCacheManager::UpdateCacheStateWithFilterHash(const std::shared_ptr<RSFilter>& filter)
{
    RS_OPTIONAL_TRACE_FUNC();
    auto filterHash = filter->Hash();
    if (cachedFilteredSnapshot_ == nullptr || cachedFilterHash_ == filterHash) {
        return;
    }
    // filter changed, clear the filtered snapshot.
    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterHash Cache expired. Reason: Cached filtered hash "
               "%{public}X does not match new hash %{public}X.",
        cachedFilterHash_, filterHash);
    cachedFilteredSnapshot_.reset();
}
<<<<<<< HEAD
void RSFilterCacheManager::PostPartialFilterRenderInit(RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dstRect, bool& shouldClearFilteredCache)
{
    RS_OPTIONAL_TRACE_FUNC();
    auto surface = canvas.GetSurface();
    auto width = surface->Width();
    auto height = surface->Height();
    Drawing::Matrix mat = canvas.GetTotalMatrix();
    auto directionBias = CalcDirectionBias(mat);
    auto previousSurfaceFlag = task_->surfaceFlag;
    task_->surfaceFlag = directionBias;
    if (previousSurfaceFlag != -1 && previousSurfaceFlag != task_->surfaceFlag) {
        StopFilterPartialRender();
    }
    task_->SetCompleted(task_->GetStatus() == CacheProcessStatus::DONE);
    if (task_->GetStatus() == CacheProcessStatus::DOING) {
        task_->SetCompleted(!task_->isFirstInit_);
    }
    if (task_->GetStatus() == CacheProcessStatus::DONE) {
        task_->Reset();
        task_->isFirstInit_ = false;
        task_->SwapTexture();
        task_->SetStatus(CacheProcessStatus::WAITING);
    }
    if (task_->isTaskRelease_.load()) {
        return;
    }
    if (task_->cachedFirstFilter_ != nullptr && task_->isFirstInit_ &&
        task_->GetStatus() == CacheProcessStatus::DOING) {
        cachedFilteredSnapshot_ = task_->cachedFirstFilter_;
        shouldClearFilteredCache = IsClearFilteredCache(filter, dstRect);
    } else {
        task_->cachedFirstFilter_ = nullptr;
    }
    if (RSFilterCacheTask::FilterPartialRenderEnabled &&
        (cachedSnapshot_ != nullptr && cachedSnapshot_->cachedImage_ != nullptr) &&
        (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) &&
        IsNearlyFullScreen(cachedSnapshot_->cachedRect_, width, height)) {
        task_->isLastRender_ = false;
        PostPartialFilterRenderTask(filter, dstRect);
    }
}
=======
>>>>>>> zhangpeng/master

void RSFilterCacheManager::UpdateCacheStateWithFilterRegion()
{
    if (!IsCacheValid()) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithFilterRegion Cache expired. Reason: Filter region is not "
               "within the cached region.");
    InvalidateFilterCache();
}

void RSFilterCacheManager::UpdateCacheStateWithDirtyRegion()
{
    if (!IsCacheValid()) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();
    if (cacheUpdateInterval_ > 0) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache "
                    "invalidation for %{public}d frames.",
            cacheUpdateInterval_);
        pendingPurge_ = true;
    } else {
        InvalidateFilterCache();
    }
}

bool RSFilterCacheManager::UpdateCacheStateWithDirtyRegion(const RSDirtyRegionManager& dirtyManager)
{
    if (!IsCacheValid()) {
        return false;
    }
    RS_OPTIONAL_TRACE_FUNC();
    auto& cachedImageRect = GetCachedImageRegion();
    if (dirtyManager.currentFrameDirtyRegion_.Intersect(cachedImageRect) ||
        std::any_of(dirtyManager.visitedDirtyRegions_.begin(), dirtyManager.visitedDirtyRegions_.end(),
            [&cachedImageRect](const RectI& rect) { return rect.Intersect(cachedImageRect); })) {
        // The underlying image is affected by the dirty region, determine if the cache should be invalidated by cache
        // age. [PLANNING]: also take into account the filter radius / cache size / percentage of intersected area.
        if (cacheUpdateInterval_ > 0) {
            ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion Delaying cache "
                       "invalidation for %{public}d frames.",
                cacheUpdateInterval_);
            pendingPurge_ = true;
        } else {
            InvalidateFilterCache();
        }
        return false;
    }
    if (pendingPurge_) {
        ROSEN_LOGD("RSFilterCacheManager::UpdateCacheStateWithDirtyRegion MergeDirtyRect at %{public}d frames",
            cacheUpdateInterval_);
        InvalidateFilterCache();
        return true;
    } else {
        return false;
    }
}

void RSFilterCacheManager::DrawFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const bool needSnapshotOutset, const std::optional<Drawing::RectI>& srcRect,
    const std::optional<Drawing::RectI>& dstRect)
{
    RS_OPTIONAL_TRACE_FUNC();
    if (canvas.GetDeviceClipBounds().IsEmpty()) {
        return;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect);
    if (src.IsEmpty() || dst.IsEmpty()) {
        return;
    }
    RS_TRACE_NAME_FMT("RSFilterCacheManager::DrawFilter status: %s", GetCacheState());
    CheckCachedImages(canvas);
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src, needSnapshotOutset);
    }

    bool shouldClearFilteredCache = false;
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        auto previousFilterHash = cachedFilterHash_;
        GenerateFilteredSnapshot(canvas, filter, dst);
        // If 1. the filter hash matches, 2. the filter region is whole snapshot region, we can safely clear original
        // snapshot, else we need to clear the filtered snapshot.
        shouldClearFilteredCache = previousFilterHash != cachedFilterHash_ || !isEqualRect(dst, snapshotRegion_);
    }
    CompactFilterCache(shouldClearFilteredCache);
    needForceCache_ =false;
}

const std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> RSFilterCacheManager::GeneratedCachedEffectData(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const std::optional<Drawing::RectI>& srcRect, const std::optional<Drawing::RectI>& dstRect,
    const std::tuple<bool, bool>& forceCacheFlags)
{
    RS_OPTIONAL_TRACE_FUNC();
    if (canvas.GetDeviceClipBounds().IsEmpty()) {
        return nullptr;
    }
    const auto& [src, dst] = ValidateParams(canvas, srcRect, dstRect, forceCacheFlags);
    if (src.IsEmpty() || dst.IsEmpty()) {
        return nullptr;
    }
    RS_TRACE_NAME_FMT("RSFilterCacheManager::GeneratedCachedEffectData status: %s", GetCacheState());
    CheckCachedImages(canvas);
    if (!IsCacheValid()) {
        TakeSnapshot(canvas, filter, src);
    }

    bool shouldClearFilteredCache = false;
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        auto previousFilterHash = cachedFilterHash_;
        GenerateFilteredSnapshot(canvas, filter, dst);
        // If 1. the filter hash matches, 2. the filter region is whole snapshot region, we can safely clear original
        // snapshot, else we need to clear the filtered snapshot.
        shouldClearFilteredCache = previousFilterHash != cachedFilterHash_ || !isEqualRect(dst, snapshotRegion_);
    }
    // Keep a reference to the cached image, since CompactCache may invalidate it.
    auto cachedFilteredSnapshot = cachedFilteredSnapshot_;
    // To reduce the memory consumption, we only keep either the cached snapshot or the filtered image.
    if (needForceCache_) {
        shouldClearFilteredCache = false;
    }
    CompactFilterCache(shouldClearFilteredCache);
    needForceCache_ =false;
    return cachedFilteredSnapshot;
}

void RSFilterCacheManager::TakeSnapshot(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter,
    const Drawing::RectI& srcRect, const bool needSnapshotOutset)
{
    auto drawingSurface = canvas.GetSurface();
    if (drawingSurface == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // shrink the srcRect by 1px to avoid edge artifacts.
    Drawing::RectI snapshotIBounds;
    snapshotIBounds = srcRect;
    if (needSnapshotOutset) {
        snapshotIBounds.MakeOutset(-1, -1);
    }

    // Take a screenshot.
    auto snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds);
    if (snapshot == nullptr) {
        ROSEN_LOGD("RSFilterCacheManager::TakeSnapshot failed to make an image snapshot.");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(snapshot->GetWidth(), snapshot->GetHeight())) {
        ROSEN_LOGD("TakeSnapshot cache image resource(width:%{public}d, height:%{public}d).", snapshot->GetWidth(),
            snapshot->GetHeight());
        snapshot->HintCacheGpuResource();
    }
    filter->PreProcess(snapshot);

    // Update the cache state.
    snapshotRegion_ = RectI(srcRect.GetLeft(), srcRect.GetTop(), srcRect.GetWidth(), srcRect.GetHeight());
    cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(snapshot), snapshotIBounds);
    cachedFilterHash_ = 0;
}

void RSFilterCacheManager::GenerateFilteredSnapshot(
    RSPaintFilterCanvas& canvas, const std::shared_ptr<RSDrawingFilter>& filter, const Drawing::RectI& dstRect)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr || cachedSnapshot_ == nullptr || cachedSnapshot_->cachedImage_ == nullptr) {
        return;
    }
    // The cache type has been validated, so filterRegion_ and cachedImage_ should be valid. There is no need to check
    // them again.
    RS_OPTIONAL_TRACE_FUNC();

    // Create an offscreen canvas with the same size as the filter region.
    auto offscreenRect = dstRect;
    auto offscreenSurface = surface->MakeSurface(offscreenRect.GetWidth(), offscreenRect.GetHeight());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    // Src rect and dst rect, with origin at (0, 0).
    auto src = Drawing::Rect(0, 0, cachedSnapshot_->cachedRect_.GetWidth(), cachedSnapshot_->cachedRect_.GetHeight());
    auto dst = Drawing::Rect(0, 0, offscreenRect.GetWidth(), offscreenRect.GetHeight());

    // Draw the cached snapshot on the offscreen canvas, apply the filter, and post-process.
    filter->DrawImageRect(offscreenCanvas, cachedSnapshot_->cachedImage_, src, dst);
    filter->PostProcess(offscreenCanvas);

    // Update the cache state with the filtered snapshot.
    auto filteredSnapshot = offscreenSurface->GetImageSnapshot();
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(
            filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight())) {
        ROSEN_LOGD("GenerateFilteredSnapshot cache image resource(width:%{public}d, height:%{public}d).",
            filteredSnapshot->GetWidth(), filteredSnapshot->GetHeight());
        filteredSnapshot->HintCacheGpuResource();
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (filteredSnapshot->IsTextureBacked()) {
            RS_LOGI("RSFilterCacheManager::GenerateFilteredSnapshot cachedImage from texture to raster image");
            filteredSnapshot = filteredSnapshot->MakeRasterImage();
        }
    }
    cachedFilteredSnapshot_ =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(filteredSnapshot), offscreenRect);
}

void RSFilterCacheManager::DrawCachedFilteredSnapshot(RSPaintFilterCanvas& canvas, const Drawing::RectI& dstRect) const
{
    if (cachedFilteredSnapshot_ == nullptr || cachedFilteredSnapshot_->cachedImage_ == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_FUNC();

    // Draw in device coordinates.
    Drawing::AutoCanvasRestore autoRestore(canvas, true);
    canvas.ResetMatrix();

    // Only draw within the visible rect.
    ClipVisibleRect(canvas);

    // The cache type and parameters has been validated, dstRect must be subset of cachedFilteredSnapshot_->cachedRect_.
    Drawing::Rect dst = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom()};
    Drawing::Rect src = {dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom()};
    src.Offset(-cachedFilteredSnapshot_->cachedRect_.GetLeft(), -cachedFilteredSnapshot_->cachedRect_.GetTop());

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*cachedFilteredSnapshot_->cachedImage_, src, dst, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
}

void RSFilterCacheManager::InvalidateFilterCache(FilterCacheType clearType)
{
    if (clearType == FilterCacheType::BOTH) {
        cachedSnapshot_.reset();
        cachedFilteredSnapshot_.reset();
        RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::InvalidateFilterCache BOTH");
        return;
    }
    if (clearType == FilterCacheType::SNAPSHOT) {
        cachedSnapshot_.reset();
        RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::InvalidateFilterCache SNAPSHOT");
        return;
    }
    if (clearType == FilterCacheType::FILTERED_SNAPSHOT) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::InvalidateFilterCache FILTERED_SNAPSHOT");
        cachedFilteredSnapshot_.reset();
    }
}

void RSFilterCacheManager::ReleaseCacheOffTree()
{
    RS_OPTIONAL_TRACE_FUNC();
    cachedSnapshot_.reset();
    cachedFilteredSnapshot_.reset();
}

FilterCacheType RSFilterCacheManager::GetCachedType() const
{
    if (cachedSnapshot_ == nullptr && cachedFilteredSnapshot_ == nullptr) {
        return FilterCacheType::NONE;
    }
    if (cachedSnapshot_ != nullptr) {
        return FilterCacheType::SNAPSHOT;
    }
 
    if (cachedFilteredSnapshot_ != nullptr) {
        return FilterCacheType::FILTERED_SNAPSHOT;
    }
    return FilterCacheType::BOTH;
}

inline void RSFilterCacheManager::ClipVisibleRect(RSPaintFilterCanvas& canvas)
{
    auto visibleRectF = canvas.GetVisibleRect();
    visibleRectF.Round();
    Drawing::RectI visibleIRect = { (int)visibleRectF.GetLeft(), (int)visibleRectF.GetTop(),
        (int)visibleRectF.GetRight(), (int)visibleRectF.GetBottom() };
    auto deviceClipRect = canvas.GetDeviceClipBounds();
    if (!visibleIRect.IsEmpty() && deviceClipRect.Intersect(visibleIRect)) {
        canvas.ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
}

const RectI& RSFilterCacheManager::GetCachedImageRegion() const
{
    static const auto emptyRect = RectI();
    return IsCacheValid() ? snapshotRegion_ : emptyRect;
}

void RSFilterCacheManager::SetForceCache(bool forceCache)
{
    needForceCache_ = forceCache;
}

inline static bool IsCacheInvalid(const RSPaintFilterCanvas::CachedEffectData& cache, RSPaintFilterCanvas& canvas)
{
    return cache.cachedImage_ == nullptr || !cache.cachedImage_->IsValid(canvas.GetGPUContext().get());
}

void RSFilterCacheManager::CheckCachedImages(RSPaintFilterCanvas& canvas)
{
    if (needForceCache_) {
        return;
    }
    if (cachedSnapshot_ != nullptr && IsCacheInvalid(*cachedSnapshot_, canvas)) {
        ROSEN_LOGE("RSFilterCacheManager::CheckCachedImages cachedSnapshot_ is invalid");
        cachedSnapshot_.reset();
    }
    if (cachedFilteredSnapshot_ != nullptr && IsCacheInvalid(*cachedFilteredSnapshot_, canvas)) {
        ROSEN_LOGE("RSFilterCacheManager::CheckCachedImages cachedFilteredSnapshot_ is invalid");
        cachedFilteredSnapshot_.reset();
    }
}

std::tuple<Drawing::RectI, Drawing::RectI> RSFilterCacheManager::ValidateParams(RSPaintFilterCanvas& canvas,
    const std::optional<Drawing::RectI>& srcRect, const std::optional<Drawing::RectI>& dstRect,
    const std::tuple<bool, bool>& forceCacheFlags)
{
    Drawing::RectI src;
    Drawing::RectI dst;
    auto deviceRect = Drawing::RectI(0, 0, canvas.GetImageInfo().GetWidth(), canvas.GetImageInfo().GetHeight());
    if (!srcRect.has_value()) {
        src = canvas.GetDeviceClipBounds();
    } else {
        src = srcRect.value();
        src.Intersect(deviceRect);
    }
    if (!dstRect.has_value()) {
        dst = src;
    } else {
        dst = dstRect.value();
        dst.Intersect(deviceRect);
    }
    if (snapshotRegion_.GetLeft() > dst.GetLeft() || snapshotRegion_.GetRight() < dst.GetRight() ||
        snapshotRegion_.GetTop() > dst.GetTop() || snapshotRegion_.GetBottom() < dst.GetBottom()) {
        // dst region is out of snapshot region, cache is invalid.
        // It should already be checked by UpdateCacheStateWithFilterRegion in prepare phase, we should never be here.
        ROSEN_LOGD("RSFilterCacheManager::ValidateParams Cache expired. Reason: dst region is out of snapshot region.");
        if (!needForceCache_ && IsCacheValid()) {
            InvalidateFilterCache();
        }
    }
    return { src, dst };
}

void RSFilterCacheManager::CompactFilterCache(bool shouldClearFilteredCache)
{
    InvalidateFilterCache(shouldClearFilteredCache ?
        FilterCacheType::FILTERED_SNAPSHOT : FilterCacheType::SNAPSHOT);
}
} // namespace Rosen
} // namespace OHOS
#endif
