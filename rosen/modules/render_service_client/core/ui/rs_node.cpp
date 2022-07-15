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

#include "ui/rs_node.h"

#include <algorithm>
#include <sstream>
#include <string>

#include "animation/rs_animation.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "command/rs_node_command.h"
#include "common/rs_color.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {

RSNode::RSNode(bool isRenderServiceNode) : RSBaseNode(isRenderServiceNode), stagingProperties_(false)
{
    UpdateImplicitAnimator();
}

RSNode::~RSNode()
{
    FallbackAnimationsToRoot();
}

void RSNode::OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator == nullptr) {
        ROSEN_LOGE("Failed to open implicit animation, implicit animator is null!");
        return;
    }

    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, finishCallback);
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
    const std::function<void()>& finishCallback)
{
    if (propertyCallback == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, property callback is null!");
        return {};
    }

    if (RSImplicitAnimatorMap::Instance().GetAnimator(gettid()) == nullptr) {
        ROSEN_LOGE("Failed to add curve animation, implicit animator is null!");
        return {};
    }

    OpenImplicitAnimation(timingProtocol, timingCurve, finishCallback);
    propertyCallback();
    return CloseImplicitAnimation();
}

void RSNode::FallbackAnimationsToRoot()
{
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode();

    if (target == nullptr) {
        ROSEN_LOGE("Failed to move animation to root, root node is null!");
        return;
    }
    for (const auto& [animationId, animation] : animations_) {
        target->AddAnimationInner(animation);
    }
}

void RSNode::AddAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    animations_[animation->GetId()] = animation;
    animatingPropertyNum_[animation->GetProperty()]++;
}

void RSNode::RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation)
{
    auto animationItr = animations_.find(animation->GetId());
    animations_.erase(animationItr);
    animatingPropertyNum_[animation->GetProperty()]--;
}

void RSNode::FinishAnimationByProperty(const RSAnimatableProperty& property)
{
    for (const auto& [animationId, animation] : animations_) {
        if (animation->GetProperty() == property) {
            animation->Finish();
        }
    }
}

const RSProperties& RSNode::GetStagingProperties() const
{
    return stagingProperties_;
}

void RSNode::CopyPropertiesFrom(const RSNode& other)
{
    auto& properties = other.GetStagingProperties();

    SetBounds(properties.GetBounds());
    SetFrame(properties.GetFrame());

    SetPositionZ(properties.GetPositionZ());

    SetPivot(properties.GetPivot());

    SetCornerRadius(properties.GetCornerRadius());

    SetRotation(properties.GetQuaternion());
    SetRotation(properties.GetRotation());
    SetRotationX(properties.GetRotationX());
    SetRotationY(properties.GetRotationY());
    SetTranslate(properties.GetTranslate());
    SetScale(properties.GetScale());

    SetAlpha(properties.GetAlpha());

    SetForegroundColor(properties.GetForegroundColor().AsArgbInt());
    SetBackgroundColor(properties.GetBackgroundColor().AsArgbInt());
    SetBackgroundShader(properties.GetBackgroundShader());
    SetBgImage(properties.GetBgImage());
    SetBgImageSize(properties.GetBgImageHeight(), properties.GetBgImageWidth());
    SetBgImagePosition(properties.GetBgImagePositionX(), properties.GetBgImagePositionY());

    SetBorderColor(properties.GetBorderColor());
    SetBorderStyle(properties.GetBorderStyle());
    SetBorderWidth(properties.GetBorderWidth());

    SetSublayerTransform(properties.GetSublayerTransform());

    SetBackgroundFilter(properties.GetBackgroundFilter());
    SetFilter(properties.GetFilter());
    SetCompositingFilter(nullptr);

    SetShadowColor(properties.GetShadowColor().AsArgbInt());
    SetShadowOffset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    SetShadowAlpha(properties.GetShadowAlpha());
    SetShadowElevation(properties.GetShadowElevation());
    SetShadowRadius(properties.GetShadowRadius());
    SetShadowPath(properties.GetShadowPath());

    SetFrameGravity(properties.GetFrameGravity());
    SetClipBounds(properties.GetClipBounds());
    SetClipToBounds(properties.GetClipToBounds());
    SetClipToFrame(properties.GetClipToFrame());

    SetVisible(properties.GetVisible());
    SetPaintOrder(other.drawContentLast_);
    SetMask(properties.GetMask());

    SetTransitionEffect(other.transitionEffect_);
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
        FinishAnimationByProperty(animation->GetProperty());
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
}

