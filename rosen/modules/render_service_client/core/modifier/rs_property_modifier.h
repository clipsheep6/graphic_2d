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

using RSBoundsModifier = RSAnimatableModifierTemplate<RSBoundsRenderModifier, Vector4f, RSModifierType::BOUNDS>;

using RSFrameModifier = RSAnimatableModifierTemplate<RSFrameRenderModifier, Vector4f, RSModifierType::FRAME>;

using RSPositionZModifier = RSAnimatableModifierTemplate<RSPositionZRenderModifier, float, RSModifierType::POSITION_Z>;

using RSPivotModifier = RSAnimatableModifierTemplate<RSPivotRenderModifier, Vector2f, RSModifierType::PIVOT>;

using RSQuaternionModifier =
    RSAnimatableModifierTemplate<RSQuaternionRenderModifier, Quaternion, RSModifierType::QUATERNION>;

using RSRotationModifier = RSAnimatableModifierTemplate<RSRotationRenderModifier, float, RSModifierType::ROTATION>;

using RSRotationXModifier = RSAnimatableModifierTemplate<RSRotationXRenderModifier, float, RSModifierType::ROTATION_X>;

using RSRotationYModifier = RSAnimatableModifierTemplate<RSRotationYRenderModifier, float, RSModifierType::ROTATION_Y>;

using RSScaleModifier = RSAnimatableModifierTemplate<RSScaleRenderModifier, Vector2f, RSModifierType::SCALE>;

using RSTranslateModifier =
    RSAnimatableModifierTemplate<RSTranslateRenderModifier, Vector2f, RSModifierType::TRANSLATE>;

using RSTranslateZModifier =
    RSAnimatableModifierTemplate<RSTranslateZRenderModifier, float, RSModifierType::TRANSLATE_Z>;

using RSCornerRadiusModifier =
    RSAnimatableModifierTemplate<RSCornerRadiusRenderModifier, Vector4f, RSModifierType::CORNER_RADIUS>;

using RSAlphaModifier = RSAnimatableModifierTemplate<RSAlphaRenderModifier, float, RSModifierType::ALPHA>;

using RSAlphaOffscreenModifier =
    RSModifierTemplate<RSAlphaOffscreenRenderModifier, bool, RSModifierType::ALPHA_OFFSCREEN>;

using RSForegroundColorModifier =
    RSAnimatableModifierTemplate<RSForegroundColorRenderModifier, Color, RSModifierType::FOREGROUND_COLOR>;

using RSBackgroundColorModifier =
    RSAnimatableModifierTemplate<RSBackgroundColorRenderModifier, Color, RSModifierType::BACKGROUND_COLOR>;

using RSBackgroundShaderModifier =
    RSModifierTemplate<RSBackgroundShaderRenderModifier, std::shared_ptr<RSShader>, RSModifierType::BACKGROUND_SHADER>;

using RSBgImageModifier =
    RSModifierTemplate<RSBgImageRenderModifier, std::shared_ptr<RSImage>, RSModifierType::BG_IMAGE>;

using RSBgImageWidthModifier =
    RSAnimatableModifierTemplate<RSBgImageWidthRenderModifier, float, RSModifierType::BG_IMAGE_WIDTH>;

using RSBgImageHeightModifier =
    RSAnimatableModifierTemplate<RSBgImageHeightRenderModifier, float, RSModifierType::BG_IMAGE_HEIGHT>;

using RSBgImagePositionXModifier =
    RSAnimatableModifierTemplate<RSBgImagePositionXRenderModifier, float, RSModifierType::BG_IMAGE_POSITION_X>;

using RSBgImagePositionYModifier =
    RSAnimatableModifierTemplate<RSBgImagePositionYRenderModifier, float, RSModifierType::BG_IMAGE_POSITION_Y>;

using RSBorderColorModifier =
    RSAnimatableModifierTemplate<RSBorderColorRenderModifier, Vector4<Color>, RSModifierType::BORDER_COLOR>;

using RSBorderWidthModifier =
    RSAnimatableModifierTemplate<RSBorderWidthRenderModifier, Vector4f, RSModifierType::BORDER_WIDTH>;

using RSBorderStyleModifier =
    RSModifierTemplate<RSBorderStyleRenderModifier, Vector4<uint32_t>, RSModifierType::BORDER_STYLE>;

using RSFilterModifier =
    RSAnimatableModifierTemplate<RSFilterRenderModifier, std::shared_ptr<RSFilter>, RSModifierType::FILTER>;

using RSBackgroundFilterModifier = RSAnimatableModifierTemplate<RSBackgroundFilterRenderModifier,
    std::shared_ptr<RSFilter>, RSModifierType::BACKGROUND_FILTER>;

using RSFrameGravityModifier = RSModifierTemplate<RSFrameGravityRenderModifier, Gravity, RSModifierType::FRAME_GRAVITY>;

using RSClipBoundsModifier =
    RSModifierTemplate<RSClipBoundsRenderModifier, std::shared_ptr<RSPath>, RSModifierType::CLIP_BOUNDS>;

using RSClipToBoundsModifier = RSModifierTemplate<RSClipToBoundsRenderModifier, bool, RSModifierType::CLIP_TO_BOUNDS>;

using RSClipToFrameModifier = RSModifierTemplate<RSClipToFrameRenderModifier, bool, RSModifierType::CLIP_TO_FRAME>;

using RSVisibleModifier = RSModifierTemplate<RSVisibleRenderModifier, bool, RSModifierType::VISIBLE>;

using RSShadowColorModifier =
    RSAnimatableModifierTemplate<RSShadowColorRenderModifier, Color, RSModifierType::SHADOW_COLOR>;

using RSShadowOffsetXModifier =
    RSAnimatableModifierTemplate<RSShadowOffsetXRenderModifier, float, RSModifierType::SHADOW_OFFSET_X>;

using RSShadowOffsetYModifier =
    RSAnimatableModifierTemplate<RSShadowOffsetYRenderModifier, float, RSModifierType::SHADOW_OFFSET_Y>;

using RSShadowAlphaModifier =
    RSAnimatableModifierTemplate<RSShadowAlphaRenderModifier, float, RSModifierType::SHADOW_ALPHA>;

using RSShadowElevationModifier =
    RSAnimatableModifierTemplate<RSShadowElevationRenderModifier, float, RSModifierType::SHADOW_ELEVATION>;

using RSShadowRadiusModifier =
    RSAnimatableModifierTemplate<RSShadowRadiusRenderModifier, float, RSModifierType::SHADOW_RADIUS>;

using RSShadowPathModifier =
    RSModifierTemplate<RSShadowPathRenderModifier, std::shared_ptr<RSPath>, RSModifierType::SHADOW_PATH>;

using RSMaskModifier = RSModifierTemplate<RSMaskRenderModifier, std::shared_ptr<RSMask>, RSModifierType::MASK>;

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_MODIFIER_H
