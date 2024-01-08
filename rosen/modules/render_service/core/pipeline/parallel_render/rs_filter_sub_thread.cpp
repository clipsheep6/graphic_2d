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
#include "rs_filter_sub_thread.h"

#include <string>

#include "GLES3/gl3.h"
#include "include/core/SkCanvas.h"
#include "memory/rs_memory_manager.h"
#include "memory/rs_tag_tracker.h"
#include "rs_trace.h"

#include "common/rs_optional_trace.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"
#ifdef RES_SCHED_ENABLE
#include "res_sched_client.h"
#include "res_type.h"
#endif

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif // RS_ENABLE_VK

namespace OHOS::Rosen {
namespace {
#ifdef RES_SCHED_ENABLE
const uint32_t RS_SUB_QOS_LEVEL = 7;
constexpr const char* RS_BUNDLE_NAME = "render_service";
#endif
} // namespace
RSFilterSubThread::~RSFilterSubThread()
{
    RS_LOGI("~RSSubThread():%{public}d", threadIndex_);
    RSFilter::postTask = nullptr;
    PostTask([this]() { DestroyShareEglContext(); });
}

void RSFilterSubThread::Start()
{
    RS_LOGI("RSFilterSubThread::Start():%{public}d", threadIndex_);
    std::string name = "RSFilterSubThread" + std::to_string(threadIndex_);
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
    RSFilter::postTask = [this](std::weak_ptr<RSFilter::RSFilterTask> task) {
        filterTaskList_.emplace_back(task);
        RS_TRACE_NAME_FMT("postTask:%zu", filterTaskList_.size());
    };

    RSFilter::clearGpuContext = [this]() { ResetGrContext(); };

    RSFilter::setRelease = [this](std::weak_ptr<RSFilter::RSFilterTask> task) {
        PostTask([this, task]() {
            auto workTask = task.lock();
            if (!workTask) {
                RS_LOGE("Color picker task is null");
                return;
            }
            workTask->SetTaskRelease();
        });
    };
}

void RSFilterSubThread::StartColorPicker()
{
    RS_LOGI("RSColorPickerSubThread::Start():%{public}d", threadIndex_);
    std::string name = "RSColorPickerSubThread" + std::to_string(threadIndex_);
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
    RSColorPickerCacheTask::postColorPickerTask = [this](std::weak_ptr<RSColorPickerCacheTask> task) {
        auto colorPickerTask = task.lock();
        if (RSMainThread::Instance()->GetNoNeedToPostTask()) {
            colorPickerTask->SetStatus(CacheProcessStatus::WAITING);
            return;
        }
        PostTask([this, task]() { ColorPickerRenderCache(task); });
    };
}

void RSFilterSubThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSFilterSubThread::PostSyncTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSFilterSubThread::DumpMem(DfxString& log)
{
    PostSyncTask([&log, this]() { MemoryManager::DumpDrawingGpuMemory(log, grContext_.get()); });
}

float RSFilterSubThread::GetAppGpuMemoryInMB()
{
    float total = 0.f;
    PostSyncTask([&total, this]() {
        total = MemoryManager::GetAppGpuMemoryInMB(grContext_.get());
    });
    return total;
}

void RSFilterSubThread::CreateShareEglContext()
{
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr");
        return;
    }
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
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

void RSFilterSubThread::DestroyShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    if (renderContext_ != nullptr) {
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
#endif
}

void RSFilterSubThread::RenderCache(std::vector<std::weak_ptr<RSFilter::RSFilterTask>>& filterTaskList)
{
    RS_TRACE_NAME_FMT("RSFilterSubThread::RenderCache:%zu", filterTaskList.size());
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
    }
    if (fence_->Wait(SYNC_TIME_OUT) < 0) {
        RS_LOGE("RSFilterSubThread::RenderCache: fence time out");
        filterTaskList.clear();
        isWorking_.store(false);
        return;
    }
    if (grContext_ == nullptr) {
        RS_LOGE("RSFilterSubThread::RenderCache: grContext is null");
        filterTaskList.clear();
        isWorking_.store(false);
        return;
    }
    for (auto& task : filterTaskList) {
        auto workTask = task.lock();
        if (!workTask) {
            RS_LOGE("RSFilterSubThread::RenderCache: Render task is null");
            continue;
        }
        if (!workTask->InitSurface(grContext_.get())) {
            RS_LOGE("RSFilterSubThread::RenderCache: InitSurface failed");
            continue;
        }
        if (!workTask->Render()) {
            RS_LOGE("RSFilterSubThread::RenderCache: Render failed");
            continue;
        }
    }
#ifndef USE_ROSEN_DRAWING
    grContext_->flushAndSubmit(true);
#else
    grContext_->FlushAndSubmit(true);
#endif
    for (auto& task : filterTaskList) {
        auto workTask = task.lock();
        if (!workTask) {
            RS_LOGE("RSFilterSubThread::RenderCache, SaveFilteredImage task is null");
            continue;
        }
        if (!workTask->SaveFilteredImage()) {
            RS_LOGE("RSFilterSubThread::RenderCache, SaveFilteredImage failed");
            continue;
        }
        if (!workTask->SetDone()) {
            RS_LOGE("RSFilterSubThread::RenderCache, SetDone failed");
            continue;
        }
    }
    filterTaskList.clear();
    isWorking_.store(false);
}

