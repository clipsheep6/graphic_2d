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

#include "ui/rs_node.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

#include "animation/rs_animation.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "command/rs_node_command.h"
#include "common/rs_color.h"
#include "common/rs_obj_geometry.h"
#include "modifier/rs_modifier.h"
#include "modifier/rs_property_modifier.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []() -> int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

namespace OHOS {
namespace Rosen {
namespace {
bool IsPathAnimatableModifier(const RSModifierType& type)
{
    if (type == RSModifierType::BOUNDS || type == RSModifierType::FRAME || type == RSModifierType::TRANSLATE) {
        return true;
    }
    return false;
}
}

RSNode::RSNode(bool isRenderServiceNode)
    : RSBaseNode(isRenderServiceNode), stagingPropertiesExtractor_(GetId()), showingPropertiesFreezer_(GetId())
{
    UpdateImplicitAnimator();
}

RSNode::RSNode(bool isRenderServiceNode, NodeId id)
    : RSBaseNode(isRenderServiceNode, id), stagingPropertiesExtractor_(id), showingPropertiesFreezer_(GetId())
{
    UpdateImplicitAnimator();
}

RSNode::~RSNode()
{
    FallbackAnimationsToRoot();
    ClearAllModifiers();
}

void RSNode::OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return;
    }

    std::shared_ptr<AnimationFinishCallback> animationFinishCallback;
    if (finishCallback != nullptr) {
        animationFinishCallback = std::make_shared<AnimationFinishCallback>(finishCallback);
    }
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(animationFinishCallback));
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::CloseImplicitAnimation()
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to close implicit animation, implicit animator is null!");
        return {};
    }

    return implicitAnimator->CloseImplicitAnimation();
}

void RSNode::AddKeyFrame(
    float fraction, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to add keyframe, implicit animator is null!");
        return;
    }

    implicitAnimator->BeginImplicitKeyFrameAnimation(fraction, timingCurve);
    propertyCallback();
    implicitAnimator->EndImplicitKeyFrameAnimation();
}

void RSNode::AddKeyFrame(float fraction, const PropertyCallback& propertyCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to add keyframe, implicit animator is null!");
        return;
    }

    implicitAnimator->BeginImplicitKeyFrameAnimation(fraction);
    propertyCallback();
    implicitAnimator->EndImplicitKeyFrameAnimation();
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::Animate(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const PropertyCallback& propertyCallback,
    const std::function<void()>& finishCallback, const std::function<void()>& repeatCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }

    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return {};
    }
    std::shared_ptr<AnimationFinishCallback> animationFinishCallback;
    if (finishCallback != nullptr) {
        animationFinishCallback = std::make_shared<AnimationFinishCallback>(finishCallback);
    }
    std::shared_ptr<AnimationRepeatCallback> animationRepeatCallback;
    if (repeatCallback != nullptr) {
        animationRepeatCallback = std::make_shared<AnimationRepeatCallback>(repeatCallback);
    }
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(animationFinishCallback),
        std::move(animationRepeatCallback));
    propertyCallback();
    return implicitAnimator->CloseImplicitAnimation();
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::AnimateWithCurrentOptions(
    const PropertyCallback& propertyCallback, const std::function<void()>& finishCallback, bool timingSensitive)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }
    if (finishCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, finish callback is null!");
        propertyCallback();
        return {};
    }

    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return {};
    }
    // re-use the current options and replace the finish callback
    auto animationFinishCallback = std::make_shared<AnimationFinishCallback>(finishCallback, timingSensitive);
    implicitAnimator->OpenImplicitAnimation(std::move(animationFinishCallback));
    propertyCallback();
    return implicitAnimator->CloseImplicitAnimation();
}

std::vector<std::shared_ptr<RSAnimation>> RSNode::AnimateWithCurrentCallback(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
    const PropertyCallback& propertyCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }

    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return {};
    }
    // re-use the current finish callback and replace the options
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    propertyCallback();
    return implicitAnimator->CloseImplicitAnimation();
}

void RSNode::ExecuteWithoutAnimation(
    const PropertyCallback& callback, std::shared_ptr<RSImplicitAnimator> implicitAnimator)
{
    if (callback == nullptr) {
        ROSEN_LOGE("Failed to execute without animation, property callback is null!");
        return;
    }
    if (implicitAnimator == nullptr) {
        implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    }
    if (implicitAnimator == nullptr) {
        callback();
    } else {
        implicitAnimator->ExecuteWithoutAnimation(callback);
    }
}

