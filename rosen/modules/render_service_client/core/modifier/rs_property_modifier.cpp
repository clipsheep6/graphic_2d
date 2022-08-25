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

// #include "modifier/rs_property_modifier.h"

// #include "modifier/rs_modifier_type.h"
// #include "modifier/rs_render_modifier.h"

namespace OHOS {
namespace Rosen {
// template<typename renderModifierType, typename modifierType, RSModifierType typeEnum>
// std::shared_ptr<RSRenderModifier>
// RSModifierTemplate<renderModifierType, modifierType, typeEnum>::CreateRenderModifier() const
// {
//     auto renderProperty =
//         std::make_shared<RSRenderProperty<modifierType>>(this->property_->Get(), this->property_->GetId());
//     auto renderModifier = std::make_shared<renderModifierType>(renderProperty);
//     return renderModifier;
// }

// template<typename renderModifierType, typename modifierType, RSModifierType typeEnum>
// std::shared_ptr<RSRenderModifier>
// RSAnimatableModifierTemplate<renderModifierType, modifierType, typeEnum>::CreateRenderModifier() const
// {
//     auto renderProperty =
//         std::make_shared<RSRenderAnimatableProperty<modifierType>>(this->property_->Get(), this->property_->GetId());
//     auto renderModifier = std::make_shared<renderModifierType>(renderProperty);
//     renderModifier->SetIsAdditive(this->isAdditive_);
//     return renderModifier;
// }

// // explicit instantiation and registration
// #define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE) \
//     template class RSAnimatableModifierTemplate<RS##MODIFIER_NAME##RenderModifier, TYPE, RSModifierType::MODIFIER_TYPE>;

// #define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE) \
//     template class RSModifierTemplate<RS##MODIFIER_NAME##RenderModifier, TYPE, RSModifierType::MODIFIER_TYPE>;

// #include "modifier/rs_modifiers_def.in"

// #undef DECLARE_ANIMATABLE_MODIFIER
// #undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
