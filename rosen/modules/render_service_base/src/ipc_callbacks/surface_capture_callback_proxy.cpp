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

#include "surface_capture_callback_proxy.h"
#include <message_option.h>
#include <message_parcel.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSurfaceCaptureCallbackProxy::RSSurfaceCaptureCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSISurfaceCaptureCallback>(impl)
{
}

void RSSurfaceCaptureCallbackProxy::OnSurfaceCapture(NodeId id, Media::PixelMap* pixelmap)
{
    constexpr auto interfaceCode = RSISurfaceCaptureCallbackInterfaceCode::ON_SURFACE_CAPTURE;
    if (!securityManager_.IsInterfaceCodeAccessible(interfaceCode, GetCallerName(__func__))) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSISurfaceCaptureCallback::GetDescriptor())) {
        ROSEN_LOGE("SurfaceCaptureCallbackProxy: data.WriteInterfaceToken error");
        return;
    }
    data.WriteUint64(id);
    data.WriteParcelable(pixelmap);
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(interfaceCode);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("SurfaceCaptureCallbackProxy: Remote()->SendRequest() error");
    }
}

template<size_t N>
std::string RSSurfaceCaptureCallbackProxy::GetCallerName(const char (&callerFuncCstr)[N]) const
{
    const std::string callerFunction{callerFuncCstr};
    std::string callerName{callerPrefix_ + callerFunction};
    return callerName;
}

const RSInterfaceCodeSecurityManager<RSISurfaceCaptureCallbackInterfaceCode> \
    RSSurfaceCaptureCallbackProxy::securityManager_ = CreateRSISurfaceCaptureCallbackInterfaceCodeSecurityManager();
} // namespace Rosen
} // namespace OHOS
