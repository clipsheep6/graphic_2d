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

#define EGL_EGLEXT_PROTOTYPES
#include "rs_parallel_sub_thread.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include "GLES3/gl3.h"
#include "SkCanvas.h"
#include "SkColor.h"
#include "SkRect.h"
#include "EGL/egl.h"
#include "rs_trace.h"
#include "pipeline/rs_display_render_node.h"
#include "rs_parallel_render_manager.h"
#include "include/gpu/GrContext.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "rs_parallel_render_ext.h"
#include "pipeline/rs_main_thread.h"

namespace OHOS {
namespace Rosen {
RSParallelSubThread::RSParallelSubThread(int threadIndex)
    : threadIndex_(threadIndex), renderType_(ParallelRenderType::DRAW_IMAGE) {}

RSParallelSubThread::RSParallelSubThread(RenderContext *context, ParallelRenderType renderType, int threadIndex)
    : threadIndex_(threadIndex), renderContext_(context), renderType_(renderType) {}

RSParallelSubThread::~RSParallelSubThread()
{
    eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
    eglShareContext_ = EGL_NO_CONTEXT;
    eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    texture_ = nullptr;
    canvas_ = nullptr;
    skSurface_ = nullptr;
    RS_LOGI("~RSParallelSubThread():%d", threadIndex_);
}

void RSParallelSubThread::MainLoop()
{
    InitSubThread();
    CreateShareEglContext();
    while (true) {
        WaitTaskSync();
        if (RSParallelRenderManager::Instance()->GetParallelRenderingStatus() == ParallelStatus::OFF) {
            return;
        }
        // parallel rendering will be enable when the windows number is greater than 50
        RSParallelRenderManager::Instance()->CommitSurfaceNum(50);
        if (RSParallelRenderManager::Instance()->GetTaskType() == TaskType::PREPARE_TASK) {
            StartPrepare();
            Prepare();
            RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
        } else {
            StartRender();
            Render();
            ParallelStatus status = RSParallelRenderManager::Instance()->GetParallelRenderingStatus();
            if (status == ParallelStatus::FIRSTFLUSH || status == ParallelStatus::WAITFIRSTFLUSH) {
                RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
            }
            Flush();
        }
    }
}

void RSParallelSubThread::StartSubThread()
{
    subThread_ = new std::thread(&RSParallelSubThread::MainLoop, this);
}

void RSParallelSubThread::WaitTaskSync()
{
    RSParallelRenderManager::Instance()->SubMainThreadWait(threadIndex_);
    if (renderType_ == ParallelRenderType::FLUSH_ONE_BUFFER) {
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_);
    }
}

void RSParallelSubThread::InitSubThread()
{
    pthread_setname_np(pthread_self(), "SubMainThread");
    // Thread prority number interval: [-20, 20); Lower number refers to a higher thread priority,
    // set -8 as experience reference.
    setpriority(PRIO_PROCESS, 0, -8);
    struct sched_param param = {0};
    // sched_priority interval: [1, 99]; higher number refers to a higher thread priority,
    // set 2 as experience reference.
    param.sched_priority = 2;
    (void)sched_setscheduler(0, SCHED_FIFO, &param);
}

void RSParallelSubThread::CreateShareEglContext()
{
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr");
        return;
    }
    eglShareContext_ = renderContext_->CreateShareContext();
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        RS_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
        if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
            RS_LOGE("eglMakeCurrent failed");
            return;
        }
    }
}

void RSParallelSubThread::StartPrepare()
{
    RSUniRenderVisitor *uniVisitor = RSParallelRenderManager::Instance()->GetUniVisitor();
    if (uniVisitor == nullptr) {
        RS_LOGE("uniVisitor is nullptr");
        return;
    }
    visitor_ = std::make_shared<RSUniRenderVisitor>(*uniVisitor);
}

