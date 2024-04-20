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

#include "rs_trace.h"

#include "animation/rs_animation_common.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSAnimationTraceUtils RSAnimationTraceUtils::instance_;

std::string RSAnimationTraceUtils::ParseRenderPropertyVaule(
    const std::shared_ptr<RSRenderPropertyBase>& value, const int16_t type)
{
    std::string str;
    auto propertyType = value->GetPropertyType() == RSRenderPropertyType::INVALID ? type : value->GetPropertyType();
    switch (propertyType) {
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
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(value);
            if (property != nullptr) {
                str = property->Get()->GetDescription();
            } else {
                str = "FILTER";
            }
            break;
        }
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

void RSAnimationTraceUtils::addAnimationNameTrace(const std::string name, const uint64_t nodeId)
{
    if (RSSystemProperties::GetAnimationTraceEnabled()) {
        auto str = name + "node " + std::to_string(nodeId);
        RS_TRACE_NAME(str.c_str());
    }
}

void RSAnimationTraceUtils::addAnimationFinishTrace(const uint64_t nodeId, const uint64_t animationId)
{
    if (RSSystemProperties::GetAnimationTraceEnabled()) {
        RS_TRACE_NAME_FMT("AnimationFinish node[%llu] animate[%llu]", nodeId, animationId);
    }
}

void RSAnimationTraceUtils::addAnimationCreateTrace(const uint64_t nodeId, const uint64_t propertyId,
    const uint64_t animationId, const int animationType, const int propertyType,
    const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue,
    const int animationDelay, const int animationDur)
{
    if (RSSystemProperties::GetAnimationTraceEnabled()) {
        auto startStr = RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(startValue);
        auto endStr = RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(endValue);
        RS_TRACE_NAME_FMT(
            "CreateImplicitAnimation node[%llu] pro[%llu] animate[%llu], animateType %d, propertyType %d, " \
            "startValue[%s], endValue[%s], delay %d, dur %d",
            nodeId, propertyId, animationId, animationType, propertyType, startStr.c_str(), endStr.c_str(),
            animationDelay, animationDur);
    }
}

void RSAnimationTraceUtils::addAnimationFrameTrace(const uint64_t nodeId, const uint64_t animationId,
    const uint64_t propertyId, const float fraction, const std::shared_ptr<RSRenderPropertyBase>& value,
    const int64_t time)
{
    if (RSSystemProperties::GetAnimationTraceEnabled()) {
        auto propertyValue = ParseRenderPropertyVaule(value);
        RS_TRACE_NAME_FMT("frame animation node[%llu] pro[%llu] animate[%llu], fraction %f, value[%s], time[%lld]",
            nodeId, propertyId, animationId, fraction, propertyValue.c_str(), time);
    }
}

void addSpringInitialVelocityTrace(const uint64_t propertyId, const uint64_t animationId,
        const std::shared_ptr<RSRenderPropertyBase>& initialVelocity,
        const std::shared_ptr<RSRenderPropertyBase>& value)
{
    if (RSSystemProperties::GetAnimationTraceEnabled()) {
        auto propertyValue = ParseRenderPropertyVaule(initialVelocity,value->GetPropertyType());
        RS_TRACE_NAME_FMT("spring pro[%llu] animate[%llu], initialVelocity[%s]",
            propertyId, animationId, propertyValue.c_str());
    }
}

void RSAnimationTraceUtils::addTranstionFrameTrace(
    const uint64_t nodeId, const uint64_t animationId, const float fraction, const float valueFraction)
{
    if (RSSystemProperties::GetAnimationTraceEnabled()) {
        RS_TRACE_NAME_FMT("transtion frame animation node[%llu] animate[%llu], fraction %f, value %f", nodeId,
            animationId, fraction, valueFraction);
    }
}

void RSAnimationTraceUtils::addTranstionEffectValueTrace(const uint64_t propertyId, const float fraction,
    const uint16_t type, const std::shared_ptr<RSRenderPropertyBase>& value)
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
    auto node = value->node_.lock();
    [[maybe_unused]] int nodeId = node == nullptr ? 0 : node->GetId();
    RS_TRACE_NAME_FMT("transtion frame animation node[%llu] pro[%llu], fraction %f, type %s, value[%s]", nodeId,
        propertyId, fraction, typeStr.c_str(), valueStr.c_str());
}

void RSAnimationTraceUtils::addRenderNodeTrace(const RSRenderNode& node, const std::string name)
{
    if (!RSSystemProperties::GetAnimationTraceEnabled()) {
        return;
    }
    [[maybe_unused]] auto alpha = node.GetGlobalAlpha();
    [[maybe_unused]] auto rotation = node.GetRenderProperties().GetRotation();
    [[maybe_unused]] auto bounds = node.GetRenderProperties().GetBoundsRect().ToString();
    [[maybe_unused]] auto translateX = node.GetRenderProperties().GetTranslateX();
    [[maybe_unused]] auto translateY = node.GetRenderProperties().GetTranslateY();
    [[maybe_unused]] auto absRect =
        std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry())->GetAbsRect()
            .ToString();
    [[maybe_unused]] auto scaleX = node.GetRenderProperties().GetScaleX();
    [[maybe_unused]] auto scaleY = node.GetRenderProperties().GetScaleY();
    [[maybe_unused]] auto cornerRadius = node.GetRenderProperties().GetCornerRadius().x_;
    std::string nameStr;
    if (name.empty()) {
        nameStr = "ProcessCanvasRenderNode:";
    } else {
        nameStr = "ProcessSurfaceRenderNode:name " + name;
    }
    RS_TRACE_NAME_FMT("%s node[%llu] alpha %f rotation %f bounds %s, translateX %f translateY %f absRect %s, " \
                      "scaleX %f scaleY %f cornerRadius %f",
        nameStr.c_str(), node.GetId(), alpha, rotation, bounds.c_str(), translateX, translateY, absRect.c_str(),
        scaleX, scaleY, cornerRadius);
}
} // namespace Rosen
} // namespace OHOS