void RSNode::FallbackAnimationsToRoot()
{
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode();

    if (target == nullptr) {
        ROSEN_LOGE("Failed to move animation to root, root node is null!");
        return;
    }
    for (auto& [unused, animation] : animations_) {
        if (animation && animation->GetRepeatCount() == -1) {
            continue;
        }
        target->AddAnimationInner(std::move(animation));
    }
    animations_.clear();
}

void RSNode::AddAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    animations_.emplace(animation->GetId(), animation);
    animatingPropertyNum_[animation->GetPropertyId()]++;
}

void RSNode::RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    if (auto it = animatingPropertyNum_.find(animation->GetPropertyId()); it != animatingPropertyNum_.end()) {
        it->second--;
        if (it->second == 0) {
            animatingPropertyNum_.erase(it);
            animation->SetPropertyOnAllAnimationFinish();
        }
    }
    animations_.erase(animation->GetId());
}

void RSNode::FinishAnimationByProperty(const PropertyId& id)
{
    for (const auto& [animationId, animation] : animations_) {
        if (animation->GetPropertyId() == id) {
            animation->Finish();
        }
    }
}

void RSNode::CancelAnimationByProperty(const PropertyId& id)
{
    animatingPropertyNum_.erase(id);
    for (const auto& [animationId, animation] : animations_) {
        if (animation && (animation->GetPropertyId() == id)) {
            animations_.erase(animationId);
        }
    }
}

const RSModifierExtractor& RSNode::GetStagingProperties() const
{
    return stagingPropertiesExtractor_;
}

const RSShowingPropertiesFreezer& RSNode::GetShowingProperties() const
{
    return showingPropertiesFreezer_;
}

void RSNode::AddAnimation(const std::shared_ptr<RSAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to add animation, animation is null!");
        return;
    }

    auto animationId = animation->GetId();
    if (animations_.find(animationId) != animations_.end()) {
        ROSEN_LOGE("Failed to add animation, animation already exists!");
        return;
    }

    if (animation->GetDuration() <= 0) {
        FinishAnimationByProperty(animation->GetPropertyId());
    }

    AddAnimationInner(animation);
    animation->StartInner(std::static_pointer_cast<RSNode>(shared_from_this()));
}

void RSNode::RemoveAllAnimations()
{
    for (const auto& [id, animation] : animations_) {
        RemoveAnimation(animation);
    }
}

void RSNode::RemoveAnimation(const std::shared_ptr<RSAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to remove animation, animation is null!");
        return;
    }

    if (animations_.find(animation->GetId()) == animations_.end()) {
        ROSEN_LOGE("Failed to remove animation, animation not exists!");
        return;
    }

    animation->Finish();
}

void RSNode::SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption)
{
    motionPathOption_ = motionPathOption;
    UpdateModifierMotionPathOption();
}

const std::shared_ptr<RSMotionPathOption> RSNode::GetMotionPathOption() const
{
    return motionPathOption_;
}

bool RSNode::HasPropertyAnimation(const PropertyId& id)
{
    auto it = animatingPropertyNum_.find(id);
    return it != animatingPropertyNum_.end() && it->second > 0;
}

template<typename ModifierName, typename PropertyName, typename T>
void RSNode::SetProperty(RSModifierType modifierType, T value)
{
    auto iter = propertyModifiers_.find(modifierType);
    if (iter != propertyModifiers_.end()) {
        auto property = std::static_pointer_cast<PropertyName>(iter->second->GetProperty());
        if (property == nullptr) {
            ROSEN_LOGE("RSNode::SetProperty: failed to set property, property is null!");
            return;
        }
        property->Set(value);
        return;
    }
    auto property = std::make_shared<PropertyName>(value);
    auto propertyModifier = std::make_shared<ModifierName>(property);
    propertyModifiers_.emplace(modifierType, propertyModifier);
    AddModifier(propertyModifier);
}

// alpha
void RSNode::SetAlpha(float alpha)
{
    SetProperty<RSAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::ALPHA, alpha);
}

void RSNode::SetAlphaOffscreen(bool alphaOffscreen)
{
    SetProperty<RSAlphaOffscreenModifier, RSProperty<bool>>(RSModifierType::ALPHA_OFFSCREEN, alphaOffscreen);
}

