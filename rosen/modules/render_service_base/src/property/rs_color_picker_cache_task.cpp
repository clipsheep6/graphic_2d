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

#if defined(NEW_SKIA) && defined(RS_ENABLE_GL)
#include "include/gpu/GrBackendSurface.h"
#include "src/image/SkImage_Base.h"

#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
#define CHECK_CACHE_PROCESS_STATUS                                       \
    do {                                                                 \
        if (cacheProcessStatus_.load() == CacheProcessStatus::WAITING) { \
            return false;                                                \
        }                                                                \
    } while (false)

const bool RSColorPickerCacheTask::FilterPartialRenderEnabled =
    RSSystemProperties::GetFilterPartialRenderEnabled() && RSUniRenderJudgement::IsUniRender();

RSColorPickerCacheTask::~RSColorPickerCacheTask()
{
    ReleaseCacheOffTree();
}

#ifndef USE_ROSEN_DRAWING
bool RSColorPickerCacheTask::InitSurface(GrRecordingContext* grContext)
#else
bool RSColorPickerCacheTask::InitSurface(Drawing::GPUContext* grContext)
#endif
{
    RS_TRACE_NAME("RSColorPickerCacheTask InitSurface");
    if (cacheSurface_ != nullptr) {
        return true;
    }
    auto runner = AppExecFwk::EventRunner::Current();
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
#ifndef USE_ROSEN_DRAWING
    SkImageInfo info = SkImageInfo::MakeN32Premul(imageSnapshotCache_.width(), imageSnapshotCache_.height());
    cacheSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kYes, info);
#else
#endif
    return cacheSurface_ != nullptr;
}

bool RSColorPickerCacheTask::InitTask(const sk_sp<SkImage> imageSnapshot)
{
    RS_TRACE_NAME("RSColorPickerCacheTask InitTask");
    if (imageSnapshot == nullptr) {
        RS_TRACE_NAME("RSColorPickerCacheTask imageSnapshot is null");
        return false;
    }
    if (imageSnapshotCache_) {
        cacheBackendTexture_ = imageSnapshotCache_->getBackendTexture(false);
        return true;
    }
    imageSnapshotCache_ = imageSnapshot;
    return false;
}

bool RSColorPickerCacheTask::Render()
{
    RS_TRACE_NAME_FMT("RSColorPickerCacheTask::Render:%p", this);
    if (cacheSurface_ == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGD("RSColorPickerCacheTask cacheSurface is null");
        return false;
    }
    CHECK_CACHE_PROCESS_STATUS;
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface_.get());
    if (cacheCanvas == nullptr) {
        SetStatus(CacheProcessStatus::WAITING);
        ROSEN_LOGD("RSColorPickerCacheTask cacheCanvas is null");
        return false;
    }

    inUse_ = true;
    auto threadImage = SkImage::MakeFromTexture(cacheCanvas->recordingContext(), cacheBackendTexture_,
        kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
    
    SkString shaderString(R"(
        uniform shader imageInput;

        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            return half4(c.rgb, 1.0);
        }
    )");

    auto [effect, error] = SkRuntimeEffect::MakeForShader(shaderString);
    if (!effect) {
        ROSEN_LOGD("RSColorPickerCacheTask effect is null");
        SetStatus(CacheProcessStatus::WAITING);
        return false;
    }

    SkSamplingOptions linear(SkFilterMode::kLinear, SkMipmapMode::kNone);
    SkRuntimeShaderBuilder effectBulider(effect);
    SkImageInfo pcInfo;
    auto matrix = SkMatrix::Scale(1.0, 1.0);
    if (threadImage->width() * threadImage->height() < 10000) {
        pcInfo = SkImageInfo::MakeN32Premul(10, 10);
        matrix = SkMatrix::Scale(10.0 / threadImage->width(), 10.0 / threadImage->height());
    } else {
        pcInfo = SkImageInfo::MakeN32Premul(100, 100);
        matrix = SkMatrix::Scale(100.0 / threadImage->width(), 100.0 / threadImage->height());
    }
    effectBulider.child("imageInput") = threadImage->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, linear, matrix);
    sk_sp<skImage> tmpColorImg = effectBulider.makeImage(cacheCanvas->recordingContext(), nullptr, pcInfo, false);

    const int buffLen = tmpColorImg->width() * tmpColorImg->height();
    uint32_t* pixel = new uint32_t[buffLen];

    std::shared_ptr<SkPixmap> dst = std::make_shared<SkPixmap>(info, pixel, info.width() * info.bytesPerPixel());
    bool flag = tmpColorImg->readPixels(*dst, 0, 0);

    SkColor color;
    if (flag) {
        uint32_t errorCode;
        std::shared_ptr<RSColorPicker> colorPicker = RSColorPicker::CreateColorPicker(dst, errorCode);
        colorPicker->GetAverageColor(color);
        std::unique_lock<std::mutex> lock(parallelRenderMutex_);
        color_ = RSColor(SkColorGetR(color), SkColorGetG(color), SkColorGetB(color), SkColorGetA(color));
        valid_ = true;
    } else {
        valid_ = false;
    }
    delete [] pixel;

    CHECK_CACHE_PROCESS_STATUS;
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    cacheProcessStatus_.store(CacheProcessStatus::DONE);
    Notify();
    return true;
}

bool RSColorPickerCacheTask::GetColor(RSColor& color)
{
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    color = color_;
    return valid_;
}

void RSColorPickerCacheTask::ResetGrContext()
{
    if (cacheSurface_ != nullptr) {
        GrDirectContext* grContext_ = cacheSurface_->recordingContext()->asDirectContext();
        cacheSurface_ = nullptr;
        grContext_->freeGpuResources();
    }
}

void RSColorPickerCacheTask::ReleaseCacheOffTree()
{
    SetStatus(CacheProcessStatus::WAITING);
    Reset();
    if (handler_ != nullptr) {
        handler_->PostTask_([this]() {ResetGrContext();}, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

} // namespace Rosen
} // namespace OHOS
#endif
