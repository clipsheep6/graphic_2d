/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "animation/rs_render_transition_effect.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_value_estimator.h"
#include "modifier/rs_render_modifier.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
namespace {
enum RSTransitionEffectType : uint16_t {
    FADE = 1,
    SCALE,
    TRANSLATE,
    ROTATE,
    UNDEFINED,
};
}

RSRenderTransitionEffect* RSRenderTransitionEffect::Unmarshalling(Parcel& parcel)
{
    uint16_t transitionType = 0;
    if (!parcel.ReadUint16(transitionType)) {
        ROSEN_LOGE("RSRenderTransitionEffect::Unmarshalling, ParseParam Failed");
        return nullptr;
    }
    switch (transitionType) {
        case RSTransitionEffectType::FADE:
            return RSTransitionFade::Unmarshalling(parcel);
        case RSTransitionEffectType::SCALE:
            return RSTransitionScale::Unmarshalling(parcel);
        case RSTransitionEffectType::TRANSLATE:
            return RSTransitionTranslate::Unmarshalling(parcel);
        case RSTransitionEffectType::ROTATE:
            return RSTransitionRotate::Unmarshalling(parcel);
        default:
            return nullptr;
    }
}

bool RSTransitionFade::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint16(RSTransitionEffectType::FADE) && parcel.WriteFloat(alpha_);
}

RSRenderTransitionEffect* RSTransitionFade::Unmarshalling(Parcel& parcel)
{
    float alpha;
    if (!RSMarshallingHelper::Unmarshalling(parcel, alpha)) {
        ROSEN_LOGE("RSTransitionFade::Unmarshalling, unmarshalling alpha failed");
        return nullptr;
    }
    return new RSTransitionFade(alpha);
}

bool RSTransitionScale::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint16(RSTransitionEffectType::SCALE) && parcel.WriteFloat(scaleX_) &&
           parcel.WriteFloat(scaleY_) && parcel.WriteFloat(scaleZ_);
}

RSRenderTransitionEffect* RSTransitionScale::Unmarshalling(Parcel& parcel)
{
    float scaleX;
    float scaleY;
    float scaleZ;
    if (!parcel.ReadFloat(scaleX) || !parcel.ReadFloat(scaleY) || !parcel.ReadFloat(scaleZ)) {
        ROSEN_LOGE("RSTransitionScale::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionScale(scaleX, scaleY, scaleZ);
}

bool RSTransitionTranslate::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint16(RSTransitionEffectType::TRANSLATE) && parcel.WriteFloat(translateX_) &&
           parcel.WriteFloat(translateY_) && parcel.WriteFloat(translateZ_);
}

RSRenderTransitionEffect* RSTransitionTranslate::Unmarshalling(Parcel& parcel)
{
    float translateX;
    float translateY;
    float translateZ;
    if (!parcel.ReadFloat(translateX) || !parcel.ReadFloat(translateY) || !parcel.ReadFloat(translateZ)) {
        ROSEN_LOGE("RSTransitionTranslate::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionTranslate(translateX, translateY, translateZ);
}

bool RSTransitionRotate::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint16(RSTransitionEffectType::ROTATE) && parcel.WriteFloat(dx_) && parcel.WriteFloat(dy_) &&
           parcel.WriteFloat(dz_) && parcel.WriteFloat(angle_);
}

RSRenderTransitionEffect* RSTransitionRotate::Unmarshalling(Parcel& parcel)
{
    Quaternion quaternion;
    float dx;
    float dy;
    float dz;
    float angle;
    if (!parcel.ReadFloat(dx) || !parcel.ReadFloat(dy) || !parcel.ReadFloat(dz) || !parcel.ReadFloat(angle)) {
        ROSEN_LOGE("RSTransitionRotate::Unmarshalling, unmarshalling failed");
        return nullptr;
    }
    return new RSTransitionRotate(dx, dy, dz, angle);
}
#endif

const std::shared_ptr<RSRenderModifier>& RSTransitionFade::GetModifier()
{
    if (modifier_ == nullptr || property_ == nullptr) {
        property_ = std::make_shared<RSRenderAnimatableProperty<float>>();
        modifier_ = std::make_shared<RSAlphaRenderModifier>(property_);
    }
    return modifier_;
}

void RSTransitionFade::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    float startValue(1.0f);
    float endValue(alpha_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier>& RSTransitionScale::GetModifier()
{
    if (modifier_ == nullptr || property_ == nullptr) {
        property_ = std::make_shared<RSRenderAnimatableProperty<Vector2f>>();
        modifier_ = std::make_shared<RSScaleRenderModifier>(property_);
    }
    return modifier_;
}

void RSTransitionScale::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    Vector2f startValue(1.0f, 1.0f);
    Vector2f endValue(scaleX_, scaleY_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier>& RSTransitionTranslate::GetModifier()
{
    if (modifier_ == nullptr || property_ == nullptr) {
        property_ = std::make_shared<RSRenderAnimatableProperty<Vector2f>>();
        modifier_ = std::make_shared<RSTranslateRenderModifier>(property_);
    }
    return modifier_;
}

void RSTransitionTranslate::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    Vector2f startValue(0.0f, 0.0f);
    Vector2f endValue(translateX_, translateY_);
    auto value = startValue * (1.0f - fraction) + endValue * fraction;
    property_->Set(value);
}

const std::shared_ptr<RSRenderModifier>& RSTransitionRotate::GetModifier()
{
    if (modifier_ == nullptr || property_ == nullptr) {
        property_ = std::make_shared<RSRenderAnimatableProperty<Quaternion>>();
        modifier_ = std::make_shared<RSQuaternionRenderModifier>(property_);
    }
    return modifier_;
}

void RSTransitionRotate::UpdateFraction(float fraction) const
{
    if (property_ == nullptr) {
        return;
    }
    auto angle = angle_ * fraction;
    Quaternion value(dx_, dy_, dz_, angle);
    property_->Set(value);
}
} // namespace Rosen
} // namespace OHOS
