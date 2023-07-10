/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_animation.h"

#include "command/rs_animation_command.h"
#include "pipeline/rs_canvas_render_node.h"
#include "command/rs_message_processor.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSRenderAnimation::RSRenderAnimation(AnimationId id) : id_(id) {}
bool RSRenderAnimation::Marshalling(Parcel& parcel) const
{
    // animationId, targetId
    if (!(parcel.WriteUint64(id_))) {
        ROSEN_LOGE("RSRenderAnimation::Marshalling, write id failed");
        return false;
    }
    // RSAnimationTimingProtocol
    if (!(parcel.WriteInt32(animationFraction_.GetDuration()) &&
        parcel.WriteInt32(animationFraction_.GetStartDelay()) &&
        parcel.WriteFloat(animationFraction_.GetSpeed()) &&
        parcel.WriteInt32(animationFraction_.GetRepeatCount()) &&
        parcel.WriteBool(animationFraction_.GetAutoReverse()) &&
        parcel.WriteBool(animationFraction_.GetDirection()) &&
        parcel.WriteInt32(static_cast<std::underlying_type<FillMode>::type>(animationFraction_.GetFillMode())) &&
        parcel.WriteBool(animationFraction_.GetRepeatCallbackEnable()) &&
        parcel.WriteInt32(animationFraction_.GetFrameRateRange().min_) &&
        parcel.WriteInt32(animationFraction_.GetFrameRateRange().max_) &&
        parcel.WriteInt32(animationFraction_.GetFrameRateRange().preferred_))) {
        ROSEN_LOGE("RSRenderAnimation::Marshalling, write param failed");
        return false;
    }
    return true;
}

bool RSRenderAnimation::ParseParam(Parcel& parcel)
{
    int32_t duration = 0;
    int32_t startDelay = 0;
    int32_t repeatCount = 0;
    int32_t fillMode = 0;
    float speed = 0.0;
    bool autoReverse = false;
    bool direction = false;
    bool isRepeatCallbackEnable = false;
    int fpsMin = 0;
    int fpsMax = 0;
    int fpsPreferred = 0;
    if (!(parcel.ReadUint64(id_) && parcel.ReadInt32(duration) && parcel.ReadInt32(startDelay) &&
            parcel.ReadFloat(speed) && parcel.ReadInt32(repeatCount) && parcel.ReadBool(autoReverse) &&
            parcel.ReadBool(direction) && parcel.ReadInt32(fillMode) && parcel.ReadBool(isRepeatCallbackEnable) &&
            parcel.ReadInt32(fpsMin) && parcel.ReadInt32(fpsMax) && parcel.ReadInt32(fpsPreferred))) {
        ROSEN_LOGE("RSRenderAnimation::ParseParam, read param failed");
        return false;
    }
    SetDuration(duration);
    SetStartDelay(startDelay);
    SetRepeatCount(repeatCount);
    SetAutoReverse(autoReverse);
    SetSpeed(speed);
    SetDirection(direction);
    SetFillMode(static_cast<FillMode>(fillMode));
    SetRepeatCallbackEnable(isRepeatCallbackEnable);
    return true;
}
AnimationId RSRenderAnimation::GetAnimationId() const
{
    return id_;
}

bool RSRenderAnimation::IsStarted() const
{
    return state_ != AnimationState::INITIALIZED;
}

bool RSRenderAnimation::IsRunning() const
{
    return state_ == AnimationState::RUNNING;
}

bool RSRenderAnimation::IsPaused() const
{
    return state_ == AnimationState::PAUSED;
}

bool RSRenderAnimation::IsFinished() const
{
    return state_ == AnimationState::FINISHED;
}

PropertyId RSRenderAnimation::GetPropertyId() const
{
    return 0;
}

void RSRenderAnimation::Attach(RSRenderNode* renderNode)
{
    if (target_ != nullptr) {
        Detach();
    }
    target_ = renderNode;
    if (target_ != nullptr) {
        targetId_ = target_->GetId();
        target_->CheckGroupableAnimation(GetPropertyId(), true);
    }
    OnAttach();
    Start();
    needUpdateStartTime_ = false;
}

void RSRenderAnimation::Detach()
{
    OnDetach();
    if (target_ != nullptr) {
        target_->CheckGroupableAnimation(GetPropertyId(), false);
    }
    target_ = nullptr;
}

NodeId RSRenderAnimation::GetTargetId() const
{
    return targetId_;
}

void RSRenderAnimation::Start()
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to start animation, animation has started!");
        return;
    }

    state_ = AnimationState::RUNNING;
    needUpdateStartTime_ = true;
    ProcessFillModeOnStart(animationFraction_.GetStartFraction());
}

void RSRenderAnimation::Finish()
{
    if (!IsPaused() && !IsRunning()) {
        ROSEN_LOGE("Failed to finish animation, animation is not running!");
        return;
    }

    state_ = AnimationState::FINISHED;
    ProcessFillModeOnFinish(animationFraction_.GetEndFraction());
}

