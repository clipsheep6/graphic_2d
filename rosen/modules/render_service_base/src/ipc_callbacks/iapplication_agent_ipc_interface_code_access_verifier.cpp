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

#include "ipc_callbacks/iapplication_agent_ipc_interface_code_access_verifier.h"

namespace OHOS {
namespace Rosen {
IApplicationAgentInterfaceCodeAccessVerifier::IApplicationAgentInterfaceCodeAccessVerifier()
{
    CheckCodeUnderlyingTypeStandardized<CodeEnumType>(codeEnumTypeName_);
    AddIApplicationAgentInterfaceCodePermission();
}

bool IApplicationAgentInterfaceCodeAccessVerifier::IsExclusiveVerificationPassed(CodeUnderlyingType code)
{
    bool hasPermission = true;
    switch (code) {
        case static_cast<CodeUnderlyingType>(CodeEnumType::COMMIT_TRANSACTION): {
            /* to implement access interception */
            hasPermission = CheckInterfacePermission(codeEnumTypeName_ + "::COMMIT_TRANSACTION", code);
            break;
        }
        default: {
            break;
        }
    }
    return hasPermission;
}
void IApplicationAgentInterfaceCodeAccessVerifier::AddIApplicationAgentInterfaceCodePermission()
{
    for (auto& mapping : permissionIApplicationAgentInterfaceMappings_) {
        CodeEnumType interfaceName = mapping.first;
        PermissionType permission = mapping.second;
        std::string newPermission = PermissionEnumToString(permission);
        if (newPermission == "unknown") {
            continue;
        }
        CodeUnderlyingType code = static_cast<CodeUnderlyingType>(interfaceName);
        AddPermission(code, newPermission);
    }
}
bool IApplicationAgentInterfaceCodeAccessVerifier::CheckInterfacePermission(const std::string interfaceName, CodeUnderlyingType code) const
{
    auto permissionVec = GetPermissions(code);
    CheckPermission(interfaceName, permissionVec);
    return true;
}


} // namespace Rosen
} // namespace OHOS
