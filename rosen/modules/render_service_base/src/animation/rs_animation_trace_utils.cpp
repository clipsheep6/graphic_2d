/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "animation/rs_animation_trace_utils.h"

#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

enum RSTransitionEffectType : uint16_t {
    FADE = 1,
    SCALE,
    TRANSLATE,
    ROTATE,
    UNDEFINED,
};

RSAnimationTraceUtils RSAnimationTraceUtils::instance_;

std::string RSAnimationTraceUtils::ParseRenderPropertyVaule(const std::shared_ptr<RSRenderPropertyBase> value)
{
    std::string str;
    switch (value->GetPropertyType()) {
        case RSRenderPropertyType::PROPERTY_FLOAT: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(value);
            if (property != nullptr) {
                str = "float:" + std::to_string(property->Get());
            }
            break;
        }
        case RSRenderPropertyType::PROPERTY_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(value);
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
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Quaternion>>(value);
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
        case RSRenderPropertyType::PROPERTY_VECTOR2F:
        case RSRenderPropertyType::PROPERTY_VECTOR4F:
        case RSRenderPropertyType::PROPERTY_VECTOR4_COLOR: {
            str = ParseRenderPropertyVauleOther(value);
            break;
        }
        default: {
            return "None";
        }
    }
    return str;
}

std::string RSAnimationTraceUtils::ParseRenderPropertyVauleOther(const std::shared_ptr<RSRenderPropertyBase> value)
{
    std::string str;
    switch (value->GetPropertyType()) {
        case RSRenderPropertyType::PROPERTY_VECTOR2F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(value);
            if (property != nullptr) {
                str = "Vector2f:x:" + std::to_string(property->Get().x_) + "," +
                    "y:" + std::to_string(property->Get().y_);
            }
            break;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(value);
            if (property != nullptr) {
                str = "Vector4f:x:" + std::to_string(property->Get().x_) + "," +
                    "y:" + std::to_string(property->Get().y_) + "," +
                    "z:" + std::to_string(property->Get().z_) + "," +
                    "w:" + std::to_string(property->Get().w_);
            }
            break;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4<Color>>>(value);
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

void RSAnimationTraceUtils::addAnimationCreateTrace(const int nodeId, const int propertyId, const int animationType,
    const std::shared_ptr<RSRenderPropertyBase> startValue, const std::shared_ptr<RSRenderPropertyBase> endValue)
{
    auto startStr = RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(startValue);
    auto endStr = RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(endValue);
    std::string traceStr = "create animation trace nodeId=" + std::to_string(nodeId) + "," +
        "propertyId=" + std::to_string(propertyId) + "," +
        "animationType=" + std::to_string(animationType) + "," +
        "startValue=" + startStr + "," +
        "endValue=" + endStr;
    AddTraceFlag(traceStr);
}

void RSAnimationTraceUtils::addAnimationFrameTrace(const int nodeId, const int animationId, const int propertyId,
    const float fraction, const std::shared_ptr<RSRenderPropertyBase> value)
{
    auto propertyValue = ParseRenderPropertyVaule(value);
    std::string traceStr = "frame animation trace nodeId=" + std::to_string(nodeId) + "," +
        "animationId=" + std::to_string(animationId) + "," +
        "propertyId=" + std::to_string(propertyId) + "," +
        "fraction=" + std::to_string(fraction) + "," +
        "value=" + propertyValue;
    AddTraceFlag(traceStr);
}

void RSAnimationTraceUtils::addTranstionFrameTrace(const int nodeId, const int animationId, const float fraction,
    const float valueFraction)
{
    std::string traceStr = "transtion frame animation trace nodeId=" + std::to_string(nodeId) + "," +
        "animationId=" + std::to_string(animationId) + "," +
        "fraction=" + std::to_string(fraction) + "," +
        "valueFraction=" + std::to_string(valueFraction);
    AddTraceFlag(traceStr);
}

void RSAnimationTraceUtils::addTranstionEffectValueTrace(const int propertyId, const float fraction,
    const uint16_t type, const std::shared_ptr<RSRenderPropertyBase> value)
{
    if (!RSSystemProperties::GetAnimationTraceEnabled()) {
        return;
    }
    std::string valueStr, typeStr;
    switch (type) {
        case RSTransitionEffectType::FADE: {
            typeStr = "FADE";
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(value);
            if (property != nullptr) {
                valueStr = "float:" + std::to_string(property->Get());
            }
            break;
        }
        case RSTransitionEffectType::SCALE:
        case RSTransitionEffectType::TRANSLATE: {
            typeStr = (type == RSTransitionEffectType::SCALE) ? "SCALE" : "TRANSLATE";
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(value);
            if (property != nullptr) {
                valueStr = "Vector2f:x:" + std::to_string(property->Get().x_) + "," +
                    "y:" + std::to_string(property->Get().y_);
            }
            break;
        }
        case RSTransitionEffectType::ROTATE: {
            typeStr = "ROTATE";
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Quaternion>>(value);
            if (property != nullptr) {
                valueStr = "Quaternion:x:" + std::to_string(property->Get().x_) + "," +
                    "y:" + std::to_string(property->Get().y_) + "," +
                    "z:" + std::to_string(property->Get().z_) + "," +
                    "w:" + std::to_string(property->Get().w_);
            }
            break;
        }
        default: {
            typeStr = "UNDEFINED";
            valueStr = "NONE";
            break;
        }
    }
    std::string traceStr = "transtion frame animation trace propertyId=" + std::to_string(propertyId) + "," +
        "valueFraction=" + std::to_string(fraction) + "," +
        "type=" + typeStr + "," + "value=" + valueStr;
    AddTraceFlag(traceStr);
}

void RSAnimationTraceUtils::AddTraceFlag(const std::string& str)
{
    RS_TRACE_NAME(str.c_str());
}
} // namespace Rosen
} // namespace OHOS
