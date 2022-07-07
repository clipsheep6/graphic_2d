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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_ANIMATION_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_ANIMATION_COMMAND_H

#include "animation/rs_render_animation.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition.h"
#include "command/rs_command_templates.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
enum RSAnimationCommandType : uint16_t {
    // curve animation
    ANIMATION_CREATE_CURVE_FLOAT,
    ANIMATION_CREATE_CURVE_COLOR,
    ANIMATION_CREATE_CURVE_MATRIX3F,
    ANIMATION_CREATE_CURVE_VEC2F,
    ANIMATION_CREATE_CURVE_VEC4F,
    ANIMATION_CREATE_CURVE_QUATERNION,
    ANIMATION_CREATE_CURVE_FILTER,
    ANIMATION_CREATE_CURVE_VEC4_COLOR,
    // keyframe animation
    ANIMATION_CREATE_KEYFRAME_FLOAT,
    ANIMATION_CREATE_KEYFRAME_COLOR,
    ANIMATION_CREATE_KEYFRAME_MATRIX3F,
    ANIMATION_CREATE_KEYFRAME_VEC2F,
    ANIMATION_CREATE_KEYFRAME_VEC4F,
    ANIMATION_CREATE_KEYFRAME_QUATERNION,
    ANIMATION_CREATE_KEYFRAME_FILTER,
    ANIMATION_CREATE_KEYFRAME_VEC4_COLOR,
    // path animation
    ANIMATION_CREATE_PATH,
    // transition animation
    ANIMATION_CREATE_TRANSITION,
    // spring animation
    ANIMATION_CREATE_SPRING_FLOAT,
    ANIMATION_CREATE_SPRING_COLOR,
    ANIMATION_CREATE_SPRING_MATRIX3F,
    ANIMATION_CREATE_SPRING_VEC2F,
    ANIMATION_CREATE_SPRING_VEC4F,
    ANIMATION_CREATE_SPRING_QUATERNION,
    ANIMATION_CREATE_SPRING_FILTER,
    ANIMATION_CREATE_SPRING_VEC4_COLOR,

    // operations
    ANIMATION_START,
    ANIMATION_PAUSE,
    ANIMATION_RESUME,
    ANIMATION_FINISH,
    ANIMATION_REVERSE,
    ANIMATION_SET_FRACTION,

    // UI operation
    ANIMATION_FINISH_CALLBACK,

    // remove animation
    ANIMATION_REMOVE_ANIMATION_FLOAT,
    ANIMATION_REMOVE_ANIMATION_COLOR,
    ANIMATION_REMOVE_ANIMATION_MATRIX3F,
    ANIMATION_REMOVE_ANIMATION_VEC2F,
    ANIMATION_REMOVE_ANIMATION_VEC4F,
    ANIMATION_REMOVE_ANIMATION_QUATERNION,
    ANIMATION_REMOVE_ANIMATION_FILTER,
    ANIMATION_REMOVE_ANIMATION_VEC4_COLOR,
};

