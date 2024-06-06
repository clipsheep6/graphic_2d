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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERACTIVE_IMPLICT_ANIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERACTIVE_IMPLICT_ANIMATOR_H

#include <vector>

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum class RSInteractiveAnimationState {
    INACTIVE,
    ACTIVE,
    RUNNING,
    PAUSED,
};

class RSC_EXPORT RSInteractiveImplictAnimator : public std::enable_shared_from_this<RSInteractiveImplictAnimator> {
public:
    virtual ~RSInteractiveImplictAnimator();

    /*
     * @brief create interactiveIplictAnimator object only this way
     * @param timingProtocol property set change create animation use this timingProtocal when AddAnimation
     * @param timingCurve property set change create animation use this timingCurve when AddAnimation
     * @return Returns interactiveIplictAnimator object
     *
     */
    static std::shared_ptr<RSInteractiveImplictAnimator> Create(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);

    /*
     * @brief add animations form callback
     * @param callback use property set change or RSNode::Animate to create aniamtion
     *                 property set use animator protocal and curve create animation
     *                 RSNode::Animate use self Animate protocal and curve crate aniamtion
     * @param isSupportPropertyChangeCreate default is true can use both to create animation
     *                 isAddImplictAnimation is false just can use RSNode::Animate to crate animation
     * @return Returns size_t the number of this callback create animations
     *
     */
    size_t AddAnimation(std::function<void()> callback, bool isSupportPropertyChangeCreate = true);

    // aniamtor operation to start all animations in animator
    int32_t StartAnimation();

    // aniamtor operation to pause all animations in animator
    void PauseAnimation();

    // aniamtor operation to continue all animations in animator
    void ContinueAnimation();

    /*
     * @brief finsih all animation in animator
     * @param position position is START or CURRENT or END
     *                  START all animaton in animator finish on startvalue and sync stagevalue
     *                  CURRENT all animaton in animator finish on currentvalue and sync stagevalue
     *                  START all animaton in animator finish on endvalue and sync stagevalue
     */
    void FinishAnimation(RSInteractiveAnimationPosition position);

    // animation reverse play to start position
    void ReverseAnimation();

    /*
     * @brief set value fraction for all animations
     *          but just support curve animation and interpolating spring aniamtion
     *          curve animation not support step interpolater and custom interpolater
     * @param fraction animation value faraction
     */
    void SetFraction(float fraction);

    /*
     * @brief get value fraction for first animation support interactiveImplictAnimator
     * @return Returns the fraction of animation value
     *
     */
    float GetFraction();

    // return animator current status
    RSInteractiveAnimationState GetStatus() const
    {
        return state_;
    }

    // estimate animator is in running status
    bool IsRunning();

    // estimate animator is in pause status
    bool IsPaused();

    // estimate animator is in finish status
    bool IsFinished();

    /*
     * @brief set callback of all animation finish
     * @param finishCallback all animations in animator use this finishcallback, just one
     */
    void SetFinishCallBack(const std::function<void()>& finishCallback);

protected:
    explicit RSInteractiveImplictAnimator(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);

private:
    static InteractiveImplictAnimatorId GenerateId();
    const InteractiveImplictAnimatorId id_;

    static void InitUniRenderEnabled();
    bool IsUniRenderEnabled() const;
    void FinishOnCurrent();
    void CallFinishCallback();
    std::shared_ptr<InteractiveAnimatorFinishCallback> GetAnimatorFinishCallback();

    RSInteractiveAnimationState state_ { RSInteractiveAnimationState::INACTIVE };
    RSAnimationTimingProtocol timingProtocol_;
    RSAnimationTimingCurve timingCurve_;

    std::vector<std::pair<std::weak_ptr<RSAnimation>, NodeId>> animations_;
    std::function<void()> finishCallback_;
    std::weak_ptr<InteractiveAnimatorFinishCallback> animatorCallback_;
    AnimationId fractionAnimationId_ { 0 };
    NodeId fractionNodeId_ { 0 };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERACTIVE_IMPLICT_ANIMATOR_H
