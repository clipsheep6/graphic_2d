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

#include "platform/windows/event_runner_windows.h"

namespace OHOS {
namespace Rosen {
EventRunnerWindows::EventRunnerWindows(bool inNewThread) : RSEventRunner(inNewThread) {}

EventRunnerWindows::~EventRunnerWindows() {}

std::shared_ptr<RSEventRunner> RSEventRunner::Create(bool inNewThread)
{
    std::shared_ptr<EventRunnerWindows> windowsRunner = std::make_shared<EventRunnerWindows>(inNewThread);
    std::shared_ptr<RSEventRunner> rsRunner = std::static_pointer_cast<RSEventRunner>(windowsRunner);
    return rsRunner;
}

int EventRunnerWindows::Stop()
{
    return 0;
}

int EventRunnerWindows::Run()
{
    return 0;
}
} // namespace Rosen
} // namespace OHOS
