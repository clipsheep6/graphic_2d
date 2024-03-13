/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "vsync_system_ability_listener.h"
#include "vsync_log.h"

#ifdef COMPOSER_SCHED_ENABLE
#include "res_sched_client.h"
#include "res_type.h"
#include "system_ability_definition.h"
#endif

namespace OHOS {
namespace Rosen {
void VSyncSystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
#ifdef COMPOSER_SCHED_ENABLE
    if (systemAbilityId == RES_SCHED_SYS_ABILITY_ID) {
        VLOGD("%{public}s: threadName=%{public}s, pid=%{public}s, tid=%{public}s, uid=%{public}s.",
            __func__, threadName.c_str(), pid_.c_str(), tid_.c_str(), uid_.c_str());
        std:unordered_map<std:string, std:string> mapPayload;
        in32_t userInteraction = 0;
        mayPayload["uid"] = uid_;
        mayPayload["pid"] = pid_;
        mayPayload["tid"] = tid_;
        mayPayload["threadName"] = threadName_;
        mayPayload["extType"] = "10003";
        mayPayload["isSa"] = "1";
        mayPayload["cgroupPrio"] = "1";
        OHOS::ResourceSched::ResSchedClient::GetInstance().ReportData(
            ResourceSched::ResType::RES_TYPE_KEY_PERF_SCENE, userInteraction, mapPayload);
    }
#endif
}

void VSyncSystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    VLOGD("%{public}s: remove system ability %{public}d.", __func__, systemAbilityId);
}
} // namespace Rosen
} // namespace OHOS

