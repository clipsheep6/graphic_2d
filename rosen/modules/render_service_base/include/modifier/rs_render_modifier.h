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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_MODIFIER_H

#include <memory>
#include <unordered_map>

#include "parcel.h"

#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "property/rs_properties.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {
class RSProperties;
class RSPaintFilterCanvas;

struct RSModifyContext {
    RSProperties& property_;
    RSPaintFilterCanvas* canvas_ = nullptr;
};

class RSRenderModifier {
public:
    RSRenderModifier() = default;
    virtual ~RSRenderModifier() = default;

    virtual void Apply(RSModifyContext& context) = 0;

    virtual PropertyId GetPropertyId() = 0;
    virtual std::shared_ptr<RSRenderPropertyBase> GetProperty() = 0;
    virtual RSModifierType GetType() = 0;
    virtual void Update(const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta) = 0;

    virtual bool Marshalling(Parcel& parcel) = 0;
    static RSRenderModifier* Unmarshalling(Parcel& parcel);
    virtual void SetIsAdditive(bool isAdditive) = 0;

protected:
    // unmarshalling function Look up table
    using ModifierUnmarshallingFunc = RSRenderModifier* (*)(Parcel& parcel);
    inline static std::unordered_map<RSModifierType, ModifierUnmarshallingFunc> unmarshallingFuncLUT_;

    // Helper class for automatically registry
    template<RSModifierType typeEnum, RSRenderModifier::ModifierUnmarshallingFunc func>
    struct UnmarshallingFuncRegister {
        UnmarshallingFuncRegister()
        {
            RSRenderModifier::unmarshallingFuncLUT_.emplace(typeEnum, func);
        }
    };
};

class RSDrawCmdListRenderModifier : public RSRenderModifier {
public:
    RSDrawCmdListRenderModifier(const std::shared_ptr<RSRenderProperty<DrawCmdListPtr>>& property)
        : property_(property ? property : std::make_shared<RSRenderProperty<DrawCmdListPtr>>())
    {}
    ~RSDrawCmdListRenderModifier() override = default;
    void Apply(RSModifyContext& context) override;
    void Update(const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta) override;
    bool Marshalling(Parcel& parcel) override;

    PropertyId GetPropertyId() override
    {
        return property_->GetId();
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty() override
    {
        return property_;
    }
    static RSRenderModifier* Unmarshalling(Parcel& parcel);

    void SetIsAdditive(bool isAdditive) override {}
    RSModifierType GetType() override
    {
        return drawStyle_;
    }
    void SetType(RSModifierType type)
    {
        drawStyle_ = type;
    }

protected:
    RSModifierType drawStyle_ = RSModifierType::EXTENDED;
    std::shared_ptr<RSRenderProperty<DrawCmdListPtr>> property_;
};

template<typename T>
class RSAnimatableRenderModifier : public RSRenderModifier {
public:
    RSAnimatableRenderModifier(const std::shared_ptr<T>& property)
        : property_(property ? property : std::make_shared<T>())
    {}

    virtual ~RSAnimatableRenderModifier() = default;

    PropertyId GetPropertyId() override
    {
        return property_->GetId();
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty() override
    {
        return property_;
    }

    void SetIsAdditive(bool isAdditive) override
    {
        isAdditive_ = isAdditive;
    }

protected:
    std::shared_ptr<T> property_;
    bool isAdditive_ { false };
    bool isFirstSet_ { true };
    std::shared_ptr<T> lastValue_ = std::make_shared<T>();

    friend class RSRenderPropertyAnimation;
};

template<typename T, RSModifierType typeEnum, auto getter, auto setter>
class RSAnimatableRenderModifierTemplate : public RSAnimatableRenderModifier<RSRenderAnimatableProperty<T>> {
public:
    RSAnimatableRenderModifierTemplate(const std::shared_ptr<RSRenderAnimatableProperty<T>>& property)
        : RSAnimatableRenderModifier<RSRenderAnimatableProperty<T>>(property)
    {}
    ~RSAnimatableRenderModifierTemplate() override = default;

    RSModifierType GetType() override
    {
        return typeEnum;
    }

    bool Marshalling(Parcel& parcel) override;
    static RSRenderModifier* Unmarshalling(Parcel& parcel);
    void Apply(RSModifyContext& context) override;
    void Update(const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta) override;

private:
    inline static RSRenderModifier::UnmarshallingFuncRegister<typeEnum, Unmarshalling> register_;
};

template<typename T, RSModifierType typeEnum, auto setter>
class RSRenderModifierTemplate : public RSAnimatableRenderModifier<RSRenderProperty<T>> {
public:
    RSRenderModifierTemplate(const std::shared_ptr<RSRenderProperty<T>>& property)
        : RSAnimatableRenderModifier<RSRenderProperty<T>>(property)
    {}
    ~RSRenderModifierTemplate() override = default;

    RSModifierType GetType() override
    {
        return typeEnum;
    }

    bool Marshalling(Parcel& parcel) override;
    static RSRenderModifier* Unmarshalling(Parcel& parcel);
    void Apply(RSModifyContext& context) override;
    void Update(const std::shared_ptr<RSRenderPropertyBase>& newProp, bool isDelta) override;

private:
    inline static RSRenderModifier::UnmarshallingFuncRegister<typeEnum, Unmarshalling> register_;
};

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_ENUM)                               \
    using RS##MODIFIER_NAME##RenderModifier = RSAnimatableRenderModifierTemplate<TYPE, MODIFIER_ENUM, \
        &RSProperties::Get##MODIFIER_NAME, &RSProperties::Set##MODIFIER_NAME>;

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_ENUM) \
    using RS##MODIFIER_NAME##RenderModifier =                             \
        RSRenderModifierTemplate<TYPE, MODIFIER_ENUM, &RSProperties::Set##MODIFIER_NAME>;

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_MODIFIER_H
