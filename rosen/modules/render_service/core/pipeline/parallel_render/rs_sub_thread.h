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
#include "include/gpu/GrDirectContext.h"
#ifdef RS_ENABLE_VK
#include"include/gpu/vk/GrVkBackendContext.h"
#endif
#include "pipeline/parallel_render/rs_render_task.h"
#include "render_context/render_context.h"
#include "event_handler.h"

namespace OHOS::Rosen {
class RSSubThread {
public:
    RSSubThread(RenderContext* context, uint32_t threadIndex) : threadIndex_(threadIndex), renderContext_(context) {}
    ~RSSubThread();

    pid_t Start();
    void PostTask(const std::function<void()>& task, const std::string& name = std::string());
    void PostSyncTask(const std::function<void()>& task);
    void RemoveTask(const std::string& name);
    void RenderCache(const std::shared_ptr<RSSuperRenderTask>& threadTask);
#ifdef RS_PARALLEL
    void DrawableCache(DrawableV2::RSSurfaceRenderNodeDrawable* nodeDrawable);
    bool CheckGrContext();
    std::shared_ptr<Drawing::Surface> TryGetCacheSurface(DrawableV2::RSSurfaceRenderNodeDrawable* nodeDrawable);
    void Subdraw(DrawableV2::RSSurfaceRenderNodeDrawable* nodeDrawable, std::shared_ptr<Drawing::Surface> cacheSurface);
    void SubmitToGPU(std::shared_ptr<Drawing::Surface> cacheSurface);
    void PreAlloc(DrawableV2::RSSurfaceRenderNodeDrawable* nodeDrawable, Vector2f cacheSize);
    void DumpDebugImage(
        DrawableV2::RSSurfaceRenderNodeDrawable* nodeDrawable, std::shared_ptr<Drawing::Surface> cacheSurface);
#endif
    void ReleaseSurface();
    void AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface);
    void ResetGrContext();
    void ThreadSafetyReleaseTexture();
    void DumpMem(DfxString& log);
    MemoryGraphic CountSubMem(int pid);
    float GetAppGpuMemoryInMB();
#ifdef RS_PARALLEL
    uint32_t getThreadIndex()
    {
        return threadIndex_;
    }
    unsigned int GetDoingCacheProcessNum()
    {
        return doingCacheProcessNum.load();
    }
    inline void DoingCacheProcessNumInc()
    {
        doingCacheProcessNum++;
    }
    unsigned int GetSurfacePreAllocedNum()
    {
        return preAllocedSurfaceNum.load();
    }
    inline void SurfacePreAllocedNumInc()
    {
        preAllocedSurfaceNum++;
    }
    inline void SurfacePreAllocedNumDec()
    {
        preAllocedSurfaceNum--;
    }
#endif
private:
    void CreateShareEglContext();
    void DestroyShareEglContext();
    std::shared_ptr<Drawing::GPUContext> CreateShareGrContext();

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    uint32_t threadIndex_ = UNI_RENDER_THREAD_INDEX;
    RenderContext *renderContext_ = nullptr;
#ifdef RS_ENABLE_GL
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
#endif
    std::shared_ptr<Drawing::GPUContext> grContext_ = nullptr;
    std::mutex mutex_;
    std::queue<std::shared_ptr<Drawing::Surface>> tmpSurfaces_;
#ifdef RS_PARALLEL
    std::atomic<unsigned int> doingCacheProcessNum = 0;
    std::atomic<unsigned int> preAllocedSurfaceNum = 0;
#endif
};
}
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_SUB_THREAD_H