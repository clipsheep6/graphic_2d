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

#include "vsync_connection_stub.h"
#include <unistd.h>
#include "graphic_common.h"

namespace OHOS {
namespace Rosen {
int32_t VSyncConnectionStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case IVSYNC_CONNECTION_REQUEST_NEXT_VSYNC: {
            RequestNextVSync();
            break;
        }
        case IVSYNC_CONNECTION_GET_RECEIVE_FD: {
            int32_t fd = -1;
            int32_t ret = GetReceiveFd(fd);
            if (ret != VSYNC_ERROR_OK) {
                // check add log
                return ret;
            }
            reply.WriteFileDescriptor(fd);
            break;
        }
        case IVSYNC_CONNECTION_SET_RATE: {
            int32_t rate = data.ReadInt32();
            bool autoTrigger = data.ReadBool();
            int32_t ret = SetVSyncRate(rate, autoTrigger);
            if (ret != VSYNC_ERROR_OK) {
                // check add log
                return ret;
            }
            break;
        }
        case IVSYNC_CONNECTION_GET_PERIOD: {
            int64_t period = 0;
            int32_t ret = GetVSyncPeriod(period);
            if (ret != VSYNC_ERROR_OK) {
                return ret;
            }
            reply.WriteInt64(period);
            break;
        }
        case IVSYNC_CONNECTION_SET_REFRESHRATE: {
            int32_t refreshRate = data.ReadInt32();
            int32_t ret = SetVSyncRefreshRate(refreshRate);
            if (ret != VSYNC_ERROR_OK) {
                return ret;
            }
            break;
        }
        default: {
            // check add log
            return VSYNC_ERROR_INVALID_OPERATING;
        }
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
