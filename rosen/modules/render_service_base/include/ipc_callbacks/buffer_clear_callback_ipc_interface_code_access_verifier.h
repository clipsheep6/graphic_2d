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

#ifndef ROSEN_RENDER_SERVICE_BASE_IBUFFER_CLEAR_CALLBACK_INTERFACE_CODE_ACCESS_VERIFIER_H
#define ROSEN_RENDER_SERVICE_BASE_IBUFFER_CLEAR_CALLBACK_INTERFACE_CODE_ACCESS_VERIFIER_H

#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "ipc_callbacks/buffer_clear_callback_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSIBufferClearCallbackInterfaceCodeAccessVerifier : public RSInterfaceCodeAccessVerifierBase {
public:
    /*
     * specify the enum class of the associated interface code (i.e. CodeEnumType) here
     * note that term **CodeEnumType** should not be changed
     */
    using CodeEnumType = RSIBufferClearCallbackInterfaceCode;
    static inline const std::string codeEnumTypeName_{"RSIBufferClearCallbackInterfaceCode"};

    /* specify constructor and destructor here */
    RSIBufferClearCallbackInterfaceCodeAccessVerifier();
    ~RSIBufferClearCallbackInterfaceCodeAccessVerifier() noexcept override = default;

protected:
    /* specify exclusive verification rules here */
    bool IsExclusiveVerificationPassed(CodeUnderlyingType code) override;
    void AddRSIBufferClearCallbackInterfaceCodePermission();
    bool CheckInterfacePermission(const std::string interfaceName, CodeUnderlyingType code) const;
    
private:
    DISALLOW_COPY_AND_MOVE(RSIBufferClearCallbackInterfaceCodeAccessVerifier);
    static inline const std::vector<std::pair<CodeEnumType, PermissionType>> 
        permissionRSIBufferClearCallbackInterfaceMappings_ {
    };
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_IBUFFER_CLEAR_CALLBACK_INTERFACE_CODE_ACCESS_VERIFIER_H
