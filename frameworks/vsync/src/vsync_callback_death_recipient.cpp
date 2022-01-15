/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "vsync_callback_death_recipient.h"

#include <mutex>

namespace OHOS {
namespace Vsync {
VsyncCallbackDeathRecipient::VsyncCallbackDeathRecipient(VsyncManager *vm)
{
    manager = vm;
}

void VsyncCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> lock(manager->callbacksMutex_);
    for (auto it = manager->callbacks_.begin(); it != manager->callbacks_.end(); it++) {
        if (it->second->AsObject() == remote.promote()) {
            manager->callbacks_.erase(it);
            break;
        }
    }
}
} // namespace Vsync
} // namespace OHOS