// Bounds
void RSNode::SetBounds(const Vector4f& bounds)
{
    SetProperty<RSBoundsModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::BOUNDS, bounds);
    OnBoundsSizeChanged();
}

void RSNode::SetBounds(float positionX, float positionY, float width, float height)
{
    SetBounds({ positionX, positionY, width, height });
}

void RSNode::SetBoundsWidth(float width)
{
    auto iter = propertyModifiers_.find(RSModifierType::BOUNDS);
    if (iter == propertyModifiers_.end()) {
        SetBounds(0.f, 0.f, width, 0.f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto bounds = property->Get();
    bounds.z_ = width;
    property->Set(bounds);
    OnBoundsSizeChanged();
}

void RSNode::SetBoundsHeight(float height)
{
    auto iter = propertyModifiers_.find(RSModifierType::BOUNDS);
    if (iter == propertyModifiers_.end()) {
        SetBounds(0.f, 0.f, 0.f, height);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto bounds = property->Get();
    bounds.w_ = height;
    property->Set(bounds);
    OnBoundsSizeChanged();
}

// Frame
void RSNode::SetFrame(const Vector4f& bounds)
{
    SetProperty<RSFrameModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::FRAME, bounds);
}

void RSNode::SetFrame(float positionX, float positionY, float width, float height)
{
    SetFrame({ positionX, positionY, width, height });
}

void RSNode::SetFramePositionX(float positionX)
{
    auto iter = propertyModifiers_.find(RSModifierType::FRAME);
    if (iter == propertyModifiers_.end()) {
        SetFrame(positionX, 0.f, 0.f, 0.f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.x_ = positionX;
    property->Set(frame);
}

void RSNode::SetFramePositionY(float positionY)
{
    auto iter = propertyModifiers_.find(RSModifierType::FRAME);
    if (iter == propertyModifiers_.end()) {
        SetFrame(0.f, positionY, 0.f, 0.f);
        return;
    }
    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector4f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto frame = property->Get();
    frame.y_ = positionY;
    property->Set(frame);
}

void RSNode::SetSandBox(std::optional<Vector2f> parentPosition)
{
    if (!parentPosition.has_value()) {
        auto iter = propertyModifiers_.find(RSModifierType::SANDBOX);
        if (iter != propertyModifiers_.end()) {
            RemoveModifier(iter->second);
            propertyModifiers_.erase(iter);
        }
        return;
    }
    SetProperty<RSSandBoxModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::SANDBOX, parentPosition.value());
}

void RSNode::SetPositionZ(float positionZ)
{
    SetProperty<RSPositionZModifier, RSAnimatableProperty<float>>(RSModifierType::POSITION_Z, positionZ);
}

// pivot
void RSNode::SetPivot(const Vector2f& pivot)
{
    SetProperty<RSPivotModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::PIVOT, pivot);
}

void RSNode::SetPivot(float pivotX, float pivotY)
{
    SetPivot({ pivotX, pivotY });
}

void RSNode::SetPivotX(float pivotX)
{
    auto iter = propertyModifiers_.find(RSModifierType::PIVOT);
    if (iter == propertyModifiers_.end()) {
        SetPivot(pivotX, 0.5f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto pivot = property->Get();
    pivot.x_ = pivotX;
    property->Set(pivot);
}

void RSNode::SetPivotY(float pivotY)
{
    auto iter = propertyModifiers_.find(RSModifierType::PIVOT);
    if (iter == propertyModifiers_.end()) {
        SetPivot(0.5f, pivotY);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto pivot = property->Get();
    pivot.y_ = pivotY;
    property->Set(pivot);
}

void RSNode::SetPivotZ(const float pivotZ)
{
    SetProperty<RSPivotZModifier, RSAnimatableProperty<float>>(RSModifierType::PIVOT_Z, pivotZ);
}

void RSNode::SetCornerRadius(float cornerRadius)
{
    SetCornerRadius(Vector4f(cornerRadius));
}

void RSNode::SetCornerRadius(const Vector4f& cornerRadius)
{
    SetProperty<RSCornerRadiusModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::CORNER_RADIUS, cornerRadius);
}

// transform
void RSNode::SetRotation(const Quaternion& quaternion)
{
    SetProperty<RSQuaternionModifier, RSAnimatableProperty<Quaternion>>(RSModifierType::QUATERNION, quaternion);
}

void RSNode::SetRotation(float degree)
{
    SetProperty<RSRotationModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION, degree);
}

void RSNode::SetRotation(float degreeX, float degreeY, float degreeZ)
{
    SetRotationX(degreeX);
    SetRotationY(degreeY);
    SetRotation(degreeZ);
}

void RSNode::SetRotationX(float degree)
{
    SetProperty<RSRotationXModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION_X, degree);
}

void RSNode::SetRotationY(float degree)
{
    SetProperty<RSRotationYModifier, RSAnimatableProperty<float>>(RSModifierType::ROTATION_Y, degree);
}

void RSNode::SetCameraDistance(float cameraDistance)
{
    SetProperty<RSCameraDistanceModifier, RSAnimatableProperty<float>>(RSModifierType::CAMERA_DISTANCE, cameraDistance);
}

void RSNode::SetTranslate(const Vector2f& translate)
{
    SetProperty<RSTranslateModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::TRANSLATE, translate);
}

void RSNode::SetTranslate(float translateX, float translateY, float translateZ)
{
    SetTranslate({ translateX, translateY });
    SetTranslateZ(translateZ);
}
void RSNode::SetTranslateX(float translate)
{
    auto iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
    if (iter == propertyModifiers_.end()) {
        SetTranslate({ translate, 0.f });
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto trans = property->Get();
    trans.x_ = translate;
    property->Set(trans);
}

void RSNode::SetTranslateY(float translate)
{
    auto iter = propertyModifiers_.find(RSModifierType::TRANSLATE);
    if (iter == propertyModifiers_.end()) {
        SetTranslate({ 0.f, translate });
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto trans = property->Get();
    trans.y_ = translate;
    property->Set(trans);
}

void RSNode::SetTranslateZ(float translate)
{
    SetProperty<RSTranslateZModifier, RSAnimatableProperty<float>>(RSModifierType::TRANSLATE_Z, translate);
}

void RSNode::SetScale(float scale)
{
    SetScale({ scale, scale });
}

void RSNode::SetScale(float scaleX, float scaleY)
{
    SetScale({ scaleX, scaleY });
}

void RSNode::SetScale(const Vector2f& scale)
{
    SetProperty<RSScaleModifier, RSAnimatableProperty<Vector2f>>(RSModifierType::SCALE, scale);
}

void RSNode::SetScaleX(float scaleX)
{
    auto iter = propertyModifiers_.find(RSModifierType::SCALE);
    if (iter == propertyModifiers_.end()) {
        SetScale(scaleX, 1.f);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto scale = property->Get();
    scale.x_ = scaleX;
    property->Set(scale);
}

void RSNode::SetScaleY(float scaleY)
{
    auto iter = propertyModifiers_.find(RSModifierType::SCALE);
    if (iter == propertyModifiers_.end()) {
        SetScale(1.f, scaleY);
        return;
    }

    auto property = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second->GetProperty());
    if (property == nullptr) {
        return;
    }
    auto scale = property->Get();
    scale.y_ = scaleY;
    property->Set(scale);
}
// Set the foreground color of the control
void RSNode::SetEnvForegroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetProperty<RSEnvForegroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::ENV_FOREGROUND_COLOR, color);
}

// Set the foreground color strategy of the control
void RSNode::SetEnvForegroundColorStrategy(ForegroundColorStrategyType strategyType)
{
    SetProperty<RSEnvForegroundColorStrategyModifier,
        RSProperty<ForegroundColorStrategyType>>(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, strategyType);
}

// foreground
void RSNode::SetForegroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetProperty<RSForegroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::FOREGROUND_COLOR, color);
}

