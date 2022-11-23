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

#include "event_handler.h"

#include <unistd.h>

using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace AppExecFwk {
static constexpr int DATETIME_STRING_LENGTH = 80;

thread_local std::shared_ptr<EventHandler> EventHandler::currentEventHandler = nullptr;

EventHandler::EventHandler(const std::shared_ptr<EventRunner> &runner) : eventRunner_(runner)
{}

EventHandler::~EventHandler()
{

}

std::shared_ptr<EventHandler> EventHandler::Current()
{
    return currentEventHandler;
}

bool EventHandler::SendEvent(InnerEvent::Pointer &event, int64_t delayTime, Priority priority)
{
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
