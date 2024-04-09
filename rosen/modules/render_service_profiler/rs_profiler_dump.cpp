/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_profiler.h"
#include "rs_profiler_network.h"

#include "common/rs_obj_geometry.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "nlohmann/json.hpp"

#include "foundation/graphic/graphic_2d/utils/log/rs_trace.h"

#include "pipeline/rs_root_render_node.h"

namespace OHOS::Rosen {

void RSProfiler::DumpNode(const RSRenderNode& node, nlohmann::json& out)
{
    std::string type;
    node.DumpNodeType(type);
    out["type"] = type;
    out["id"] = node.GetId();
    out["instanceRootNodeId"] = node.GetInstanceRootNodeId();
    if (node.IsSuggestedDrawInGroup()) {
        out["nodeGroup"] = true;
    }
    DumpSubClassNode(node, out["subclass"]);
    DumpProperties(node.GetRenderProperties(), out["Properties"]);
    out["GetBootAnimation"] = std::to_string(node.GetBootAnimation());
    out["isContainBootAnimation_"] = std::to_string(node.isContainBootAnimation_);
    out["isNodeDirty"] = std::to_string(static_cast<int>(node.dirtyStatus_));
    out["isPropertyDirty"] = std::to_string(node.GetRenderProperties().IsDirty());
    out["IsPureContainer"] = std::to_string(node.IsPureContainer());
    DumpDrawCmdModifiers(node, out);
    DumpAnimations(node.animationManager_, out);

    for (auto& child : *node.GetSortedChildren()) {
        nlohmann::json childJson;
        DumpNode(*child, childJson);
        out["children"].push_back(std::move(childJson));
    }
    for (auto& [child, pos] : node.disappearingChildren_) {
        nlohmann::json childJson;
        DumpNode(*child, childJson);
        out["children"].push_back(std::move(childJson));
    }
}

void RSProfiler::DumpSubClassNode(const RSRenderNode& node, nlohmann::json& out)
{
    if (node.GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto surfaceNode = (static_cast<const RSSurfaceRenderNode*>(&node));
        auto p = node.parent_.lock();
        out["Parent"] = p ? p->GetId() : 0;
        out["Name"] = surfaceNode->GetName();
        const auto& surfaceHandler = static_cast<const RSSurfaceHandler&>(*surfaceNode);
        out["hasConsumer"] = surfaceHandler.HasConsumer();
        std::string contextAlpha = std::to_string(surfaceNode->contextAlpha_);
        std::string propertyAlpha = std::to_string(surfaceNode->GetRenderProperties().GetAlpha());
        out["Alpha"] = propertyAlpha + " (include ContextAlpha: " + contextAlpha + ")";
        out["Visible"] = std::to_string(surfaceNode->GetRenderProperties().GetVisible()) + " " +
                         surfaceNode->GetVisibleRegion().GetRegionInfo();
        out["Opaque"] = surfaceNode->GetOpaqueRegion().GetRegionInfo();
        out["OcclusionBg"] = std::to_string(surfaceNode->GetAbilityBgAlpha());
        out["SecurityLayer"] = std::to_string(surfaceNode->GetSecurityLayer());
        out["skipLayer"] = std::to_string(surfaceNode->GetSkipLayer());
    } else if (node.GetType() == RSRenderNodeType::ROOT_NODE) {
        auto rootNode = static_cast<const RSRootRenderNode*>(&node);
        out["Visible"] = std::to_string(rootNode->GetRenderProperties().GetVisible());
        out["Size"] = std::to_string(rootNode->GetRenderProperties().GetFrameWidth()) + ", " +
                      std::to_string(rootNode->GetRenderProperties().GetFrameHeight()) + "]";
        out["EnableRender"] = std::to_string(rootNode->GetEnableRender());
    } else if (node.GetType() == RSRenderNodeType::DISPLAY_NODE) {
        auto displayNode = static_cast<const RSDisplayRenderNode*>(&node);
        out["skipLayer"] = std::to_string(displayNode->GetSecurityDisplay());
    }
}

void RSProfiler::DumpDrawCmdModifiers(const RSRenderNode& node, nlohmann::json& out)
{
    nlohmann::json& modifiersJson = out["DrawCmdModifiers"];

    for (auto& [type, modifiers] : node.renderContent_->drawCmdModifiers_) {
        nlohmann::json modifierDesc;
        modifierDesc["type"] = std::to_string(static_cast<int>(type));
        modifierDesc["modifiers"] = nlohmann::json::array();
        for (auto& modifier : modifiers) {
            DumpDrawCmdModifier(node, modifierDesc["modifiers"], static_cast<int>(type), *modifier);
        }
        modifiersJson.emplace_back(std::move(modifierDesc));
    }
}

void RSProfiler::DumpDrawCmdModifier(
    const RSRenderNode& node, nlohmann::json& out, int type, RSRenderModifier& modifier)
{
    nlohmann::json property;
    auto modType = static_cast<RSModifierType>(type);

    if (modType < RSModifierType::ENV_FOREGROUND_COLOR) {
        auto propertyValue =
            std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier.GetProperty())->Get();
        auto propertyStr = propertyValue->GetOpsWithDesc();
        std::vector<std::string> properties;
        size_t pos = 0;
        size_t oldpos = 0;

        while ((pos = propertyStr.find('\n', oldpos)) != std::string::npos) {
            properties.emplace_back(propertyStr.substr(oldpos, pos - oldpos));
            oldpos = pos + 1;
        }

        property["drawCmdList"] = properties;
    } else if (modType == RSModifierType::ENV_FOREGROUND_COLOR) {
        auto propertyValue = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(modifier.GetProperty())->Get();
        property["ENV_FOREGROUND_COLOR"] = std::to_string(propertyValue.AsRgbaInt());
    } else if (modType == RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY) {
        auto propertyValue =
            std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(modifier.GetProperty())->Get();
        property["ENV_FOREGROUND_COLOR_STRATEGY"] = std::to_string(static_cast<int>(propertyValue));
    } else if (modType == RSModifierType::GEOMETRYTRANS) {
        auto propertyValue = std::static_pointer_cast<RSRenderProperty<SkMatrix>>(modifier.GetProperty())->Get();
        std::string str;
        propertyValue.dump(str, 0);
        property["GEOMETRYTRANS"] = str;
    }