const std::shared_ptr<RSMotionPathOption> RSNode::GetMotionPathOption() const
{
    return motionPathOption_;
}

bool RSNode::HasPropertyAnimation(const RSAnimatableProperty& property) const
{
    // check if any animation matches the property bitmask
    auto pred = [property](const auto& it) -> bool {
        return it.second > 0 && (static_cast<unsigned long long>(it.first) & static_cast<unsigned long long>(property));
    };
    return std::any_of(animatingPropertyNum_.begin(), animatingPropertyNum_.end(), pred);
}

namespace {
template<typename T>
bool IsValid(const T& value)
{
    return true;
}
template<>
bool IsValid(const float& value)
{
    return !isinf(value);
}
template<>
bool IsValid(const Vector2f& value)
{
    return !value.IsInfinite();
}
template<>
bool IsValid(const Vector4f& value)
{
    return !value.IsInfinite();
}
} // namespace

#define SET_ANIMATABLE_PROPERTY(propertyName, value, property)                                              \
    do {                                                                                                    \
        auto currentValue = stagingProperties_.Get##propertyName();                                         \
        if (ROSEN_EQ(value, currentValue)) {                                                                \
            return;                                                                                         \
        }                                                                                                   \
        UpdateImplicitAnimator();                                                                           \
        if (implicitAnimator_ && implicitAnimator_->NeedImplicitAnimation() && IsValid(currentValue)) {     \
            implicitAnimator_->CreateImplicitAnimation(*this, property, currentValue, value);               \
        } else if (HasPropertyAnimation(property)) {                                                        \
            std::unique_ptr<RSCommand> command =                                                            \
                std::make_unique<RSNodeSet##propertyName##Delta>(GetId(), (value)-currentValue);            \
            auto transactionProxy = RSTransactionProxy::GetInstance();                                      \
            if (transactionProxy != nullptr) {                                                              \
                transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());           \
                if (NeedForcedSendToRemote()) {                                                             \
                    std::unique_ptr<RSCommand> commandForRemote =                                           \
                        std::make_unique<RSNodeSet##propertyName##Delta>(GetId(), (value)-currentValue);    \
                    transactionProxy->AddCommand(commandForRemote, true, GetFollowType(), GetId());               \
                }                                                                                           \
            }                                                                                               \
            stagingProperties_.Set##propertyName(value);                                                    \
        } else {                                                                                            \
            std::unique_ptr<RSCommand> command = std::make_unique<RSNodeSet##propertyName>(GetId(), value); \
            auto transactionProxy = RSTransactionProxy::GetInstance();                                      \
            if (transactionProxy != nullptr) {                                                              \
                transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());           \
                if (NeedForcedSendToRemote()) {                                                             \
                    std::unique_ptr<RSCommand> commandForRemote =                                           \
                        std::make_unique<RSNodeSet##propertyName>(GetId(), value);                          \
                    transactionProxy->AddCommand(commandForRemote, true, GetFollowType(), GetId());               \
                }                                                                                           \
            }                                                                                               \
            stagingProperties_.Set##propertyName(value);                                                    \
        }                                                                                                   \
    } while (0)

#define SET_NONANIMATABLE_PROPERTY(propertyName, value)                                                 \
    do {                                                                                                \
        auto currentValue = stagingProperties_.Get##propertyName();                                     \
        if (ROSEN_EQ(value, currentValue)) {                                                            \
            return;                                                                                     \
        }                                                                                               \
        std::unique_ptr<RSCommand> command = std::make_unique<RSNodeSet##propertyName>(GetId(), value); \
        auto transactionProxy = RSTransactionProxy::GetInstance();                                      \
        if (transactionProxy != nullptr) {                                                              \
            transactionProxy->AddCommand(command, IsRenderServiceNode(), GetFollowType(), GetId());           \
            if (NeedForcedSendToRemote()) {                                                             \
                std::unique_ptr<RSCommand> commandForRemote =                                           \
                    std::make_unique<RSNodeSet##propertyName>(GetId(), value);                          \
                transactionProxy->AddCommand(commandForRemote, true, GetFollowType(), GetId());               \
            }                                                                                           \
        }                                                                                               \
        stagingProperties_.Set##propertyName(value);                                                    \
    } while (0)