class AnimationCommandHelper {
public:
    template<void (RSRenderAnimation::*OP)()>
    static void AnimOp(RSContext& context, NodeId nodeId, AnimationId animId)
    {
        auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            return;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animId);
        if (animation == nullptr) {
            return;
        }
        (*animation.*OP)();
    }
    template<void (RSRenderAnimation::*OP)()>
    static void AnimOpReg(RSContext& context, NodeId nodeId, AnimationId animId)
    {
        auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            return;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animId);
        if (animation == nullptr) {
            return;
        }
        (*animation.*OP)();
        // register node on animation start or resume
        context.RegisterAnimatingRenderNode(node);
    }
    template<typename T, void (RSRenderAnimation::*OP)(T)>
    static void AnimOp(RSContext& context, NodeId nodeId, AnimationId animId, T param)
    {
        auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            return;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animId);
        if (animation == nullptr) {
            return;
        }
        (*animation.*OP)(param);
    }
    static void CreateAnimation(
        RSContext& context, NodeId targetId, const std::shared_ptr<RSRenderAnimation>& animation)
    {
        auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
        if (node == nullptr) {
            return;
        }
        node->GetAnimationManager().AddAnimation(animation);
        animation->Attach(node.get());
        animation->Start();
        // register node on animation add
        context.RegisterAnimatingRenderNode(node);
    }
    template<typename T>
    static void RemoveAnimation(RSContext& context, NodeId targetId, AnimationId animId, T param)
    {
        auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
        if (node == nullptr) {
            return;
        }
        auto animation = node->GetAnimationManager().GetFinishedAnimation(animId);
        auto renderPropertyAnimation = std::static_pointer_cast<RSRenderPropertyAnimation<T>>(animation);
        if (renderPropertyAnimation == nullptr) {
            return;
        }
        renderPropertyAnimation->SetEndValueBeforeRemove(param);
        node->GetAnimationManager().RemoveFinishedAnimation(animId);
    }

    using FinishCallbackProcessor = void (*)(NodeId, AnimationId);
    static void AnimationFinishCallback(RSContext& context, NodeId targetId, AnimationId animId);
    static void SetFinishCallbackProcessor(FinishCallbackProcessor processor);
};

// animation operation
ADD_COMMAND(RSAnimationStart,
    ARG(ANIMATION, ANIMATION_START, AnimationCommandHelper::AnimOpReg<&RSRenderAnimation::Start>, NodeId, AnimationId))
ADD_COMMAND(RSAnimationPause,
    ARG(ANIMATION, ANIMATION_PAUSE, AnimationCommandHelper::AnimOp<&RSRenderAnimation::Pause>, NodeId, AnimationId))
ADD_COMMAND(RSAnimationResume, ARG(ANIMATION, ANIMATION_RESUME,
                                   AnimationCommandHelper::AnimOpReg<&RSRenderAnimation::Resume>, NodeId, AnimationId))
ADD_COMMAND(RSAnimationFinish,
    ARG(ANIMATION, ANIMATION_FINISH, AnimationCommandHelper::AnimOp<&RSRenderAnimation::Finish>, NodeId, AnimationId))
ADD_COMMAND(RSAnimationReverse,
    ARG(ANIMATION, ANIMATION_REVERSE, AnimationCommandHelper::AnimOp<bool, &RSRenderAnimation::SetReversed>, NodeId,
        AnimationId, bool))
ADD_COMMAND(RSAnimationSetFraction,
    ARG(ANIMATION, ANIMATION_SET_FRACTION, AnimationCommandHelper::AnimOp<float, &RSRenderAnimation::SetFraction>,
        NodeId, AnimationId, float))

ADD_COMMAND(RSAnimationFinishCallback,
    ARG(ANIMATION, ANIMATION_FINISH_CALLBACK, AnimationCommandHelper::AnimationFinishCallback, NodeId, AnimationId))

// create curve animation
ADD_COMMAND(
    RSAnimationCreateCurveFloat, ARG(ANIMATION, ANIMATION_CREATE_CURVE_FLOAT, AnimationCommandHelper::CreateAnimation,
                                     NodeId, std::shared_ptr<RSRenderCurveAnimation<float>>))
ADD_COMMAND(
    RSAnimationCreateCurveColor, ARG(ANIMATION, ANIMATION_CREATE_CURVE_COLOR, AnimationCommandHelper::CreateAnimation,
                                     NodeId, std::shared_ptr<RSRenderCurveAnimation<Color>>))
ADD_COMMAND(RSAnimationCreateCurveMatrix3f,
    ARG(ANIMATION, ANIMATION_CREATE_CURVE_MATRIX3F, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderCurveAnimation<Matrix3f>>))
ADD_COMMAND(
    RSAnimationCreateCurveVec2f, ARG(ANIMATION, ANIMATION_CREATE_CURVE_VEC2F, AnimationCommandHelper::CreateAnimation,
                                     NodeId, std::shared_ptr<RSRenderCurveAnimation<Vector2f>>))
