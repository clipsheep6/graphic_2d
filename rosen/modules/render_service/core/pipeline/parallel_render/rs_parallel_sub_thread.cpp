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
#include <string>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include "GLES3/gl3.h"
#include "SkCanvas.h"
#include "SkColor.h"
#include "SkRect.h"
#include "EGL/egl.h"
#include "rs_trace.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_node_cost_manager.h"
#include "rs_parallel_render_manager.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_uni_render_visitor.h"
#include "rs_parallel_render_ext.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_engine.h"

namespace OHOS {
namespace Rosen {
RSParallelSubThread::RSParallelSubThread(int threadIndex)
    : threadIndex_(threadIndex), subThread_(nullptr), renderType_(ParallelRenderType::DRAW_IMAGE) {}

RSParallelSubThread::RSParallelSubThread(RenderContext *context, ParallelRenderType renderType, int threadIndex)
    : threadIndex_(threadIndex), subThread_(nullptr), renderContext_(context), renderType_(renderType) {}

RSParallelSubThread::~RSParallelSubThread()
{
    if (renderContext_ != nullptr) {
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    texture_ = nullptr;
    canvas_ = nullptr;
    skSurface_ = nullptr;
    RS_LOGI("~RSParallelSubThread():%d", threadIndex_);
}

void RSParallelSubThread::MainLoop()
{
    InitSubThread();
#ifdef RS_ENABLE_GL
    CreateShareEglContext();
#endif
    while (true) {
        WaitTaskSync();
        if (RSParallelRenderManager::Instance()->GetParallelRenderingStatus() == ParallelStatus::OFF) {
            return;
        }
        // parallel rendering will be enable when the windows number is greater than 50
        RSParallelRenderManager::Instance()->CommitSurfaceNum(50);
        switch (RSParallelRenderManager::Instance()->GetTaskType()) {
            case TaskType::PREPARE_TASK: {
                RS_TRACE_BEGIN("SubThreadCostPrepare[" + std::to_string(threadIndex_) + "]");
                StartPrepare();
                Prepare();
                RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
                RS_TRACE_END();
                break;
            }
            case TaskType::CALC_COST_TASK: {
                RS_TRACE_BEGIN("SubThreadCalcCost[" + std::to_string(threadIndex_) + "]");
                CalcCost();
                RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
                RS_TRACE_END();
                break;
            }
            case TaskType::PROCESS_TASK: {
                RS_TRACE_BEGIN("SubThreadCostProcess[" + std::to_string(threadIndex_) + "]");
                StartRender();
                Render();
                ParallelStatus status = RSParallelRenderManager::Instance()->GetParallelRenderingStatus();
                if (status == ParallelStatus::FIRSTFLUSH || status == ParallelStatus::WAITFIRSTFLUSH) {
                    RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
                }
                RS_TRACE_END();
                Flush();
                break;
            }
            case TaskType::COMPOSITION_TASK: {
                StartComposition();
                Composition();
                RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
                break;
            }
            case TaskType::CACHE_TASK: {
                RS_TRACE_BEGIN("SubThreadCacheProcess[" + std::to_string(threadIndex_) + "]");
                StartRenderCache();
                RenderCache();
                RSParallelRenderManager::Instance()->SubMainThreadNotify(threadIndex_);
                RS_TRACE_END();
                break;
            }
            default: {
                break;
            }
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
}

void RSParallelSubThread::InitSubThread()
{
    pthread_setname_np(pthread_self(), "SubMainThread");
    struct sched_param param = {0};
    // sched_priority interval: [1, 99]; higher number refers to a higher thread priority,
    // set 2 as experience reference.
    param.sched_priority = 2;
    (void)sched_setscheduler(0, SCHED_FIFO, &param);
}

void RSParallelSubThread::CreateShareEglContext()
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
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
        if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
            RS_LOGE("eglMakeCurrent failed");
            return;
        }
    }
#endif
}

void RSParallelSubThread::StartPrepare()
{
    InitUniVisitor();
}

void RSParallelSubThread::InitUniVisitor()
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
        RSParallelRenderManager::Instance()->StartTiming(threadIndex_);
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
        RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
            threadIndex_, task->GetIdx(), TaskType::PREPARE_TASK);
    }
}

void RSParallelSubThread::CalcCost()
{
    if (threadTask_ == nullptr) {
        RS_LOGE("CalcCost thread task is null");
        return;
    }
    RSUniRenderVisitor *uniVisitor = RSParallelRenderManager::Instance()->GetUniVisitor();
    if (uniVisitor == nullptr) {
        RS_LOGE("CalcCost visitor is null");
        return;
    }
    std::shared_ptr<RSNodeCostManager> manager = std::make_shared<RSNodeCostManager>(
        RSParallelRenderManager::Instance()->IsDoAnimate(),
        RSParallelRenderManager::Instance()->IsOpDropped(),
        RSParallelRenderManager::Instance()->IsSecurityDisplay());

    while (threadTask_->GetTaskSize() > 0) {
        RSParallelRenderManager::Instance()->StartTiming(threadIndex_);
        auto task = threadTask_->GetNextRenderTask();
        if (task == nullptr || task->GetIdx() == 0) {
            RS_LOGI("CalcCost task is invalid");
            continue;
        }
        auto node = task->GetNode();
        if (node == nullptr) {
            RS_LOGI("CalcCost node is null");
            continue;
        }
        auto surfaceNodePtr = node->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNodePtr == nullptr) {
            RS_LOGI("CalcCost surface node is null");
            continue;
        }
        manager->CalcNodeCost(*surfaceNodePtr);
        surfaceNodePtr->SetNodeCost(manager->GetDirtyNodeCost());
        RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
            threadIndex_, task->GetIdx(), TaskType::CALC_COST_TASK);
    }
}

