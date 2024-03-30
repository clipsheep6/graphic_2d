/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_sub_thread_rcd.h"
#include <string>
#include "memory/rs_tag_tracker.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"

#ifdef RES_SCHED_ENABLE
#include "res_type.h"
#include "res_sched_client.h"
#endif

namespace OHOS {
namespace Rosen {
RSSubThreadRCD::~RSSubThreadRCD()
{
    RS_LOGD("~RSSubThreadRCD");
}

void RSSubThreadRCD::Start(RenderContext* context)
{
    if (renderContext_ != nullptr) {
        RS_LOGD("RSSubThreadRCD already start!");
        return;
    }
    RS_LOGD("RSSubThreadRCD Started");
    std::string name = "RoundCornerDisplaySubThread";
    runner_ = AppExecFwk::EventRunner::Create(name);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
#ifdef RES_SCHED_ENABLE
    PostTask([this]() {
        std::string strPid = std::to_string(getpid());
        std::string strTid = std::to_string(gettid());
        const uint32_t RS_SUB_QOS_LEVEL = 7;
        std::string strQos = std::to_string(RS_SUB_QOS_LEVEL);
        std::unordered_map<std::string, std::string> mapPayload;
        mapPayload["pid"] = strPid;
        mapPayload[strTid] = strQos;
        mapPayload["bundleName"] = "RoundCornerDisplay";
        uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_THREAD_QOS_CHANGE;
        int64_t value = 0;
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, mapPayload);
    });
#endif
    renderContext_ = context;
}

void RSSubThreadRCD::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}
} // namespace Rosen
} // namespace OHOS