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

#include "rs_uiextension_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSUIExtensionCallbackProxy::RSUIExtensionCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIUIExtensionCallback>(impl)
{
}

void RSUIExtensionCallbackProxy::OnUIExtension(std::shared_ptr<RSUIExtensionData> uiExtensionData, uint64_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIUIExtensionCallback::GetDescriptor())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    data.WriteParcelable(uiExtensionData.get());
    data.WriteUint64(userId);
    uint32_t code = static_cast<uint32_t>(RSIUIExtensionCallbackInterfaceCode::ON_UIEXTENSION);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSUIExtensionCallbackProxy::OnUIExtension error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
