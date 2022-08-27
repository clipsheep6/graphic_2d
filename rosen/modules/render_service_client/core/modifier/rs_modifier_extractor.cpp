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

#include "modifier/rs_modifier_extractor.h"

#include <securec.h>

#include "modifier/rs_modifier_type.h"
#include "modifier/rs_property_modifier.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
RSModifierExtractor::RSModifierExtractor(NodeId id) : id_(id) {}

namespace {
template<typename T>
T& Add(T& a, const T& b)
{
    return a += b;
}
template<typename T>
T& Mul(T& a, const T& b)
{
    return a *= b;
}
template<typename T>
T Last(T& a, const T& b)
{
    return b;
}
} // namespace

template<typename T, auto op>
T RSModifierExtractor::GetProperty(RSModifierType type, const T& defaultValue) const
{
    auto node = RSNodeMap::Instance().GetNode<RSNode>(id_);
    if (node == nullptr) {
        return defaultValue;
    }
    auto iter = node->propertyModifiers_.find(type);
    if (iter != node->propertyModifiers_.end()) {
        return std::static_pointer_cast<RSProperty<T>>(iter->second->GetProperty())->Get();
    }

    T value = defaultValue;
    for (auto& [_, modifier] : node->modifiers_) {
        if (modifier->GetModifierType() != type) {
            continue;
        }
        auto modifierPtr = std::static_pointer_cast<RSModifier<RSProperty<T>>>(modifier);
        if (modifierPtr->isAdditive_) {
            value = op(value, std::static_pointer_cast<RSProperty<T>>(modifierPtr->GetProperty())->Get());
        } else {
            value = std::static_pointer_cast<RSProperty<T>>(modifierPtr->GetProperty())->Get();
        }
    }
    return value;
}

Vector4f RSModifierExtractor::GetBounds() const
{
    return GetProperty<Vector4f, Last<Vector4f>>(RSModifierType::BOUNDS, Vector4f());
}

Vector4f RSModifierExtractor::GetFrame() const
{
    return GetProperty<Vector4f, Last<Vector4f>>(RSModifierType::FRAME, Vector4f());
}

float RSModifierExtractor::GetPositionZ() const
{
    return GetProperty<float, Add<float>>(RSModifierType::POSITION_Z, 0.f);
}

Vector2f RSModifierExtractor::GetPivot() const
{
    return GetProperty<Vector2f, Last<Vector2f>>(RSModifierType::PIVOT, Vector2f(0.5f, 0.5f));
}

Quaternion RSModifierExtractor::GetQuaternion() const
{
    return GetProperty<Quaternion, Last<Quaternion>>(RSModifierType::QUATERNION, Quaternion());
}

float RSModifierExtractor::GetRotation() const
{
    return GetProperty<float, Add<float>>(RSModifierType::ROTATION, 0.f);
}

float RSModifierExtractor::GetRotationX() const
{
    return GetProperty<float, Add<float>>(RSModifierType::ROTATION_X, 0.f);
}

float RSModifierExtractor::GetRotationY() const
{
    return GetProperty<float, Add<float>>(RSModifierType::ROTATION_Y, 0.f);
}

Vector2f RSModifierExtractor::GetTranslate() const
{
    return GetProperty<Vector2f, Add<Vector2f>>(RSModifierType::TRANSLATE, Vector2f(0.f, 0.f));
}

float RSModifierExtractor::GetTranslateZ() const
{
    return GetProperty<float, Add<float>>(RSModifierType::TRANSLATE_Z, 0.f);
}

Vector2f RSModifierExtractor::GetScale() const
{
    return GetProperty<Vector2f, Mul<Vector2f>>(RSModifierType::SCALE, Vector2f(1.f, 1.f));
}

float RSModifierExtractor::GetAlpha() const
{
    return GetProperty<float, Mul<float>>(RSModifierType::ALPHA, 1.f);
}

bool RSModifierExtractor::GetAlphaOffscreen() const
{
    return GetProperty<bool, Last<bool>>(RSModifierType::ALPHA_OFFSCREEN, true);
}

Vector4f RSModifierExtractor::GetCornerRadius() const
{
    return GetProperty<Vector4f, Last<Vector4f>>(RSModifierType::CORNER_RADIUS, 0.f);
}

Color RSModifierExtractor::GetForegroundColor() const
{
    return GetProperty<Color, Last<Color>>(RSModifierType::FOREGROUND_COLOR, RgbPalette::Transparent());
}

Color RSModifierExtractor::GetBackgroundColor() const
{
    return GetProperty<Color, Last<Color>>(RSModifierType::BACKGROUND_COLOR, RgbPalette::Transparent());
}

Color RSModifierExtractor::GetSurfaceBgColor() const
{
    return GetProperty<Color, Last<Color>>(RSModifierType::SURFACE_BG_COLOR, RgbPalette::Transparent());
}

std::shared_ptr<RSShader> RSModifierExtractor::GetBackgroundShader() const
{
    return GetProperty<std::shared_ptr<RSShader>, Last<std::shared_ptr<RSShader>>>(
        RSModifierType::BACKGROUND_SHADER, nullptr);
}

std::shared_ptr<RSImage> RSModifierExtractor::GetBgImage() const
{
    return GetProperty<std::shared_ptr<RSImage>, Last<std::shared_ptr<RSImage>>>(RSModifierType::BG_IMAGE, nullptr);
}

float RSModifierExtractor::GetBgImageWidth() const
{
    return GetProperty<float, Last<float>>(RSModifierType::BG_IMAGE_WIDTH, 0.f);
}

float RSModifierExtractor::GetBgImageHeight() const
{
    return GetProperty<float, Last<float>>(RSModifierType::BG_IMAGE_HEIGHT, 0.f);
}

