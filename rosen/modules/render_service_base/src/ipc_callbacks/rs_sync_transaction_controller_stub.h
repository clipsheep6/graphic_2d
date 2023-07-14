/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_SYNC_TRANSACTION_CONTROLLER_STUB_H
#define ROSEN_RENDER_SERVICE_BASE_RS_SYNC_TRANSACTION_CONTROLLER_STUB_H

#include <iremote_stub.h>
#include <nocopyable.h>

#include "common/rs_macros.h"
#include "ipc_callbacks/rs_isync_transaction_controller.h"
#include "ipc_callbacks/rs_isync_transaction_controller_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSyncTransactionControllerStub : public IRemoteStub<RSISyncTransactionController> {
public:
    RSSyncTransactionControllerStub() = default;
    virtual ~RSSyncTransactionControllerStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    DISALLOW_COPY_AND_MOVE(RSSyncTransactionControllerStub);
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_SYNC_TRANSACTION_CONTROLLER_STUB_H
