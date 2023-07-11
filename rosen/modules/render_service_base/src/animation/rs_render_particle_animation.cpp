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

#include "animation/rs_render_particle_animation.h"

#include "animation/rs_value_estimator.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSRenderParticleAnimation::RSRenderParticleAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue, const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue) : RSRenderPropertyAnimation(id, propertyId, originValue),
    startValue_(startValue), endValue_(endValue)
{}

void RSRenderParticleAnimation::SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator)
{
    interpolator_ = interpolator;
}

const std::shared_ptr<RSInterpolator>& RSRenderParticleAnimation::GetInterpolator() const
{
    return interpolator_;
}

void RSRenderParticleAnimation::AttachRenderProperty(const std::shared_ptr<RSRenderPropertyBase>& property)
{
    property_ = property;
    if (property_ == nullptr) {
        return;
    }
    InitValueEstimator();
    if (originValue_ != nullptr) {
        property_->SetPropertyType(originValue_->GetPropertyType());
    }
}

bool RSRenderParticleAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_) && interpolator_ != nullptr &&
            interpolator_->Marshalling(parcel))) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }
    return true;
}

RSRenderParticleAnimation* RSRenderParticleAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderParticleAnimation* renderParticleAnimation = new RSRenderParticleAnimation();
    if (!renderParticleAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Unmarshalling, failed");
        delete renderParticleAnimation;
        return nullptr;
    }
    return renderParticleAnimation;
}

bool RSRenderParticleAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling Fail");
        return false;
    }

    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    SetInterpolator(interpolator);
    return true;
}

void RSRenderParticleAnimation::OnSetFraction(float fraction)
{
    OnAnimateInner(fraction, linearInterpolator_);
    SetFractionInner(valueEstimator_->EstimateFraction(interpolator_));
}

void RSRenderParticleAnimation::OnAnimate(float fraction)
{
    //实现粒子动画仿真
    OnAnimateInner(fraction, interpolator_);
}

void RSRenderParticleAnimation::OnAnimateInner(float fraction, const std::shared_ptr<RSInterpolator>& interpolator)
{
    if (GetPropertyId() == 0) {
        return;
    }

    if (valueEstimator_ == nullptr) {
        return;
    }
    valueEstimator_->UpdateAnimationValue(interpolator_->Interpolate(fraction), GetAdditive());
}

void RSRenderParticleAnimation::InitValueEstimator()
{
    if (valueEstimator_ == nullptr) {
        valueEstimator_ = property_->CreateRSValueEstimator(RSValueEstimatorType::CURVE_VALUE_ESTIMATOR);
    }
    valueEstimator_->InitParticleAnimationValue(property_, startValue_, endValue_, lastValue_);
}
} // namespace Rosen
} // namespace OHOS
