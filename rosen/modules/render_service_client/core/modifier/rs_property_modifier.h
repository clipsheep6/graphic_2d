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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H

#include "modifier/rs_modifier.h"

namespace OHOS {
namespace Rosen {
template<typename propertyType, RSModifierType typeEnum>
class RS_EXPORT RSModifierTemplate : public RSModifier<propertyType> {
public:
    explicit RSModifierTemplate(const std::shared_ptr<propertyType>& property)
        : RSModifier<propertyType>(property, typeEnum)
    {}
    virtual ~RSModifierTemplate() = default;

protected:
    RSModifierType GetModifierType() const
    {
        return typeEnum;
    }

    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const;
};

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_ENUM) \
    using RS##MODIFIER_NAME##Modifier = RSModifierTemplate<RSAnimatableProperty<TYPE>, MODIFIER_ENUM>;
#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_ENUM) \
    using RS##MODIFIER_NAME##Modifier = RSModifierTemplate<RSProperty<TYPE>, MODIFIER_ENUM>;

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H
