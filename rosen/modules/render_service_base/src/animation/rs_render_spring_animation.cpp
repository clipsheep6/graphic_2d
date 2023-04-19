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

#include "animation/rs_render_spring_animation.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr static float SECOND_TO_MILLISECOND = 1e3;
constexpr static float MILLISECOND_TO_SECOND = 1e-3;
constexpr static float SECOND_TO_NANOSECOND = 1e9;
constexpr static float RESPONSE_THRESHOLD = 0.001f;
constexpr static float FRACTION_THRESHOLD = 0.001f;
} // namespace

RSRenderSpringAnimation::RSRenderSpringAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue, const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue), RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>(),
      startValue_(startValue), endValue_(endValue)
{
    // spring model is not initialized, so we can't calculate estimated duration
    SetAnimationTimingMode(AnimationTimingMode::BY_TIME);
}

void RSRenderSpringAnimation::SetSpringParameters(float response, float dampingRatio, float blendDuration)
{
    response_ = response;
    finalResponse_ = response;
    dampingRatio_ = dampingRatio;
    blendDuration_ = blendDuration * SECOND_TO_NANOSECOND; // convert to ns
}

bool RSRenderSpringAnimation::Animate(int64_t time)
{
    switch (timingMode_) {
        case AnimationTimingMode::BY_FRACTION: {
            auto isFinished = RSRenderAnimation::Animate(time);
            return isFinished;
        }
        case AnimationTimingMode::BY_TIME: {
            if (!IsRunning()) {
                ROSEN_LOGI("RSRenderAnimation::Animate, IsRunning is false!");
                return state_ == AnimationState::FINISHED;
            }

            // set start time and return
            if (needUpdateStartTime_) {
                SetStartTime(time);
                return state_ == AnimationState::FINISHED;
            }

            // if time not changed since last frame, return
            if (time == animationFraction_.GetLastFrameTime()) {
                return state_ == AnimationState::FINISHED;
            }

            if (needInitialize_) {
                // normally this only run once, but in spring animation with blendDuration, it may run multiple times
                OnInitialize(time);
            }
            auto [playTime, isInStartDelay] = animationFraction_.GetAnimationPlayTime(time);
            if (isInStartDelay) {
                auto fillMode = GetFillMode();
                if (fillMode == FillMode::BACKWARDS || fillMode == FillMode::BOTH) {
                    OnAnimateByTime(0);
                }
                ROSEN_LOGD("RSRenderSpringAnimation::Animate, isInStartDelay is true");
                return false;
            }
            auto isFinished = OnAnimateByTime(playTime);
            animationFraction_.UpdateReverseState(isFinished);
            if (isFinished) {
                auto fillMode = GetFillMode();
                if (fillMode == FillMode::NONE || fillMode == FillMode::BACKWARDS) {
                    OnRemoveOnCompletion();
                }
                ROSEN_LOGD("RSRenderAnimation::Animate, isFinished is true");
                return true;
            }
            return isFinished;
        }
        default: {
            return false;
        }
    }
}

#ifdef ROSEN_OHOS
bool RSRenderSpringAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }

    if (!(RSMarshallingHelper::Marshalling(parcel, response_) &&
            RSMarshallingHelper::Marshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, blendDuration_))) {
        return false;
    }

    return true;
}

RSRenderSpringAnimation* RSRenderSpringAnimation::Unmarshalling(Parcel& parcel)
{
    auto* renderSpringAnimation = new RSRenderSpringAnimation();
    if (!renderSpringAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::Unmarshalling, failed");
        delete renderSpringAnimation;
        return nullptr;
    }
    return renderSpringAnimation;
}

bool RSRenderSpringAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        return false;
    }

    if (!(RSMarshallingHelper::Unmarshalling(parcel, response_) &&
            RSMarshallingHelper::Unmarshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, blendDuration_))) {
        return false;
    }
    // copy response to final response
    finalResponse_ = response_;

    return true;
}
#endif

void RSRenderSpringAnimation::OnSetFraction(float fraction)
{
    // spring animation should not support set fraction
    ROSEN_LOGE("RSRenderSpringAnimation not support to set fraction");
    return;
}

void RSRenderSpringAnimation::OnAnimate(float fraction)
{
    if (GetPropertyId() == 0) {
        return;
    } else if (ROSEN_EQ(fraction, 1.0f, FRACTION_THRESHOLD)) {
        SetAnimationValue(endValue_);
        prevMappedTime_ = GetDuration() * MILLISECOND_TO_SECOND;
        return;
    }
    auto mappedTime = fraction * GetDuration() * MILLISECOND_TO_SECOND;
    auto displacement = CalculateDisplacement(mappedTime);
    SetAnimationValue(endValue_ + displacement);

    // keep the mapped time, this will be used to calculate instantaneous velocity
    prevMappedTime_ = mappedTime;
}