    if (!property.empty()) {
        out.push_back(std::move(property));
    }
}

static std::string Hex(uint32_t value)
{
    std::stringstream sstream;
    sstream << std::hex << value;
    return sstream.str();
}

void RSProfiler::DumpProperties(const RSProperties& p, nlohmann::json& out)
{
    out["Bounds"] = { p.GetBoundsPositionX(), p.GetBoundsPositionY(), p.GetBoundsWidth(), p.GetBoundsHeight() };
    out["Frame"] = { p.GetFramePositionX(), p.GetFramePositionY(), p.GetFrameWidth(), p.GetFrameHeight() };

    if (!p.GetVisible()) {
        out["IsVisible"] = false;
    }

    DumpPropertiesTransform(p, out);
    DumpPropertiesDecoration(p, out);
    DumpPropertiesEffects(p, out);
    DumpPropertiesShadow(p, out);
    DumpPropertiesColor(p, out);
}

void RSProfiler::DumpPropertiesTransform(const RSProperties& p, nlohmann::json& out)
{
    std::unique_ptr<Transform> defaultTrans = std::make_unique<Transform>();
    Vector2f pivot = p.GetPivot();
    if ((!ROSEN_EQ(pivot[0], defaultTrans->pivotX_) || !ROSEN_EQ(pivot[1], defaultTrans->pivotY_))) {
        out["Pivot"] = { pivot[0], pivot[1] };
    }

    if (!ROSEN_EQ(p.GetRotation(), defaultTrans->rotation_)) {
        out["Rotation"] = p.GetRotation();
    }

    if (!ROSEN_EQ(p.GetRotationX(), defaultTrans->rotationX_)) {
        out["RotationX"] = p.GetRotationX();
    }

    if (!ROSEN_EQ(p.GetRotationY(), defaultTrans->rotationY_)) {
        out["RotationY"] = p.GetRotationY();
    }

    if (!ROSEN_EQ(p.GetTranslateX(), defaultTrans->translateX_)) {
        out["TranslateX"] = p.GetTranslateX();
    }

    if (!ROSEN_EQ(p.GetTranslateY(), defaultTrans->translateY_)) {
        out["TranslateY"] = p.GetTranslateY();
    }

    if (!ROSEN_EQ(p.GetTranslateZ(), defaultTrans->translateZ_)) {
        out["TranslateZ"] = p.GetTranslateZ();
    }

    if (!ROSEN_EQ(p.GetScaleX(), defaultTrans->scaleX_)) {
        out["ScaleX"] = p.GetScaleX();
    }

    if (!ROSEN_EQ(p.GetScaleY(), defaultTrans->scaleY_)) {
        out["ScaleY"] = p.GetScaleY();
    }

    if (!ROSEN_EQ(p.GetPositionZ(), 0.f)) {
        out["PositionZ"] = p.GetPositionZ();
    }
}

