/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HOS_CAMERA_VPSS_MANAGER_H
#define HOS_CAMERA_VPSS_MANAGER_H

#include "imanager.h"
#include "device_manager_adapter.h"
#include "vpss_controller.h"

namespace OHOS {
namespace Camera {
class VpssManager : public IManager {
public:
    VpssManager();
    explicit VpssManager(ManagerId managerId);
    virtual ~VpssManager();
    RetCode CreateController(ControllerId controllerId, std::string hardwareName);
    std::shared_ptr<IController> GetController(ControllerId controllerId);
    void Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        vpss_->Configure(meta);
    };
    RetCode ConfigVpss()
    {
        return vpss_->ConfigVpss();
    };
    RetCode StartVpss()
    {
        return vpss_->ConfigVpss();
    };
    RetCode StopVpss()
    {
        return vpss_->ConfigVpss();
    };
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
    {
        vpss_->SetAbilityMetaDataTag(abilityMetaDataTag);
    };
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        return vpss_->GetAbilityMetaData(meta);
    };
    bool GetMetaDataFlag()
    {
        return vpss_->GetMetaDataFlag();
    };
    void SetMetaDataFlag(bool metaDataFlag)
    {
        vpss_->SetMetaDataFlag(metaDataFlag);
    };

private:
    std::shared_ptr<VpssController> vpss_ = nullptr;
};
} // namespace Camera
} // namespace OHOS
#endif