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

#include "rs_surface_occlusion_change_callback_stub.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSSurfaceOcclusionChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (!securityManager_.IsInterfaceCodeAccessible(code)) {
        RS_LOGE("RSSurfaceOcclusionChangeCallbackStub::OnRemoteRequest "
                "no permission to access codeID=%{public}u.", code);
        return ERR_INVALID_STATE;
    }

    auto token = data.ReadInterfaceToken();
    if (token != RSISurfaceOcclusionChangeCallback::GetDescriptor()) {
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(
            RSISurfaceOcclusionChangeCallbackInterfaceCode::ON_SURFACE_OCCLUSION_VISIBLE_CHANGED): {
            float visibleAreaRatio = data.ReadFloat();
            OnSurfaceOcclusionVisibleChanged(visibleAreaRatio);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}

const RSInterfaceCodeSecurityManager RSSurfaceOcclusionChangeCallbackStub::securityManager_ = \
    RSInterfaceCodeSecurityManager::CreateInstance<RSISurfaceOcclusionChangeCallbackInterfaceCodeAccessVerifier>();
} // namespace Rosen
} // namespace OHOS
