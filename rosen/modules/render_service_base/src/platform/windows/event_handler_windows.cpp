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

#include "platform/windows/event_handler_windows.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSEventHandler> RSEventHandler::Create(const std::shared_ptr<RSEventRunner>& runner)
{
    std::shared_ptr<EventHandlerWindows> windowsHandler = std::make_shared<EventHandlerWindows>(runner);
    std::shared_ptr<RSEventHandler> rsHandler = std::static_pointer_cast<RSEventHandler>(windowsHandler);
    return rsHandler;
}

EventHandlerWindows::EventHandlerWindows(const std::shared_ptr<RSEventRunner>& runner) : RSEventHandler(runner) {}

EventHandlerWindows::~EventHandlerWindows() {}

bool EventHandlerWindows::PostTask(const Callback& callback, Priority priority)
{
    return false;
}

bool EventHandlerWindows::PostTask(const Callback& callback)
{
    return false;
}

void EventHandlerWindows::RemoveAllEvents() {}
} // namespace Rosen
} // namespace OHOS
