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
#include "modifier/rs_render_modifier.h"

namespace OHOS {
namespace Rosen {

template<typename renderModifierType, typename modifierType, RSModifierType typeEnum>
class RS_EXPORT RSAnimatableModifierTemplate : public RSModifier<RSAnimatableProperty<modifierType>> {
public:
    explicit RSAnimatableModifierTemplate(const std::shared_ptr<RSAnimatableProperty<modifierType>>& property)
        : RSModifier<RSAnimatableProperty<modifierType>>(property, typeEnum)
    {}
    virtual ~RSAnimatableModifierTemplate() = default;

protected:
    RSModifierType GetModifierType() const
    {
        return typeEnum;
    }

    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const
    {
        auto renderProperty = std::make_shared<RSRenderAnimatableProperty<modifierType>>(
            this->property_->Get(), this->property_->GetId());
        auto renderModifier = std::make_shared<renderModifierType>(renderProperty);
        renderModifier->SetIsAdditive(this->isAdditive_);
        return renderModifier;
    }
};

template<typename renderModifierType, typename modifierType, RSModifierType typeEnum>
class RS_EXPORT RSModifierTemplate : public RSModifier<RSProperty<modifierType>> {
public:
    explicit RSModifierTemplate(const std::shared_ptr<RSProperty<modifierType>>& property)
        : RSModifier<RSProperty<modifierType>>(property, typeEnum)
    {}
    virtual ~RSModifierTemplate() = default;

    RSModifierType GetModifierType() const
    {
        return typeEnum;
    }

    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const
    {
        auto renderProperty =
            std::make_shared<RSRenderProperty<modifierType>>(this->property_->Get(), this->property_->GetId());
        auto renderModifier = std::make_shared<renderModifierType>(renderProperty);
        return renderModifier;
    }
};

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE) \
    using RS##MODIFIER_NAME##Modifier =                                 \
        RSAnimatableModifierTemplate<RS##MODIFIER_NAME##RenderModifier, TYPE, RSModifierType::MODIFIER_TYPE>;

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE) \
    using RS##MODIFIER_NAME##Modifier =                                   \
        RSModifierTemplate<RS##MODIFIER_NAME##RenderModifier, TYPE, RSModifierType::MODIFIER_TYPE>;

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H
