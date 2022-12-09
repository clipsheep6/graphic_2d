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

#ifndef RS_SCREEN_HDR_CAPABILITY
#define RS_SCREEN_HDR_CAPABILITY

#include <cstdint>
#ifdef ROSEN_OHOS
#include <parcel.h>
#endif
#include <vector>

#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
class RSScreenHDRCapability : public Parcelable {
#else
class RSScreenHDRCapability {
#endif
public:
    RSScreenHDRCapability() = default;
    RSScreenHDRCapability(float maxLum, float minLum, float maxAverageLum, const std::vector<ScreenHDRFormat>& formats);
    ~RSScreenHDRCapability() noexcept = default;
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel &parcel) const override;
    static RSScreenHDRCapability *Unmarshalling(Parcel &parcel);
#endif
    float GetMaxLum() const;
    float GetMinLum() const;
    float GetMaxAverageLum() const;
    const std::vector<ScreenHDRFormat>& GetHdrFormats() const;

    void SetMaxLum(float maxLum);
    void SetMinLum(float minLum);
    void SetMaxAverageLum(float maxAverageLum);
    void SetHdrFormats(const std::vector<ScreenHDRFormat>& formats);

private:
#ifdef ROSEN_OHOS
    bool WriteVector(const std::vector<ScreenHDRFormat>& formats, Parcel &parcel) const;
    static bool ReadVector(std::vector<ScreenHDRFormat>& unmarFormats, Parcel &parcel);
#endif
    float maxLum_ = 0.f;
    float minLum_ = 0.f;
    float maxAverageLum_ = 0.f;
    std::vector<ScreenHDRFormat> hdrFormats_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_HDR_CAPABILITY
