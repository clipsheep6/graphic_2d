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

#include "platform/linux/event_runner_linux.h"

namespace OHOS {
namespace Rosen {
EventRunnerLinux::EventRunnerLinux(bool inNewThread) : RSEventRunner(inNewThread) {}

EventRunnerLinux::~EventRunnerLinux() {}

std::shared_ptr<RSEventRunner> RSEventRunner::Create(bool inNewThread)
{
    std::shared_ptr<EventRunnerLinux> linuxRunner = std::make_shared<EventRunnerLinux>(inNewThread);
    std::shared_ptr<RSEventRunner> rsRunner = std::static_pointer_cast<RSEventRunner>(linuxRunner);
    return rsRunner;
}

int EventRunnerLinux::Stop()
{
    return 0;
}

int EventRunnerLinux::Run()
{
    return 0;
}
} // namespace Rosen
} // namespace OHOS
