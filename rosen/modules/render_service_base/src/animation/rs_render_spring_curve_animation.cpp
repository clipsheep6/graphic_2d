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

#include "animation/rs_render_spring_curve_animation.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSRenderSpringCurveAnimation::RSRenderSpringCurveAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue,
    const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue),
    RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>(),
    startValue_(startValue), endValue_(endValue)
{
    // spring model is not initialized, so we can't calculate estimated duration
}

void RSRenderSpringCurveAnimation::SetSpringParameters(float response, float dampingRatio, float normalizedInitialVelocity)
{
    response_ = response;
    dampingRatio_ = dampingRatio;
    normalizedInitialVelocity_ = normalizedInitialVelocity;
}

#ifdef ROSEN_OHOS
bool RSRenderSpringCurveAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderSpringCurveAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderSpringCurveAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }

    if (!(RSMarshallingHelper::Marshalling(parcel, response_) &&
            RSMarshallingHelper::Marshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, normalizedInitialVelocity_))) {
        return false;
    }

    return true;
}

RSRenderSpringCurveAnimation* RSRenderSpringCurveAnimation::Unmarshalling(Parcel& parcel)
{
    auto* renderSpringCurveAnimation = new RSRenderSpringCurveAnimation();
    if (!renderSpringCurveAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RenderSpringCurveAnimation::Unmarshalling, failed");
        delete renderSpringCurveAnimation;
        return nullptr;
    }
    return renderSpringCurveAnimation;
}

bool RSRenderSpringCurveAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderSpringCurveAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        return false;
    }

    if (!(RSMarshallingHelper::Unmarshalling(parcel, response_) &&
            RSMarshallingHelper::Unmarshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, normalizedInitialVelocity_))) {
        return false;
    }

    return true;
}
#endif

void RSRenderSpringCurveAnimation::OnSetFraction(float fraction)
{
    // springCurve animation should not support set fraction
    OnAnimate(fraction);
}

void RSRenderSpringCurveAnimation::OnAnimate(float fraction)
{
    if (GetPropertyId() == 0) {
        return;
    } else if (ROSEN_EQ(fraction, 1.0f)) {
        SetAnimationValue(endValue_);
        return;
    }
    auto mappedTime = fraction * GetDuration() * MILLISECOND_TO_SECOND;
    auto displacement = CalculateDisplacement(mappedTime);
    auto animationValue = endValue_ + displacement;
    SetAnimationValue(animationValue);
}

void RSRenderSpringCurveAnimation::OnInitialize()
{
    // set the initial status of spring model
    initialOffset_ = startValue_ - endValue_;
    auto absouluteInitialOffset = initialOffset_->Clone();
    absouluteInitialOffset -> Absolute();
    initialVelocity_ = absouluteInitialOffset * normalizedInitialVelocity_;
    lastValue_ = endValue_ * 0.0f;
    CalculateSpringParameters();
    // use duration calculated by spring model as animation duration
    SetDuration(std::lroundf(EstimateDuration() * SECOND_TO_MILLISECOND));
}
} // namespace Rosen
} // namespace OHOS