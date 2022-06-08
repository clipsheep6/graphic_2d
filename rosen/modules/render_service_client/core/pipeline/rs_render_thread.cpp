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

#include "pipeline/rs_render_thread.h"

#ifdef OHOS_RSS_CLIENT
#include <unordered_map>
#endif

#include "pipeline/rs_frame_report.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#ifdef OHOS_RSS_CLIENT
#include "res_sched_client.h"
#include "res_type.h"
#endif
#include "rs_trace.h"
#include "ui/rs_ui_director.h"
#include "transaction/rs_render_service_client.h"
#ifdef ROSEN_OHOS
#include <sys/prctl.h>
#include <unistd.h>

#include "platform/ohos/rs_render_service_connect_hub.h"
#endif

static void SystemCallSetThreadName(const std::string& name)
{
#ifdef ROSEN_OHOS
    if (prctl(PR_SET_NAME, name.c_str()) < 0) {
        return;
    }
#endif
}

namespace OHOS {
namespace Rosen {
RSRenderThread& RSRenderThread::Instance()
{
    static RSRenderThread renderThread;
    return renderThread;
}

RSRenderThread::RSRenderThread()
{
#ifdef ACE_ENABLE_GL
    renderContext_ = new RenderContext();
    ROSEN_LOGD("Create RenderContext, its pointer is %p", renderContext_);
#endif
    mainFunc_ = [&]() {
        uint64_t renderStartTimeStamp = jankDetector_.GetSysTimeNs();
        std::string str = "RSRenderThread DrawFrame: " + std::to_string(timestamp_);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, str.c_str());
        {
            prevTimestamp_ = timestamp_;
            ProcessCommands();
            jankDetector_.ProcessUiDrawFrameMsg();
        }

        ROSEN_LOGD("RSRenderThread DrawFrame(%llu) in %s", prevTimestamp_, renderContext_ ? "GPU" : "CPU");
        Animate(prevTimestamp_);
        Render();
        RS_ASYNC_TRACE_BEGIN("waiting GPU running", 1111); // 1111 means async trace code for gpu
        SendCommands();
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        jankDetector_.CalculateSkippedFrame(renderStartTimeStamp, jankDetector_.GetSysTimeNs());
    };
}

RSRenderThread::~RSRenderThread()
{
    Stop();

    if (renderContext_ != nullptr) {
        ROSEN_LOGD("Destroy renderContext!!");
        delete renderContext_;
        renderContext_ = nullptr;
    }
}

void RSRenderThread::Start()
{
    ROSEN_LOGD("RSRenderThread start.");
    running_.store(true);
    if (thread_ == nullptr) {
        thread_ = std::make_unique<std::thread>(&RSRenderThread::RenderLoop, this);
    }

#ifdef ROSEN_OHOS
    RSRenderServiceConnectHub::SetOnConnectCallback(
        [weakThis = wptr<RSRenderThread>(this)](sptr<RSIRenderServiceConnection>& conn) {
            sptr<IApplicationRenderThread> renderThreadSptr = weakThis.promote();
            if (renderThreadSptr == nullptr) {
                return;
            }
            conn->RegisterApplicationRenderThread(getpid(), renderThreadSptr);
        });
#endif
}

void RSRenderThread::Stop()
{
    running_.store(false);

    if (runner_ != nullptr) {
        runner_->Stop();
    }

    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
    }

    thread_ = nullptr;
    ROSEN_LOGD("RSRenderThread stopped.");
}

void RSRenderThread::RecvTransactionData(std::unique_ptr<RSTransactionData>& transactionData)
{
    {
        std::unique_lock<std::mutex> cmdLock(cmdMutex_);
        uiTimestamp_ = transactionData->GetTimestamp();
        std::string str = "RecvCommands ptr:" + std::to_string(reinterpret_cast<uintptr_t>(transactionData.get()));
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, str.c_str());
        cmds_.emplace_back(std::move(transactionData));
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    }
    // [PLANNING]: process in next vsync (temporarily)
    RSRenderThread::Instance().RequestNextVSync();
    jankDetector_.UpdateUiDrawFrameMsg(uiStartTimeStamp_, jankDetector_.GetSysTimeNs(), uiDrawAbilityName_);
}

void RSRenderThread::RequestNextVSync()
{
    if (handler_) {
        RS_TRACE_FUNC();
        handler_->PostTask([this]() {
            VSyncReceiver::FrameCallback fcb = {
                .userData_ = this,
                .callback_ = std::bind(&RSRenderThread::OnVsync, this, std::placeholders::_1),
            };
            if (receiver_ != nullptr) {
                receiver_->RequestNextVSync(fcb);
            }
        });
    }
}

int32_t RSRenderThread::GetTid()
{
    return tid_;
}

