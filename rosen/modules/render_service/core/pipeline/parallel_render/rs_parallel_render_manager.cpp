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

#include "rs_parallel_render_manager.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include "rs_trace.h"
#include "EGL/egl.h"
#include "rs_render_task.h"
#include "pipeline/rs_base_render_engine.h"
#include "render_context?render_context.h"
#include "pipeline/rs_main_thread.h"

namespace OHOS {
namespace Rosen {

static constexpr int PARALLEL_THREAD_NUM = 3;

RSParallelRenderManager* RSParallelRenderManager::Instance()
{
    static RSParallelRenderManager instance;
    return &instance;
}

void RSParallelRenderManager::SetParallelMode(bool parallelMode)
{
    parallelMode_ = parallelMode;
    TryEnableParallelRendering();
}

// You should always use GetParallelModeSafe() instead of GetParallelMode()
// except initialize variable 'isParallel' in 'rs_uni_render_visitor.cpp'
void RSParallelRenderManager::GetParallelModeSafe() const
{
    return GetParallelRenderingStatus() == ParallelStatus::ON;
}

void RSParallelRenderManager::GetParallelMode() const
{
    ParallelStatus status = GetParallelRenderingStatus();
    return (status == ParallelStatus::ON) || (status == ParallelStatus::FIRSTFLUSH);
}
 
void RSParallelRenderManager::StartSubRenderThread(uint32_t threadNum, RenderContext *context)
{
    if (GetParallelRenderingStatus() == ParallelStatus::OFF) {
        expectedSUbThreadNum_ = threadNum;
        firstFlush_ = true;
        renderContext_ = context;
        for (int i = 0; i < threadNum; ++i) {
            auto curThread = std::make_unique<RSParallelSubThread>(context, renderType_, i);
            curThread->StartSubThread();
            threadList_.push_back(std::move(curThread));
        }
        taskManager_.Initialize(threadNum);
    }
}

void RSParallelRenderManager::EndSubRenderThread()
{
    if (GetParallelRenderingStatus() == ParallelStatus::ON) {
        readySubThreadNum_ = expectedSubThreaNum_ = 0;
        isTaskReady_ = true;
        cvTask_.notify_all();
        packVisitor_ = nullptr;
        std::vector<std::unique_ptr<RSParallelSubThread>>().swap(threadList_);
    }
}

void RSParallelRenderManager::ReadySubThreadNumIncrement()
{
    ++readySubThreadNum_;
}

ParallelStatus RSParallelRenderManager::GetParallelRenderingStatus()
{
    if (expectedSubThreaNum_ == 0) {
        return ParallelStatus::OFF;
    } else if (expectedSubThreaNum_ == readySubThreadNum_) {
        return ParallelStatus::ON;
    } else if (firstFlush_) {
        return ParallelStatus::FIRSTFLUSH;
    } else {
        return ParallelStatus::WAITFIRSTFLUSH;
    }
}

void RSParallelRenderManager::CopyVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node)
{
    packVisitor_ = std::make_shared<RSParallelPackVisitor>(visitor);
    displayNode_ = node.shared_from_this();
    taskManager_.Reset();
    packVisitor_->ProcessDisplayRenderNode(node);
}

void RSParallelRenderManager::PackRenderTask(RSSurfaceRenderNode &node)
{
    taskManager_.PushRenderTask(std::make_unique<RSRenderTask>(node));
}

void RSParallelRenderManager::LoadBalanceAndNotify()
{
    taskManager_.LBCalcAndSubmitSuperTask(displayNode_);
    isTaskReady_ = true;
    cvTask_.notify_all();
}

void RSParallelRenderManager::UpdataTaskEvalCost()
{
}

void RSParallelRenderManager::MergeRenderResult(std::shared_ptr<SkCanvas> canvas)
{
    if (GetParallelRenderingStatus() == ParallelStatus::FIRSTFLUSH) {
        firstFlush_ = false;
        for (int i = 0; i < expectedSubThreaNum_; ++i) {
            threadList_[i]->WaitFlushReady();
        }
    } else {
        if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
            for (int i = 0; i < taskManager_.GetTaskNum(); ++i) {
                RS_TRACE_BEGIN("Wait Render finish");
                threadList_[i]->WaitFlushReady();
                RS_TRACE_END();
                RS_TRACE_BEGIN("Wait Fence Ready");
                threadList_[i]->WaitReleaseFence();
                RS_TRACE_END();
                auto texture = threadList_[i]->GetTexture();
                if (texture == nullptr) {
                    RS_LOGE("Texture %d is nullptr", i);
                    continue;
                }
                canvas->drawImage(texture, 0, 0);
            }
        } else {
            renderContext_->ShareMakeCurrent(EGL_NO_CONTEXT);
            for (int i = 0; i < threadList_.size(); ++i) {
                threadList_[i]->WaitFlushReady();
                renderContext_->ShareMakeCurrent(threadList_[i]->GetSharedContext());
                RS_TRACE_BEGIN("Start Flush");
                threadList_[i]->GetSkSurface()->flush();
                RS_TRACE_END();
                renderContext->ShareMakeCurrent(EGL_NO_CONTEXT);
            }
            renderContext_->MakeSelfCurrent();
        }
    }
    isTaskReady_ = false;
    cvTask_.notify_all();
}

void RSParallelRenderManager::SetFrameSize(int width, int height)
{
    width_ = width;
    height_ = height;
}

void RSParallelRenderManager::GetFrameSize(int &width, int &height)
{
    width = width_;
    height = height_;
}

void RSParallelRenderManager::SubmitSuperTask(int taskIndex, std::unique_ptr<RSSuperRenderTask> superRenderTask)
{
    if (taskIndex >= threadList_.size()) {
        RS_LOGE("taskIndex geq thread num");
        return;
    }
    threadList_[taskIndex]->SetSuperTask(std::move(superRenderTask));
}

void RSParallelRenderManager::WaitTaskReady()
{
    std::unique_lock<std::mutex> lock(taskMutex_);
    cvTask_.wait(lock, [&]() {
        return isTaskReady_;
    });
}

void RSParallelRenderManager::WaitRenderFinish()
{
    std::unique_lock<std::mutex> lock(taskMutex_);
    cvTask_.wait(lock, [&]() {
        return !isTaskReady_;
    });
}

void RSParallelRenderManager::SetRenderTaskCost(uint32_t subMainThreadIdx, uint64_t loadId, float cost)
{
    taskManager_.SetSubThreadRenderTaskLoad(subMainThreadIdx, loadId, cost);
}

bool RSParallelRenderManager::ParallelRenderExtEnabled()
{
    return taskManager_.GetParallelRenderExtEnable();
}

void RSParallelRenderManager::TryEnableParallelRendering(ParallelRenderType parallelEnableOption)
{
    if (parallelMode_) {
        StartSubRenderThread(PARALLEL_THREAD_NUM,
            RSMainThread::Instance()->GetRenderEngine()->GetRenderContext().get());
    } else {
        EndSubRenderThread();
    }
}

} // namespace Rosen
} // namespace OHOS