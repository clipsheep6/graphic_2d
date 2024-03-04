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

#include "offscreen_render/rs_offscreen_render_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#ifdef ROSEN_OHOS
#include "render_context/render_context.h"
#endif

#ifdef RES_BASE_SCHED_ENABLE
#include "res_type.h"
#include "res_sched_client.h"
#endif

namespace OHOS::Rosen {
#ifndef ROSEN_CROSS_PLATFORM
namespace {
    constexpr int32_t SCHED_PRIORITY = 2;
    void SetThreadPriority()
    {
        struct sched_param param = {0};
        param.sched_priority = SCHED_PRIORITY;
        if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
            RS_LOGE("RSOffscreenRender Couldn't set SCHED_FIFO.");
        } else {
            RS_LOGI("RSOffscreenRender set SCHED_FIFO succeed.");
        }
        return;
    }
}
#endif

RSOffscreenRenderThread& RSOffscreenRenderThread::Instance()
{
#ifndef ROSEN_CROSS_PLATFORM
    SetThreadPriority();
#endif
    static RSOffscreenRenderThread instance;
    return instance;
}

RSOffscreenRenderThread::RSOffscreenRenderThread()
{
    runner_ = AppExecFwk::EventRunner::Create("RSOffscreenRender");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);

    #ifdef RES_BASE_SCHED_ENABLE
        uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_REPORT_RENDER_SERVICE;
        int64_t value = OHOS::ResourceSchedule::ResType::RenderServiceGroupType::Normal;
        constexpr int64_t REPORT_RSOffscreenRenderThread_DELAY = 3000;
        handler_->PostTask(
            [type, value] {
                std::string strBundleName = "RSOffscreenRenderThread";
                std::string strPid = std::to_string(getpid());
                std::string strTid = std::to_string(gettid());
                RS_LOGI("RSOffscreenRenderThread pid=%{public}d, tid=%{public}d.", getpid(), gettid());
                std::unordered_map<std::string, std::string> mapPayload;
                mapPayload["pid"] = strPid;
                mapPayload["tid"] = strTid;
                mapPayload["bundleName"] = strBundleName;
                OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, mapPayload);
            },
        REPORT_RSOffscreenRenderThread_DELAY);
    #endif

    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
#ifdef ROSEN_OHOS
    PostTask([this]() {
        renderContext_ = std::make_shared<RenderContext>();
#ifdef RS_ENABLE_GL
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
            renderContext_->InitializeEglContext();
        }
#endif

        renderContext_->SetUpGpuContext(nullptr);
    });
#endif
}

void RSOffscreenRenderThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

#ifdef ROSEN_OHOS
const std::shared_ptr<RenderContext>& RSOffscreenRenderThread::GetRenderContext()
{
    if (!renderContext_) {
        return nullptr;
    }
    return renderContext_;
}

void RSOffscreenRenderThread::CleanGrResource()
{
    PostTask([this]() {
        if (!renderContext_) {
            return;
        }
        auto grContext = renderContext_->GetDrGPUContext();
        if (grContext == nullptr) {
            RS_LOGE("RSOffscreenRenderThread::grContext is nullptr");
            return;
        }
        grContext->FreeGpuResources();
        RS_LOGD("RSOffscreenRenderThread::CleanGrResource() finished");
    });
}
#endif
}
