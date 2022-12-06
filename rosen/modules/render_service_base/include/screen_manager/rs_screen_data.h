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

#ifndef RS_SCREEN_DATA
#define RS_SCREEN_DATA

#include <cstdint>
#ifdef ROSEN_OHOS
#include <parcel.h>
#include <refbase.h>
#endif
#include <string>
#include <vector>

#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/rs_screen_props.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
class RSScreenData : public Parcelable {
#else
class RSScreenData {
#endif
public:
    RSScreenData() = default;
    RSScreenData(RSScreenCapability capability, RSScreenModeInfo activityModeInfo,
        const std::vector<RSScreenModeInfo>& supportModeInfo, ScreenPowerStatus powerStatus);
    ~RSScreenData() = default;
#ifdef ROSEN_OHOS
    static RSScreenData* Unmarshalling(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
#endif
    void SetCapability(const RSScreenCapability& capability);
    void SetActivityModeInfo(const RSScreenModeInfo& activityModeInfo);
    void SetSupportModeInfo(const std::vector<RSScreenModeInfo>& supportModeInfo);
    void SetPowerStatus(ScreenPowerStatus powerStatus);

    RSScreenCapability GetCapability() const;
    RSScreenModeInfo GetActivityModeInfo() const;
    const std::vector<RSScreenModeInfo>& GetSupportModeInfo() const;
    ScreenPowerStatus GetPowerStatus() const;

private:
#ifdef ROSEN_OHOS
    bool WriteVector(const std::vector<RSScreenModeInfo> &supportModes, Parcel &parcel) const;
    static bool ReadVector(std::vector<RSScreenModeInfo> &unmarsupportModes, uint32_t unmarModeCount, Parcel &parcel);
#endif
    RSScreenCapability capability_;
    RSScreenModeInfo activityModeInfo_;
    std::vector<RSScreenModeInfo> supportModeInfo_;
    ScreenPowerStatus powerStatus_ = INVALID_POWER_STATUS;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_DATA