ADD_COMMAND(
    RSAnimationCreateCurveVec4f, ARG(ANIMATION, ANIMATION_CREATE_CURVE_VEC4F, AnimationCommandHelper::CreateAnimation,
                                     NodeId, std::shared_ptr<RSRenderCurveAnimation<Vector4f>>))
ADD_COMMAND(RSAnimationCreateCurveQuaternion,
    ARG(ANIMATION, ANIMATION_CREATE_CURVE_QUATERNION, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderCurveAnimation<Quaternion>>))
ADD_COMMAND(
    RSAnimationCreateCurveFilter, ARG(ANIMATION, ANIMATION_CREATE_CURVE_FILTER, AnimationCommandHelper::CreateAnimation,
                                     NodeId, std::shared_ptr<RSRenderCurveAnimation<std::shared_ptr<RSFilter>>>))
ADD_COMMAND(RSAnimationCreateCurveVec4Color,
    ARG(ANIMATION, ANIMATION_CREATE_CURVE_VEC4_COLOR, AnimationCommandHelper::CreateAnimation,
                                     NodeId, std::shared_ptr<RSRenderCurveAnimation<Vector4<Color>>>))

// create keyframe animation
ADD_COMMAND(RSAnimationCreateKeyframeFloat,
    ARG(ANIMATION, ANIMATION_CREATE_KEYFRAME_FLOAT, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderKeyframeAnimation<float>>))
ADD_COMMAND(RSAnimationCreateKeyframeColor,
    ARG(ANIMATION, ANIMATION_CREATE_KEYFRAME_COLOR, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderKeyframeAnimation<Color>>))
ADD_COMMAND(RSAnimationCreateKeyframeMatrix3f,
    ARG(ANIMATION, ANIMATION_CREATE_KEYFRAME_MATRIX3F, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderKeyframeAnimation<Matrix3f>>))
ADD_COMMAND(RSAnimationCreateKeyframeVec2f,
    ARG(ANIMATION, ANIMATION_CREATE_KEYFRAME_VEC2F, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderKeyframeAnimation<Vector2f>>))
ADD_COMMAND(RSAnimationCreateKeyframeVec4f,
    ARG(ANIMATION, ANIMATION_CREATE_KEYFRAME_VEC4F, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderKeyframeAnimation<Vector4f>>))
ADD_COMMAND(RSAnimationCreateKeyframeQuaternion,
    ARG(ANIMATION, ANIMATION_CREATE_KEYFRAME_QUATERNION, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderKeyframeAnimation<Quaternion>>))
ADD_COMMAND(RSAnimationCreateKeyframeFilter,
    ARG(ANIMATION, ANIMATION_CREATE_KEYFRAME_FILTER, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderKeyframeAnimation<std::shared_ptr<RSFilter>>>))
ADD_COMMAND(RSAnimationCreateKeyframeVec4Color,
    ARG(ANIMATION, ANIMATION_CREATE_KEYFRAME_VEC4_COLOR, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderKeyframeAnimation<Vector4<Color>>>))

// create path animation
ADD_COMMAND(RSAnimationCreatePath, ARG(ANIMATION, ANIMATION_CREATE_PATH, AnimationCommandHelper::CreateAnimation,
                                       NodeId, std::shared_ptr<RSRenderPathAnimation>))

// create transition animation
ADD_COMMAND(
    RSAnimationCreateTransition, ARG(ANIMATION, ANIMATION_CREATE_TRANSITION, AnimationCommandHelper::CreateAnimation,
                                     NodeId, std::shared_ptr<RSRenderTransition>))

// create spring animation
ADD_COMMAND(RSAnimationCreateSpringFloat,
    ARG(ANIMATION, ANIMATION_CREATE_SPRING_FLOAT, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderSpringAnimation<float>>))
