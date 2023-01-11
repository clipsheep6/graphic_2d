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

#ifndef RENDER_SERVICE_BASE_LINUX_EVENT_RUNNER_LINUX_H
#define RENDER_SERVICE_BASE_LINUX_EVENT_RUNNER_LINUX_H

#include "platform/common/rs_event_runner.h"

namespace OHOS {
namespace Rosen {
class EventRunnerLinux : public RSEventRunner {
public:
    EventRunnerLinux(bool inNewThread);
    virtual ~EventRunnerLinux();
    int Stop() override;
    int Run() override;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // #ifndef RENDER_SERVICE_BASE_LINUX_EVENT_RUNNER_LINUX_H
