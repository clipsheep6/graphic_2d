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

#include "foundation/graphic/graphic_2d/utils/log/rs_trace.h"
#include "rs_profiler.h"
#include "rs_profiler_jsonhelper.h"
#include "rs_profiler_network.h"

#include "common/rs_obj_geometry.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {

void RSProfiler::DumpNode(const RSRenderNode& node, Json::Value& out)
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

    if (node.GetSortedChildren()) {
        for (auto& child : *node.GetSortedChildren()) {
            if (child) {
                Json::Value childJson;
                DumpNode(*child, childJson);
                out["children"].append(std::move(childJson));
            }
        }
    }
    for (auto& [child, pos] : node.disappearingChildren_) {
        if (child) {
            Json::Value childJson;
            DumpNode(*child, childJson);
            out["children"].append(std::move(childJson));
        }
    }
}

void RSProfiler::DumpSubClassNode(const RSRenderNode& node, Json::Value& out)
{
    if (node.GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto& surfaceNode = static_cast<const RSSurfaceRenderNode&>(node);
        auto p = node.parent_.lock();
        out["Parent"] = p ? p->GetId() : 0;
        out["Name"] = surfaceNode.GetName();
        const auto& surfaceHandler = static_cast<const RSSurfaceHandler&>(surfaceNode);
        out["hasConsumer"] = surfaceHandler.HasConsumer();
        std::string contextAlpha = std::to_string(surfaceNode.contextAlpha_);
        std::string propertyAlpha = std::to_string(surfaceNode.GetRenderProperties().GetAlpha());
        out["Alpha"] = propertyAlpha + " (include ContextAlpha: " + contextAlpha + ")";
        out["Visible"] = std::to_string(surfaceNode.GetRenderProperties().GetVisible()) + " " +
                         surfaceNode.GetVisibleRegion().GetRegionInfo();
        out["Opaque"] = surfaceNode.GetOpaqueRegion().GetRegionInfo();
        out["OcclusionBg"] = surfaceNode.GetAbilityBgAlpha();
        out["SecurityLayer"] = surfaceNode.GetSecurityLayer();
        out["skipLayer"] = surfaceNode.GetSkipLayer();
    } else if (node.GetType() == RSRenderNodeType::ROOT_NODE) {
        auto& rootNode = static_cast<const RSRootRenderNode&>(node);
        out["Visible"] = rootNode.GetRenderProperties().GetVisible();
        JsonHelper::PushArray(out["Size"],
            { rootNode.GetRenderProperties().GetFrameWidth(), rootNode.GetRenderProperties().GetFrameHeight() });
        out["EnableRender"] = rootNode.GetEnableRender();
    } else if (node.GetType() == RSRenderNodeType::DISPLAY_NODE) {
        auto& displayNode = static_cast<const RSDisplayRenderNode&>(node);
        out["skipLayer"] = displayNode.GetSecurityDisplay();
    }
}

void RSProfiler::DumpDrawCmdModifiers(const RSRenderNode& node, Json::Value& out)
{
    auto& modifiersJson = out["DrawCmdModifiers"];

    if (!node.renderContent_) {
        return;
    }

    for (auto& [type, modifiers] : node.renderContent_->drawCmdModifiers_) {
        Json::Value modifierDesc;
        modifierDesc["type"] = static_cast<int>(type);
        modifierDesc["modifiers"] = Json::arrayValue;
        for (const auto& modifier : modifiers) {
            if (modifier) {
                DumpDrawCmdModifier(node, modifierDesc["modifiers"], static_cast<int>(type), *modifier);
            }
        }
        modifiersJson.append(std::move(modifierDesc));
    }
}

static std::string Hex(uint32_t value)
{
    std::stringstream sstream;
    sstream << std::hex << value;
    return sstream.str();
}