#define CREATE_PATH_ANIMATION(propertyName, value, property)                                            \
    do {                                                                                                \
        auto currentValue = stagingProperties_.Get##propertyName();                                     \
        UpdateImplicitAnimator();                                                                       \
        if (implicitAnimator_ && implicitAnimator_->NeedImplicitAnimation() && IsValid(currentValue)) { \
            implicitAnimator_->BeginImplicitPathAnimation(motionPathOption_);                           \
            implicitAnimator_->CreateImplicitAnimation(*this, property, currentValue, value);           \
            implicitAnimator_->EndImplicitPathAnimation();                                              \
            return;                                                                                     \
        } else if (HasPropertyAnimation(property)) {                                                    \
            FinishAnimationByProperty(property);                                                        \
            SET_NONANIMATABLE_PROPERTY(propertyName, value);                                            \
            return;                                                                                     \
        } else {                                                                                        \
            SET_NONANIMATABLE_PROPERTY(propertyName, value);                                            \
            return;                                                                                     \
        }                                                                                               \
    } while (0)

// alpha
void RSNode::SetAlpha(float alpha)
{
    SET_ANIMATABLE_PROPERTY(Alpha, alpha, RSAnimatableProperty::ALPHA);
}

// Bounds
void RSNode::SetBounds(const Vector4f& bounds)
{
    if (motionPathOption_ != nullptr) {
        auto value = bounds;
        SetBoundsPosition(value[0], value[1]);
        SetBoundsSize(value[2], value[3]);
        return;
    }

    SET_ANIMATABLE_PROPERTY(Bounds, bounds, RSAnimatableProperty::BOUNDS);
    OnBoundsSizeChanged();
}

void RSNode::SetBounds(float positionX, float positionY, float width, float height)
{
    SetBounds({ positionX, positionY, width, height });
}

void RSNode::SetBoundsSize(const Vector2f& size)
{
    SET_ANIMATABLE_PROPERTY(BoundsSize, size, RSAnimatableProperty::BOUNDS_SIZE);
    OnBoundsSizeChanged();
}

void RSNode::SetBoundsSize(float width, float height)
{
    SetBoundsSize({ width, height });
}

void RSNode::SetBoundsWidth(float width)
{
    SET_ANIMATABLE_PROPERTY(BoundsWidth, width, RSAnimatableProperty::BOUNDS_WIDTH);
    OnBoundsSizeChanged();
}

void RSNode::SetBoundsHeight(float height)
{
    SET_ANIMATABLE_PROPERTY(BoundsHeight, height, RSAnimatableProperty::BOUNDS_HEIGHT);
    OnBoundsSizeChanged();
}

void RSNode::SetBoundsPosition(const Vector2f& boundsPosition)
{
    if (motionPathOption_ != nullptr) {
        CREATE_PATH_ANIMATION(BoundsPosition, boundsPosition, RSAnimatableProperty::BOUNDS_POSITION);
        return;
    }

    SET_ANIMATABLE_PROPERTY(BoundsPosition, boundsPosition, RSAnimatableProperty::BOUNDS_POSITION);
}

void RSNode::SetBoundsPosition(float positionX, float positionY)
{
    SetBoundsPosition({ positionX, positionY });
}

void RSNode::SetBoundsPositionX(float positionX)
{
    SET_ANIMATABLE_PROPERTY(BoundsPositionX, positionX, RSAnimatableProperty::BOUNDS_POSITION_X);
}

void RSNode::SetBoundsPositionY(float positionY)
{
    SET_ANIMATABLE_PROPERTY(BoundsPositionY, positionY, RSAnimatableProperty::BOUNDS_POSITION_Y);
}

// Frame
void RSNode::SetFrame(const Vector4f& bounds)
{
    if (motionPathOption_ != nullptr) {
        auto value = bounds;
        SetFramePosition(value[0], value[1]);
        SetFrameSize(value[2], value[3]);
        return;
    }

    SET_ANIMATABLE_PROPERTY(Frame, bounds, RSAnimatableProperty::FRAME);
}

void RSNode::SetFrame(float positionX, float positionY, float width, float height)
{
    SetFrame({ positionX, positionY, width, height });
}

void RSNode::SetFrameSize(const Vector2f& size)
{
    SET_ANIMATABLE_PROPERTY(FrameSize, size, RSAnimatableProperty::FRAME_SIZE);
}

void RSNode::SetFrameSize(float width, float height)
{
    SetFrameSize({ width, height });
}

void RSNode::SetFrameWidth(float width)
{
    SET_ANIMATABLE_PROPERTY(FrameWidth, width, RSAnimatableProperty::FRAME_WIDTH);
}

void RSNode::SetFrameHeight(float height)
{
    SET_ANIMATABLE_PROPERTY(FrameHeight, height, RSAnimatableProperty::FRAME_HEIGHT);
}

