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

#include "rs_parallel_task_manager.h"
#include <cstdint>
#include <memory>
#include "rs_parallel_render_manager.h"
#include "rs_parallel_render_ext.h"

namespace OHOS {
namespace Rosen {

RSParallelTaskManager::RSParallelTaskManager()
    : isParallelRenderExtEnabled_(RSParallelRenderExt::OpenParallelRenderExt())
{
    if (isParallelRenderExtEnabled_) {
        auto initParallelRenderExt = reinterpret_cast<void(*)()>(RSParallelRenderExt::initParallelRenderLBFunc_);
        initParallelRenderExt();
    }
}

RSParallelTaskManager::~RSParallelTaskManager()
{
    if (isParallelRenderExtEnabled_) {
        auto freeParallelRenderExt = reinterpret_cast<void(*)()>(RSParallelRenderExt::freeParallelRenderLBFunc_);
        freeParallelRenderExt();
    }
    RSParallelRenderExt::CloseParallelRenderExt();
}

void RSParallelTaskManager::Initialize(uint32_t threadNum)
{
    threadNum_ = threadNum;
    if (isParallelRenderExtEnabled_) {
        auto parallelRenderExtSetThreadNumCall = reinterpret_cast<void(*)(uint32_t)>(
            RSParallelRenderExt::setSubRenderThreadNumFunc_);
        parallelRenderExtSetThreadNumCall(threadNum);
    }
}

void RSParallelTaskManager::PushRenderTask(std::unique_ptr<RSRenderTask> renderTask)
{
    if (isParallelRenderExtEnabled_) {
        auto parallelRenderExtAddRenderLoad = reinterpret_cast<void(*)(uint64_t, float)>(
            RSParallelRenderExt::addRenderLoadFunc_);
        parallelRenderExtAddRenderLoad(renderTask->GetIdx(), 0.f);
    }
    renderTaskList_.push_back(std::move(renderTask));
}

void RSParallelTaskManager::LBCalcAndSubmitSuperTask(std::shared_ptr<RSBaseRenderNode> displayNode)
{
    if (renderTaskList_.size() == 0) {
        return;
    }
    std::vector<uint32_t> loadNumPerThread = LoadBalancing();
    uint32_t index = 0;
    uint32_t loadNumSum = 0;
    taskNum_ = 0;
    cachedSuperRenderTask_ = std::make_unique<RSSuperRenderTask>(displayNode);
    for (uint32_t i = 0; i < loadNumPerThread.size(); i++) {
        loadNumSum += loadNumPerThread[i];
        while (index < loadNumSum) {
            cachedSuperRenderTask_->AddTask(std::move(renderTaskList_[index]));
            index++;
        }
        RSParallelRenderManager::Instance()->SubmitSuperTask(taskNum_, std::move(cachedSuperRenderTask_));
        taskNum_++;
        cachedSuperRenderTask_ = std::make_unique<RSSuperRenderTask>(displayNode);
    }
}

std::vector<uint32_t> RSParallelTaskManager::LoadBalancing()
{
    std::vector<uint32_t> loadNumPerThread;
    if (isParallelRenderExtEnabled_) {
        auto parallelRenderExtLB = reinterpret_cast<void(*)(std::vector<uint32_t> &)>(
            RSParallelRenderExt::loadBalancingFunc_);
        parallelRenderExtLB(loadNumPerThread);
    } else {
        uint32_t avgLoadNum = renderTaskList_.size() / threadNum_;
        uint32_t loadMod = renderTaskList_.size() % threadNum_;
        for (uint32_t i = threadNum_; i > 0; i--) {
            if (i <= loadMod) {
                loadNumPerThread.push_back(avgLoadNum + 1);
            } else {
                loadNumPerThread.push_back(avgLoadNum);
            }
        }
    }
    return loadNumPerThread;
}

int RSParallelTaskManager::GetTaskNum()
{
    return taskNum_;
}

void RSParallelTaskManager::Reset()
{
    renderTaskList_.clear();
    superRenderTaskList_.clear();
    if (isParallelRenderExtEnabled_) {
        auto parallelRenderExtClearRenderLoad = reinterpret_cast<void(*)()>(RSParallelRenderExt::clearRenderLoadFunc_);
        parallelRenderExtClearRenderLoad();
    }
}

void RSParallelTaskManager::SetSubThreadRenderTaskLoad(uint32_t threadIdx, uint64_t loadId, float cost)
{
    if (isParallelRenderExtEnabled_) {
        auto parallelRenderExtUpdateLoadCost = reinterpret_cast<void(*)(uint32_t, uint64_t, float)>(
            RSParallelRenderExt::updateLoadCostFunc_);
        parallelRenderExtUpdateLoadCost(threadIdx, loadId, cost);
    }
}
} // namespace Rosen
} // namespace OHOS