void RSProfiler::DumpPropertiesDecoration(const RSProperties& p, nlohmann::json& out)
{
    if (!p.GetCornerRadius().IsZero()) {
        out["CornerRadius"] = { p.GetCornerRadius().x_, p.GetCornerRadius().y_, p.GetCornerRadius().z_,
            p.GetCornerRadius().w_ };
    }

    if (p.pixelStretch_.has_value()) {
        out["PixelStretch"]["left"] = p.pixelStretch_->z_;
        out["PixelStretch"]["top"] = p.pixelStretch_->y_;
        out["PixelStretch"]["right"] = p.pixelStretch_->z_;
        out["PixelStretch"]["bottom"] = p.pixelStretch_->w_;
    }

    if (!ROSEN_EQ(p.GetAlpha(), 1.f)) {
        out["Alpha"] = p.GetAlpha();
    }

    if (!ROSEN_EQ(p.GetSpherize(), 0.f)) {
        out["Spherize"] = p.GetSpherize();
    }

    if (!ROSEN_EQ(p.GetForegroundColor(), RgbPalette::Transparent())) {
        out["ForegroundColor"] = "#" + Hex(p.GetForegroundColor().AsArgbInt());
    }

    if (!ROSEN_EQ(p.GetBackgroundColor(), RgbPalette::Transparent())) {
        out["BackgroundColor"] = "#" + Hex(p.GetBackgroundColor().AsArgbInt());
    }

    std::unique_ptr<Decoration> defaultDecoration = std::make_unique<Decoration>();
    if ((!ROSEN_EQ(p.GetBgImagePositionX(), defaultDecoration->bgImageRect_.left_) ||
        !ROSEN_EQ(p.GetBgImagePositionY(), defaultDecoration->bgImageRect_.top_) ||
        !ROSEN_EQ(p.GetBgImageWidth(), defaultDecoration->bgImageRect_.width_) ||
        !ROSEN_EQ(p.GetBgImageHeight(), defaultDecoration->bgImageRect_.height_))) {
        out["BgImage"] = { p.GetBgImagePositionX(), p.GetBgImagePositionY(), p.GetBgImageWidth(),
            p.GetBgImageHeight() };
    }
}

void RSProfiler::DumpPropertiesShadow(const RSProperties& p, nlohmann::json& out)
{
    if (!ROSEN_EQ(p.GetShadowColor(), Color(DEFAULT_SPOT_COLOR))) {
        out["ShadowColor"] = "#" + Hex(p.GetShadowColor().AsArgbInt());
    }

    if (!ROSEN_EQ(p.GetShadowOffsetX(), DEFAULT_SHADOW_OFFSET_X)) {
        out["ShadowOffsetX"] = p.GetShadowOffsetX();
    }

    if (!ROSEN_EQ(p.GetShadowOffsetY(), DEFAULT_SHADOW_OFFSET_Y)) {
        out["ShadowOffsetY"] = p.GetShadowOffsetY();
    }

    if (!ROSEN_EQ(p.GetShadowAlpha(), 0.f)) {
        out["ShadowAlpha"] = p.GetShadowAlpha();
    }

    if (!ROSEN_EQ(p.GetShadowElevation(), 0.f)) {
        out["ShadowElevation"] = p.GetShadowElevation();
    }

    if (!ROSEN_EQ(p.GetShadowRadius(), 0.f)) {
        out["ShadowRadius"] = p.GetShadowRadius();
    }

    if (!ROSEN_EQ(p.GetShadowIsFilled(), false)) {
        out["ShadowIsFilled"] = p.GetShadowIsFilled();
    }
}