void RSProfiler::DumpDrawCmdModifier(const RSRenderNode& node, Json::Value& out, int type, RSRenderModifier& modifier)
{
    Json::Value property;
    auto modType = static_cast<RSModifierType>(type);

    if (modType < RSModifierType::ENV_FOREGROUND_COLOR) {
        auto propertyPtr = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier.GetProperty());
        auto drawCmdListPtr = propertyPtr ? propertyPtr->Get() : nullptr;
        auto propertyStr = drawCmdListPtr ? drawCmdListPtr->GetOpsWithDesc() : "";
        size_t pos = 0;
        size_t oldpos = 0;

        while ((pos = propertyStr.find('\n', oldpos)) != std::string::npos) {
            property["drawCmdList"].append(propertyStr.substr(oldpos, pos - oldpos));
            oldpos = pos + 1;
        }
    } else if (modType == RSModifierType::ENV_FOREGROUND_COLOR) {
        auto propertyPtr = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(modifier.GetProperty());
        if (propertyPtr) {
            property["ENV_FOREGROUND_COLOR"] = "#" + Hex(propertyPtr->Get().AsRgbaInt()) + " (RGBA)";
        }

    } else if (modType == RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY) {
        auto propertyPtr =
            std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(modifier.GetProperty());
        if (propertyPtr) {
            property["ENV_FOREGROUND_COLOR_STRATEGY"] = static_cast<int>(propertyPtr->Get());
        }
    } else if (modType == RSModifierType::GEOMETRYTRANS) {
        auto propertyPtr = std::static_pointer_cast<RSRenderProperty<SkMatrix>>(modifier.GetProperty());
        if (propertyPtr) {
            std::string str;
            propertyPtr->Get().dump(str, 0);
            property["GEOMETRYTRANS"] = str;
        }
    }

    if (!property.empty()) {
        out.append(std::move(property));
    }
}

void RSProfiler::DumpProperties(const RSProperties& p, Json::Value& out)
{
    JsonHelper::PushArray(
        out["Bounds"], { p.GetBoundsPositionX(), p.GetBoundsPositionY(), p.GetBoundsWidth(), p.GetBoundsHeight() });
    JsonHelper::PushArray(
        out["Frame"], { p.GetFramePositionX(), p.GetFramePositionY(), p.GetFrameWidth(), p.GetFrameHeight() });

    if (!p.GetVisible()) {
        out["IsVisible"] = false;
    }

    DumpPropertiesTransform(p, out);
    DumpPropertiesDecoration(p, out);
    DumpPropertiesEffects(p, out);
    DumpPropertiesShadow(p, out);
    DumpPropertiesColor(p, out);
}

void RSProfiler::DumpPropertiesTransform(const RSProperties& p, Json::Value& out)
{
    Transform defaultTransform;
    Vector2f pivot = p.GetPivot();
    if ((!ROSEN_EQ(pivot[0], defaultTransform.pivotX_) || !ROSEN_EQ(pivot[1], defaultTransform.pivotY_))) {
        JsonHelper::PushArray(out["Pivot"], { pivot[0], pivot[1] });
    }

    if (!ROSEN_EQ(p.GetRotation(), defaultTransform.rotation_)) {
        JsonHelper::PushValue(out["Rotation"], p.GetRotation());
    }

    if (!ROSEN_EQ(p.GetRotationX(), defaultTransform.rotationX_)) {
        JsonHelper::PushValue(out["RotationX"], p.GetRotationX());
    }

    if (!ROSEN_EQ(p.GetRotationY(), defaultTransform.rotationY_)) {
        JsonHelper::PushValue(out["RotationY"], p.GetRotationY());
    }

    if (!ROSEN_EQ(p.GetTranslateX(), defaultTransform.translateX_)) {
        JsonHelper::PushValue(out["TranslateX"], p.GetTranslateX());
    }

    if (!ROSEN_EQ(p.GetTranslateY(), defaultTransform.translateY_)) {
        JsonHelper::PushValue(out["TranslateY"], p.GetTranslateY());
    }

    if (!ROSEN_EQ(p.GetTranslateZ(), defaultTransform.translateZ_)) {
        JsonHelper::PushValue(out["TranslateZ"], p.GetTranslateZ());
    }

    if (!ROSEN_EQ(p.GetScaleX(), defaultTransform.scaleX_)) {
        JsonHelper::PushValue(out["ScaleX"], p.GetScaleX());
    }

    if (!ROSEN_EQ(p.GetScaleY(), defaultTransform.scaleY_)) {
        JsonHelper::PushValue(out["ScaleY"], p.GetScaleY());
    }

    if (!ROSEN_EQ(p.GetPositionZ(), 0.f)) {
        JsonHelper::PushValue(out["PositionZ"], p.GetPositionZ());
    }
}