void RSParallelSubThread::StartRenderCache()
{
    InitUniVisitor();
    if (visitor_) {
        visitor_->CopyPropertyForParallelVisitor(RSParallelRenderManager::Instance()->GetUniVisitor());
    }
}

void RSParallelSubThread::RenderCache()
{
#ifdef RS_ENABLE_GL
    while (threadTask_->GetTaskSize() > 0) {
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
        auto surfaceNodePtr = node->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceNodePtr) {
            RS_LOGE("RenderCache ReinterpretCastTo fail");
            continue;
        }
        if (surfaceNodePtr->GetCacheSurface() == nullptr) {
            int width = std::ceil(surfaceNodePtr->GetRenderProperties().GetBoundsRect().GetWidth());
            int height = std::ceil(surfaceNodePtr->GetRenderProperties().GetBoundsRect().GetHeight());
            AcquireSubSkSurface(width, height);
            surfaceNodePtr->InitCacheSurface(skSurface_);
        }
        bool needNotify = !surfaceNodePtr->HasCachedTexture();
        node->Process(visitor_);
        auto skCanvas = surfaceNodePtr->GetCacheSurface() ? surfaceNodePtr->GetCacheSurface()->getCanvas() : nullptr;
        if (skCanvas) {
            RS_TRACE_NAME_FMT("render cache flush, %s", surfaceNodePtr->GetName().c_str());
            skCanvas->flush();
        } else {
            RS_LOGE("skCanvas is nullptr, flush failed");
        }
        if (needNotify) {
            RSParallelRenderManager::Instance()->NodeTaskNotify(node->GetId());
        }
    }
    eglSync_ = eglCreateSyncKHR(renderContext_->GetEGLDisplay(), EGL_SYNC_FENCE_KHR, nullptr);
    WaitReleaseFence();
#endif
}

void RSParallelSubThread::StartRender()
{
    CreateResource();
}

void RSParallelSubThread::Render()
{
    if (threadTask_ == nullptr) {
        RS_LOGE("threadTask is nullptr");
        return;
    }
    auto physicalDisplayNode = std::static_pointer_cast<RSDisplayRenderNode>(threadTask_->GetNode());
    auto physicalGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(
        physicalDisplayNode->GetRenderProperties().GetBoundsGeometry());
#ifdef RS_ENABLE_GL
    if (canvas_ == nullptr) {
        RS_LOGE("Canvas is nullptr");
        return;
    }
    int saveCount = canvas_->save();
    if (RSMainThread::Instance()->GetRenderEngine()) {
        canvas_->SetHighContrast(RSMainThread::Instance()->GetRenderEngine()->IsHighContrastEnabled());
    }
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
        canvas_->clear(SK_ColorTRANSPARENT);
    }
    canvas_->save();
    if (physicalGeoPtr != nullptr) {
        canvas_->concat(physicalGeoPtr->GetMatrix());
        canvas_->SetCacheType(physicalGeoPtr->IsNeedClientCompose() ? RSPaintFilterCanvas::CacheType::ENABLED
                                                                    : RSPaintFilterCanvas::CacheType::DISABLED);
    }
    while (threadTask_->GetTaskSize() > 0) {
        RSParallelRenderManager::Instance()->StartTiming(threadIndex_);
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
        RSParallelRenderManager::Instance()->StopTimingAndSetRenderTaskCost(
            threadIndex_, task->GetIdx(), TaskType::PROCESS_TASK);
    }
    canvas_->restoreToCount(saveCount);
#elif RS_ENABLE_VK
    if (!displayNode_) {
        RS_LOGE("RSParallelSubThread::Render displayNode_ nullptr");
        return;
    }
    displayNode_->SetScreenId(physicalDisplayNode->GetScreenId());
    displayNode_->SetDisplayOffset(
        physicalDisplayNode->GetDisplayOffsetX(), physicalDisplayNode->GetDisplayOffsetY());
    displayNode_->SetForceSoftComposite(physicalDisplayNode->IsForceSoftComposite());
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(
        displayNode_->GetRenderProperties().GetBoundsGeometry());
    if (physicalGeoPtr && geoPtr) {
        *geoPtr = *physicalGeoPtr;
        geoPtr->UpdateByMatrixFromSelf();
    }
    while (threadTask_->GetTaskSize() > 0) {
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
        displayNode_->AddCrossParentChild(node);
    }
    displayNode_->Process(visitor_);
    for (auto& child : displayNode_->GetChildren()) {
        if (auto renderChild = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(child.lock())) {
            displayNode_->RemoveCrossParentChild(renderChild, physicalDisplayNode);
        }
    }
