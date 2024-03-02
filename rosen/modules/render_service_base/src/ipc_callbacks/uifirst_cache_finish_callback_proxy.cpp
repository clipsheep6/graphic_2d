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

#include "uifirst_cache_finish_callback_proxy.h"
#include <message_option.h>
#include <message_parcel.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSUIFirstCacheFinishCallbackProxy::RSUIFirstCacheFinishCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIUIFirstCacheFinishCallback>(impl)
{
}

void RSUIFirstCacheFinishCallbackProxy::OnCacheFinish()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIUIFirstCacheFinishCallback::GetDescriptor())) {
        ROSEN_LOGE("RSUIFirstCacheFinishCallbackProxy: data.WriteInterfaceToken error");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIUIFirstCacheFinishCallbackInterfaceCode::APP_CACHE_FINISH);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSUIFirstCacheFinishCallbackProxy: Remote()->SendRequest() error");
    }
}
} // namespace Rosen
} // namespace OHOS
