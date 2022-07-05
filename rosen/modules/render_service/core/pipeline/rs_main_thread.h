/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RS_MAIN_THREAD
#define RS_MAIN_THREAD

#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "refbase.h"
#include "rs_render_engine.h"
#include "vsync_distributor.h"
#include "vsync_helper.h"
#include "vsync_receiver.h"

#include "command/rs_command.h"
#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "ipc_callbacks/iapplication_agent.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "pipeline/rs_context.h"
#include "platform/drawing/rs_vsync_client.h"

namespace OHOS::Rosen {
class RSTransactionData;

namespace Detail {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> t(new ScheduledTask(std::forward<Task&&>(task)));
        return std::make_pair(t, t->task_.get_future());
    }

    void Run()
    {
        task_();
    }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() override = default;

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace Detail

class RSMainThread {
public:
    static RSMainThread* Instance();

    void Init();
    void Start();
    void RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void RequestNextVSync();
    void PostTask(RSTaskMessage::RSTask task);
    void RenderServiceTreeDump(std::string& dumpString);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }

    const std::shared_ptr<RSRenderEngine>& GetRenderEngine() const
    {
        return renderEngine_;
    }

    RSContext& GetContext()
    {
        return context_;
    }
    std::thread::id Id() const
    {
        return mainThreadId_;
    }
    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app);
    void UnRegisterApplicationAgent(sptr<IApplicationAgent> app);

    void RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback);
    void UnRegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback);
    void CleanOcclusionListener();

    void WaitUtilUniRenderFinished();
    void NotifyUniRenderFinish();

    sptr<VSyncDistributor> rsVSyncDistributor_;

private:
    RSMainThread();
    ~RSMainThread() noexcept;
    RSMainThread(const RSMainThread&) = delete;
    RSMainThread(const RSMainThread&&) = delete;
    RSMainThread& operator=(const RSMainThread&) = delete;
    RSMainThread& operator=(const RSMainThread&&) = delete;

    void OnVsync(uint64_t timestamp, void* data);
    void ProcessCommand();
    void Animate(uint64_t timestamp);
    void ConsumeAndUpdateAllNodes();
    void ReleaseAllNodesBuffer();
    void Render();
    void CalcOcclusion();
    void CallbackToWMS(VisibleData& curVisVec);
    void SendCommands();

    bool DoParallelComposition(std::shared_ptr<RSBaseRenderNode> rootNode);
    void ResetSortedChildren(std::shared_ptr<RSBaseRenderNode> node);

    std::mutex transitionDataMutex_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RSTaskMessage::RSTask mainLoop_;
    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    std::unordered_map<NodeId, uint64_t> bufferTimestamps_;

    std::unordered_map<NodeId, std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>> cachedCommands_;
    std::vector<std::unique_ptr<RSCommand>> effectiveCommands_;
    std::vector<std::unique_ptr<RSCommand>> pendingEffectiveCommands_;
    std::string pendingTransactionFlags_;

    uint64_t timestamp_ = 0;
    std::unordered_map<uint32_t, sptr<IApplicationAgent>> applicationAgentMap_;

    RSContext context_;
    std::thread::id mainThreadId_;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
    std::vector<sptr<RSIOcclusionChangeCallback>> occlusionListeners_;

    mutable std::mutex uniRenderMutex_;
    bool uniRenderFinished_ = false;
    std::condition_variable uniRenderCond_;
    VisibleData lastVisVec_;
    bool doAnimate_ = false;
    uint32_t lastSurfaceCnt_ = 0;

    std::shared_ptr<RSRenderEngine> renderEngine_;
};
} // namespace OHOS::Rosen
#endif // RS_MAIN_THREAD
