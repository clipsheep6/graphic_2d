/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_SCREEN_MODE_INFO_EXT
#define RS_SCREEN_MODE_INFO_EXT

#include "rs_screen_mode_info.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSScreenModeInfoExt : public RSScreenModeInfo {
public:
    RSScreenModeInfoExt() = default;
    RSScreenModeInfoExt(int32_t width, int32_t height, uint32_t refreshRate, int32_t id, uint32_t groupId);
    RSScreenModeInfoExt(const RSScreenModeInfo& screenModeInfo, uint32_t groupId);
    ~RSScreenModeInfoExt() noexcept = default;

    RSScreenModeInfoExt(const RSScreenModeInfoExt& other);
    RSScreenModeInfoExt& operator=(const RSScreenModeInfoExt& other);

    bool Marshalling(Parcel &parcel) const override;
    static RSScreenModeInfoExt *Unmarshalling(Parcel &parcel);

    uint32_t GetGroupId() const;
    void SetGroupId(uint32_t groupId);

private:
    uint32_t groupId_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_MODE_INFO_EXT