void RSNode::SetBackgroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetProperty<RSBackgroundColorModifier, RSAnimatableProperty<Color>>(RSModifierType::BACKGROUND_COLOR, color);
}

void RSNode::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    SetProperty<RSBackgroundShaderModifier, RSProperty<std::shared_ptr<RSShader>>>(
        RSModifierType::BACKGROUND_SHADER, shader);
}

// background
void RSNode::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    image->SetNodeId(GetId());
    SetProperty<RSBgImageModifier, RSProperty<std::shared_ptr<RSImage>>>(RSModifierType::BG_IMAGE, image);
}

void RSNode::SetBgImageSize(float width, float height)
{
    SetBgImageWidth(width);
    SetBgImageHeight(height);
}

void RSNode::SetBgImageWidth(float width)
{
    SetProperty<RSBgImageWidthModifier, RSAnimatableProperty<float>>(RSModifierType::BG_IMAGE_WIDTH, width);
}

void RSNode::SetBgImageHeight(float height)
{
    SetProperty<RSBgImageHeightModifier, RSAnimatableProperty<float>>(RSModifierType::BG_IMAGE_HEIGHT, height);
}

void RSNode::SetBgImagePosition(float positionX, float positionY)
{
    SetBgImagePositionX(positionX);
    SetBgImagePositionY(positionY);
}