void RSProfiler::DumpPropertiesEffects(const RSProperties& p, nlohmann::json& out)
{
    if (p.border_ && p.border_->HasBorder()) {
        out["Border"] = p.border_->ToString();
    }

    auto filter = p.GetFilter();
    if (filter && filter->IsValid()) {
        out["Filter"] = filter->GetDescription();
    }

    auto backgroundFilter = p.GetBackgroundFilter();
    if (backgroundFilter && backgroundFilter->IsValid()) {
        out["BackgroundFilter"] = backgroundFilter->GetDescription();
    }

    if (p.outline_ && p.outline_->HasBorder()) {
        out["Outline"] = p.outline_->ToString();
    }

    if (!ROSEN_EQ(p.GetFrameGravity(), Gravity::DEFAULT)) {
        out["FrameGravity"] = p.GetFrameGravity();
    }

    if (p.GetUseEffect()) {
        out["GetUseEffect"] = true;
    }

    auto grayScale = p.GetGrayScale();
    if (grayScale.has_value() && !ROSEN_EQ(*grayScale, 0.f)) {
        out["GrayScale"] = *grayScale;
    }

    if (!ROSEN_EQ(p.GetLightUpEffect(), 1.f)) {
        out["LightUpEffect"] = p.GetLightUpEffect();
    }

    auto dynamicLightUpRate = p.GetDynamicLightUpRate();
    if (dynamicLightUpRate.has_value() && !ROSEN_EQ(*dynamicLightUpRate, 0.f)) {
        out["DynamicLightUpRate"] = *dynamicLightUpRate;
    }

    auto dynamicLightUpDegree = p.GetDynamicLightUpDegree();
    if (dynamicLightUpDegree.has_value() && !ROSEN_EQ(*dynamicLightUpDegree, 0.f)) {
        out["DynamicLightUpDegree"] = *dynamicLightUpDegree;
    }
}

void RSProfiler::DumpPropertiesColor(const RSProperties& p, nlohmann::json& out)
{
    auto brightness = p.GetBrightness();
    if (brightness.has_value() && !ROSEN_EQ(*brightness, 1.f)) {
        out["Brightness"] = *brightness;
    }

    auto contrast = p.GetContrast();
    if (contrast.has_value() && !ROSEN_EQ(*contrast, 1.f)) {
        out["Contrast"] = *contrast;
    }

    auto saturate = p.GetSaturate();
    if (saturate.has_value() && !ROSEN_EQ(*saturate, 1.f)) {
        out["Saturate"] = *saturate;
    }

    auto sepia = p.GetSepia();
    if (sepia.has_value() && !ROSEN_EQ(*sepia, 0.f)) {
        out["Sepia"] = *sepia;
    }

    auto invert = p.GetInvert();
    if (invert.has_value() && !ROSEN_EQ(*invert, 0.f)) {
        out["Invert"] = *invert;
    }

    auto hueRotate = p.GetHueRotate();
    if (hueRotate.has_value() && !ROSEN_EQ(*hueRotate, 0.f)) {
        out["HueRotate"] = *hueRotate;
    }

    auto colorBlend = p.GetColorBlend();
    if (colorBlend.has_value() && !ROSEN_EQ(*colorBlend, RgbPalette::Transparent())) {
        out["ColorBlend"] = "#" + Hex(colorBlend->AsArgbInt());
    }

    if (!ROSEN_EQ(p.GetColorBlendMode(), 0)) {
        out["skblendmode"] = p.GetColorBlendMode() - 1;
        out["blendType"] = p.GetColorBlendApplyType();
    }
}

void RSProfiler::DumpAnimations(const RSAnimationManager& animationManager, nlohmann::json& out)
{
    if (animationManager.animations_.empty()) {
        return;
    }
    for (auto [id, animation] : animationManager.animations_) {
        if (!animation) {
            continue;
        }
        DumpAnimation(*animation, out["RSAnimationManager"]);
    }
}

void RSProfiler::DumpAnimation(const RSRenderAnimation& animation, nlohmann::json& out)
{
    nlohmann::json obj;
    obj["id"] = animation.id_;
    obj["type"] = "unknown";
    obj["AnimationState"] = std::to_string(static_cast<int>(animation.state_));
    obj["StartDelay"] = std::to_string(animation.animationFraction_.GetDuration());
    obj["Duration"] = std::to_string(animation.animationFraction_.GetStartDelay());
    obj["Speed"] = std::to_string(animation.animationFraction_.GetSpeed());
    obj["RepeatCount"] = std::to_string(animation.animationFraction_.GetRepeatCount());
    obj["AutoReverse"] = std::to_string(animation.animationFraction_.GetAutoReverse());
    obj["Direction"] = std::to_string(animation.animationFraction_.GetDirection());
    obj["FillMode"] = std::to_string(static_cast<int>(animation.animationFraction_.GetFillMode()));
    obj["RepeatCallbackEnable"] = std::to_string(animation.animationFraction_.GetRepeatCallbackEnable());
    obj["FrameRateRange_min"] = std::to_string(animation.animationFraction_.GetFrameRateRange().min_);
    obj["FrameRateRange_max"] = std::to_string(animation.animationFraction_.GetFrameRateRange().max_);
    obj["FrameRateRange_prefered"] = std::to_string(animation.animationFraction_.GetFrameRateRange().preferred_);
    out.push_back(std::move(obj));
}

} // namespace OHOS::Rosen