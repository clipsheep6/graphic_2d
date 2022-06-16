/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ACCESSORS_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ACCESSORS_H

#include <unordered_map>

#include "animation/rs_animatable_property.h"
#include "common/rs_macros.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSBasePropertyAccessors {
public:
    RSBasePropertyAccessors() {}
    virtual ~RSBasePropertyAccessors() {}

    static const std::shared_ptr<RSBasePropertyAccessors> GetAccessor(RSAnimatableProperty property);

private:
    const static std::unordered_map<RSAnimatableProperty, std::shared_ptr<RSBasePropertyAccessors>>
        PROPERTY_ACCESSOR_LUT;
};

template<typename T>
class RS_EXPORT RSPropertyAccessors : public RSBasePropertyAccessors {
    typedef void (RSProperties::*SetProperty)(T value);
    typedef T (RSProperties::*GetProperty)() const;

public:
    explicit RSPropertyAccessors(SetProperty setter, GetProperty getter) : setter_(setter), getter_(getter) {};
    ~RSPropertyAccessors() override = default;

    const SetProperty setter_;
    const GetProperty getter_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ACCESSORS_H
