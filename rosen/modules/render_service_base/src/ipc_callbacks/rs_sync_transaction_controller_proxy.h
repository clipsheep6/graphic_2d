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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_SYNC_TRANSACTION_CONTROLLER_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_RS_SYNC_TRANSACTION_CONTROLLER_PROXY_H

#include <iremote_proxy.h>
#include <string>

#include "common/rs_macros.h"
#include "ipc_callbacks/rs_isync_transaction_controller.h"
#include "ipc_callbacks/rs_isync_transaction_controller_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSyncTransactionControllerProxy : public IRemoteProxy<RSISyncTransactionController> {
public:
    explicit RSSyncTransactionControllerProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSSyncTransactionControllerProxy() =default;

    void CreateTransactionFinished() override;

private:
    bool WriteInterfaceToken(MessageParcel& data);
    static inline BrokerDelegator<RSSyncTransactionControllerProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_SYNC_TRANSACTION_CONTROLLER_PROXY_H
