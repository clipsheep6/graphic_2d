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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_CACHE_TASK_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_CACHE_TASK_H

#if defined(NEW_SKIA) && defined(RS_ENABLE_GL)
#include <condition_variable>
#include <mutex>

#include "event_handler.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkImageInfo.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#else
#include "draw/surface.h"
#include "utils/rect.h"
#endif

#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"
#include "event_handler.h"
#include "render/rs_color_picker.h"
#include "include/effects/SkRuntimeEffect.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSColorPickerCacheTask final {
public:
    static const bool FilterPartialRenderEnabled;
    RSColorPickerCacheTask() = default;
    ~RSColorPickerCacheTask();
#ifndef USE_ROSEN_DRAWING
    bool InitSurface(GrRecordingContext* grContext);
#else
    bool InitSurface(Drawing::GPUContext* grContext);
#endif
    bool Render();

    CacheProcessStatus GetStatus() const
    {
        return cacheProcessStatus_.load();
    }

    void SetStatus(CacheProcessStatus cacheProcessStatus)
    {
        cacheProcessStatus_.store(cacheProcessStatus);
    }

#ifndef USE_ROSEN_DRAWING
    bool InitTask(const sk_sp<SkImage> imageSnapshot);
#else
    bool InitTask(const sk_sp<SkImage> imageSnapshot);
#endif

    void Reset()
    {
        imageSnapshotCache_.reset();
    }

    void ResetGrContext();


    void ReleaseCacheOffTree();

    void Notify()
    {
        cvParallelRender_.notify_one();
    }

private:
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> cacheSurface_ = nullptr;
    GrBackendTexture cacheBackendTexture_;
#else
    std::shared_ptr<Drawing::Surface> cacheSurface_ = nullptr;
    Drawing::BackendTexture cacheBackendTexture_;
#endif
    bool inUse_ = false;
    bool valid_ = false;
    std::atomic<CacheProcessStatus> cacheProcessStatus_ = CacheProcessStatus::WAITING;
    sk_sp<SkImage> imageSnapshotCache_ = nullptr;
    RSColor color_;
    std::mutex parallelRenderMutex_;
    std::condition_variable cvParallelRender_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_CACHE_TASK_H
