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

#include "platform/linux/event_handler_linux.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSEventHandler> RSEventHandler::Create(const std::shared_ptr<RSEventRunner> &runner)
{
    std::shared_ptr<EventHandlerLinux> linuxHandler = std::make_shared<EventHandlerLinux>(runner);
    std::shared_ptr<RSEventHandler> rsHandler = std::static_pointer_cast<RSEventHandler>(linuxHandler);
    return rsHandler;
}

EventHandlerLinux::EventHandlerLinux(const std::shared_ptr<RSEventRunner> &runner)
    :RSEventHandler(runner)
{}

EventHandlerLinux::~EventHandlerLinux()
{}

bool EventHandlerLinux::PostTask(const Callback &callback, Priority priority)
{
    return false;
}

bool EventHandlerLinux::PostTask(const Callback &callback)
{
    return false;
}

void EventHandlerLinux::RemoveAllEvents()
{}
} // namespace Rosen
} // namespace OHOS