void RSParallelSubThread::Prepare()
{
    if (threadTask_ == nullptr) {
        RS_LOGE("threadTask is nullptr");
        return;
    }
    while (threadTask_->GetTaskSize() > 0) {
        if (RSParallelRenderManager::Instance()->ParallelRenderExtEnabled()) {
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &startTime_);
        }
        auto task = threadTask_->GetNextRenderTask();
        if (!task || (task->GetIdx() == 0)) {
            RS_LOGE("surfaceNode is nullptr");
            continue;
        }
        auto node = task->GetNode();
        if (!node) {
            RS_LOGE("surfaceNode is nullptr");
            continue;
        }
        node->Prepare(visitor_);
        if (RSParallelRenderManager::Instance()->ParallelRenderExtEnabled()) {
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stopTime_);
            float cost = (stopTime_.tv_sec * 1000.0f + stopTime_.tv_nsec * 1e-6) -
                (startTime_.tv_sec * 1000.0f + startTime_.tv_nsec * 1e-6);
            RSParallelRenderManager::Instance()->SetRenderTaskCost(threadIndex_, task->GetIdx(), cost,
                TaskType::PREPARE_TASK);
        }
    }
}

void RSParallelSubThread::StartRender()
{
    CreateResource();
}

void RSParallelSubThread::Render()
{
    if (canvas_ == nullptr) {
        RS_LOGE("Canvas is nullptr");
        return;
    }
    if (threadTask_ == nullptr) {
        RS_LOGE("threadTask is nullptr");
        return;
    }
    int saveCount = canvas_->save();
    canvas_->SetHighContrast(RSMainThread::Instance()->GetRenderEngine()->IsHighContrastEnabled());
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
        canvas_->clear(SK_ColorTRANSPARENT);
    }
    canvas_->save();
    auto displayNode = std::static_pointer_cast<RSDisplayRenderNode>(threadTask_->GetNode());
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(displayNode->GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        canvas_->concat(geoPtr->GetMatrix());
        canvas_->SetCacheEnabled(geoPtr->IsNeedClientCompose());
    }
    while (threadTask_->GetTaskSize() > 0) {
        if (RSParallelRenderManager::Instance()->ParallelRenderExtEnabled()) {
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &startTime_);
        }
        auto task = threadTask_->GetNextRenderTask();
        if (!task || (task->GetIdx() == 0)) {
            RS_LOGE("renderTask is nullptr");
            continue;
        }
        auto node = task->GetNode();
        if (!node) {
            RS_LOGE("surfaceNode is nullptr");
            continue;
        }
        node->Process(visitor_);
        if (RSParallelRenderManager::Instance()->ParallelRenderExtEnabled()) {
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stopTime_);
            float cost = (stopTime_.tv_sec * 1000.0f + stopTime_.tv_nsec * 1e-6) -
                (startTime_.tv_sec * 1000.0f + startTime_.tv_nsec * 1e-6);
            RSParallelRenderManager::Instance()->SetRenderTaskCost(threadIndex_, task->GetIdx(), cost,
                TaskType::PROCESS_TASK);
        }
    }
    canvas_->restoreToCount(saveCount);
}

void RSParallelSubThread::Flush()
{
    threadTask_ = nullptr;
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
        RS_TRACE_BEGIN("Flush");
        skSurface_->flush();
        RS_TRACE_END();
        RS_TRACE_BEGIN("Create Fence");
        eglSync_ = eglCreateSyncKHR(renderContext_->GetEGLDisplay(), EGL_SYNC_FENCE_KHR, nullptr);
        RS_TRACE_END();
        texture_ = skSurface_->makeImageSnapshot();
        skCanvas_->discard();
    } else {
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    // FIRSTFLUSH or WAITFIRSTFLUSH
    ParallelStatus parallelStatus = RSParallelRenderManager::Instance()->GetParallelRenderingStatus();
    if (parallelStatus == ParallelStatus::FIRSTFLUSH || parallelStatus == ParallelStatus::WAITFIRSTFLUSH) {
        RSParallelRenderManager::Instance()->ReadySubThreadNumIncrement();
    } else {
        RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
    }
}

