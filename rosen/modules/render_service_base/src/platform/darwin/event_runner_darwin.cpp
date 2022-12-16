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

#include "platform/darwin/event_runner_darwin.h"

namespace OHOS {
namespace Rosen {
EventRunnerDarwin::EventRunnerDarwin(bool inNewThread) : RSEventRunner(inNewThread)
{}

EventRunnerDarwin::~EventRunnerDarwin()
{}

std::shared_ptr<RSEventRunner> RSEventRunner::Create(bool inNewThread)
{
    std::shared_ptr<EventRunnerDarwin> darwinRunner = std::make_shared<EventRunnerDarwin>(inNewThread);
    std::shared_ptr<RSEventRunner> rsRunner = std::static_pointer_cast<RSEventRunner>(darwinRunner);
    return rsRunner;
}

int EventRunnerDarwin::Stop()
{
    return 0;
}

int EventRunnerDarwin::Run()
{
    return 0;
}
} // namespace Rosen
} // namespace OHOS