void RSRenderAnimation::FinishOnCurrentPosition()
{
    if (!IsPaused() && !IsRunning()) {
        ROSEN_LOGE("Failed to finish animation, animation is not running!");
        return;
    }

    state_ = AnimationState::FINISHED;
}

void RSRenderAnimation::Pause()
{
    if (!IsRunning()) {
        ROSEN_LOGE("Failed to pause animation, animation is not running!");
        return;
    }

    state_ = AnimationState::PAUSED;
}

void RSRenderAnimation::Resume()
{
    if (!IsPaused()) {
        ROSEN_LOGE("Failed to resume animation, animation is not paused!");
        return;
    }

    state_ = AnimationState::RUNNING;
    needUpdateStartTime_ = true;
}

void RSRenderAnimation::SetFraction(float fraction)
{
    if (!IsPaused()) {
        ROSEN_LOGE("Failed to set fraction, animation is not paused!");
        return;
    }

    fraction = std::min(std::max(fraction, 0.0f), 1.0f);
    OnSetFraction(fraction);
}

void RSRenderAnimation::SetReversed(bool isReversed)
{
    if (!IsPaused() && !IsRunning()) {
        ROSEN_LOGE("Failed to reverse animation, animation is not running!");
        return;
    }

    animationFraction_.SetDirectionAfterStart(isReversed ? ForwardDirection::REVERSE : ForwardDirection::NORMAL);
}

RSRenderNode* RSRenderAnimation::GetTarget() const
{
    return target_;
}

void RSRenderAnimation::SetFractionInner(float fraction)
{
    animationFraction_.UpdateRemainTimeFraction(fraction);
}

void RSRenderAnimation::OnSetFraction(float fraction)
{
    SetFractionInner(fraction);
}

void RSRenderAnimation::ProcessFillModeOnStart(float startFraction)
{
    if (GetAnimationTimingMode() == AnimationTimingMode::BY_FRACTION) {
        ProcessFillModeOnStartByFraction(startFraction);
    } else {
        ProcessFillModeOnStartByTime();
    }
}

void RSRenderAnimation::ProcessFillModeOnStartByFraction(float startFraction)
{
    auto fillMode = GetFillMode();
    if (fillMode == FillMode::BACKWARDS || fillMode == FillMode::BOTH) {
        OnAnimate(startFraction);
    }
}

void RSRenderAnimation::ProcessFillModeOnStartByTime(float startTime) const {}

void RSRenderAnimation::ProcessFillModeOnFinish(float endFraction)
{
    if (GetAnimationTimingMode() == AnimationTimingMode::BY_FRACTION) {
        ProcessFillModeOnFinishByFraction(endFraction);
    } else {
        ProcessFillModeOnFinishByTime();
    }
}

void RSRenderAnimation::ProcessFillModeOnFinishByFraction(float endFraction)
{
    auto fillMode = GetFillMode();
    if (fillMode == FillMode::FORWARDS || fillMode == FillMode::BOTH) {
        OnAnimate(endFraction);
    } else {
        OnRemoveOnCompletion();
    }
}

void RSRenderAnimation::ProcessOnRepeatFinish()
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(targetId_, id_, REPEAT_FINISHED);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(id_), command);
}

bool RSRenderAnimation::Animate(int64_t time)
{
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

    float currentProgress = 0.0f;
    bool isInStartDelay = false;
    bool isFinished = false;
    bool isRepeatFinished = false;

    // convert time to fraction or real play time in current round of animation
    if (GetAnimationTimingMode() == AnimationTimingMode::BY_FRACTION) {
        std::tie(currentProgress, isInStartDelay, isFinished, isRepeatFinished) =
            animationFraction_.GetAnimationFraction(time);
    } else {
        std::tie(currentProgress, isInStartDelay) = animationFraction_.GetAnimationPlayTime(time);
    }

    if (isInStartDelay) {
        ProcessFillModeOnStart(currentProgress);
        ROSEN_LOGD("RSRenderAnimation::Animate, isInStartDelay is true");
        return false;
    }

    if (GetAnimationTimingMode() == AnimationTimingMode::BY_FRACTION) {
        OnAnimate(currentProgress);
    } else {
        std::tie(isFinished, isRepeatFinished) = OnAnimateByTime(currentProgress);
    }

    if (isRepeatFinished) {
        ProcessOnRepeatFinish();
    }
    if (isFinished) {
        ProcessFillModeOnFinish(currentProgress);
        ROSEN_LOGD("RSRenderAnimation::Animate, isFinished is true");
        return true;
    }
    return isFinished;
}

void RSRenderAnimation::SetStartTime(int64_t time)
{
    animationFraction_.SetLastFrameTime(time);
    needUpdateStartTime_ = false;
}
} // namespace Rosen
} // namespace OHOS
