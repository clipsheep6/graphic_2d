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

#include "rs_render_service_connection_stub.h"

#include <string>
#include <iremote_proxy.h>
#include "ipc_types.h"

#include "vsync_distributer.h"
#include "platform/ohos/rs_irender_service_qos.h"

namespace OHOS {
namespace Rosen {

int RSRenderServiceQosStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int ret = ERR_NONE;
    switch (code) {
        case SET_APP_VSYNC_RATE: {
            auto interfaceToken = data.ReadInterfaceToken();
            if (interfaceToken != RSIRenderService::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            const uint8_t* recvData = data.ReadBuffer(1024);
            size_t pos = 0;
            Bytes bytes(1024, 0);
            memcpy_s(bytes.data(), 1024, recvData, 1024);
            uint32_t pid;
            int rate;
            if (!ReadFromBytes<uint32_t>(bytes, pos, pid) ||
                !ReadFromBytes<int>(bytes, pos, rate)) {
                ret = IPC_STUB_INVALID_DATA_ERR;
                break;
            }
            bool result = SetVSyncRate(pid, rate);
            reply.WriteBool(ret);
            break;
        }
        case GET_APP_VSYNC_RATE: {
            auto interfaceToken = data.ReadInterfaceToken();
            if (interfaceToken != RSIRenderService::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            std::vector<ConnectionInfo> appVsyncCountVec;
            uint64_t now = GetUsec();
            if (!RequestVSyncRate(appVsyncCountVec)) {
                ret = IPC_STUB_INVALID_DATA_ERR;
                break;
            }

            size pos = 0;
            Bytes bytes(1024, 0);
            int connSize = std::count_if(appVsyncCountVec.begin(), appVsyncCountVec.end(),
                [](ConnectionInfo info){return info.name_.find() != std::string::nops;});
            
            if (!WriteToBytes<uint64_t>(bytes, pos, now) ||
                !WriteToBytes<int>(bytes, pos, connSize)) {
                ret = IPC_STUB_INVALID_DATA_ERR;
                break;
            }
            for (auto info : appVsyncCountVec) {
                if (info.name_.find("ACE") == std::string::npos) {
                    continue;
                }
                int pos_ = info.name.find("_");
                uint32_t tmpPid = atoi(info.name_.substr(pos_ + 1).c_str());
                if (!WriteToBytes<uint32_t>(bytes, pos, tmpPid) ||
                    !WriteToBytes<uint64_t>(bytes, pos, info.postVSyncCount_) ||
                    !WriteToBytes<int>(bytes, pos, info.rate)) {
                    ret = IPC_STUB_INVALID_DATA_ERR;
                    break;
                }
            }
            if (!reply.WriteBuffer(bytes.data(), 1024)) {
                ret = IPC_STUB_INVALID_DATA_ERR;
                break;
            }
            break;
        }
        default: {
            ret = ERR_UNKNOWN_TRANSACTION;
            break;
        }
    }

    return ret;
}

} // namespace Rosen
} // namespace OHOS