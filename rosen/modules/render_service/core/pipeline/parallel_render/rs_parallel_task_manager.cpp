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
        auto InitParallelRenderExt = (void(*)())RSParallelRenderExt::initParallelRenderLBFunc_;
        InitParallelRenderExt();
    }
}

RSParallelTaskManager::~RSParallelTaskManager()
{
    if (isParallelRenderExtEnabled_) {
        auto FreeParallelRenderExt = (void(*)())RSParallelRenderExt::freeParallelRenderLBFunc_;
        FreeParallelRenderExt();
    }
    RSParallelRenderExt::CloseParallelRenderExt();
}

void RSParallelTaskManager::Initialize(uint32_t threadNum) 
{
    threadNum_ = threadNum;
    if (isParallelRenderExtEnabled_) {
        auto ParallelRenderExtSetThreadNumCall = (void(*)(uint32_t))RSParallelRenderExt::setSubRenderThreadNumFunc_;
        ParallelRenderExtSetThreadNumCall();
    }
}

void RSParallelTaskManager::PushRenderTask(std::unique_ptr<RSRenderTask> renderTask)
{
    if (isParallelRenderExtEnabled_) {
        auto ParallelRenderExtAddRenderLoad = (void(*)(uint64_t, float))RSParallelRenderExt::addRenderLoadFunc_;
        ParallelRenderExtAddRenderLoad();
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
        RSParallelTaskManager::Instance()->SubmitSuperTask(taskNum_, std::move(cachedSuperRenderTask_));
        taskNum_++;
        cachedSuperRenderTask_ = std::make_unique<RSSuperRenderTask>(displayNode);
    }
}

std::vector<uint32_t> RSParallelTaskManager::LoadBalancing()
{
    std::vector<uint32_t> loadNumPerThread;
    if (isParallelRenderExtEnabled_) {
        auto ParallelRenderExtLB = (void(*)(std::vector<uint32_t &>))RSParallelRenderExt::loadBalancingFunc_;
        ParallelRenderExtLB(loadNumPerThread);
    } else {
        uint32_t avgLoadNum = renderTaskList_.size() / threadNum_;
        uint32_t loadMod = renderTaskList_.size() % threadNum_;
        for (uint32_t i = threadNum_; i > 0; i--) {
            if (i <= loadMod) {
                loadNumPerThread.push_back(avgLoadNum + 1);
            } else {
                loadNumPerThread.push_back(avgLoadNum)
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
        auto ParallelRenderExtClearRenderLoad = (void(*)())RSParallelRenderExt::clearRenderLoadFunc_;
        ParallelRenderExtClearRenderLoad();
    }
}

void RSParallelTaskManager::SetSubThreadRenderTaskLoad(uint32_t threadIdx, uint64_t loadId, float cost)
{
    if (isParallelRenderExtEnabled_) {
        auto ParallelRenderExtUpdateLoadCost =
            (void(*)(uint32_t, uint64_t, float))RSParallelRenderExt::updateLoadCostFunc_;
        ParallelRenderExtUpdateLoadCost(threadIdx, loadId, cost);
    }
}
} // OHOS
} // Rosen