/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "include/core/SkMatrix.h"
#include "parcel.h"

#include "common/rs_color.h"
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_draw_cmd_list.h"
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
class RSRenderNode;

struct RSModifierContext {
    RSProperties& property_;
    RSPaintFilterCanvas* canvas_ = nullptr;
};

class RSB_EXPORT RSRenderModifier {
public:
    RSRenderModifier() = default;
    virtual ~RSRenderModifier() = default;

    virtual void Apply(RSModifierContext& context) const = 0;

    virtual PropertyId GetPropertyId() = 0;
    virtual std::shared_ptr<RSRenderPropertyBase> GetProperty() = 0;
    virtual RSModifierType GetType() = 0;
    virtual void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) = 0;

    virtual bool Marshalling(Parcel& parcel) = 0;
    [[nodiscard]] static RSRenderModifier* Unmarshalling(Parcel& parcel);
};

class RSB_EXPORT RSGeometryTransRenderModifier : public RSRenderModifier {
public:
    RSGeometryTransRenderModifier(const std::shared_ptr<RSRenderProperty<SkMatrix>>& property)
        : property_(property ? property : std::make_shared<RSRenderProperty<SkMatrix>>())
    {}
    ~RSGeometryTransRenderModifier() override = default;
    void Apply(RSModifierContext& context) const override;
    void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) override;
    bool Marshalling(Parcel& parcel) override;
    PropertyId GetPropertyId() override
    {
        return property_->GetId();
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty() override
    {
        return property_;
    }

    void SetType(RSModifierType type)
    {
        drawStyle_ = type;
    }

    RSModifierType GetType() override
    {
        return drawStyle_;
    }

protected:
    RSModifierType drawStyle_ = RSModifierType::GEOMETRYTRANS;
    std::shared_ptr<RSRenderProperty<SkMatrix>> property_;
};

class RSB_EXPORT RSDrawCmdListRenderModifier : public RSRenderModifier {
public:
    RSDrawCmdListRenderModifier(const std::shared_ptr<RSRenderProperty<DrawCmdListPtr>>& property)
        : property_(property ? property : std::make_shared<RSRenderProperty<DrawCmdListPtr>>())
    {}
    ~RSDrawCmdListRenderModifier() override = default;
    void Apply(RSModifierContext& context) const override;
    void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) override;
    bool Marshalling(Parcel& parcel) override;

    PropertyId GetPropertyId() override
    {
        return property_->GetId();
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty() override
    {
        return property_;
    }

    RSModifierType GetType() override
    {
        return drawStyle_;
    }
    void SetType(RSModifierType type)
    {
        drawStyle_ = type;
    }

    // functions that are dedicated to driven render [start]
    RectF GetCmdsClipRect() const;
    void ApplyForDrivenContent(RSModifierContext& context) const;
    // functions that are dedicated to driven render [end]
protected:
    RSModifierType drawStyle_ = RSModifierType::EXTENDED;
    std::shared_ptr<RSRenderProperty<DrawCmdListPtr>> property_;
};

class RSAnimatableRenderModifier : public RSRenderModifier {
public:
    RSAnimatableRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : property_(property ? property : std::make_shared<RSRenderPropertyBase>())
    {}

    ~RSAnimatableRenderModifier() override = default;

    PropertyId GetPropertyId() override
    {
        return property_->GetId();
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty() override
    {
        return property_;
    }

protected:
    std::shared_ptr<RSRenderPropertyBase> property_;

    friend class RSRenderPropertyAnimation;
};

class RSGeometryRenderModifier : public RSAnimatableRenderModifier {
public:
    RSGeometryRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : RSAnimatableRenderModifier(property)
    {}

    ~RSGeometryRenderModifier() override = default;
};

class RSBackgroundRenderModifier : public RSAnimatableRenderModifier {
public:
    RSBackgroundRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : RSAnimatableRenderModifier(property)
    {}

    ~RSBackgroundRenderModifier() override = default;
};

class RSContentRenderModifier : public RSAnimatableRenderModifier {
public:
    RSContentRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : RSAnimatableRenderModifier(property)
    {}

    ~RSContentRenderModifier() override = default;
};

class RSForegroundRenderModifier : public RSAnimatableRenderModifier {
public:
    RSForegroundRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : RSAnimatableRenderModifier(property)
    {}

    ~RSForegroundRenderModifier() override = default;
};

class RSOverlayRenderModifier : public RSAnimatableRenderModifier {
public:
    RSOverlayRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : RSAnimatableRenderModifier(property)
    {}

    ~RSOverlayRenderModifier() override = default;
};

class RSAppearanceRenderModifier : public RSAnimatableRenderModifier {
public:
    RSAppearanceRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : RSAnimatableRenderModifier(property)
    {}

    ~RSAppearanceRenderModifier() override = default;
};

class RSB_EXPORT RSEnvForegroundColorRenderModifier : public RSForegroundRenderModifier {
public:
    RSEnvForegroundColorRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : RSForegroundRenderModifier(property)
    {}
    ~RSEnvForegroundColorRenderModifier() override = default;
    void Apply(RSModifierContext& context) const override;
    void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) override;
    bool Marshalling(Parcel& parcel) override;
    RSModifierType GetType() override
    {
        return RSModifierType::ENV_FOREGROUND_COLOR;
    }
};

class RSB_EXPORT RSEnvForegroundColorStrategyRenderModifier : public RSForegroundRenderModifier {
public:
    RSEnvForegroundColorStrategyRenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)
        : RSForegroundRenderModifier(property)
    {}
    ~RSEnvForegroundColorStrategyRenderModifier() override = default;
    void Apply(RSModifierContext& context) const override;
    void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) override;
    bool Marshalling(Parcel& parcel) override;
    RSModifierType GetType() override
    {
        return RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY;
    }
    Color GetInvertBackgroundColor(RSModifierContext& context) const;
    Color CalculateInvertColor(Color backgroundColor) const;
};

// declare RenderModifiers like RSBoundsRenderModifier
#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER)    \
    class RSB_EXPORT RS##MODIFIER_NAME##RenderModifier : public RS##MODIFIER_TIER##RenderModifier { \
    public:                                                                                         \
        RS##MODIFIER_NAME##RenderModifier(const std::shared_ptr<RSRenderPropertyBase>& property)    \
            : RS##MODIFIER_TIER##RenderModifier(property)                                           \
        {}                                                                                          \
        ~RS##MODIFIER_NAME##RenderModifier() override = default;                                    \
        void Apply(RSModifierContext& context) const override;                                      \
        void Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) override;      \
        bool Marshalling(Parcel& parcel) override;                                                  \
        RSModifierType GetType() override                                                           \
        {                                                                                           \
            return RSModifierType::MODIFIER_TYPE;                                                   \
        }                                                                                           \
    };

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER) \
    DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, Add, MODIFIER_TIER)

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_MODIFIER_H
