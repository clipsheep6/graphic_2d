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

#define EGL_EGLEXT_PROTOTYPES
#include "rs_sub_thread_filter.h"

#include <string>

#include "GLES3/gl3.h"
#include "include/core/SkCanvas.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_tag_tracker.h"
#include "rs_trace.h"

#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"
#ifdef RES_SCHED_ENABLE
#include "res_sched_client.h"
#include "res_type.h"
#endif

namespace OHOS::Rosen {
namespace {
#ifdef RES_SCHED_ENABLE
const uint32_t RS_SUB_QOS_LEVEL = 7;
constexpr const char* RS_BUNDLE_NAME = "render_service";
#endif
} // namespace
RSSubThreadFilter::~RSSubThreadFilter()
{
    RS_LOGI("~RSSubThread():%{public}d", threadIndex_);
    PostTask([this]() { DestroyShareEglContext(); });
}

void RSSubThreadFilter::Start()
{
    RS_LOGI("RSSubThreadFilter::Start():%{public}d", threadIndex_);
    std::string name = "RSSubThreadFilter" + std::to_string(threadIndex_);
    runner_ = AppExecFwk::EventRunner::Create(name);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    PostTask([this]() {
#ifdef RES_SCHED_ENABLE
        std::string strBundleName = RS_BUNDLE_NAME;
        std::string strPid = std::to_string(getpid());
        std::string strTid = std::to_string(gettid());
        std::string strQos = std::to_string(RS_SUB_QOS_LEVEL);
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["pid"] = strPid;
        mapPayload[strTid] = strQos;
        mapPayload["bundleName"] = strBundleName;
        uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_THREAD_QOS_CHANGE;
        int64_t value = 0;
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, mapPayload);
#endif
        grContext_ = CreateShareGrContext();
    });
}

void RSSubThreadFilter::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSSubThreadFilter::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSSubThreadFilter::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}

void RSSubThreadFilter::DumpMem(DfxString& log)
{
    PostSyncTask([&log, this]() { MemoryManager::DumpDrawingGpuMemory(log, grContext_.get()); });
}

float RSSubThreadFilter::GetAppGpuMemoryInMB()
{
    float total = 0.f;
    PostSyncTask([&total, this]() {
#ifndef USE_ROSEN_DRAWING
        total = MemoryManager::GetAppGpuMemoryInMB(grContext_.get());
#else
        RS_LOGE("Drawing Unsupport GetAppGpuMemoryInMB");
#endif
    });
    return total;
}

void RSSubThreadFilter::CreateShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr");
        return;
    }
    eglShareContext_ = renderContext_->CreateShareContext();
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        RS_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
        RS_LOGE("eglMakeCurrent failed");
        return;
    }
#endif
}

void RSSubThreadFilter::DestroyShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (renderContext_ != nullptr) {
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
#endif
}

void RSSubThreadFilter::RenderCache(
    std::function<void()> ThreadProcess, RSFilterCacheManager& cacheManager, float width, float height)
{
    RS_TRACE_NAME("RenderCache");
    std::unique_lock<std::mutex> lock(cacheManager.filterThreadMutex_);
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
        if (grContext_ == nullptr) {
            RS_LOGI("grContext is null");
            return;
        }
    }
    if (cacheManager.GetCacheSurfaceProcessedStatus() == CacheProcessStatus::WAITING) {
        return;
    }
    cacheManager.InitSurface(grContext_.get(), width, height);
    if (cacheManager.GetCacheSurfaceProcessedStatus() == CacheProcessStatus::WAITING) {
        return;
    }
    ThreadProcess();
    if (cacheManager.GetCacheSurfaceProcessedStatus() == CacheProcessStatus::WAITING) {
        return;
    }
    cacheManager.GetCacheSurface()->flush();
    if (cacheManager.GetCacheSurfaceProcessedStatus() == CacheProcessStatus::WAITING) {
        return;
    }
    cacheManager.UpdateBackendTexture();
    if (cacheManager.GetCacheSurfaceProcessedStatus() == CacheProcessStatus::WAITING) {
        return;
    }
    cacheManager.SetCacheSurfaceProcessedStatus(CacheProcessStatus::DONE);
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSSubThreadFilter::CreateShareGrContext()
#else
sk_sp<GrContext> RSSubThreadFilter::CreateShareGrContext()
#endif
{
    RS_TRACE_NAME("CreateShareGrContext");
    CreateShareEglContext();
    const GrGLInterface* grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    /* /data/service/el0/render_service is shader cache dir*/
    handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeGL(std::move(glInterface), options);
#else
    sk_sp<GrContext> grContext = GrContext::MakeGL(std::move(glInterface), options);
#endif
    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }
    return grContext;
}
#else
std::shared_ptr<Drawing::GPUContext> RSSubThreadFilter::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    CreateShareEglContext();
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    /* /data/service/el0/render_service is shader cache dir*/
    handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

    if (!gpuContext->BuildFromGL(options)) {
        RS_LOGE("nullptr gpuContext is null");
        return nullptr;
    }
    return gpuContext;
}
#endif

void RSSubThreadFilter::ResetGrContext()
{
    RS_TRACE_NAME("ResetGrContext release resource");
    if (grContext_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    grContext_->freeGpuResources();
#endif
}

void RSSubThreadFilter::ReleaseSurface()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (tmpSurfaces_.size() > 0) {
        auto tmp = tmpSurfaces_.front();
        tmpSurfaces_.pop();
        tmp = nullptr;
    }
}

} // namespace OHOS::Rosen