bool RSParallelSubThread::WaitReleaseFence()
{
    if (eglSync_ != EGL_NO_SYNC_KHR) {
        EGLint ret = eglWaitSyncKHR(renderContext_->GetEGLDisplay(), eglSync_, 0);
        if (ret == EGL_FALSE) {
            ROSEN_LOGE("eglClientWaitSyncKHR error 0x%{public}x", eglGetError());
            return false;
        } else if (ret == EGL_TIMEOUT_EXPIRED_KHR) {
            ROSEN_LOGE("create eglClientWaitSyncKHR timeout");
            return false;
        }
        eglDestroySyncKHR(renderContext_->GetEGLDisplay(), eglSync_);
    }
    eglSync_ = EGL_NO_SYNC_KHR;
    return true;
}

void RSParallelSubThread::CreateResource()
{
    int width, height;
    RSParallelRenderManager::Instance()->GetFrameSize(width, height);
    if (width != surfaceWidth_ || height != surfaceHeight_) {
        RS_LOGE("CreateResource %d, new size [%d, %d], old size [%d, %d]",
            threadIndex_, width, height, surfaceWidth_, surfaceHeight_);
        surfaceWidth_ = width;
        surfaceHeight_ = height;
        AcquireSubSkSurface(surfaceWidth_, surfaceHeight_);
        skCanvas_ = skSurface_->getCanvas();
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skCanvas_);
    }
    visitor_ = std::make_shared<RSUniRenderVisitor>(canvas_);
}

sk_sp<GrContext> RSParallelSubThread::CreateShareGrContext()
{
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers = GpuPathRenderers::kAll & ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    sk_sp<GrContext> grContext = GrContext::MakeGL(std::move(glInterface), options);
    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }
    return grContext;
}

void RSParallelSubThread::AcquireSubSkSurface(int width, int height)
{
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
    }
    
    if (grContext_ == nullptr) {
        RS_LOGE("Share GrContext is not ready!!!");
        return;
    }

    if (renderType_ == ParallelRenderType::FLUSH_ONE_BUFFER) {
        GrGLFramebufferInfo framebufferInfo;
        framebufferInfo.fFBOID = 0;
        framebufferInfo.fFormat = GL_RGBA8;

        SkColorType colorType = kRGBA_8888_SkColorType;
        constexpr uint8_t bitsPerColor = 8;
        GrBackendRenderTarget backendRenderTarget(width, height, 0, bitsPerColor, framebufferInfo);
        SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;

        sk_sp<SkColorSpace> skColorSpace = nullptr;

        auto colorSpace = renderContext_->GetColorSpace();
        skcms_TransferFunction function = SkNamedTransferFn::kSRGB;
        switch (colorSpace) {
            case COLOR_GAMUT_DISPLAY_P3:
                skColorSpace = SkColorSpace::MakeRGB(function, SkNamedGamut::kDCIP3);
                break;
            case COLOR_GAMUT_ADOBE_RGB:
                skColorSpace = SkColorSpace::MakeRGB(function, SkNamedGamut::kAdobeRGB);
                break;
            case COLOR_GAMUT_BT2020:
                skColorSpace = SkColorSpace::MakeRGB(function, SkNamedGamut::kRec2020);
                break;
            default:
                break;
        }

        skSurface_ = SkSurface::MakeFromBackendRenderTarget(
            grContext_.get(), backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, skColorSpace, &surfaceProps);
    } else {
        auto surfaceInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
        skSurface_ = SkSurface::MakeRenderTarget(grContext_.get(), SkBudgeted::kYes, surfaceInfo);
    }
    if (skSurface_ == nullptr) {
        RS_LOGE("skSurface is not ready!!!");
        return;
    }
}

EGLContext RSParallelSubThread::GetSharedContext()
{
    return eglShareContext_;
}

sk_sp<SkSurface> RSParallelSubThread::GetSkSurface()
{
    return skSurface_;
}

void RSParallelSubThread::SetSuperTask(std::unique_ptr<RSSuperRenderTask> superRenderTask)
{
    threadTask_ = std::move(superRenderTask);
}

sk_sp<SkImage> RSParallelSubThread::GetTexture()
{
    return texture_;
}

} // namespace Rosen
} // namespace OHOS