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

#ifndef OHOS_EVENT_HANDLER_OHOS_H
#define OHOS_EVENT_HANDLER_OHOS_H

#include "platform/common/rs_event_handler.h"
#include "event_handler.h"

namespace OHOS {
namespace Rosen {
class EventHandlerOhos : public RSEventHandler {
public:
    EventHandlerOhos(const std::shared_ptr<RSEventRunner> &runner = nullptr);
    virtual ~EventHandlerOhos();

    bool PostTask(const Callback &callback, Priority priority) override;
    bool PostTask(const Callback &callback) override;
    void RemoveAllEvents() override;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> Get();
private:
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS

#endif  // #ifndef OHOS_EVENT_HANDLER_OHOS_H
