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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CLIENT_LOG_UTILS_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CLIENT_LOG_UTILS_H

#include <cstdint>
#include <string>
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {

class RSAnimationClientLogUtils {
public:
    static std::string GetRSModifierTypeName(RSModifierType type)
    {
        switch (type)
        {
            case RSModifierType::INVALID:
                return "INVALID";
            case RSModifierType::BOUNDS:
                return "BOUNDS";
            case RSModifierType::FRAME:
                return "FRAME";
            case RSModifierType::POSITION_Z:
                return "POSITION_Z";
            case RSModifierType::PIVOT:
                return "PIVOT";
            case RSModifierType::PIVOT_Z:
                return "PIVOT_Z";
            case RSModifierType::QUATERNION:
                return "QUATERNION";
            case RSModifierType::ROTATION:
                return "ROTATION";
            case RSModifierType::ROTATION_X:
                return "ROTATION_X";
            case RSModifierType::ROTATION_Y:
                return "ROTATION_Y";
            case RSModifierType::CAMERA_DISTANCE:
                return "CAMERA_DISTANCE";
            case RSModifierType::SCALE:
                return "SCALE";
            case RSModifierType::TRANSLATE:
                return "TRANSLATE";
            case RSModifierType::TRANSLATE_Z:
                return "TRANSLATE_Z";
            case RSModifierType::SUBLAYER_TRANSFORM:
                return "SUBLAYER_TRANSFORM";
            case RSModifierType::CORNER_RADIUS:
                return "CORNER_RADIUS";
            case RSModifierType::ALPHA:
                return "ALPHA";
            case RSModifierType::ALPHA_OFFSCREEN:
                return "ALPHA_OFFSCREEN";
            case RSModifierType::FOREGROUND_COLOR:
                return "FOREGROUND_COLOR";
            case RSModifierType::BACKGROUND_COLOR:
                return "BACKGROUND_COLOR";
            case RSModifierType::BACKGROUND_SHADER:
                return "BACKGROUND_SHADER";
            case RSModifierType::BG_IMAGE:
                return "BG_IMAGE";
            case RSModifierType::BG_IMAGE_WIDTH:
                return "BG_IMAGE_WIDTH";
            case RSModifierType::BG_IMAGE_HEIGHT:
                return "BG_IMAGE_HEIGHT";
            case RSModifierType::BG_IMAGE_POSITION_X:
                return "BG_IMAGE_POSITION_X";
            case RSModifierType::BG_IMAGE_POSITION_Y:
                return "BG_IMAGE_POSITION_Y";
            case RSModifierType::SURFACE_BG_COLOR:
                return "SURFACE_BG_COLOR";
            case RSModifierType::BORDER_COLOR:
                return "BORDER_COLOR";
            case RSModifierType::BORDER_WIDTH:
                return "BORDER_WIDTH";
            case RSModifierType::BORDER_STYLE:
                return "BORDER_STYLE";
            case RSModifierType::FILTER:
                return "FILTER";
            case RSModifierType::BACKGROUND_FILTER:
                return "BACKGROUND_FILTER";
            case RSModifierType::LINEAR_GRADIENT_BLUR_PARA:
                return "LINEAR_GRADIENT_BLUR_PARA";
            case RSModifierType::DYNAMIC_LIGHT_UP_RATE:
                return "DYNAMIC_LIGHT_UP_RATE";
            case RSModifierType::DYNAMIC_LIGHT_UP_DEGREE:
                return "DYNAMIC_LIGHT_UP_DEGREE";
            case RSModifierType::FRAME_GRAVITY:
                return "FRAME_GRAVITY";
            case RSModifierType::CLIP_RRECT:
                return "CLIP_RRECT";
            case RSModifierType::CLIP_BOUNDS:
                return "CLIP_BOUNDS";
            case RSModifierType::CLIP_TO_BOUNDS:
                return "CLIP_TO_BOUNDS";
            case RSModifierType::CLIP_TO_FRAME:
                return "CLIP_TO_FRAME";
            case RSModifierType::VISIBLE:
                return "VISIBLE";
            case RSModifierType::SHADOW_COLOR:
                return "SHADOW_COLOR";
            case RSModifierType::SHADOW_OFFSET_X:
                return "SHADOW_OFFSET_X";
            case RSModifierType::SHADOW_OFFSET_Y:
                return "SHADOW_OFFSET_Y";
            case RSModifierType::SHADOW_ALPHA:
                return "SHADOW_ALPHA";
            case RSModifierType::SHADOW_ELEVATION:
                return "SHADOW_ELEVATION";
            case RSModifierType::SHADOW_RADIUS:
                return "SHADOW_RADIUS";
            case RSModifierType::SHADOW_PATH:
                return "SHADOW_PATH";
            case RSModifierType::SHADOW_MASK:
                return "SHADOW_MASK";
            case RSModifierType::SHADOW_COLOR_STRATEGY:
                return "SHADOW_COLOR_STRATEGY";
            case RSModifierType::MASK:
                return "MASK";
            case RSModifierType::SPHERIZE:
                return "SPHERIZE";
            case RSModifierType::LIGHT_UP_EFFECT:
                return "LIGHT_UP_EFFECT";
            case RSModifierType::PIXEL_STRETCH:
                return "PIXEL_STRETCH";
            case RSModifierType::PIXEL_STRETCH_PERCENT:
                return "PIXEL_STRETCH_PERCENT";
            case RSModifierType::USE_EFFECT:
                return "USE_EFFECT";
            case RSModifierType::COLOR_BLEND_MODE:
                return "COLOR_BLEND_MODE";
            case RSModifierType::COLOR_BLEND_APPLY_TYPE:
                return "COLOR_BLEND_APPLY_TYPE";
            case RSModifierType::SANDBOX:
                return "SANDBOX";
            case RSModifierType::GRAY_SCALE:
                return "GRAY_SCALE";
            case RSModifierType::BRIGHTNESS:
                return "BRIGHTNESS";
            case RSModifierType::CONTRAST:
                return "CONTRAST";
            case RSModifierType::SATURATE:
                return "SATURATE";
            case RSModifierType::SEPIA:
                return "SEPIA";
            case RSModifierType::INVERT:
                return "INVERT";
            case RSModifierType::AIINVERT:
                return "AIINVERT";
            case RSModifierType::HUE_ROTATE:
                return "HUE_ROTATE";
            case RSModifierType::COLOR_BLEND:
                return "COLOR_BLEND";
            case RSModifierType::PARTICLE:
                return "PARTICLE";
            case RSModifierType::SHADOW_IS_FILLED:
                return "SHADOW_IS_FILLED";
            case RSModifierType::OUTLINE_COLOR:
                return "OUTLINE_COLOR";
            case RSModifierType::OUTLINE_WIDTH:
                return "OUTLINE_WIDTH";
            case RSModifierType::OUTLINE_STYLE:
                return "OUTLINE_STYLE";
            case RSModifierType::OUTLINE_RADIUS:
                return "OUTLINE_RADIUS";
            case RSModifierType::USE_SHADOW_BATCHING:
                return "USE_SHADOW_BATCHING";
            case RSModifierType::GREY_COEF1:
                return "GREY_COEF1";
            case RSModifierType::GREY_COEF2:
                return "GREY_COEF2";
            case RSModifierType::LIGHT_INTENSITY:
                return "LIGHT_INTENSITY";
            case RSModifierType::LIGHT_POSITION:
                return "LIGHT_POSITION";
            case RSModifierType::ILLUMINATED_BORDER_WIDTH:
                return "ILLUMINATED_BORDER_WIDTH";
            case RSModifierType::ILLUMINATED_TYPE:
                return "ILLUMINATED_TYPE";
            case RSModifierType::BLOOM:
                return "BLOOM";
            case RSModifierType::CUSTOM:
                return "CUSTOM";
            case RSModifierType::EXTENDED:
                return "EXTENDED";
            case RSModifierType::TRANSITION:
                return "TRANSITION";
            case RSModifierType::BACKGROUND_STYLE:
                return "BACKGROUND_STYLE";
            case RSModifierType::CONTENT_STYLE:
                return "CONTENT_STYLE";
            case RSModifierType::FOREGROUND_STYLE:
                return "FOREGROUND_STYLE";
            case RSModifierType::OVERLAY_STYLE:
                return "OVERLAY_STYLE";
            case RSModifierType::NODE_MODIFIER:
                return "NODE_MODIFIER";
            case RSModifierType::ENV_FOREGROUND_COLOR:
                return "ENV_FOREGROUND_COLOR";
            case RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY:
                return "ENV_FOREGROUND_COLOR_STRATEGY";
            case RSModifierType::GEOMETRYTRANS:
                return "GEOMETRYTRANS";
#ifdef USE_ROSEN_DRAWING
            case RSModifierType::MAX_RS_MODIFIER_TYPE:
                return "MAX_RS_MODIFIER_TYPE";
#endif
            default:
                return "UNKNOWN";
        }
    }

    static std::string ParsePropertyVaule(const std::shared_ptr<RSPropertyBase>& value)
    {
        std::string str;
        switch (value->GetPropertyType()) {
            case RSRenderPropertyType::PROPERTY_FLOAT: {
                auto property = std::static_pointer_cast<RSAnimatableProperty<float>>(value);
                if (property != nullptr) {
                    str = "float:" + std::to_string(property->Get());
                }
                break;
            }
            case RSRenderPropertyType::PROPERTY_COLOR: {
                auto property = std::static_pointer_cast<RSAnimatableProperty<Color>>(value);
                if (property != nullptr) {
                    str = "Color:red:" + std::to_string(property->Get().GetRed()) + "," +
                        "green:" + std::to_string(property->Get().GetGreen()) + "," +
                        "blue:" + std::to_string(property->Get().GetBlue());
                }
                break;
            }
            case RSRenderPropertyType::PROPERTY_MATRIX3F: {
                str = "Matrix3f";
                break;
            }
            case RSRenderPropertyType::PROPERTY_QUATERNION: {
                auto property = std::static_pointer_cast<RSAnimatableProperty<Quaternion>>(value);
                if (property != nullptr) {
                    str = "Quaternion:x:" + std::to_string(property->Get().x_) + "," +
                        "y:" + std::to_string(property->Get().y_) + "," +
                        "z:" + std::to_string(property->Get().z_) + "," +
                        "w:" + std::to_string(property->Get().w_);
                }
                break;
            }
            case RSRenderPropertyType::PROPERTY_FILTER: {
                str = "FILTER";
                break;
            }
            case RSRenderPropertyType::PROPERTY_VECTOR2F: {
                auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(value);
                if (property != nullptr) {
                    str = "Vector2f:x:" + std::to_string(property->Get().x_) + "," +
                        "y:" + std::to_string(property->Get().y_);
                }
                break;
            }
            case RSRenderPropertyType::PROPERTY_VECTOR4F: {
                auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(value);
                if (property != nullptr) {
                    str = "Vector4f:x:" + std::to_string(property->Get().x_) + "," +
                        "y:" + std::to_string(property->Get().y_) + "," +
                        "z:" + std::to_string(property->Get().z_) + "," +
                        "w:" + std::to_string(property->Get().w_);
                }
                break;
            }
            case RSRenderPropertyType::PROPERTY_VECTOR4_COLOR: {
                auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4<Color>>>(value);
                if (property != nullptr) {
                    str = "Vector4<Color>:x(red):" + std::to_string(property->Get().x_.GetRed()) + ","
                        "x(green):" + std::to_string(property->Get().x_.GetGreen()) + ","
                        "x(blue):" + std::to_string(property->Get().x_.GetBlue()) + ","
                        "y(red):" + std::to_string(property->Get().y_.GetRed()) + ","
                        "y(green):" + std::to_string(property->Get().y_.GetGreen()) + ","
                        "y(blue):" + std::to_string(property->Get().y_.GetBlue()) + ","
                        "z(red):" + std::to_string(property->Get().z_.GetRed()) + ","
                        "z(green):" + std::to_string(property->Get().z_.GetGreen()) + ","
                        "z(blue):" + std::to_string(property->Get().z_.GetBlue()) + ","
                        "w(red):" + std::to_string(property->Get().w_.GetRed()) + ","
                        "w(green):" + std::to_string(property->Get().w_.GetGreen()) + ","
                        "w(blue):" + std::to_string(property->Get().w_.GetBlue());
                }
                break;
            }
            default: {
                return "None";
            }
        }
        return str;
    }
private:
    RSAnimationClientLogUtils() = default;
    RSAnimationClientLogUtils(const RSAnimationClientLogUtils&) = delete;
    RSAnimationClientLogUtils(const RSAnimationClientLogUtils&&) = delete;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CLIENT_LOG_UTILS_H
