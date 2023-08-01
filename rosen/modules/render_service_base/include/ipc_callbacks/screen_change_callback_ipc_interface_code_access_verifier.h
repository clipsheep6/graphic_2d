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

#ifndef ROSEN_RENDER_SERVICE_BASE_ISCREEN_CHANGE_CALLBACK_INTERFACE_CODE_ACCESS_VERIFIER_H
#define ROSEN_RENDER_SERVICE_BASE_ISCREEN_CHANGE_CALLBACK_INTERFACE_CODE_ACCESS_VERIFIER_H

#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "ipc_callbacks/screen_change_callback_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSIScreenChangeCallbackInterfaceCodeAccessVerifier : public RSInterfaceCodeAccessVerifierBase {
public:
    /*
     * specify the enum class of the associated interface code (i.e. CodeEnumType) here
     * note that term **CodeEnumType** should not be changed
     */
    using CodeEnumType = RSIScreenChangeCallbackInterfaceCode;
    static inline const std::string codeEnumTypeName_{"RSIScreenChangeCallbackInterfaceCode"};

    /* specify constructor and destructor here */
    RSIScreenChangeCallbackInterfaceCodeAccessVerifier();
    ~RSIScreenChangeCallbackInterfaceCodeAccessVerifier() noexcept override = default;

protected:
    /* specify the initialization of accessMap_ here */
    void InitializeAccessMap() override;

    /* specify exclusive verification rules here */
    bool IsExtraVerificationPassed(CodeUnderlyingType code, const std::string& caller) override;

private:
    RSIScreenChangeCallbackInterfaceCodeAccessVerifier(
        const RSIScreenChangeCallbackInterfaceCodeAccessVerifier&) = delete;
    RSIScreenChangeCallbackInterfaceCodeAccessVerifier& operator=(
        const RSIScreenChangeCallbackInterfaceCodeAccessVerifier&) = delete;
    RSIScreenChangeCallbackInterfaceCodeAccessVerifier(
        RSIScreenChangeCallbackInterfaceCodeAccessVerifier&&) = delete;
    RSIScreenChangeCallbackInterfaceCodeAccessVerifier& operator=(
        RSIScreenChangeCallbackInterfaceCodeAccessVerifier&&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_ISCREEN_CHANGE_CALLBACK_INTERFACE_CODE_ACCESS_VERIFIER_H
