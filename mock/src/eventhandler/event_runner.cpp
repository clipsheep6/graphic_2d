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

#include "event_runner.h"

#include <condition_variable>
#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

#include <sys/prctl.h>

#include "event_handler.h"


namespace OHOS {
namespace AppExecFwk {

std::shared_ptr<EventRunner> EventRunner::mainRunner_;

std::shared_ptr<EventRunner> EventRunner::Create(bool inNewThread)
{
    return nullptr;
}

std::shared_ptr<EventRunner> EventRunner::Create(const std::string &threadName)
{
    return nullptr;
}

std::shared_ptr<EventRunner> EventRunner::Current()
{
    return nullptr;
}
EventRunner::~EventRunner()
{
}

std::shared_ptr<EventQueue> EventRunner::GetCurrentEventQueue()
{
    return nullptr;
}

std::shared_ptr<EventRunner> EventRunner::GetMainEventRunner()
{
    return nullptr;
}
}  // namespace AppExecFwk
}  // namespace OHOS