void RSNode::SetFramePosition(const Vector2f& position)
{
    if (motionPathOption_ != nullptr) {
        CREATE_PATH_ANIMATION(FramePosition, position, RSAnimatableProperty::FRAME_POSITION);
        return;
    }

    SET_ANIMATABLE_PROPERTY(FramePosition, position, RSAnimatableProperty::FRAME_POSITION);
}

void RSNode::SetFramePosition(float positionX, float positionY)
{
    SetFramePosition({ positionX, positionY });
}

void RSNode::SetFramePositionX(float positionX)
{
    SET_ANIMATABLE_PROPERTY(FramePositionX, positionX, RSAnimatableProperty::FRAME_POSITION_X);
}

void RSNode::SetFramePositionY(float positionY)
{
    SET_ANIMATABLE_PROPERTY(FramePositionY, positionY, RSAnimatableProperty::FRAME_POSITION_Y);
}

void RSNode::SetPositionZ(float positionZ)
{
    SET_ANIMATABLE_PROPERTY(PositionZ, positionZ, RSAnimatableProperty::POSITION_Z);
}

// pivot
void RSNode::SetPivot(const Vector2f& pivot)
{
    SET_ANIMATABLE_PROPERTY(Pivot, pivot, RSAnimatableProperty::PIVOT);
}

void RSNode::SetPivot(float pivotX, float pivotY)
{
    SetPivot({ pivotX, pivotY });
}

void RSNode::SetPivotX(float pivotX)
{
    SET_ANIMATABLE_PROPERTY(PivotX, pivotX, RSAnimatableProperty::PIVOT_X);
}

void RSNode::SetPivotY(float pivotY)
{
    SET_ANIMATABLE_PROPERTY(PivotY, pivotY, RSAnimatableProperty::PIVOT_Y);
}

void RSNode::SetCornerRadius(float cornerRadius)
{
    SetCornerRadius(Vector4f(cornerRadius));
}

void RSNode::SetCornerRadius(const Vector4f& cornerRadius)
{
    SET_ANIMATABLE_PROPERTY(CornerRadius, cornerRadius, RSAnimatableProperty::CORNER_RADIUS);
}

// transform
void RSNode::SetRotation(const Quaternion& quaternion)
{
    SET_ANIMATABLE_PROPERTY(Quaternion, quaternion, RSAnimatableProperty::ROTATION_3D);
}

void RSNode::SetRotation(float degree)
{
    SET_ANIMATABLE_PROPERTY(Rotation, degree, RSAnimatableProperty::ROTATION);
}

void RSNode::SetRotation(float degreeX, float degreeY, float degreeZ)
{
    SetRotationX(degreeX);
    SetRotationY(degreeY);
    SetRotation(degreeZ);
}

void RSNode::SetRotationX(float degree)
{
    SET_ANIMATABLE_PROPERTY(RotationX, degree, RSAnimatableProperty::ROTATION_X);
}

void RSNode::SetRotationY(float degree)
{
    SET_ANIMATABLE_PROPERTY(RotationY, degree, RSAnimatableProperty::ROTATION_Y);
}

void RSNode::SetTranslate(const Vector2f& translate)
{
    if (motionPathOption_ != nullptr) {
        CREATE_PATH_ANIMATION(Translate, translate, RSAnimatableProperty::TRANSLATE);
        return;
    }

    SET_ANIMATABLE_PROPERTY(Translate, translate, RSAnimatableProperty::TRANSLATE);
}

void RSNode::SetTranslate(float translateX, float translateY, float translateZ)
{
    SetTranslate({ translateX, translateY });
    SetTranslateZ(translateZ);
}

void RSNode::SetTranslateX(float translate)
{
    SET_ANIMATABLE_PROPERTY(TranslateX, translate, RSAnimatableProperty::TRANSLATE_X);
}

void RSNode::SetTranslateY(float translate)
{
    SET_ANIMATABLE_PROPERTY(TranslateY, translate, RSAnimatableProperty::TRANSLATE_Y);
}

void RSNode::SetTranslateZ(float translate)
{
    SET_ANIMATABLE_PROPERTY(TranslateZ, translate, RSAnimatableProperty::TRANSLATE_Z);
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
    SET_ANIMATABLE_PROPERTY(Scale, scale, RSAnimatableProperty::SCALE);
}

void RSNode::SetScaleX(float scale)
{
    SET_ANIMATABLE_PROPERTY(ScaleX, scale, RSAnimatableProperty::SCALE_X);
}

