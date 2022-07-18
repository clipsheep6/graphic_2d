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

#include "modifier/rs_modifier.h"

#include "modifier/rs_render_modifier.h"
#include "modifier/rs_modifier_type.h"

namespace OHOS {
namespace Rosen {
#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE)                                               \
    RS##MODIFIER_NAME##Modifier::RS##MODIFIER_NAME##Modifier(const std::shared_ptr<RSProperty<TYPE>> property)        \
        : RSAnimatableModifier<TYPE>(property, RSModifierType::MODIFIER_TYPE)                                         \
    {}                                                                                                                \
    RSModifierType RS##MODIFIER_NAME##Modifier::GetModifierType() const                                               \
    {                                                                                                                 \
        return RSModifierType::MODIFIER_TYPE;                                                                         \
    }                                                                                                                 \
    std::shared_ptr<RSRenderModifier> RS##MODIFIER_NAME##Modifier::CreateRenderModifier() const                       \
    {                                                                                                                 \
        auto renderProperty =                                                                                         \
            std::make_shared<RSRenderProperty<TYPE>>(property_->Get(), property_->GetId());                 \
        auto renderModifier =  std::make_shared<RS##MODIFIER_NAME##RenderModifier>(renderProperty);                   \
        renderModifier->SetIsAdditive(isAdditive_);                                                                   \
        return renderModifier;                                                                                        \
    }

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE)                                      \
    DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE)

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
