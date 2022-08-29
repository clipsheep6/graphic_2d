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

#include "modifier/rs_property_modifier.h"

#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_modifier.h"

namespace OHOS::Rosen {
#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_ENUM)                                           \
    template<>                                                                                                    \
    std::shared_ptr<RSRenderModifier>                                                                             \
    RSModifierTemplate<RSAnimatableProperty<TYPE>, MODIFIER_ENUM>::CreateRenderModifier() const                   \
    {                                                                                                             \
        auto renderProperty =                                                                                     \
            std::make_shared<RSRenderAnimatableProperty<TYPE>>(this->property_->Get(), this->property_->GetId()); \
        auto renderModifier = std::make_shared<RS##MODIFIER_NAME##RenderModifier>(renderProperty);                \
        renderModifier->SetIsAdditive(this->isAdditive_);                                                         \
        return renderModifier;                                                                                    \
    }
#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_ENUM)                                         \
    template<>                                                                                                    \
    std::shared_ptr<RSRenderModifier> RSModifierTemplate<RSProperty<TYPE>, MODIFIER_ENUM>::CreateRenderModifier() \
        const                                                                                                     \
    {                                                                                                             \
        auto renderProperty =                                                                                     \
            std::make_shared<RSRenderProperty<TYPE>>(this->property_->Get(), this->property_->GetId());           \
        auto renderModifier = std::make_shared<RS##MODIFIER_NAME##RenderModifier>(renderProperty);                \
        return renderModifier;                                                                                    \
    }

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace OHOS::Rosen
