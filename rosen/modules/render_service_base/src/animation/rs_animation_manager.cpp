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

#include "animation/rs_animation_manager.h"

#include <algorithm>
#include <string>

#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_transition_properties.h"

namespace OHOS {
namespace Rosen {
class RSRootRenderNode;

void RSAnimationManager::AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    AnimationId key = animation->GetAnimationId();
    if (animations_.find(key) != animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::AddAnimation, The animation already exists when is added");
        return;
    }
    animations_.emplace(key, animation);
    OnAnimationAdd(animation);
}

void RSAnimationManager::ClearAnimation()
{
    animations_.clear();
    animationNum_.clear();
    transition_.clear();
    springAnimations_.clear();
}

void RSAnimationManager::RemoveAnimation(AnimationId keyId)
{
    auto animationItr = animations_.find(keyId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::RemoveAnimation, The Animation does not exist when is deleted");
        return;
    }
    OnAnimationRemove(animationItr->second);
    animations_.erase(animationItr);
}

bool RSAnimationManager::Animate(int64_t time)
{
    // process animation
    bool hasRunningAnimation = false;

    // iterate and execute all animations, remove finished animations
    std::__libcpp_erase_if_container(animations_, [this, &hasRunningAnimation, time](auto& iter) -> bool {
        auto& animation = iter.second;
        bool isFinished = animation->Animate(time);
        if (isFinished) {
            OnAnimationFinished(animation);
        } else {
            hasRunningAnimation = animation->IsRunning() || hasRunningAnimation ;
        }
        return isFinished;
    });

    return hasRunningAnimation;
}

const std::shared_ptr<RSRenderAnimation> RSAnimationManager::GetAnimation(AnimationId id) const
{
    auto animationItr = animations_.find(id);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::GetAnimation, animtor[%lld] is not found", id);
        return nullptr;
    }
    return animationItr->second;
}

void RSAnimationManager::OnAnimationRemove(const std::shared_ptr<RSRenderAnimation>& animation)
{
    animationNum_[animation->GetPropertyId()]--;
}

void RSAnimationManager::OnAnimationAdd(const std::shared_ptr<RSRenderAnimation>& animation)
{
    animationNum_[animation->GetPropertyId()]++;
}

namespace {
    inline constexpr uint32_t ExtractPid(AnimationId animId)
    {
        return animId >> 32;
    }
}

void RSAnimationManager::OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation)
{
    NodeId targetId = animation->GetTarget() ? animation->GetTarget()->GetId() : 0;
    AnimationId animationId = animation->GetAnimationId();

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationFinishCallback>(targetId, animationId);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), command);
    OnAnimationRemove(animation);
    animation->Detach();
}

void RSAnimationManager::RegisterTransition(AnimationId id, const TransitionCallback& transition, bool isTransitionIn)
{
    transition_.emplace_back(id, transition, isTransitionIn);
}

void RSAnimationManager::UnregisterTransition(AnimationId id)
{
    if (transition_.empty()) {
        ROSEN_LOGE("RSAnimationManager::ClearTransition, transition_ is empty");
        return;
    }
    transition_.remove_if([id](auto& transition) -> bool { return id == std::get<NodeId>(transition); });
}

std::unique_ptr<RSTransitionProperties> RSAnimationManager::GetTransitionProperties()
{
    if (transition_.empty()) {
        return nullptr;
    }
    auto transitionProperties = std::make_unique<RSTransitionProperties>();
    for (auto& transition : transition_) {
        auto& callback = std::get<TransitionCallback>(transition);
        if (callback != nullptr) {
            callback(transitionProperties);
        }
    }
    return transitionProperties;
}

bool RSAnimationManager::HasDisappearingTransition() const
{
    return !transition_.empty() && std::any_of(transition_.begin(), transition_.end(), [](auto& transition) -> bool {
        // return true if it has disappearing animation
        return !std::get<bool>(transition);
    });
}

void RSAnimationManager::RegisterSpringAnimation(PropertyId propertyId, AnimationId animId)
{
    springAnimations_[propertyId] = animId;
}

void RSAnimationManager::UnregisterSpringAnimation(PropertyId propertyId, AnimationId animId)
{
    if (springAnimations_[propertyId] == animId) {
        springAnimations_.erase(propertyId);
    }
}

AnimationId RSAnimationManager::QuerySpringAnimation(PropertyId propertyId)
{
    auto it = springAnimations_.find(propertyId);
    if (it == springAnimations_.end()) {
        return 0;
    }
    return it->second;
}
} // namespace Rosen
} // namespace OHOS