void RSNode::SetScaleY(float scale)
{
    SET_ANIMATABLE_PROPERTY(ScaleY, scale, RSAnimatableProperty::SCALE_Y);
}

// foreground
void RSNode::SetForegroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SET_ANIMATABLE_PROPERTY(ForegroundColor, color, RSAnimatableProperty::FOREGROUND_COLOR);
}

void RSNode::SetBackgroundColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SET_ANIMATABLE_PROPERTY(BackgroundColor, color, RSAnimatableProperty::BACKGROUND_COLOR);
}

void RSNode::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    SET_NONANIMATABLE_PROPERTY(BackgroundShader, shader);
}

// background
void RSNode::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    SET_NONANIMATABLE_PROPERTY(BgImage, image);
}

void RSNode::SetBgImageSize(float width, float height)
{
    SetBgImageWidth(width);
    SetBgImageHeight(height);
}

void RSNode::SetBgImageWidth(float width)
{
    SET_ANIMATABLE_PROPERTY(BgImageWidth, width, RSAnimatableProperty::BGIMAGE_WIDTH);
}

void RSNode::SetBgImageHeight(float height)
{
    SET_ANIMATABLE_PROPERTY(BgImageHeight, height, RSAnimatableProperty::BGIMAGE_HEIGHT);
}

void RSNode::SetBgImagePosition(float positionX, float positionY)
{
    SetBgImagePositionX(positionX);
    SetBgImagePositionY(positionY);
}

void RSNode::SetBgImagePositionX(float positionX)
{
    SET_ANIMATABLE_PROPERTY(BgImagePositionX, positionX, RSAnimatableProperty::BGIMAGE_POSITION_X);
}

void RSNode::SetBgImagePositionY(float positionY)
{
    SET_ANIMATABLE_PROPERTY(BgImagePositionY, positionY, RSAnimatableProperty::BGIMAGE_POSITION_Y);
}

// border
void RSNode::SetBorderColor(uint32_t colorValue)
{
    Vector4<Color> color(Color::FromArgbInt(colorValue));
    SET_ANIMATABLE_PROPERTY(BorderColor, color, RSAnimatableProperty::BORDER_COLOR);
}

void RSNode::SetBorderColor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    Vector4<Color> color(
        Color::FromArgbInt(left), Color::FromArgbInt(top), Color::FromArgbInt(right), Color::FromArgbInt(bottom));
    SET_ANIMATABLE_PROPERTY(BorderColor, color, RSAnimatableProperty::BORDER_COLOR);
}

void RSNode::SetBorderColor(const Vector4<Color>& color)
{
    SET_ANIMATABLE_PROPERTY(BorderColor, color, RSAnimatableProperty::BORDER_COLOR);
}

void RSNode::SetBorderWidth(float width)
{
    SET_ANIMATABLE_PROPERTY(BorderWidth, Vector4f(width), RSAnimatableProperty::BORDER_WIDTH);
}

void RSNode::SetBorderWidth(float left, float top, float right, float bottom)
{
    Vector4f width(left, top, right, bottom);
    SET_ANIMATABLE_PROPERTY(BorderWidth, width, RSAnimatableProperty::BORDER_WIDTH);
}

void RSNode::SetBorderWidth(const Vector4f& width)
{
    SET_ANIMATABLE_PROPERTY(BorderWidth, width, RSAnimatableProperty::BORDER_WIDTH);
}

void RSNode::SetBorderStyle(uint32_t styleValue)
{
    Vector4<BorderStyle> style(static_cast<BorderStyle>(styleValue));
    SET_NONANIMATABLE_PROPERTY(BorderStyle, style);
}

void RSNode::SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    Vector4<BorderStyle> style(static_cast<BorderStyle>(left), static_cast<BorderStyle>(top),
        static_cast<BorderStyle>(right), static_cast<BorderStyle>(bottom));
    SET_NONANIMATABLE_PROPERTY(BorderStyle, style);
}

void RSNode::SetBorderStyle(const Vector4<BorderStyle>& style)
{
    SET_NONANIMATABLE_PROPERTY(BorderStyle, style);
}

// others
void RSNode::SetSublayerTransform(Matrix3f sublayerTransform)
{
    SET_ANIMATABLE_PROPERTY(SublayerTransform, sublayerTransform, RSAnimatableProperty::SUB_LAYER_TRANSFORM);
}

void RSNode::SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter)
{
    SET_ANIMATABLE_PROPERTY(BackgroundFilter, backgroundFilter, RSAnimatableProperty::BACKGROUND_FILTER);
}