void RSNode::SetBgImagePositionX(float positionX)
{
    SetProperty<RSBgImagePositionXModifier, RSAnimatableProperty<float>>(
        RSModifierType::BG_IMAGE_POSITION_X, positionX);
}

void RSNode::SetBgImagePositionY(float positionY)
{
    SetProperty<RSBgImagePositionYModifier, RSAnimatableProperty<float>>(
        RSModifierType::BG_IMAGE_POSITION_Y, positionY);
}

// border
void RSNode::SetBorderColor(uint32_t colorValue)
{
    SetBorderColor(colorValue, colorValue, colorValue, colorValue);
}

void RSNode::SetBorderColor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    Vector4<Color> color(Color::FromArgbInt(left), Color::FromArgbInt(top),
                         Color::FromArgbInt(right), Color::FromArgbInt(bottom));
    SetBorderColor(color);
}

void RSNode::SetBorderColor(const Vector4<Color>& color)
{
    SetProperty<RSBorderColorModifier, RSAnimatableProperty<Vector4<Color>>>(RSModifierType::BORDER_COLOR, color);
}

void RSNode::SetBorderWidth(float width)
{
    SetBorderWidth(width, width, width, width);
}

void RSNode::SetBorderWidth(float left, float top, float right, float bottom)
{
    Vector4f width(left, top, right, bottom);
    SetBorderWidth(width);
}

void RSNode::SetBorderWidth(const Vector4f& width)
{
    SetProperty<RSBorderWidthModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::BORDER_WIDTH, width);
}

void RSNode::SetBorderStyle(uint32_t styleValue)
{
    SetBorderStyle(styleValue, styleValue, styleValue, styleValue);
}

void RSNode::SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    Vector4<BorderStyle> style(static_cast<BorderStyle>(left), static_cast<BorderStyle>(top),
                               static_cast<BorderStyle>(right), static_cast<BorderStyle>(bottom));
    SetBorderStyle(style);
}

void RSNode::SetBorderStyle(const Vector4<BorderStyle>& style)
{
    Vector4<uint32_t> styles(static_cast<uint32_t>(style.x_), static_cast<uint32_t>(style.y_),
                             static_cast<uint32_t>(style.z_), static_cast<uint32_t>(style.w_));
    SetProperty<RSBorderStyleModifier, RSProperty<Vector4<uint32_t>>>(RSModifierType::BORDER_STYLE, styles);
}

void RSNode::SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter)
{
    SetProperty<RSBackgroundFilterModifier, RSAnimatableProperty<std::shared_ptr<RSFilter>>>(
        RSModifierType::BACKGROUND_FILTER, backgroundFilter);
}

void RSNode::SetFilter(const std::shared_ptr<RSFilter>& filter)
{
    SetProperty<RSFilterModifier, RSAnimatableProperty<std::shared_ptr<RSFilter>>>(RSModifierType::FILTER, filter);
}

void RSNode::SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para)
{
    SetProperty<RSLinearGradientBlurParaModifier, RSProperty<std::shared_ptr<RSLinearGradientBlurPara>>>
                                                                    (RSModifierType::LINEAR_GRADIENT_BLUR_PARA, para);
}

void RSNode::SetCompositingFilter(const std::shared_ptr<RSFilter>& compositingFilter) {}

void RSNode::SetShadowColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SetProperty<RSShadowColorModifier, RSAnimatableProperty<Color>>(RSModifierType::SHADOW_COLOR, color);
}

void RSNode::SetShadowOffset(float offsetX, float offsetY)
{
    SetShadowOffsetX(offsetX);
    SetShadowOffsetY(offsetY);
}

void RSNode::SetShadowOffsetX(float offsetX)
{
    SetProperty<RSShadowOffsetXModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_OFFSET_X, offsetX);
}

void RSNode::SetShadowOffsetY(float offsetY)
{
    SetProperty<RSShadowOffsetYModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_OFFSET_Y, offsetY);
}

void RSNode::SetShadowAlpha(float alpha)
{
    SetProperty<RSShadowAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_ALPHA, alpha);
}

