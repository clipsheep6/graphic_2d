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

#ifndef OHOS_EVENT_RUNNER_OHOS_H
#define OHOS_EVENT_RUNNER_OHOS_H

#include <string>
#include <memory>

#include "platform/common/rs_event_runner.h"
#include "event_runner.h"

namespace OHOS {
namespace Rosen {
class EventRunnerOhos : public RSEventRunner {
public:
    EventRunnerOhos(bool inNewThread);
    virtual ~EventRunnerOhos();
    int Stop() override;
    int Run() override;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> Get();
private:
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // #ifndef OHOS_EVENT_RUNNER_OHOS_H
