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

#ifndef DARWIN_EVENT_RUNNER_DARWIN_H
#define DARWIN_EVENT_RUNNER_DARWIN_H

#include <string>
#include <memory>

#include "platform/common/rs_event_runner.h"

namespace OHOS {
namespace Rosen {
class EventRunnerDarwin : public RSEventRunner {
public:
    EventRunnerDarwin(bool inNewThread);
    virtual ~EventRunnerDarwin();
    int Stop() override;
    int Run() override;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // #ifndef DARWIN_EVENT_RUNNER_DARWIN_H
