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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_H

#include <cstdint>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "include/core/SkSurface.h"
#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#include "pipeline/parallel_render/rs_render_task.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context_base.h"
#include "include/gpu/gl/GrGLInterface.h"
#else
#include "render_context/render_context.h"
#endif
#include "event_handler.h"

namespace OHOS::Rosen {
class RSSubThread {
public:
#ifdef NEW_RENDER_CONTEXT
    RSSubThread(std::shared_ptr<RenderContextBase> context, uint32_t threadIndex) : threadIndex_(threadIndex),
        renderContext_(context) {}
#else
    RSSubThread(RenderContext* context, uint32_t threadIndex) : threadIndex_(threadIndex), renderContext_(context) {}
#endif
    ~RSSubThread();

    void Start();
    void PostTask(const std::function<void()>& task);
    void PostSyncTask(const std::function<void()>& task);
    void RenderCache(const std::shared_ptr<RSSuperRenderTask>& threadTask);
    void ResetGrContext();
    void DumpMem(DfxString& log);
private:
#ifdef NEW_RENDER_CONTEXT
    void CreateShareContext();
#else
    void CreateShareEglContext();
#endif
#ifdef NEW_RENDER_CONTEXT
    void DestroyShareContext();
#else
    void DestroyShareEglContext();
#endif
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> CreateShareGrContext();
#else
    sk_sp<GrContext> CreateShareGrContext();
#endif
#else
    std::shared_ptr<Drawing::GPUContext> CreateShareGrContext();
#endif

    uint32_t threadIndex_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RenderContextBase> renderContext_ = nullptr;
#else    
    RenderContext *renderContext_ = nullptr;
#endif
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
#else
    std::shared_ptr<Drawing::GPUContext> grContext_ = nullptr;
#endif
};
}
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_H