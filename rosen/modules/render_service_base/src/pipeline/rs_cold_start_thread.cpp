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

#include "pipeline/rs_cold_start_thread.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkPicture.h"
#include "include/core/SkSurface.h"
#include "include/core/SkImageInfo.h"
#include "include/gpu/GrContext.h"

#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSColdStartThread::RSColdStartThread(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode) :
    surfaceNode_(surfaceRenderNode)
{
    context_ = RSSharedContext::MakeSharedGLContext();
    if (thread_ == nullptr) {
        thread_ = std::make_unique<std::thread>(&RSColdStartThread::Run, this);
    }
}

RSColdStartThread::~RSColdStartThread()
{
    if (isRunning_) {
        Stop();
    }
}

void RSColdStartThread::Stop()
{
    RS_TRACE_NAME_FMT("RSColdStartThread::Stop");
    if (handler_ != nullptr) {
        handler_->PostSyncTask([this]() {
            for (auto grContext : grContexts_) {
                if (grContext != nullptr) {
                    grContext->abandonContext();
                }
            }
        }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
    if (runner_ != nullptr) {
        runner_->Stop();
    }
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
    }
    isRunning_ = false;
}

void RSColdStartThread::Run()
{
    RS_TRACE_NAME_FMT("RSColdStartThread::Run");
    if (context_ != nullptr) {
        context_->MakeCurrent();
    }
    isRunning_ = true;
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    if (runner_ != nullptr) {
        runner_->Run();
    }
}

void RSColdStartThread::PostPlayBackTask(sk_sp<SkPicture> picture, float width, float height)
{
    if (handler_ == nullptr) {
        RS_LOGE("RSColdStartThread::PostPlayBackTask failed, handler_ is nullptr");
        return;
    }
    auto task = [picture = picture, width = width, height = height, this]() {
        if (context_ == nullptr) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask context_ is nullptr");
            return;
        }
        auto node = surfaceNode_.lock();
        if (!node) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask surfaceNode is nullptr");
            return;
        }
        if (surface_ == nullptr) {
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
            SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
            auto grContext = context_->MakeGrContext();
            grContexts_.emplace_back(grContext);
            surface_ = SkSurface::MakeRenderTarget(grContext.get(), SkBudgeted::kYes, info);
#else
            surface_ = SkSurface::MakeRasterN32Premul(width, height);
#endif
        }
        if (surface_ == nullptr || surface_->getCanvas() == nullptr) {
            RS_LOGE("RSColdStartThread::PostPlayBackTask make SkSurface failed");
            return;
        }
        auto canvas = surface_->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        canvas->drawPicture(picture);
        if (node->GetCacheSurface() == nullptr) {
            node->NotifyUIBufferAvailable();
        }
        node->SwapCachedSurface(surface_);
    };
    handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}
} // namespace Rosen
} // namespace OHOS
