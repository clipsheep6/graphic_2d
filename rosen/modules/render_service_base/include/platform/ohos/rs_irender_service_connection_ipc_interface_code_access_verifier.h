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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_ACCESS_VERIFIER_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_ACCESS_VERIFIER_H

#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "platform/ohos/rs_irender_service_connection_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSIRenderServiceConnectionInterfaceCodeAccessVerifier : public RSInterfaceCodeAccessVerifierBase {
public:
    /*
     * specify the enum class of the associated interface code (i.e. CodeEnumType) here
     * note that term **CodeEnumType** should not be changed
     */
    using CodeEnumType = RSIRenderServiceConnectionInterfaceCode;
    static inline const std::string codeEnumTypeName_ {"RSIRenderServiceConnectionInterfaceCode"};

    /* specify constructor and destructor here */
    RSIRenderServiceConnectionInterfaceCodeAccessVerifier();
    ~RSIRenderServiceConnectionInterfaceCodeAccessVerifier() noexcept override = default;
#ifdef ENABLE_IPC_SECURITY
    bool IsAccessTimesVerificationPassed(CodeUnderlyingType code, uint32_t times) const override;
#endif

protected:
    /* specify exclusive verification rules here */
    bool IsExclusiveVerificationPassed(CodeUnderlyingType code) override;

private:
    DISALLOW_COPY_AND_MOVE(RSIRenderServiceConnectionInterfaceCodeAccessVerifier);
#ifdef ENABLE_IPC_SECURITY
    void AddRSIRenderServiceConnectionInterfaceCodePermission();
    static inline const std::vector<std::pair<CodeEnumType, PermissionType>>
        permissionRSIRenderServiceConnectionInterfaceMappings_ {
            { CodeEnumType::TAKE_SURFACE_CAPTURE, PermissionType::CAPTURE_SCREEN },
            { CodeEnumType::GET_SCREEN_CAPABILITY, PermissionType::GET_RUNNING_INFO },
            { CodeEnumType::GET_TOTAL_APP_MEM_SIZE, PermissionType::GET_RUNNING_INFO },
            { CodeEnumType::SET_HARDWARE_ENABLED, PermissionType::CHANGE_ABILITY_ENABLED_STATE },
            { CodeEnumType::SET_FOCUS_APP_INFO, PermissionType::UPDATE_CONFIGURATION },
            { CodeEnumType::SET_SCREEN_REFRESH_RATE, PermissionType::UPDATE_CONFIGURATION },
            { CodeEnumType::SET_REFRESH_RATE_MODE, PermissionType::UPDATE_CONFIGURATION },
            { CodeEnumType::SET_BUFFER_AVAILABLE_LISTENER, PermissionType::RUNNING_STATE_OBSERVER },
            { CodeEnumType::REPORT_JANK_STATS, PermissionType::GET_RUNNING_INFO },
            { CodeEnumType::REPORT_EVENT_RESPONSE, PermissionType::GET_RUNNING_INFO },
            { CodeEnumType::REPORT_EVENT_COMPLETE, PermissionType::GET_RUNNING_INFO },
            { CodeEnumType::REPORT_EVENT_JANK_FRAME, PermissionType::GET_RUNNING_INFO },
            { CodeEnumType::GET_BITMAP, PermissionType::START_ABILITIES_FROM_BACKGROUND },
            { CodeEnumType::SET_BUFFER_CLEAR_LISTENER, PermissionType::RUNNING_STATE_OBSERVER },
            { CodeEnumType::REGISTER_HGM_CFG_CALLBACK, PermissionType::RUNNING_STATE_OBSERVER },
        };
    static inline const std::unordered_map<CodeEnumType, uint32_t>
        accessRSIRenderServiceConnectionInterfaceTimesRestrictions_ {
            { CodeEnumType::REGISTER_APPLICATION_AGENT, 15 },
            { CodeEnumType::CREATE_VSYNC_CONNECTION, 256 },
        };
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_INTERFACE_CODE_ACCESS_VERIFIER_H