float RSModifierExtractor::GetBgImagePositionX() const
{
    return GetProperty<float, Last<float>>(RSModifierType::BG_IMAGE_POSITION_X, 0.f);
}

float RSModifierExtractor::GetBgImagePositionY() const
{
    return GetProperty<float, Last<float>>(RSModifierType::BG_IMAGE_POSITION_Y, 0.f);
}

Vector4<Color> RSModifierExtractor::GetBorderColor() const
{
    return GetProperty<Vector4<Color>, Last<Vector4<Color>>>(
        RSModifierType::BORDER_COLOR, Vector4(RgbPalette::Transparent()));
}

Vector4f RSModifierExtractor::GetBorderWidth() const
{
    return GetProperty<Vector4f, Last<Vector4f>>(RSModifierType::BORDER_WIDTH, Vector4f(0.f));
}

Vector4<uint32_t> RSModifierExtractor::GetBorderStyle() const
{
    return GetProperty<Vector4<uint32_t>, Last<Vector4<uint32_t>>>(
        RSModifierType::BORDER_STYLE, Vector4<uint32_t>(static_cast<uint32_t>(BorderStyle::NONE)));
}

std::shared_ptr<RSFilter> RSModifierExtractor::GetBackgroundFilter() const
{
    return GetProperty<std::shared_ptr<RSFilter>, Last<std::shared_ptr<RSFilter>>>(
        RSModifierType::BACKGROUND_FILTER, nullptr);
}

std::shared_ptr<RSFilter> RSModifierExtractor::GetFilter() const
{
    return GetProperty<std::shared_ptr<RSFilter>, Last<std::shared_ptr<RSFilter>>>(RSModifierType::FILTER, nullptr);
}

Color RSModifierExtractor::GetShadowColor() const
{
    return GetProperty<Color, Last<Color>>(RSModifierType::SHADOW_COLOR, Color::FromArgbInt(DEFAULT_SPOT_COLOR));
}

float RSModifierExtractor::GetShadowOffsetX() const
{
    return GetProperty<float, Add<float>>(RSModifierType::SHADOW_OFFSET_X, DEFAULT_SHADOW_OFFSET_X);
}

float RSModifierExtractor::GetShadowOffsetY() const
{
    return GetProperty<float, Add<float>>(RSModifierType::SHADOW_OFFSET_Y, DEFAULT_SHADOW_OFFSET_Y);
}

float RSModifierExtractor::GetShadowAlpha() const
{
    return GetProperty<float, Mul<float>>(RSModifierType::SHADOW_ALPHA, 0.f);
}

float RSModifierExtractor::GetShadowElevation() const
{
    return GetProperty<float, Add<float>>(RSModifierType::SHADOW_ELEVATION, 0.f);
}

float RSModifierExtractor::GetShadowRadius() const
{
    return GetProperty<float, Add<float>>(RSModifierType::SHADOW_RADIUS, 0.f);
}

std::shared_ptr<RSPath> RSModifierExtractor::GetShadowPath() const
{
    return GetProperty<std::shared_ptr<RSPath>, Last<std::shared_ptr<RSPath>>>(RSModifierType::SHADOW_PATH, nullptr);
}

Gravity RSModifierExtractor::GetFrameGravity() const
{
    return GetProperty<Gravity, Last<Gravity>>(RSModifierType::FRAME_GRAVITY, Gravity::DEFAULT);
}

std::shared_ptr<RSPath> RSModifierExtractor::GetClipBounds() const
{
    return GetProperty<std::shared_ptr<RSPath>, Last<std::shared_ptr<RSPath>>>(RSModifierType::CLIP_BOUNDS, nullptr);
}

bool RSModifierExtractor::GetClipToBounds() const
{
    return GetProperty<bool, Last<bool>>(RSModifierType::CLIP_TO_BOUNDS, false);
}

bool RSModifierExtractor::GetClipToFrame() const
{
    return GetProperty<bool, Last<bool>>(RSModifierType::CLIP_TO_FRAME, false);
}

bool RSModifierExtractor::GetVisible() const
{
    return GetProperty<bool, Last<bool>>(RSModifierType::VISIBLE, true);
}

std::shared_ptr<RSMask> RSModifierExtractor::GetMask() const
{
    return GetProperty<std::shared_ptr<RSMask>, Last<std::shared_ptr<RSMask>>>(RSModifierType::MASK, nullptr);
}

std::string RSModifierExtractor::Dump() const
{
    std::string dumpInfo;
    char buffer[UINT8_MAX] = { 0 };
    auto bounds = GetBounds();
    auto frame = GetFrame();
    if (sprintf_s(buffer, UINT8_MAX, "Bounds[%.1f %.1f %.1f %.1f] Frame[%.1f %.1f %.1f %.1f]", bounds.x_, bounds.y_,
        bounds.z_, bounds.w_, frame.x_, frame.y_, frame.z_, frame.w_) != -1) {
        dumpInfo.append(buffer);
    }

    memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (!ROSEN_EQ(GetBackgroundColor(), RgbPalette::Transparent()) &&
        sprintf_s(buffer, UINT8_MAX, ", BackgroundColor[#%08X]", GetBackgroundColor().AsArgbInt()) != -1) {
        dumpInfo.append(buffer);
    }

    memset_s(buffer, UINT8_MAX, 0, UINT8_MAX);
    if (!ROSEN_EQ(GetAlpha(), 1.f) && sprintf_s(buffer, UINT8_MAX, ", Alpha[%.1f]", GetAlpha()) != -1) {
        dumpInfo.append(buffer);
    }

    if (!GetVisible()) {
        dumpInfo.append(", IsVisible[false]");
    }
    return dumpInfo;
}
} // namespace Rosen
} // namespace OHOS
