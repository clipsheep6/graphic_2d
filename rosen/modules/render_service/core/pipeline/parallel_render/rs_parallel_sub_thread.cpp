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
 * WITHOUT WARRaANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define EGL_EGLEXT_PROTOTYPES
#include "rs_parallel_sub_thread.h"
#include <cstddef>
#include <memory>
#include <muytex>
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

namespace OHOS {
namespace Rosen {
RSParallelSubThread::RSParallelSubThread():renderType_(ParallelRenderType::DRAW_IMAGE) {}

RSParallelSubThread::RSParallelSubThread(RenderContext *context, ParallelRenderType renderType, int threadIndex)
    : threadIndex_(THreadIndex), renderContext_(context), renderType_(renderType) {}

RSParallelSubThread::~RSParallelSubThread() {}

void RSParallelSubThread::MainLoop()
{
    CreateShareEglContext();
    pthread_setname_np(pthread_self(), "SubMainThread");
    while (true) {
        WaitTaskSync();
        if (RSParallelRenderManager::Instance()->IsParallelRendering() == ParallelStatus::OFF) {
            return;
        }
        StartRender();
        Render();
        // FIRSTFLUSH or WAITFIRSTFLUSH
        if (RSParallelRenderManager::Instance()->IsParallelRendering() != ParallelStatus::ON) {
            NotifyRenderEnd();
        }
        Flush();
    }
}

void RSParallelSubThread::StartSubThread()
{
    subThread_ = new std::thread(&RSParallelSubThread::MainLoop, this);
}

void RSParallelSubThread::WaitTaskSync()
{
    subThreadFinish_ = false;
    RSParallelRenderManager::Instance()->WaitTaskRendy();
    if (renderType_ == ParallelRenderType::FLUSH_ONE_BUFFER) {
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_);
    }
}

void RSParallelSubThread::NotifyRenderEnd()
{
    subThreadFinish_ = true;
    cvFlush_.notify_all();
}

bool RSParallelSubThread::GetRenderFinish()
{
    return subThreadFinish_;
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
        if (eglPSurface_ == EGL_NO_SURFACE) {
            eglPSurface_ = renderContext_->CreateNewPBufferSurface();
        }
        if (eglPSurface_ == EGL_NO_CONTEXT) {
            RS_LOGE("eglPSurface is not ready");
        }
        if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), eglPSurface_, eglPSurface_, eglShareContext_)) {
            RS_LOGE("eglMakeCurrent failed");
            return;
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
    if (renderType_ == ParallelRenderType:: DRAW_IMAGE) {
        canvas_->clear(SK_ColorTRANSPARENT);
    }
    canvas_->save();
    auto displayNode = std::static_pointer_cast<RSDisplayRenderNode>(threadTask_->GetNode());
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(displayNode->GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        canvas_->concat(geoPtr->GetMatrix());
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
            RSParallelRenderManager::Instance()->SetRenderTaskCost(threadIndex_, task->GetIdx(), cost);
        }
    }
    canvas_->restore();
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
        texture = skSurface_->makeImageSnapshot();
        skCanvas_->discard();
    } else {
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    // FIRSTFLUSH or WAITFIRSTFLUSH
    if (RSParallelRenderManager::Instance()->IsParallelRendering() != ParallelStatus::ON) {
        RSParallelRenderManager::Instance()->ReadySubThreadNumIncrement();
    } else {
        NotifyRenderEnd();
    }
    RSParallelRenderManager::Instance()->WaitRenderFinish();
}

void RSParallelSubThread::WaitFlushReady()
{
    std::unique_lock<std::mutex> lock(firstMutex_);
    cvFlush_.wait(lock, [&]() {
        return subThreadFinish_;
    });
}

bool RSParallelSubThread::WaitReleaseFence()
{
    if (eglSync_ != EGL_NO_SYNC_KHR) {
        EGLint ret = eglClientWaitSyncKHR(renderCOntext_->GetEGLDisplay(), eglSYnc_, 0, 1000000000);
        if (ret == EGL_FALSE) {
            ROSEN_LOGE("eglClientWaitSyncKHR error 0x%{public}x", eglGetError());
            return false;
        } else if (ret == EGL_TIMEOUT_EXPIRED_KHR) {
            ROSEN_LOGE("create eglClientWaitSYncKHR timeout");
            return false;
        }
        eglDestorySyncKHR(renderContext_->GetEGLDisplay(), eglSync_);
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
        skCanvas_ = skSurface->getCanvas();
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skCanvas_);
    }
    visitor_ = std::make_shared<RSUniRenderVisitor>(canvas_);
}

sk_sp<GrContext> RSParallelSubThread::CreateShareGrContext()
{
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface().get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers = GpuPathRenderers::kAlll & ~GpuPathRenderers::kConverageCounting;
    options.fPreferExternalImagesOverRS3 = true;
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
    
    if (grContext_ == nullptr) {
        RS_LOGE("Share GrContext is not ready!!!");
        return;
    }

    if (renderType_ == ParallelRenderType::FLUSH_ONE_BUFFER) {
        GrGLFramebufferInfo framebufferInfo;
        framebufferInfo.fFBOID = 0;
        framebufferInfo.fFormat = GLRGBA8;

        SkColorType colorType = kRGBA_8888_SkColorType;

        GrBackendRenderTarget backendRenderTarget(width, height, 0, 8, framebufferInfo);
        SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;

        sk_sp<SkColorSpace> skColorSpace = nullptr;

        auto colorSpace = renderContext_->GetColorSpace();

        switch (colorSpace) {
            // [planning] in order to stay consistant with the colorspace used before, we disabled
            // COLOR_GAMUT_SRGB to let the branch to default, then skColorSpace is set to nullptr
            case COLOR_GAMUT_DISPLAY_P3:
                skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
                break;
            case COLOR_GAMUT_ADOBE_RGB:
                skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kAdobeRGB);
                break;
            case COLOR_GAMUT_BT2020:
                skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kRec2020);
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
        RS_LOGE("suSurface is not ready!!!")；
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
    return texture;
}

} // namespace Resen
} // namespace OHOS