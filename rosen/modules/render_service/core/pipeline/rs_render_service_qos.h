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

#ifndef RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_QOS_H
#define RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_QOS_H

#include <stdint.h>
#include <vector>

#include "vsync_distributer.h"
#include "platform/ohos/rs_irender_service_qos.h"
#include "transaction/rs_irender_service_qos_stub.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceQos : public RSRenderServiceQosStub {
public:
    RSRenderServiceQos(sptr<VSyncDistributo>& appVSyncDistributor) : appVSyncDistributor_(appVSyncDistributor) {}
    ~RSRenderServiceQos() noexcept {};

    RSRenderServiceQos(const RSRenderServiceQos&) = delete;
    bool Init();

private:
    bool RequestVSyncRate(std::vector<ConnectionInfo>& appVSyncInfoVec) override;
    bool SetVSyncRate(uint32_t pid, int rate) override;
    sptr<VSyncDistributo> appVSyncDistributor_;
}

} // namespace Rosen
} // namespace OHOS



#endif // RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_QOS_H