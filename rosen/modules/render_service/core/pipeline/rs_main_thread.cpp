/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_main_thread.h"

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_render_service_visitor.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_platform_canvas.h"
#include "platform/drawing/rs_vsync_client.h"
#include "transaction/rs_transaction_proxy.h"

using namespace OHOS::Rosen;

RSMainThread* RSMainThread::Instance()
{
    static RSMainThread instance;
    return &instance;
}

void RSMainThread::Start()
{
    mainLoop_ = [&]() {
        ROSEN_LOGI("RsDebug mainLoop start");
        ProcessCommand();
        Draw();
        ROSEN_LOGI("RsDebug mainLoop end");
    };

    threadLooper_ = RSThreadLooper::Create();
    threadHandler_ = RSThreadHandler::Create();
    vsyncClient_ = RSVsyncClient::Create();
    if (vsyncClient_) {
        vsyncClient_->SetVsyncCallback(std::bind(&RSMainThread::OnVsync, this, std::placeholders::_1));
    }
    while (1) {
        threadLooper_->ProcessAllMessages(-1);
    }
}

void RSMainThread::ProcessCommand()
{
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        std::swap(cacheCommandQueue_, effectCommandQueue_);
    }
    while (!effectCommandQueue_.empty())
    {
        auto rsTransaction = std::move(effectCommandQueue_.front());
        effectCommandQueue_.pop();
        if (rsTransaction) {
            rsTransaction->Process(context_);
        }
    }
}

void RSMainThread::Draw()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_.GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        ROSEN_LOGE("RSMainThread::Draw GetGlobalRootRenderNode fail");
        return;
    }
    std::shared_ptr<RSNodeVisitor> visitor = std::make_shared<RSRenderServiceVisitor>();
    rootNode->Prepare(visitor);
    rootNode->Process(visitor);
}

void RSMainThread::RequestNextVSync()
{
    if (vsyncClient_ != nullptr) {
        vsyncClient_->RequestNextVsync();
    }
}

void RSMainThread::OnVsync(uint64_t timestamp)
{
    if (threadHandler_) {
        if (!taskHandle_) {
            taskHandle_ = RSThreadHandler::StaticCreateTask(mainLoop_);
        }
        threadHandler_->PostTaskDelay(taskHandle_, 0);
    }
}

void RSMainThread::RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cacheCommandQueue_.push(std::move(rsTransactionData));
    }
    RequestNextVSync();
}

void RSMainThread::PostTask(RSTaskMessage::RSTask task)
{
    if (threadHandler_) {
        auto taskHandle = threadHandler_->CreateTask(task);
        threadHandler_->PostTask(taskHandle, 0);
    }
}