bool RSRenderSpringAnimation::OnAnimateByTime(int64_t time)
{
    bool isFinished = false;
    if (GetPropertyId() == 0) {
        return true;
    }
    float timeInSecond = static_cast<float>(time * (1 / SECOND_TO_NANOSECOND));
    auto displacement = CalculateDisplacement(timeInSecond);
    auto curValue = animationFraction_.CurrentIsReverseCycle() ? startValue_ - displacement : endValue_ + displacement;
    auto targetValue = animationFraction_.CurrentIsReverseCycle() ? startValue_ : endValue_;
    if (curValue->IsNearSame(targetValue)) {
        constexpr double TIME_INTERVAL = 1e-6f; // 1e-6f : 1 microsecond to seconds
        constexpr double VELOCITY_COEFF = 1.0f / 60.0f;
        auto velocity = (CalculateDisplacement(timeInSecond + TIME_INTERVAL) - displacement) * (1 / TIME_INTERVAL);
        velocity = velocity * VELOCITY_COEFF;
        auto zero = velocity - velocity;
        // only when the velocity is also close to 0, can the spring animation be seen as finished
        if (velocity->IsNearSame(zero)) {
            SetAnimationValue(targetValue);
            animationFraction_.OnCurrentAnimationRoundFinish();
            animationFraction_.ResetPlayTime();
        }
        if (animationFraction_.GetLeftRepeatCount() == 0) {
            isFinished = true;
        }
    }
    SetAnimationValue(curValue);
    // keep the mapped time, this will be used to calculate instantaneous velocity
    prevMappedTime_ = time;
    return isFinished;
}

void RSRenderSpringAnimation::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::OnAttach, target is nullptr");
        return;
    }
    // check if any other spring animation running on this property
    auto propertyId = GetPropertyId();
    auto prevAnimation = target->GetAnimationManager().QuerySpringAnimation(propertyId);
    target->GetAnimationManager().RegisterSpringAnimation(propertyId, GetAnimationId());

    // return if no other spring animation(s) running, or the other animation is finished
    // meanwhile, align run time for both spring animations, prepare for status inheritance
    if (prevAnimation == nullptr || prevAnimation->Animate(animationFraction_.GetLastFrameTime())) {
        blendDuration_ = 0;
        return;
    }

    // extract spring status from previous spring animation
    auto prevSpringAnimation = std::static_pointer_cast<RSRenderSpringAnimation>(prevAnimation);

    // inherit spring status from previous spring animation
    InheritSpringStatus(prevSpringAnimation.get());
    // inherit spring response
    response_ = prevSpringAnimation->response_;
    if (ROSEN_EQ(response_, finalResponse_, RESPONSE_THRESHOLD)) {
        // if response is not changed, we can skip blend duration
        blendDuration_ = 0;
    } else if (blendDuration_ == 0) {
        // if blend duration is not set, we can skip blend duration
        response_ = finalResponse_;
    } else if (ROSEN_EQ(finalResponse_, prevSpringAnimation->finalResponse_, RESPONSE_THRESHOLD)) {
        // if previous spring is blending to the same final response, we can continue previous blending process
        blendDuration_ = prevSpringAnimation->blendDuration_;
    }

    // set previous spring animation to FINISHED
    prevSpringAnimation->FinishOnCurrentPosition();
}

void RSRenderSpringAnimation::OnDetach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::OnDetach, target is nullptr");
        return;
    }
    auto propertyId = GetPropertyId();
    auto id = GetAnimationId();
    target->GetAnimationManager().UnregisterSpringAnimation(propertyId, id);
}

void RSRenderSpringAnimation::OnInitialize(int64_t time)
{
    if (blendDuration_) {
        auto lastFrameTime = animationFraction_.GetLastFrameTime();

        // reset animation fraction
        InheritSpringStatus(this);
        animationFraction_.ResetFraction();
        prevMappedTime_ = 0.0f;

        // blend response by linear interpolation
        uint64_t blendTime = (time - lastFrameTime) * animationFraction_.GetAnimationScale();
        if (blendTime < blendDuration_) {
            auto blendRatio = static_cast<float>(blendTime) / static_cast<float>(blendDuration_);
            response_ += (finalResponse_ - response_) * blendRatio;
            blendDuration_ -= blendTime;
        } else {
            // if blend duration is over, set response to final response
            response_ = finalResponse_;
            blendDuration_ = 0;
        }
    }

    // set the initial status of spring model
    initialOffset_ = startValue_ - endValue_;
    if (initialVelocity_ == nullptr) {
        initialVelocity_ = initialOffset_ * 0.f;
    }
    CalculateSpringParameters();

    if (blendDuration_) {
        // blend is still in progress, no need to estimate duration, use 300ms as default
        SetDuration(300);
    } else {
        // blend finished, estimate duration until the spring system reaches rest
        SetDuration(std::lroundf(EstimateDuration() * SECOND_TO_MILLISECOND));
        // this will set needInitialize_ to false
        RSRenderPropertyAnimation::OnInitialize(time);
    }
}

std::tuple<std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSRenderPropertyBase>, int64_t>
RSRenderSpringAnimation::GetSpringStatus() const
{
    // if animation is never started, return start value and initial velocity
    if (ROSEN_EQ(prevMappedTime_, 0.0f, FRACTION_THRESHOLD)) {
        return { startValue_, initialVelocity_, animationFraction_.GetLeftDelayTime() };
    }

    auto displacement = lastValue_ - endValue_;

    // use average velocity over a small time interval to estimate instantaneous velocity
    constexpr double TIME_INTERVAL = 1e-6f; // 1e-6f : 1 microsecond to seconds
    auto velocity = (CalculateDisplacement(prevMappedTime_ + TIME_INTERVAL) - displacement) * (1 / TIME_INTERVAL);

    // return current position and velocity
    return { lastValue_->Clone(), velocity, animationFraction_.GetLeftDelayTime() };
}

void RSRenderSpringAnimation::InheritSpringStatus(const RSRenderSpringAnimation* from)
{
    // inherit spring status from another spring animation
    int64_t leftDelayTime = 0;
    std::tie(startValue_, initialVelocity_, leftDelayTime) = from->GetSpringStatus();
    originValue_ = startValue_->Clone();
    lastValue_ = startValue_->Clone();
    animationFraction_.SetLeftDelayTime(leftDelayTime);
}
} // namespace Rosen
} // namespace OHOS
