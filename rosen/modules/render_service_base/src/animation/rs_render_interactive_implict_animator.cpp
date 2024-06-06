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

#include "animation/rs_render_interactive_implict_animator.h"
#include "pipeline/rs_context.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSRenderInteractiveImplictAnimator::RSRenderInteractiveImplictAnimator(
    InteractiveImplictAnimatorId id, const std::weak_ptr<RSContext>& context) : id_(id), context_(context)
{}

void RSRenderInteractiveImplictAnimator::AddAnimations(std::vector<std::pair<NodeId, AnimationId>>& animations)
{
    auto context = context_.lock();
    if (context == nullptr) {
        RS_LOGE("RSRenderInteractiveImplictAnimator::AddAnimations, context is nullptr");
        return;
    }
    animations_.clear();
    for (auto& [nodeId, animationId] : animations) {
        auto animation = FindAnimation(nodeId, animationId);
        if (animation == nullptr) {
            RS_LOGE("AddAnimations find animation failed,node:%{public}" PRIu64 ""
                " animation:%{public}" PRIu64 " is nullptr", nodeId, animationId);
            continue;
        }
        animations_.emplace_back(nodeId, animationId);
    }
}

void RSRenderInteractiveImplictAnimator::PauseAnimator()
{
    auto context = context_.lock();
    if (context == nullptr) {
        RS_LOGE("RSRenderInteractiveImplictAnimator::PauseAnimator, context is nullptr");
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto animation = FindAnimation(nodeId, animationId);
        if (animation == nullptr) {
            RS_LOGE("PauseAnimator find animation failed,node:%{public}" PRIu64 ""
                " animation:%{public}" PRIu64 " is nullptr", nodeId, animationId);
            continue;
        }
        animation->Pause();
    }
}

void RSRenderInteractiveImplictAnimator::ContinueAnimator()
{
    auto context = context_.lock();
    if (context == nullptr) {
        RS_LOGE("RSRenderInteractiveImplictAnimator::ContinueAnimator, context is nullptr");
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            RS_LOGE("RSRenderInteractiveImplictAnimator::ContinueAnimator,"
                "node:%{public}" PRIu64 " is nullptr", nodeId);
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
        if (animation == nullptr) {
            RS_LOGE("RSRenderInteractiveImplictAnimator::ContinueAnimator,"
                "animation:%{public}" PRIu64 " is nullptr", animationId);
            continue;
        }
        animation->Resume();
        // register node on animation start or resume
        context->RegisterAnimatingRenderNode(node);
    }
}

void RSRenderInteractiveImplictAnimator::FinishAnimator(RSInteractiveAnimationPosition finishPosition)
{
    auto context = context_.lock();
    if (context == nullptr) {
        RS_LOGE("RSRenderInteractiveImplictAnimator::FinishAnimator, context is nullptr");
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto animation = FindAnimation(nodeId, animationId);
        if (animation == nullptr) {
            RS_LOGE("FinishAnimator find animation failed,node:%{public}" PRIu64 ""
                " animation:%{public}" PRIu64 " is nullptr", nodeId, animationId);
            continue;
        }
        animation->FinishOnPosition(finishPosition);
    }
}

void RSRenderInteractiveImplictAnimator::ReverseAnimator()
{
    auto context = context_.lock();
    if (context == nullptr) {
        RS_LOGE("RSRenderInteractiveImplictAnimator::ReverseAnimator, context is nullptr");
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            RS_LOGE("RSRenderInteractiveImplictAnimator::ReverseAnimator,"
                "node:%{public}" PRIu64 " is nullptr", nodeId);
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
        if (animation == nullptr) {
            RS_LOGE("RSRenderInteractiveImplictAnimator::ReverseAnimator,"
                "animation:%{public}" PRIu64 " is nullptr", animationId);
            continue;
        }
        animation->SetReversedAndContinue();
        context->RegisterAnimatingRenderNode(node);
    }
}

void RSRenderInteractiveImplictAnimator::SetFractionAnimator(float fraction)
{
    auto context = context_.lock();
    if (context == nullptr) {
        RS_LOGE("RSRenderInteractiveImplictAnimator::SetFractionAnimator, context is nullptr");
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto animation = FindAnimation(nodeId, animationId);
        if (animation == nullptr) {
            RS_LOGE("SetFractionAnimator find animation failed,node:%{public}" PRIu64 ""
                " animation:%{public}" PRIu64 " is nullptr", nodeId, animationId);
            continue;
        }
        animation->SetFraction(fraction);
    }
}

std::shared_ptr<OHOS::Rosen::RSRenderAnimation> RSRenderInteractiveImplictAnimator::FindAnimation(
    NodeId nodeId, AnimationId animationId)
{
    auto context = context_.lock();
    if (context == nullptr) {
        return nullptr;
    }
    auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    if (node != nullptr) {
        return node->GetAnimationManager().GetAnimation(animationId);
    }
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS
