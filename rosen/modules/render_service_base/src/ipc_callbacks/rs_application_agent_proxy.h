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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_APPLICATION_AGENT_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_RS_APPLICATION_AGENT_PROXY_H

#include <iremote_proxy.h>

#include "ipc_callbacks/iapplication_agent.h"
#include "ipc_security/rs_ipc_interface_code_security_manager_registry.h"

namespace OHOS {
namespace Rosen {
class RSApplicationAgentProxy : public IRemoteProxy<IApplicationAgent> {
public:
    explicit RSApplicationAgentProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSApplicationAgentProxy() noexcept = default;

    void OnTransaction(std::shared_ptr<RSTransactionData> transactionData) override;

private:
    static inline const std::string callerPrefix_{"RSApplicationAgentProxy::"};

    static const RSInterfaceCodeSecurityManager<IApplicationAgentInterfaceCode> securityManager_;

    static inline BrokerDelegator<RSApplicationAgentProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_APPLICATION_AGENT_PROXY_H