void RSNode::SetFilter(const std::shared_ptr<RSFilter>& filter)
{
    SET_ANIMATABLE_PROPERTY(Filter, filter, RSAnimatableProperty::FILTER);
}

void RSNode::SetCompositingFilter(const std::shared_ptr<RSFilter>& compositingFilter) {}

void RSNode::SetShadowColor(uint32_t colorValue)
{
    auto color = Color::FromArgbInt(colorValue);
    SET_ANIMATABLE_PROPERTY(ShadowColor, color, RSAnimatableProperty::SHADOW_COLOR);
}

void RSNode::SetShadowOffset(float offsetX, float offsetY)
{
    SetShadowOffsetX(offsetX);
    SetShadowOffsetY(offsetY);
}

void RSNode::SetShadowOffsetX(float offsetX)
{
    SET_ANIMATABLE_PROPERTY(ShadowOffsetX, offsetX, RSAnimatableProperty::SHADOW_OFFSET_X);
}

void RSNode::SetShadowOffsetY(float offsetY)
{
    SET_ANIMATABLE_PROPERTY(ShadowOffsetY, offsetY, RSAnimatableProperty::SHADOW_OFFSET_Y);
}

void RSNode::SetShadowAlpha(float alpha)
{
    SET_ANIMATABLE_PROPERTY(ShadowAlpha, alpha, RSAnimatableProperty::SHADOW_ALPHA);
}

void RSNode::SetShadowElevation(float elevation)
{
    SET_ANIMATABLE_PROPERTY(ShadowElevation, elevation, RSAnimatableProperty::SHADOW_ELEVATION);
}

void RSNode::SetShadowRadius(float radius)
{
    SET_ANIMATABLE_PROPERTY(ShadowRadius, radius, RSAnimatableProperty::SHADOW_RADIUS);
}

void RSNode::SetShadowPath(const std::shared_ptr<RSPath>& shadowPath)
{
    SET_NONANIMATABLE_PROPERTY(ShadowPath, shadowPath);
}

void RSNode::SetFrameGravity(Gravity gravity)
{
    SET_NONANIMATABLE_PROPERTY(FrameGravity, gravity);
}

void RSNode::SetClipBounds(const std::shared_ptr<RSPath>& path)
{
    SET_NONANIMATABLE_PROPERTY(ClipBounds, path);
}

void RSNode::SetClipToBounds(bool clipToBounds)
{
    SET_NONANIMATABLE_PROPERTY(ClipToBounds, clipToBounds);
}

void RSNode::SetClipToFrame(bool clipToFrame)
{
    SET_NONANIMATABLE_PROPERTY(ClipToFrame, clipToFrame);
}

void RSNode::SetVisible(bool visible)
{
    SET_NONANIMATABLE_PROPERTY(Visible, visible);
    if (transitionEffect_ != nullptr) {
        NotifyTransition(transitionEffect_, visible);
    }
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

    implicitAnimator_->BeginImplicitTransition(effect);
    implicitAnimator_->CreateImplicitTransition(*this, isTransitionIn);
    implicitAnimator_->EndImplicitTransition();
}

void RSNode::OnAddChildren()
{
    if (transitionEffect_ != nullptr) {
        NotifyTransition(transitionEffect_, true);
    }
}

void RSNode::OnRemoveChildren()
{
    if (transitionEffect_ != nullptr) {
        NotifyTransition(transitionEffect_, false);
    }
}

void RSNode::AnimationFinish(AnimationId animationId)
{
    auto animationItr = animations_.find(animationId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("Failed to find animation[%llu]!", animationId);
        return;
    }

    auto animation = animationItr->second;
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to finish animation[%llu], animation is null!", animationId);
        return;
    }

    animation->CallFinishCallback();
    RemoveAnimationInner(animation);
}

void RSNode::SetPaintOrder(bool drawContentLast)
{
    drawContentLast_ = drawContentLast;
}

std::string RSNode::DumpNode(int depth) const
{
    std::stringstream ss;
    ss << RSBaseNode::DumpNode(depth);
    if (!animations_.empty()) {
        ss << " animation:" << std::to_string(animations_.size());
    }
    ss << " " << stagingProperties_.Dump();
    return ss.str();
}

void RSNode::SetMask(const std::shared_ptr<RSMask>& mask)
{
    SET_NONANIMATABLE_PROPERTY(Mask, mask);
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
} // namespace Rosen
} // namespace OHOS
