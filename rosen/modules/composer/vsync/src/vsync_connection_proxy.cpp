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

#include "vsync_connection_proxy.h"
#include "graphic_common.h"
#include <hilog/log.h>

namespace OHOS {
namespace Rosen {

VSyncConnectionProxy::VSyncConnectionProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IVSyncConnection>(impl)
{
}

VsyncError VSyncConnectionProxy::RequestNextVSync()
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    arg.WriteInterfaceToken(GetDescriptor());
    int res = Remote()->SendRequest(IVSYNC_CONNECTION_REQUEST_NEXT_VSYNC, arg, ret, opt);
    if (res != NO_ERROR) {
        return VSYNC_ERROR_BINDER_ERROR;
    }
    return VSYNC_ERROR_OK;
}

int32_t VSyncConnectionProxy::GetReceiveFd()
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    arg.WriteInterfaceToken(GetDescriptor());
    int res = Remote()->SendRequest(IVSYNC_CONNECTION_GET_RECEIVE_FD, arg, ret, opt);
    if (res != NO_ERROR) {
        return -1;
    }
    int32_t fd = ret.ReadInt32();
    return fd;
}
} // namespace Vsync
} // namespace OHOS
