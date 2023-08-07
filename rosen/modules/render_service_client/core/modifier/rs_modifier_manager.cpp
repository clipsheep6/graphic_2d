
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

#include "modifier/rs_modifier_manager.h"

#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "modifier/rs_property_modifier.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
void RSModifierManager::AddModifier(const std::shared_ptr<RSModifier>& modifier)
{
    modifiers_.insert(modifier);
}

void RSModifierManager::Draw()
{
    if (modifiers_.empty()) {
        return;
    }

    RS_TRACE_NAME("RSModifierManager Draw num:[" + std::to_string(modifiers_.size()) + "]");
    for (auto modifier : modifiers_) {
        RS_TRACE_NAME("RSModifier::Draw");
        modifier->UpdateToRender();
        modifier->SetDirty(false);
        modifier->ResetRSNodeExtendModifierDirty();
    }
    modifiers_.clear();
}

void RSModifierManager::AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    AnimationId key = animation->GetAnimationId();
    if (animations_.find(key) != animations_.end()) {
        ROSEN_LOGE("RSModifierManager::AddAnimation, The animation already exists when is added");
        return;
    }
    animations_.emplace(key, animation);
}

void RSModifierManager::RemoveAnimation(AnimationId keyId)
{
    auto animationItr = animations_.find(keyId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSModifierManager::RemoveAnimation, The Animation does not exist when is deleted");
        return;
    }
    animations_.erase(animationItr);
}

bool RSModifierManager::Animate(int64_t time)
{
    RS_TRACE_NAME("RunningCustomAnimation num:[" + std::to_string(animations_.size()) + "]");
    // process animation
    bool hasRunningAnimation = false;

    // iterate and execute all animations, remove finished animations
    EraseIf(animations_, [this, &hasRunningAnimation, time](auto& iter) -> bool {
        auto animation = iter.second.lock();
        if (animation == nullptr) {
            return true;
        }
        bool isFinished = animation->Animate(time);
        if (isFinished) {
            OnAnimationFinished(animation);
        } else {
            hasRunningAnimation = animation->IsRunning() || hasRunningAnimation;
        }
        return isFinished;
    });

    return hasRunningAnimation;
}

void RSModifierManager::OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation)
{
    NodeId targetId = animation->GetTargetId();
    AnimationId animationId = animation->GetAnimationId();

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(targetId, animationId, FINISHED);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), command);

    animation->Detach();
}
} // namespace Rosen
} // namespace OHOS
