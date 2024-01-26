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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CUBIC_BEZIER_INTERPOLATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CUBIC_BEZIER_INTERPOLATOR_H

#include "animation/rs_interpolator.h"
#include <cinttypes>
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSCubicBezierInterpolator : public RSInterpolator {
public:
    explicit RSCubicBezierInterpolator(float ctrx1, float ctry1, float ctrx2, float ctry2);
    ~RSCubicBezierInterpolator() override = default;

    float InterpolateImpl(float input) const override;

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSCubicBezierInterpolator* Unmarshalling(Parcel& parcel);

private:
    float GetCubicBezierValue(const float time, const float ctr1, const float ctr2) const;

    int BinarySearch(float key) const;

    constexpr static int MAX_RESOLUTION = 4000;
    constexpr static float SEARCH_STEP = 1.0f / MAX_RESOLUTION;
    constexpr static int THIRD_RDER = 3.0;

    float controllx1_;
    float controlly1_;
    float controllx2_;
    float controlly2_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CUBIC_BEZIER_INTERPOLATOR_H
