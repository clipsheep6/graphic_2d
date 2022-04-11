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

#include "rs_window_animation_stub.h"

#include <memory>

#include "platform/common/rs_log.h"
#include "rs_window_animation_target.h"

namespace OHOS {
namespace Rosen {

int RSWindowAnimationStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int ret = ERR_NONE;
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ROSEN_LOGE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case RSIWindowAnimationController::ON_TRANSITION: {
            sptr<RSWindowAnimationTarget> from(data.ReadParcelable<RSWindowAnimationTarget>());
            if (from == nullptr) {
                ROSEN_LOGE("Failed to read animation target from!");
                return ERR_INVALID_DATA;
            }

            sptr to(data.ReadParcelable<RSWindowAnimationTarget>());
            if (to == nullptr) {
                ROSEN_LOGE("Failed to read animation target to!");
                return ERR_INVALID_DATA;
            }

            OnTransition(from, to);
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
