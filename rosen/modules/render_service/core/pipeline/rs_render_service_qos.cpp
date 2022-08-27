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

 #include "platform/ohos/rs_render_service_qos.h"

 #include <stdint>
 #include <iservice_registry.h>
 #include <system_ability_definition.h>

 #include "vsync_distributer.h"

namespace OHOS {
namespace Rosen {

bool RSRenderServiceQos::Init()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        ROSEN_LOGE("RSRenderServiceQos GetSystemAbilityManager fail");
        return false;
    }
    samgr->AddSystemAbility(RENDER_SERVICE_QOS, this);
    return true;
}

bool RSRenderServiceQos::RequestVSyncRate(std::vector<ConnectionInfo>& appVSyncInfoVec)
{
    return appVSyncDistributor_->GetVSyncConnectionInfos(appVSyncInfoVec) == VSYNC_ERROR_OK;
}

bool RSRenderServiceQos::SetVSyncRate(uint32_t pid, int rate)
{
    return appVSyncDistributor_->SetVSyncRate(pid, rate);
}

} // namespace Rosen
} // namespace OHOS