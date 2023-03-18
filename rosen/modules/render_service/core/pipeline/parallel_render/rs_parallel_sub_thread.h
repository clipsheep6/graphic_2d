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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_SUB_THREAD_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_SUB_THREAD_H

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "SkImage.h"
#include "include/core/SkSurface.h"
#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#include "pipeline/parallel_render/rs_render_task.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderVisitor;
class RSDisplayRenderNode;
enum class ParallelRenderType;

class RSParallelSubThread {
public:
    explicit RSParallelSubThread(int threadIndex);
    RSParallelSubThread(RenderContext *context, ParallelRenderType renderType, int threadIndex);
    ~RSParallelSubThread();

    void StartSubThread();
    void WaitTaskSync();
    void SetMainVisitor(RSUniRenderVisitor *mainVisitor);
    bool GetRenderFinish();
    void SetSuperTask(std::unique_ptr<RSSuperRenderTask> superRenderTask);
    EGLContext GetSharedContext();
    sk_sp<SkSurface> GetSkSurface();
    sk_sp<SkImage> GetTexture();
    bool WaitReleaseFence();
    std::shared_ptr<RSUniRenderVisitor> GetUniVisitor() const
    {
        return visitor_;
    }
    void WaitSubMainThreadEnd()
    {
        subThread_->join();
    }

private:
    void MainLoop();
    void StartRender();
    void InitSubThread();
    void Render();
    void Flush();
    void CreateResource();
    void CreatePbufferSurface();
    void CreateShareEglContext();
    void StartPrepare();
    void Prepare();
    void CalcCost();
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> CreateShareGrContext();
#else
    sk_sp<GrContext> CreateShareGrContext();
#endif
    void AcquireSubSkSurface(int width, int height);

    uint32_t threadIndex_;
    int surfaceWidth_ = 0;
    int surfaceHeight_ = 0;
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
    sk_sp<SkSurface> skSurface_ = nullptr;
    SkCanvas *skCanvas_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> canvas_ = nullptr;
    std::shared_ptr<RSUniRenderVisitor> visitor_;

    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
    EGLSurface eglPSurface_ = EGL_NO_SURFACE;
    std::function<void()> mainLoop_;
    std::thread *subThread_;
    std::condition_variable cvFlush_;
    std::mutex flushMutex_;
    RenderContext *renderContext_ = nullptr;
    std::unique_ptr<RSSuperRenderTask> threadTask_;

    RSUniRenderVisitor *mainVisitor_;
    ParallelRenderType renderType_;
    sk_sp<SkImage> texture_;
    EGLSyncKHR eglSync_ = EGL_NO_SYNC_KHR;
    timespec startTime_;
    timespec stopTime_;

#ifdef RS_ENABLE_VK
    std::shared_ptr<RSDisplayRenderNode> displayNode_;
    std::unique_ptr<RSRenderFrame> renderFrame_;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_SUB_THREAD_H