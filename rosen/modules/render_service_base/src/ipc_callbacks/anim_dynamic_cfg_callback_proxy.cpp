/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "anim_dynamic_cfg_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSAnimDynamicCfgCallbackProxy::RSAnimDynamicCfgCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIAnimDynamicCfgCallback>(impl)
{
}

void RSAnimDynamicCfgCallbackProxy::GetAnimDynamicConfigs(AnimDynamicCfgs configs)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if(!data.WriteInterfaceToken(RSIAnimDynamicCfgCallback::GetDescriptor())) {
        return;
    }

    data.WriteInt32(configs.GetAnimaDynamicConfigsCount());
    auto dynamicConfigs = configs.GetAnimaDynamicConfigs();
    for (auto config : dynamicConfigs) {
        data.WriteString(config.GetAnimType());
        data.WriteString(config.GetAnimName());
        data.WriteInt32(config.GetMinSpeed());
        data.WriteInt32(config.GetMaxSpeed());
        data.WriteInt32(config.GetPreferredFps());
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIAnimDynamicCfgCallbackInterfaceCode::GET_ANIM_DYNAMIC_CONFIG);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSAnimDynamicCfgCallbackProxy::GetAnimDynamicConfigs error = %d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