void RSRenderThread::RenderLoop()
{
    SystemCallSetThreadName("RSRenderThread");

#ifdef OHOS_RSS_CLIENT
    std::unordered_map<std::string, std::string> payload;
    payload["uid"] = std::to_string(getuid());
    payload["pid"] = std::to_string(getpid());
    ResourceSchedule::ResSchedClient::GetInstance().ReportData(
        ResourceSchedule::ResType::RES_TYPE_REPORT_RENDER_THREAD, gettid(), payload);
#endif
#ifdef ROSEN_OHOS
    tid_ = gettid();
#endif
#ifdef ACE_ENABLE_GL
    renderContext_->InitializeEglContext(); // init egl context on RT
#endif
    std::string name = "RSRenderThread_" + std::to_string(::getpid());
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    auto rsClient = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    receiver_ = rsClient->CreateVSyncReceiver(name, handler_);
    if (receiver_ == nullptr) {
        ROSEN_LOGE("RSRenderThread CreateVSyncReceiver Error");
        return;
    }
    receiver_->Init();
    if (runner_) {
        runner_->Run();
    }
}

void RSRenderThread::OnVsync(uint64_t timestamp)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::OnVsync");
    mValue = (mValue + 1) % 2; // 1 and 2 is Calculated parameters
    RS_TRACE_INT("Vsync-client", mValue);
    timestamp_ = timestamp;
    if (activeWindowCnt_.load() > 0) {
        mainFunc_(); // start render-loop now
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::UpdateWindowStatus(bool active)
{
    if (active) {
        activeWindowCnt_++;
    } else {
        activeWindowCnt_--;
    }
    ROSEN_LOGD("RSRenderThread UpdateWindowStatus %d, cur activeWindowCnt_ %d", active, activeWindowCnt_.load());
}

void RSRenderThread::UpdateUiDrawFrameMsg(uint64_t startTimeStamp, const std::string& abilityName)
{
    uiStartTimeStamp_ = startTimeStamp;
    uiDrawAbilityName_ = abilityName;
}

void RSRenderThread::ProcessCommands()
{
    std::unique_lock<std::mutex> cmdLock(cmdMutex_);
    if (cmds_.empty()) {
        return;
    }
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().ProcessCommandsStart();
    }

    ROSEN_LOGD("RSRenderThread ProcessCommands size: %lu\n", cmds_.size());
    std::vector<std::unique_ptr<RSTransactionData>> cmds;
    std::swap(cmds, cmds_);
    cmdLock.unlock();

    for (auto& cmdData : cmds) {
        std::string str = "ProcessCommands ptr:" + std::to_string(reinterpret_cast<uintptr_t>(cmdData.get()));
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, str.c_str());
        cmdData->Process(context_);
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    }
}

void RSRenderThread::Animate(uint64_t timestamp)
{
    RS_TRACE_FUNC();

    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().AnimateStart();
    }

    if (context_.animatingNodeList_.empty()) {
        return;
    }

    // iterate and animate all animating nodes, remove if animation finished
    std::__libcpp_erase_if_container(context_.animatingNodeList_, [timestamp](const auto& iter) -> bool {
        auto node = iter.second.lock();
        if (node == nullptr) {
            ROSEN_LOGD("RSRenderThread::Animate removing expired animating node");
            return true;
        }
        bool animationFinished = !node->Animate(timestamp);
        if (animationFinished) {
            ROSEN_LOGD("RSRenderThread::Animate removing finished animating node %llu", node->GetId());
        }
        return animationFinished;
    });

    RSRenderThread::Instance().RequestNextVSync();
}

void RSRenderThread::Render()
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::Render");
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().RenderStart();
    }
    std::unique_lock<std::mutex> lock(mutex_);
    const auto& rootNode = context_.GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        ROSEN_LOGE("RSRenderThread::Render, rootNode is nullptr");
        return;
    }
    if (visitor_ == nullptr) {
        visitor_ = std::make_shared<RSRenderThreadVisitor>();
    }
    rootNode->Prepare(visitor_);
    rootNode->Process(visitor_);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::SendCommands()
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::SendCommands");
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().SendCommandsStart();
    }

    RSUIDirector::RecvMessages();
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::OnTransaction(std::shared_ptr<RSTransactionData> transactionData)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::OnTransaction");
    RSUIDirector::RecvMessages(transactionData);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::Detach(NodeId id)
{
    if (auto node = context_.GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        std::unique_lock<std::mutex> lock(mutex_);
        context_.GetGlobalRootRenderNode()->RemoveChild(node);
    }
}

void RSRenderThread::PostTask(RSTaskMessage::RSTask task)
{
    if (handler_) {
        handler_->PostTask(task);
    }
}

void RSRenderThread::PostPreTask()
{
    if (handler_ && preTask_) {
        handler_->PostTask(preTask_);
    }
}
} // namespace Rosen
} // namespace OHOS
