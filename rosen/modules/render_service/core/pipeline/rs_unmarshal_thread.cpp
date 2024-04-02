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

#include "pipeline/rs_unmarshal_thread.h"

#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_frame_report.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_data.h"
#include "rs_profiler.h"

#ifdef RES_SCHED_ENABLE
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#endif

namespace OHOS::Rosen {
namespace {
    constexpr int REQUEST_FRAME_AWARE_ID = 100001;
    constexpr int REQUEST_FRAME_AWARE_LOAD = 80;
    constexpr int REQUEST_FRAME_AWARE_NUM = 2;
}

RSUnmarshalThread& RSUnmarshalThread::Instance()
{
    static RSUnmarshalThread instance;
    return instance;
}

void RSUnmarshalThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RSUnmarshalThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
#ifdef RES_SCHED_ENABLE
    SubScribeSystemAbility();
#endif
}

void RSUnmarshalThread::SubScribeSystemAbility()
{
    RS_LOGD("%{public}s", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        RS_LOGE("%{public}s failed to get system ability manager client", __func__);
        return;
    }
    std::string threadName = "RSUnmarshalThread";
    std::string strUid = std::to_string(getuid());
    std::string strPid = std::to_string(getpid());
    std::string strTid = std::to_string(gettid());

    int label_priority_schedule = 2;
    saStatusChangeListener_ = new (std::nothrow)VSyncSystemAbilityListener(threadName, strUid, strPid, strTid, label_priority_schedule);
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, saStatusChangeListener_);
    if (ret != ERR_OK) {
        RS_LOGE("%{public}s subscribe system ability %{public}d failed.", __func__, RES_SCHED_SYS_ABILITY_ID);
        saStatusChangeListener_ = nullptr;
    }
}

void RSUnmarshalThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUnmarshalThread::RecvParcel(std::shared_ptr<MessageParcel>& parcel)
{
    if (!handler_) {
        RS_LOGE("RSUnmarshalThread::RecvParcel handler_ is nullptr");
        return;
    }
    RSTaskMessage::RSTask task = [this, parcel = parcel]() {
        if (RsFrameReport::GetInstance().GetEnable()) {
            RsFrameReport::GetInstance().SetFrameParam(
                REQUEST_FRAME_AWARE_ID, REQUEST_FRAME_AWARE_LOAD, REQUEST_FRAME_AWARE_NUM, 0);
        }
        auto transData = RSBaseRenderUtil::ParseTransactionData(*parcel);
        if (!transData) {
            return;
        }
        RS_PROFILER_ON_PARCEL_RECEIVE(parcel.get(), transData.get());
        std::lock_guard<std::mutex> lock(transactionDataMutex_);
        cachedTransactionDataMap_[transData->GetSendingPid()].emplace_back(std::move(transData));
    };
    PostTask(task);
    RSMainThread::Instance()->RequestNextVSync();
}

TransactionDataMap RSUnmarshalThread::GetCachedTransactionData()
{
    TransactionDataMap transactionData;
    {
        std::lock_guard<std::mutex> lock(transactionDataMutex_);
        std::swap(transactionData, cachedTransactionDataMap_);
    }
    return transactionData;
}
}