void RSProfiler::DumpPropertiesDecoration(const RSProperties& p, Json::Value& out)
{
    if (!p.GetCornerRadius().IsZero()) {
        JsonHelper::PushArray(out["CornerRadius"],
            { p.GetCornerRadius().x_, p.GetCornerRadius().y_, p.GetCornerRadius().z_, p.GetCornerRadius().w_ });
    }

    if (p.pixelStretch_.has_value()) {
        JsonHelper::PushValue(out["PixelStretch"]["left"], p.pixelStretch_->z_);
        JsonHelper::PushValue(out["PixelStretch"]["top"], p.pixelStretch_->y_);
        JsonHelper::PushValue(out["PixelStretch"]["right"], p.pixelStretch_->z_);
        JsonHelper::PushValue(out["PixelStretch"]["bottom"], p.pixelStretch_->w_);
    }

    if (!ROSEN_EQ(p.GetAlpha(), 1.f)) {
        JsonHelper::PushValue(out["Alpha"], p.GetAlpha());
    }

    if (!ROSEN_EQ(p.GetSpherize(), 0.f)) {
        JsonHelper::PushValue(out["Spherize"], p.GetSpherize());
    }

    if (!ROSEN_EQ(p.GetForegroundColor(), RgbPalette::Transparent())) {
        out["ForegroundColor"] = "#" + Hex(p.GetForegroundColor().AsArgbInt()) + " (ARGB)";
    }

    if (!ROSEN_EQ(p.GetBackgroundColor(), RgbPalette::Transparent())) {
        out["BackgroundColor"] = "#" + Hex(p.GetBackgroundColor().AsArgbInt()) + " (ARGB)";
    }

    Decoration defaultDecoration;
    if ((!ROSEN_EQ(p.GetBgImagePositionX(), defaultDecoration.bgImageRect_.left_) ||
        !ROSEN_EQ(p.GetBgImagePositionY(), defaultDecoration.bgImageRect_.top_) ||
        !ROSEN_EQ(p.GetBgImageWidth(), defaultDecoration.bgImageRect_.width_) ||
        !ROSEN_EQ(p.GetBgImageHeight(), defaultDecoration.bgImageRect_.height_))) {
        JsonHelper::PushArray(out["BgImage"],
            { p.GetBgImagePositionX(), p.GetBgImagePositionY(), p.GetBgImageWidth(), p.GetBgImageHeight() });
    }
}

void RSProfiler::DumpPropertiesShadow(const RSProperties& p, Json::Value& out)
{
    if (!ROSEN_EQ(p.GetShadowColor(), Color(DEFAULT_SPOT_COLOR))) {
        out["ShadowColor"] = "#" + Hex(p.GetShadowColor().AsArgbInt()) + " (ARGB)";
    }

    if (!ROSEN_EQ(p.GetShadowOffsetX(), DEFAULT_SHADOW_OFFSET_X)) {
        JsonHelper::PushValue(out["ShadowOffsetX"], p.GetShadowOffsetX());
    }

    if (!ROSEN_EQ(p.GetShadowOffsetY(), DEFAULT_SHADOW_OFFSET_Y)) {
        JsonHelper::PushValue(out["ShadowOffsetY"], p.GetShadowOffsetY());
    }

    if (!ROSEN_EQ(p.GetShadowAlpha(), 0.f)) {
        JsonHelper::PushValue(out["ShadowAlpha"], p.GetShadowAlpha());
    }

    if (!ROSEN_EQ(p.GetShadowElevation(), 0.f)) {
        JsonHelper::PushValue(out["ShadowElevation"], p.GetShadowElevation());
    }

    if (!ROSEN_EQ(p.GetShadowRadius(), 0.f)) {
        JsonHelper::PushValue(out["ShadowRadius"], p.GetShadowRadius());
    }

    if (!ROSEN_EQ(p.GetShadowIsFilled(), false)) {
        out["ShadowIsFilled"] = p.GetShadowIsFilled();
    }
}