void RSNode::SetShadowElevation(float elevation)
{
    SetProperty<RSShadowElevationModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_ELEVATION, elevation);
}

void RSNode::SetShadowRadius(float radius)
{
    SetProperty<RSShadowRadiusModifier, RSAnimatableProperty<float>>(RSModifierType::SHADOW_RADIUS, radius);
}

void RSNode::SetShadowPath(const std::shared_ptr<RSPath>& shadowPath)
{
    SetProperty<RSShadowPathModifier, RSProperty<std::shared_ptr<RSPath>>>(RSModifierType::SHADOW_PATH, shadowPath);
}

void RSNode::SetShadowMask(bool shadowMask)
{
    SetProperty<RSShadowMaskModifier, RSProperty<bool>>(RSModifierType::SHADOW_MASK, shadowMask);
}

void RSNode::SetFrameGravity(Gravity gravity)
{
    SetProperty<RSFrameGravityModifier, RSProperty<Gravity>>(RSModifierType::FRAME_GRAVITY, gravity);
}

void RSNode::SetClipRRect(const Vector4f& clipRect, const Vector4f& clipRadius)
{
    SetProperty<RSClipRRectModifier, RSAnimatableProperty<RRect>>(
        RSModifierType::CLIP_RRECT, RRect(clipRect, clipRadius));
}

void RSNode::SetClipBounds(const std::shared_ptr<RSPath>& path)
{
    SetProperty<RSClipBoundsModifier, RSProperty<std::shared_ptr<RSPath>>>(RSModifierType::CLIP_BOUNDS, path);
}

void RSNode::SetClipToBounds(bool clipToBounds)
{
    SetProperty<RSClipToBoundsModifier, RSProperty<bool>>(RSModifierType::CLIP_TO_BOUNDS, clipToBounds);
}

void RSNode::SetClipToFrame(bool clipToFrame)
{
    SetProperty<RSClipToFrameModifier, RSProperty<bool>>(RSModifierType::CLIP_TO_FRAME, clipToFrame);
}

void RSNode::SetVisible(bool visible)
{
    // kick off transition only if it's on tree(has valid parent) and visibility is changed.
    if (transitionEffect_ != nullptr && GetParent() != nullptr && visible != GetStagingProperties().GetVisible()) {
        NotifyTransition(transitionEffect_, visible);
    }

    SetProperty<RSVisibleModifier, RSProperty<bool>>(RSModifierType::VISIBLE, visible);
}

void RSNode::SetMask(const std::shared_ptr<RSMask>& mask)
{
    SetProperty<RSMaskModifier, RSProperty<std::shared_ptr<RSMask>>>(RSModifierType::MASK, mask);
}

void RSNode::SetUseEffect(bool useEffect)
{
    SetProperty<RSUseEffectModifier, RSProperty<bool>>(RSModifierType::USE_EFFECT, useEffect);
}

void RSNode::SetPixelStretch(const Vector4f& stretchSize)
{
    SetProperty<RSPixelStretchModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PIXEL_STRETCH, stretchSize);
}

void RSNode::SetPixelStretchPercent(const Vector4f& stretchPercent)
{
    SetProperty<RSPixelStretchPercentModifier, RSAnimatableProperty<Vector4f>>(RSModifierType::PIXEL_STRETCH_PERCENT,
        stretchPercent);
}

void RSNode::SetFreeze(bool isFreeze)
{
    ROSEN_LOGE("SetFreeze only support RSSurfaceNode and RSCanvasNode in uniRender");
}

void RSNode::SetSpherizeDegree(float spherizeDegree)
{
    SetProperty<RSSpherizeModifier, RSAnimatableProperty<float>>(RSModifierType::SPHERIZE, spherizeDegree);
}

void RSNode::SetLightUpEffectDegree(float LightUpEffectDegree)
{
    SetProperty<RSLightUpEffectModifier, RSAnimatableProperty<float>>(
        RSModifierType::LIGHT_UP_EFFECT, LightUpEffectDegree);
}