#endif
}

void RSParallelSubThread::Flush()
{
    threadTask_ = nullptr;
#ifdef RS_ENABLE_GL
    if (skCanvas_ == nullptr) {
        RS_LOGE("in Flush(), skCanvas is nullptr");
        return;
    }
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
        RS_TRACE_BEGIN("Flush");
#ifdef NEW_SKIA
        if (grContext_) {
            grContext_->flushAndSubmit(false);
        }
#else
        // skCanvas_->flush() may tasks a long time when window is zoomed in and out. So let flush operation of
        // subMainThreads are executed in sequence to reduce probability rather than solve the question.
        RSParallelRenderManager::Instance()->LockFlushMutex();
        skCanvas_->flush();
        RSParallelRenderManager::Instance()->UnlockFlushMutex();
#endif
        RS_TRACE_END();
        RS_TRACE_BEGIN("Create Fence");
        eglSync_ = eglCreateSyncKHR(renderContext_->GetEGLDisplay(), EGL_SYNC_FENCE_KHR, nullptr);
        RS_TRACE_END();
        texture_ = skSurface_->makeImageSnapshot();
        skCanvas_->discard();
    }
#endif
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
#ifdef RS_ENABLE_GL
    int width, height;
    RSParallelRenderManager::Instance()->GetFrameSize(width, height);
    if (width != surfaceWidth_ || height != surfaceHeight_) {
        RS_LOGE("CreateResource %d, new size [%d, %d], old size [%d, %d]",
            threadIndex_, width, height, surfaceWidth_, surfaceHeight_);
        surfaceWidth_ = width;
        surfaceHeight_ = height;
        AcquireSubSkSurface(surfaceWidth_, surfaceHeight_);
        if (skSurface_ == nullptr) {
            RS_LOGE("in CreateResource, skSurface is nullptr");
            return;
        }
        skCanvas_ = skSurface_->getCanvas();
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skCanvas_);
    }
    visitor_ = std::make_shared<RSUniRenderVisitor>(canvas_, threadIndex_);
#elif RS_ENABLE_VK
    displayNode_ = RSParallelRenderManager::Instance()->GetParallelDisplayNode(threadIndex_);
    if (!displayNode_) {
        RS_LOGE("RSParallelSubThread::CreateResource displayNode_ nullptr");
        return;
    }
    visitor_ = std::make_shared<RSUniRenderVisitor>(nullptr, threadIndex_);
    visitor_->SetRenderFrame(
        RSParallelRenderManager::Instance()->GetParallelFrame(threadIndex_));
#endif
    visitor_->CopyPropertyForParallelVisitor(
        RSParallelRenderManager::Instance()->GetUniVisitor());
}

#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSParallelSubThread::CreateShareGrContext()
#else
sk_sp<GrContext> RSParallelSubThread::CreateShareGrContext()
#endif
{
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
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

void RSParallelSubThread::AcquireSubSkSurface(int width, int height)
{
    if (grContext_ == nullptr) {
        grContext_ = CreateShareGrContext();
    }
    
    if (grContext_ == nullptr) {
        RS_LOGE("Share GrContext is not ready!!!");
        return;
    }

    auto surfaceInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    skSurface_ = SkSurface::MakeRenderTarget(grContext_.get(), SkBudgeted::kYes, surfaceInfo);
    if (skSurface_ == nullptr) {
        RS_LOGE("skSurface is not ready!!!");
        return;
    }
}

void RSParallelSubThread::StartComposition()
{
    if (processorRenderEngine_ == nullptr) {
        processorRenderEngine_ = std::make_shared<RSUniRenderEngine>();
        processorRenderEngine_->Init();
    }
    compositionVisitor_ = std::make_shared<RSUniRenderVisitor>();
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    if (parallelRenderManager->GetUniParallelCompositionVisitor() != nullptr) {
        compositionVisitor_->CopyVisitorInfos(parallelRenderManager->GetUniParallelCompositionVisitor());
    } else {
        compositionVisitor_ = nullptr;
    }
}

void RSParallelSubThread::Composition()
{
    if (compositionTask_ == nullptr || compositionTask_->GetIdx() == 0) {
        RS_LOGE("compositionTask is nullptr or displayNodeId is 0");
        return;
    }
    
    auto node = compositionTask_->GetNode();
    if (node == nullptr || compositionVisitor_ == nullptr) {
        RS_LOGE("displayNode or visitor is nullptr.");
        return;
    }

    compositionVisitor_->SetProcessorRenderEngine(processorRenderEngine_);
    node->Process(compositionVisitor_);
    compositionVisitor_ = nullptr;
    compositionTask_ = nullptr;
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

void RSParallelSubThread::SetCompositionTask(std::unique_ptr<RSCompositionTask> compositionTask)
{
    compositionTask_ = std::move(compositionTask);
}

sk_sp<SkImage> RSParallelSubThread::GetTexture()
{
    return texture_;
}

} // namespace Rosen
} // namespace OHOS