void RSProfiler::DumpPropertiesEffects(const RSProperties& p, Json::Value& out)
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
        out["FrameGravity"] = static_cast<int>(p.GetFrameGravity());
    }

    if (p.GetUseEffect()) {
        out["GetUseEffect"] = true;
    }

    auto grayScale = p.GetGrayScale();
    if (grayScale.has_value() && !ROSEN_EQ(*grayScale, 0.f)) {
        JsonHelper::PushValue(out["GrayScale"], *grayScale);
    }

    if (!ROSEN_EQ(p.GetLightUpEffect(), 1.f)) {
        JsonHelper::PushValue(out["LightUpEffect"], p.GetLightUpEffect());
    }

    auto dynamicLightUpRate = p.GetDynamicLightUpRate();
    if (dynamicLightUpRate.has_value() && !ROSEN_EQ(*dynamicLightUpRate, 0.f)) {
        JsonHelper::PushValue(out["DynamicLightUpRate"], *dynamicLightUpRate);
    }

    auto dynamicLightUpDegree = p.GetDynamicLightUpDegree();
    if (dynamicLightUpDegree.has_value() && !ROSEN_EQ(*dynamicLightUpDegree, 0.f)) {
        JsonHelper::PushValue(out["DynamicLightUpDegree"], *dynamicLightUpDegree);
    }
}

void RSProfiler::DumpPropertiesColor(const RSProperties& p, Json::Value& out)
{
    auto brightness = p.GetBrightness();
    if (brightness.has_value() && !ROSEN_EQ(*brightness, 1.f)) {
        JsonHelper::PushValue(out["Brightness"], *brightness);
    }

    auto contrast = p.GetContrast();
    if (contrast.has_value() && !ROSEN_EQ(*contrast, 1.f)) {
        JsonHelper::PushValue(out["Contrast"], *contrast);
    }

    auto saturate = p.GetSaturate();
    if (saturate.has_value() && !ROSEN_EQ(*saturate, 1.f)) {
        JsonHelper::PushValue(out["Saturate"], *saturate);
    }

    auto sepia = p.GetSepia();
    if (sepia.has_value() && !ROSEN_EQ(*sepia, 0.f)) {
        JsonHelper::PushValue(out["Sepia"], *sepia);
    }

    auto invert = p.GetInvert();
    if (invert.has_value() && !ROSEN_EQ(*invert, 0.f)) {
        JsonHelper::PushValue(out["Invert"], *invert);
    }

    auto hueRotate = p.GetHueRotate();
    if (hueRotate.has_value() && !ROSEN_EQ(*hueRotate, 0.f)) {
        JsonHelper::PushValue(out["HueRotate"], *hueRotate);
    }

    auto colorBlend = p.GetColorBlend();
    if (colorBlend.has_value() && !ROSEN_EQ(*colorBlend, RgbPalette::Transparent())) {
        out["ColorBlend"] = "#" + Hex(colorBlend->AsArgbInt()) + " (ARGB)";
    }

    if (!ROSEN_EQ(p.GetColorBlendMode(), 0)) {
        out["skblendmode"] = p.GetColorBlendMode() - 1;
        out["blendType"] = p.GetColorBlendApplyType();
    }
}

void RSProfiler::DumpAnimations(const RSAnimationManager& animationManager, Json::Value& out)
{
    if (animationManager.animations_.empty()) {
        return;
    }
    for (auto [id, animation] : animationManager.animations_) {
        if (animation) {
            DumpAnimation(*animation, out["RSAnimationManager"]);
        }
    }
}

void RSProfiler::DumpAnimation(const RSRenderAnimation& animation, Json::Value& out)
{
    Json::Value obj;
    obj["id"] = animation.id_;
    obj["type"] = "unknown";
    obj["AnimationState"] = static_cast<int>(animation.state_);
    obj["StartDelay"] = animation.animationFraction_.GetDuration();
    obj["Duration"] = animation.animationFraction_.GetStartDelay();
    JsonHelper::PushValue(obj["Speed"], animation.animationFraction_.GetSpeed());
    obj["RepeatCount"] = animation.animationFraction_.GetRepeatCount();
    obj["AutoReverse"] = animation.animationFraction_.GetAutoReverse();
    obj["Direction"] = animation.animationFraction_.GetDirection();
    obj["FillMode"] = static_cast<int>(animation.animationFraction_.GetFillMode());
    obj["RepeatCallbackEnable"] = animation.animationFraction_.GetRepeatCallbackEnable();
    obj["FrameRateRange_min"] = animation.animationFraction_.GetFrameRateRange().min_;
    obj["FrameRateRange_max"] = animation.animationFraction_.GetFrameRateRange().max_;
    obj["FrameRateRange_prefered"] = animation.animationFraction_.GetFrameRateRange().preferred_;
    out.append(std::move(obj));
}

} // namespace OHOS::Rosen