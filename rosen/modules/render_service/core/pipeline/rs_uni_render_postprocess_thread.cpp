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

#include "pipeline/rs_uni_render_postprocess_thread.h"

#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSUniRenderPostprocessThread& RSUniRenderPostprocessThread::Instance()
{
    static RSUniRenderPostprocessThread instance;
    return instance;
}

void RSUniRenderPostprocessThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RSUniRenderPostprocessThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSUniRenderPostprocessThread::PostTask(const std::function<void()>& task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSUniRenderPostprocessThread::CommitLayer()
{

}

}
