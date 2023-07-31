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

#include "anim_dynamic_cfg_callback_stub.h"
#include "ipc_callbacks/anim_dynamic_cfg_callback_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
int RSAnimDynamicCfgCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto token = data.ReadInterfaceToken();
    if (token != RSIAnimDynamicCfgCallback::GetDescriptor()) {
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIAnimDynamicCfgCallbackInterfaceCode::GET_ANIM_DYNAMIC_CONFIG): {
            int32_t size = data.ReadInt32();
            AnimDynamicCfgs configs;
            for(int32_t i = 0; i < size; i++) {
                AnimaDynamicConfig config;
                config.SetAnimType(static_cast<AnimType>(data.ReadInt32()));
                config.SetAnimName(data.ReadString());
                config.SetMinSpeed(data.ReadInt32());
                config.SetMaxSpeed(data.ReadInt32());
                config.SetPreferredFps(data.ReadInt32());
                configs.AddAnimaDynamicConfigs(config);
            }
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}
} // namespace Rosen
} // namespace OHOS
