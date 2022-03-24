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

#include "rs_window_animation_proxy.h"

#include <platform/common/rs_log.h>

#include "rs_window_animation_target.h"

namespace OHOS {
namespace Rosen {

const std::string RSWindowAnimationProxy::TAG = "RSWindowAnimationProxy";

RSWindowAnimationProxy::RSWindowAnimationProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIWindowAnimationController>(impl)
{
}

bool RSWindowAnimationProxy::WriteInterfaceToken(MessageParcel& data)
{
    if (!data.WriteInterfaceToken(RSWindowAnimationProxy::GetDescriptor())) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationProxy::TAG, "Failed to write interface token!");
        return false;
    }

    return true;
}

void RSWindowAnimationProxy::OnTransition(const sptr<RSWindowAnimationTarget>& from, const sptr<RSWindowAnimationTarget>& to)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(from.GetRefPtr())) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationProxy::TAG, "Failed to write animation target from!");
        return;
    }

    if (!data.WriteParcelable(to.GetRefPtr())) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationProxy::TAG, "Failed to write animation target to!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationProxy::TAG, "remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_TRANSITION, data, reply, option);
    if (ret != NO_ERROR) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationProxy::TAG, "Failed to send transition request, error code:%d", ret);
    }
}

} // namespace Rosen
} // namespace OHOS