ADD_COMMAND(RSAnimationCreateSpringColor,
    ARG(ANIMATION, ANIMATION_CREATE_SPRING_COLOR, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderSpringAnimation<Color>>))
ADD_COMMAND(RSAnimationCreateSpringMatrix3f,
    ARG(ANIMATION, ANIMATION_CREATE_SPRING_MATRIX3F, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderSpringAnimation<Matrix3f>>))
ADD_COMMAND(RSAnimationCreateSpringVec2f,
    ARG(ANIMATION, ANIMATION_CREATE_SPRING_VEC2F, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderSpringAnimation<Vector2f>>))
ADD_COMMAND(RSAnimationCreateSpringVec4f,
    ARG(ANIMATION, ANIMATION_CREATE_SPRING_VEC4F, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderSpringAnimation<Vector4f>>))
ADD_COMMAND(RSAnimationCreateSpringQuaternion,
    ARG(ANIMATION, ANIMATION_CREATE_SPRING_QUATERNION, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderSpringAnimation<Quaternion>>))
ADD_COMMAND(RSAnimationCreateSpringFilter,
    ARG(ANIMATION, ANIMATION_CREATE_SPRING_FILTER, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderSpringAnimation<std::shared_ptr<RSFilter>>>))
ADD_COMMAND(RSAnimationCreateSpringVec4Color,
    ARG(ANIMATION, ANIMATION_CREATE_SPRING_VEC4_COLOR, AnimationCommandHelper::CreateAnimation, NodeId,
        std::shared_ptr<RSRenderSpringAnimation<Vector4<Color>>>))

// remove animation
ADD_COMMAND(RSOnAnimationRemoveFloat,
    ARG(ANIMATION, ANIMATION_REMOVE_ANIMATION_FLOAT, AnimationCommandHelper::RemoveAnimation<float>, NodeId,
        AnimationId, float))
ADD_COMMAND(RSOnAnimationRemoveColor,
    ARG(ANIMATION, ANIMATION_REMOVE_ANIMATION_COLOR, AnimationCommandHelper::RemoveAnimation<Color>, NodeId,
        AnimationId, Color))
ADD_COMMAND(RSOnAnimationRemoveMatrix3f,
    ARG(ANIMATION, ANIMATION_REMOVE_ANIMATION_MATRIX3F, AnimationCommandHelper::RemoveAnimation<Matrix3f>, NodeId,
        AnimationId, Matrix3f))
ADD_COMMAND(RSOnAnimationRemoveVector2f,
    ARG(ANIMATION, ANIMATION_REMOVE_ANIMATION_VEC2F, AnimationCommandHelper::RemoveAnimation<Vector2f>, NodeId,
        AnimationId, Vector2f))
ADD_COMMAND(RSOnAnimationRemoveVector4f,
    ARG(ANIMATION, ANIMATION_REMOVE_ANIMATION_VEC4F, AnimationCommandHelper::RemoveAnimation<Vector4f>, NodeId,
        AnimationId, Vector4f))
ADD_COMMAND(RSOnAnimationRemoveQuaternion,
    ARG(ANIMATION, ANIMATION_REMOVE_ANIMATION_QUATERNION, AnimationCommandHelper::RemoveAnimation<Quaternion>, NodeId,
        AnimationId, Quaternion))
ADD_COMMAND(RSOnAnimationRemoveFilter,
    ARG(ANIMATION, ANIMATION_REMOVE_ANIMATION_FILTER, AnimationCommandHelper::RemoveAnimation<std::shared_ptr<RSFilter>>, NodeId,
        AnimationId, std::shared_ptr<RSFilter>))
ADD_COMMAND(RSOnAnimationRemoveVec4Color,
    ARG(ANIMATION, ANIMATION_REMOVE_ANIMATION_VEC4_COLOR, AnimationCommandHelper::RemoveAnimation<Vector4<Color>>, NodeId,
        AnimationId, Vector4<Color>))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_ANIMATION_COMMAND_H