void RSFilterSubThread::FlushAndSubmit()
{
    RS_TRACE_NAME_FMT("RSFilterSubThread::FlushAndSubmit():isWorking_:%d TaskList size:%zu",
        isWorking_.load(), filterReadyTaskList_.size());

    if (filterTaskList_.empty()) {
        return;
    }

    if (isWorking_.load()) {
        return;
    }

    filterTaskList_.swap(filterReadyTaskList_);
    for (auto& task : filterReadyTaskList_) {
        auto initTask = task.lock();
        if (!initTask) {
            RS_LOGE("RSFilterSubThread::FlushAndSubmit:SwapInit task is null");
            continue;
        }
        initTask->SwapInit();
    }
    isWorking_.store(true);
    PostTask([this]() { RenderCache(filterReadyTaskList_); });
}

void RSFilterSubThread::SetFence(sptr<SyncFence> fence)
{
    fence_ = fence;
}

void RSFilterSubThread::ColorPickerRenderCache(std::weak_ptr<RSColorPickerCacheTask> colorPickerTask)
{
    RS_TRACE_NAME("ColorPickerRenderCache");
    auto task = colorPickerTask.lock();
    if (!task) {
        RS_LOGE("Color picker task is null");
        return;
    }
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
    }
    if (grContext_ == nullptr) {
        RS_LOGE("Color picker grContext is null");
        return;
    }
    if (!task->InitSurface(grContext_.get())) {
        RS_LOGE("Color picker initSurface failed");
        return;
    }
    if (!task->Render()) {
        RS_LOGE("Color picker render failed");
    }
    RSMainThread::Instance()->RequestNextVSync();
    RSMainThread::Instance()->SetColorPickerForceRequestVsync(true);
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSFilterSubThread::CreateShareGrContext()
#else
sk_sp<GrContext> RSFilterSubThread::CreateShareGrContext()
#endif
{
    RS_TRACE_NAME("CreateShareGrContext");
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
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
        handler->ConfigureContext(&options, glesVersion, size);

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
#endif

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        sk_sp<GrDirectContext> grContext = GrDirectContext::MakeVulkan(
            RsVulkanContext::GetSingleton().GetGrVkBackendContext());
        if (grContext == nullptr) {
            RS_LOGE("nullptr grContext is null");
            return nullptr;
        }
        return grContext;
    }
#endif
    return nullptr;
}
#else
std::shared_ptr<Drawing::GPUContext> RSFilterSubThread::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        CreateShareEglContext();
        Drawing::GPUContextOptions options;
        auto handler = std::make_shared<MemoryHandler>();
        auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        auto size = glesVersion ? strlen(glesVersion) : 0;
        handler->ConfigureContext(&options, glesVersion, size);
        if (!gpuContext->BuildFromGL(options)) {
            RS_LOGE("nullptr gpuContext is null");
            return nullptr;
        }
        return gpuContext;
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        Drawing::GPUContextOptions options;
        auto handler = std::make_shared<MemoryHandler>();
        std::string vulkanVersion = RsVulkanContext::GetSingleton().GetVulkanVersion();
        auto size = vulkanVersion.size();
        handler->ConfigureContext(&options, vulkanVersion.c_str(), size);
        if (!gpuContext->BuildFromVK(RsVulkanContext::GetSingleton().GetGrVkBackendContext(), options)) {
            RS_LOGE("nullptr gpuContext is null");
            return nullptr;
        }
        return gpuContext;
    }
#endif
    return nullptr;
}
#endif

void RSFilterSubThread::ResetGrContext()
{
    RS_TRACE_NAME("ResetGrContext release resource");
    if (grContext_ == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    grContext_->flushAndSubmit(true);
    grContext_->freeGpuResources();
#else
    grContext_->FlushAndSubmit(true);
    grContext_->FreeGpuResources();
#endif
}
} // namespace OHOS::Rosen
