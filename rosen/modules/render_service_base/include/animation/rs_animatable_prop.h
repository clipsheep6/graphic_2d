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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_PROP_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_PROP_H

#include "animation/rs_animatable_arithmetic.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RSAnimatableProp {
    static_assert(std::is_integral_v(T) || std::is_floating_point_v(T) ||
        std::is_base_of_v(RSAnimatableArithmetic<T>, T));
public:
    RSAnimatableProp() = default;
    virtual ~RSAnimatableProp() = default;

    void Set(const T& value)
    {
        // create animation
        stagingValue_ = value;
    }

    T Get() const
    {
        return stagingValue_;
    }

private:
    void Update(const T& value)
    {
        showingValue_ = value;
    }

    T stagingValue_;
    T showingValue_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_PROP_H
