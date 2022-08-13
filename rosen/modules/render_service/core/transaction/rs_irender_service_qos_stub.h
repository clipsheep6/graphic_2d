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

 
#ifndef RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_QOS_STUB_H
#define RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_QOS_STUB_H

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

#include "platform/ohos/rs_irender_service_qos.h"
namespace OHOS {
namespace Rosen {
class RSRenderServiceQosStub : public IRemoteStub<RSIRenderServiceQos> {
public:
    RSRenderServiceQosStub() = default;
    ~RSRenderServiceQosStub() noexcept = default;

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
}
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_QOS_STUB_H