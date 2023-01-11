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

#include "platform/ohos/event_handler_ohos.h"

#include "platform/ohos/event_runner_ohos.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSEventHandler> RSEventHandler::Create(const std::shared_ptr<RSEventRunner>& runner)
{
    std::shared_ptr<EventHandlerOhos> ohosHandler = std::make_shared<EventHandlerOhos>(runner);
    std::shared_ptr<RSEventHandler> rsHandler = std::static_pointer_cast<RSEventHandler>(ohosHandler);
    return rsHandler;
}

EventHandlerOhos::EventHandlerOhos(const std::shared_ptr<RSEventRunner>& runner) : RSEventHandler(runner)
{
    if (runner != nullptr) {
        auto ohosRunner = std::static_pointer_cast<EventRunnerOhos>(runner);
        handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(ohosRunner->Get());
    }
}

EventHandlerOhos::~EventHandlerOhos() {}

bool EventHandlerOhos::PostTask(const Callback& callback, Priority priority)
{
    if (handler_ != nullptr) {
        return handler_->PostTask(callback, static_cast<AppExecFwk::EventQueue::Priority>(priority));
    }
    return false;
}

bool EventHandlerOhos::PostTask(const Callback& callback)
{
    if (handler_ != nullptr) {
        return handler_->PostTask(callback);
    }
    return false;
}

void EventHandlerOhos::RemoveAllEvents()
{
    if (handler_ != nullptr) {
        handler_->RemoveAllEvents();
    }
}

std::shared_ptr<OHOS::AppExecFwk::EventHandler> EventHandlerOhos::Get()
{
    return handler_;
}
} // namespace Rosen
} // namespace OHOS