void RSNode::NotifyTransition(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
{
    // temporary fix for multithread issue in implicit animator
    UpdateImplicitAnimator();
    if (implicitAnimator_ == nullptr) {
        ROSEN_LOGE("Failed to notify transition, implicit animator is null!");
        return;
    }

    if (!implicitAnimator_->NeedImplicitAnimation()) {
        return;
    }

    auto& customEffects = isTransitionIn ? effect->customTransitionInEffects_ : effect->customTransitionOutEffects_;
    // temporary close the implicit animation
    ExecuteWithoutAnimation(
        [&customEffects] {
            for (auto& customEffect : customEffects) {
                customEffect->Active();
            }
        },
        implicitAnimator_);

    implicitAnimator_->BeginImplicitTransition(effect, isTransitionIn);
    for (auto& customEffect : customEffects) {
        customEffect->Identity();
    }
    implicitAnimator_->CreateImplicitTransition(*this);
    implicitAnimator_->EndImplicitTransition();
}

void RSNode::OnAddChildren()
{
    // kick off transition only if it's visible.
    if (transitionEffect_ != nullptr && GetStagingProperties().GetVisible()) {
        NotifyTransition(transitionEffect_, true);
    }
}

void RSNode::OnRemoveChildren()
{
    // kick off transition only if it's visible.
    if (transitionEffect_ != nullptr && GetStagingProperties().GetVisible()) {
        NotifyTransition(transitionEffect_, false);
    }
}

bool RSNode::AnimationCallback(AnimationId animationId, AnimationCallbackEvent event)
{
    auto animationItr = animations_.find(animationId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("Failed to find animation[%" PRIu64 "]!", animationId);
        return false;
    }

    auto animation = animationItr->second;
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to callback animation[%" PRIu64 "], animation is null!", animationId);
        return false;
    }
    if (event == FINISHED) {
        RemoveAnimationInner(animation);
        animation->CallFinishCallback();
        return true;
    } else if (event == REPEAT_FINISHED) {
        animation->CallRepeatCallback();
        return true;
    }
    ROSEN_LOGE("Failed to callback animation event[%" PRIu64 "], event is null!", event);
    return false;
}

void RSNode::SetPaintOrder(bool drawContentLast)
{
    drawContentLast_ = drawContentLast;
}

void RSNode::MarkDrivenRender(bool flag)
{
    if (drivenFlag_ != flag) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSMarkDrivenRender>(GetId(), flag);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode());
        }
        drivenFlag_ = flag;
    }
}

void RSNode::MarkDrivenRenderItemIndex(int index)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkDrivenRenderItemIndex>(GetId(), index);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::MarkDrivenRenderFramePaintState(bool flag)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSMarkDrivenRenderFramePaintState>(GetId(), flag);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::MarkContentChanged(bool isChanged)
{
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkContentChanged>(GetId(), isChanged);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}

void RSNode::ClearAllModifiers()
{
    for (auto [id, modifier] : modifiers_) {
        if (modifier) {
            modifier->DetachFromNode();
        }
    }
}

void RSNode::AddModifier(const std::shared_ptr<RSModifier> modifier)
{
    if (!modifier || modifiers_.count(modifier->GetPropertyId())) {
        return;
    }
    if (motionPathOption_ != nullptr && IsPathAnimatableModifier(modifier->GetModifierType())) {
        modifier->SetMotionPathOption(motionPathOption_);
    }
    auto rsnode = std::static_pointer_cast<RSNode>(shared_from_this());
    modifier->AttachToNode(rsnode);
    modifiers_.emplace(modifier->GetPropertyId(), modifier);
    if (modifier->GetModifierType() == RSModifierType::NODE_MODIFIER) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> cmdForRemote =
                std::make_unique<RSAddModifier>(GetId(), modifier->CreateRenderModifier());
            transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());
        }
    }
}

void RSNode::RemoveModifier(const std::shared_ptr<RSModifier> modifier)
{
    if (!modifier) {
        return;
    }
    auto iter = modifiers_.find(modifier->GetPropertyId());
    if (iter == modifiers_.end()) {
        return;
    }

    modifiers_.erase(iter);
    modifier->DetachFromNode();
    std::unique_ptr<RSCommand> command = std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        if (NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> cmdForRemote =
                std::make_unique<RSRemoveModifier>(GetId(), modifier->GetPropertyId());
            transactionProxy->AddCommand(cmdForRemote, true, GetFollowType(), GetId());
        }
    }
}

const std::shared_ptr<RSModifier> RSNode::GetModifier(const PropertyId& propertyId)
{
    auto iter = modifiers_.find(propertyId);
    if (iter != modifiers_.end()) {
        return iter->second;
    }

    return {};
}

