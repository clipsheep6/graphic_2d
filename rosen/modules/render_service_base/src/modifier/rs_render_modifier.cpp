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

#include "modifier/rs_render_modifier.h"

#include <memory>
#include <unordered_map>

#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
void RSDrawCmdListRenderModifier::Apply(RSModifyContext& context)
{
    if (context.canvas_) {
        auto cmds = property_->Get();
        RSPropertiesPainter::DrawFrame(context.property_, *context.canvas_, cmds);
    }
}

void RSDrawCmdListRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta)
{
    if (auto newProperty = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(newProp)) {
        property_->Set(newProperty->Get());
    }
}

bool RSDrawCmdListRenderModifier::Marshalling(Parcel& parcel)
{
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::EXTENDED)) &&
           RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType()));
}

RSRenderModifier* RSDrawCmdListRenderModifier::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<RSRenderProperty<std::shared_ptr<DrawCmdList>>> prop;
    int16_t type;
    if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type)) {
        return nullptr;
    }
    auto modifier = new RSDrawCmdListRenderModifier(prop);
    modifier->SetType(static_cast<RSModifierType>(type));
    return modifier;
}

RSRenderModifier* RSRenderModifier::Unmarshalling(Parcel& parcel)
{
    int16_t type = 0;
    if (!parcel.ReadInt16(type)) {
        return nullptr;
    }
    auto it = unmarshallingFuncLUT_.find(static_cast<RSModifierType>(type));
    if (it == unmarshallingFuncLUT_.end()) {
        ROSEN_LOGE("RSRenderModifier Unmarshalling cannot find func in lut %d", type);
        return nullptr;
    }
    return it->second(parcel);
}

template<typename T, RSModifierType typeEnum, auto getter, auto setter>
bool RSAnimatableRenderModifierTemplate<T, typeEnum, getter, setter>::Marshalling(Parcel& parcel)
{
    return parcel.WriteInt16(static_cast<int16_t>(typeEnum)) &&
           RSMarshallingHelper::Marshalling(parcel, this->property_) && parcel.WriteBool(this->isAdditive_);
}

template<typename T, RSModifierType typeEnum, auto getter, auto setter>
RSRenderModifier* RSAnimatableRenderModifierTemplate<T, typeEnum, getter, setter>::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<RSRenderAnimatableProperty<T>> prop;
    if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
        return nullptr;
    }
    auto modifier = new RSAnimatableRenderModifierTemplate(prop);
    if (!modifier) {
        return nullptr;
    }
    bool isAdditive = false;
    if (!parcel.ReadBool(isAdditive)) {
        return nullptr;
    }
    modifier->SetIsAdditive(isAdditive);
    return modifier;
}

template<typename T, RSModifierType typeEnum, auto getter, auto setter>
void RSAnimatableRenderModifierTemplate<T, typeEnum, getter, setter>::Apply(RSModifyContext& context)
{
    T setValue;
    if (this->isFirstSet_) {
        setValue = (context.property_.*getter)() + this->property_->Get();
        this->isFirstSet_ = false;
    } else {
        setValue = (context.property_.*getter)() + this->property_->Get() - this->lastValue_->Get();
    }
    setValue = this->isAdditive_ ? setValue : this->property_->Get();
    this->lastValue_->Set(this->property_->Get());
    (context.property_.*setter)(setValue);
}

template<typename T, RSModifierType typeEnum, auto getter, auto setter>
void RSAnimatableRenderModifierTemplate<T, typeEnum, getter, setter>::Update(
    const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta)
{
    if (auto newProperty = std::static_pointer_cast<RSRenderAnimatableProperty<T>>(newProp)) {
        if (isDelta) {
            this->property_->Set(this->property_->Get() + newProperty->Get());
        } else {
            this->property_->Set(newProperty->Get());
        }
    }
}

template<typename T, RSModifierType typeEnum, auto setter>
bool RSRenderModifierTemplate<T, typeEnum, setter>::Marshalling(Parcel& parcel)
{
    return parcel.WriteInt16(static_cast<int16_t>(typeEnum)) &&
           RSMarshallingHelper::Marshalling(parcel, this->property_) && parcel.WriteBool(this->isAdditive_);
}

template<typename T, RSModifierType typeEnum, auto setter>
RSRenderModifier* RSRenderModifierTemplate<T, typeEnum, setter>::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<RSRenderProperty<T>> prop;
    if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
        return nullptr;
    }
    auto modifier = new RSRenderModifierTemplate(prop);
    if (!modifier) {
        return nullptr;
    }
    bool isAdditive = false;
    if (!parcel.ReadBool(isAdditive)) {
        return nullptr;
    }
    modifier->SetIsAdditive(isAdditive);
    return modifier;
}

template<typename T, RSModifierType typeEnum, auto setter>
void RSRenderModifierTemplate<T, typeEnum, setter>::Apply(RSModifyContext& context)
{
    (context.property_.*setter)(this->property_->Get());
}

template<typename T, RSModifierType typeEnum, auto setter>
void RSRenderModifierTemplate<T, typeEnum, setter>::Update(
    const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta)
{
    if (auto newProperty = std::static_pointer_cast<RSRenderProperty<T>>(newProp)) {
        this->property_->Set(newProperty->Get());
    }
}

// explicit instantiation and registration
#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE)                    \
    template class RSAnimatableRenderModifierTemplate<TYPE, RSModifierType::MODIFIER_TYPE, \
        &RSProperties::Get##MODIFIER_NAME, &RSProperties::Set##MODIFIER_NAME>;

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE) \
    template class RSRenderModifierTemplate<TYPE, RSModifierType::MODIFIER_TYPE, &RSProperties::Set##MODIFIER_NAME>;

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