void RSNode::UpdateModifierMotionPathOption()
{
    for (auto& [type, modifier] : propertyModifiers_) {
        if (IsPathAnimatableModifier(type)) {
            modifier->SetMotionPathOption(motionPathOption_);
        }
    }
    for (auto& [id, modifier] : modifiers_) {
        if (IsPathAnimatableModifier(modifier->GetModifierType())) {
            modifier->SetMotionPathOption(motionPathOption_);
        }
    }
}

std::string RSNode::DumpNode(int depth) const
{
    std::stringstream ss;
    ss << RSBaseNode::DumpNode(depth);
    if (!animations_.empty()) {
        ss << " animation:" << std::to_string(animations_.size());
    }
    ss << " " << GetStagingProperties().Dump();
    return ss.str();
}

void RSNode::UpdateImplicitAnimator()
{
    auto tid = gettid();
    if (tid == implicitAnimatorTid_) {
        return;
    }
    implicitAnimatorTid_ = tid;
    implicitAnimator_ = RSImplicitAnimatorMap::Instance().GetAnimator(tid);
}

std::vector<PropertyId> RSNode::GetModifierIds() const
{
    std::vector<PropertyId> ids;
    for (const auto& [id, _] : modifiers_) {
        ids.push_back(id);
    }
    return ids;
}

void RSNode::MarkAllExtendModifierDirty()
{
    if (extendModifierIsDirty_) {
        return;
    }

    extendModifierIsDirty_ = true;
    for (auto& [id, modifier] : modifiers_) {
        if (modifier->GetModifierType() < RSModifierType::CUSTOM) {
            continue;
        }
        modifier->SetDirty(true);
    }
}

void RSNode::ResetExtendModifierDirty()
{
    extendModifierIsDirty_ = false;
}

void RSNode::SetIsCustomTextType(bool isCustomTextType)
{
    isCustomTextType_ = isCustomTextType;
}

void RSNode::SetDrawRegion(std::shared_ptr<RectF> rect)
{
    if (drawRegion_ != rect) {
        drawRegion_ = rect;
        std::unique_ptr<RSCommand> command = std::make_unique<RSSetDrawRegion>(GetId(), rect);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());
        }
    }
}

void RSNode::RegisterTransitionPair(NodeId inNodeId, NodeId outNodeId)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSRegisterGeometryTransitionNodePair>(inNodeId, outNodeId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSNode::UnregisterTransitionPair(NodeId inNodeId, NodeId outNodeId)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSUnregisterGeometryTransitionNodePair>(inNodeId, outNodeId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}

void RSNode::MarkNodeGroup(bool isNodeGroup)
{
    if (isNodeGroup_ == isNodeGroup) {
        return;
    }
    isNodeGroup_ = isNodeGroup;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkNodeGroup>(GetId(), isNodeGroup);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, IsRenderServiceNode());
    }
}


void RSNode::SetGrayScale(float grayScale)
{
    SetProperty<RSGrayScaleModifier, RSAnimatableProperty<float>>(RSModifierType::GRAY_SCALE, grayScale);
}

void RSNode::SetBrightness(float brightness)
{
    SetProperty<RSBrightnessModifier, RSAnimatableProperty<float>>(RSModifierType::BRIGHTNESS, brightness);
}

void RSNode::SetContrast(float contrast)
{
    SetProperty<RSContrastModifier, RSAnimatableProperty<float>>(RSModifierType::CONTRAST, contrast);
}

void RSNode::SetSaturate(float saturate)
{
    SetProperty<RSSaturateModifier, RSAnimatableProperty<float>>(RSModifierType::SATURATE, saturate);
}

void RSNode::SetSepia(float sepia)
{
    SetProperty<RSSepiaModifier, RSAnimatableProperty<float>>(RSModifierType::SEPIA, sepia);
}

void RSNode::SetInvert(float invert)
{
    SetProperty<RSInvertModifier, RSAnimatableProperty<float>>(RSModifierType::INVERT, invert);
}

void RSNode::SetHueRotate(float hueRotate)
{
    SetProperty<RSHueRotateModifier, RSAnimatableProperty<float>>(RSModifierType::HUE_ROTATE, hueRotate);
}

void RSNode::SetColorBlend(uint32_t colorValue)
{
    auto colorBlend = Color::FromArgbInt(colorValue);
    SetProperty<RSColorBlendModifier, RSAnimatableProperty<Color>>(RSModifierType::COLOR_BLEND, colorBlend);
}
} // namespace Rosen
} // namespace OHOS
