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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_H

#include <type_traits>
#include <unistd.h>

#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_ui_animation_manager.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_animatable_arithmetic.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_modifier_manager_map.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_node_map.h"
#include "property/rs_properties_def.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
template<class...>
struct make_void { using type = void; };
template<class... T>
using void_t = typename make_void<T...>::type;

template<class T, class = void>
struct supports_arithmetic : std::false_type {};
template<class T>
struct supports_arithmetic<T,
    void_t<decltype(std::declval<T>() == std::declval<T>())>>
        : std::true_type {};

template<class T, class = void>
struct supports_animatable_arithmetic : std::false_type {};
template<class T>
struct supports_animatable_arithmetic<T,
    void_t<decltype(std::declval<T>() + std::declval<T>()),
        decltype(std::declval<T>() - std::declval<T>()),
        decltype(std::declval<T>() * std::declval<float>()),
        decltype(std::declval<T>() == std::declval<T>())>>
        : std::true_type {};

class RS_EXPORT RSPropertyBase : public std::enable_shared_from_this<RSPropertyBase> {
public:
    RSPropertyBase();
    virtual ~RSPropertyBase() = default;

    PropertyId GetId() const
    {
        return id_;
    }

protected:
    virtual void SetIsCustom(bool isCustom) {}

    virtual bool GetIsCustom() const
    {
        return false;
    }

    virtual void SetValue(const std::shared_ptr<RSPropertyBase>& value) {}

    virtual std::shared_ptr<RSPropertyBase> Clone() const
    {
        return std::make_shared<RSPropertyBase>();
    }

    virtual void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption) {}

    virtual RSRenderPropertyType GetPropertyType() const
    {
        return RSRenderPropertyType::INVALID;
    }

    virtual void UpdateOnAllAnimationFinish() {}

    virtual void AddPathAnimation() {}

    virtual void RemovePathAnimation() {}

    virtual void UpdateShowingValue(const std::shared_ptr<const RSRenderPropertyBase>& property) {}

    void AttachModifier(const std::shared_ptr<RSModifier>& modifier)
    {
        modifier_ = modifier;
    }

    void MarkModifierDirty()
    {
        auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
        if (modifierManager == nullptr) {
            ROSEN_LOGE("Modifier manager is null while mark dirty propertyId: %llu!", GetId());
            return;
        }

        auto modifier = modifier_.lock();
        if (modifier != nullptr) {
            modifierManager->AddModifier(modifier);
        }
    }

    void UpdateExtendModifierForGeometry(const std::shared_ptr<RSNode>& node)
    {
        if (type_ == RSModifierType::BOUNDS || type_ == RSModifierType::FRAME) {
            node->MarkAllExtendModifierDirty();
        }
    }

    virtual std::shared_ptr<RSRenderPropertyBase> GetRenderProperty()
    {
        return std::make_shared<RSRenderPropertyBase>(id_);
    }

    PropertyId id_;
    RSModifierType type_ { RSModifierType::INVALID };
    std::weak_ptr<RSNode> target_;
    std::weak_ptr<RSModifier> modifier_;

private:
    virtual std::shared_ptr<RSPropertyBase> Add(const std::shared_ptr<const RSPropertyBase>& value)
    {
        return shared_from_this();
    }

    virtual std::shared_ptr<RSPropertyBase> Minus(const std::shared_ptr<const RSPropertyBase>& value)
    {
        return shared_from_this();
    }

    virtual std::shared_ptr<RSPropertyBase> Multiply(const float scale)
    {
        return shared_from_this();
    }

    virtual bool IsEqual(const std::shared_ptr<const RSPropertyBase>& value) const
    {
        return true;
    }

    friend std::shared_ptr<RSPropertyBase> operator+=(
        const std::shared_ptr<RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend std::shared_ptr<RSPropertyBase> operator-=(
        const std::shared_ptr<RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend std::shared_ptr<RSPropertyBase> operator*=(const std::shared_ptr<RSPropertyBase>& value, const float scale);
    friend std::shared_ptr<RSPropertyBase> operator+(
        const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend std::shared_ptr<RSPropertyBase> operator-(
        const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend std::shared_ptr<RSPropertyBase> operator*(
        const std::shared_ptr<const RSPropertyBase>& value, const float scale);
    friend bool operator==(
        const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend bool operator!=(
        const std::shared_ptr<const RSPropertyBase>& a, const std::shared_ptr<const RSPropertyBase>& b);
    friend class RSCurveAnimation;
    friend class RSCustomTransitionEffect;
    friend class RSExtendedModifier;
    friend class RSImplicitAnimator;
    friend class RSImplicitCurveAnimationParam;
    friend class RSImplicitKeyframeAnimationParam;
    friend class RSImplicitSpringAnimationParam;
    friend class RSImplicitTransitionParam;
    friend class RSModifier;
    friend class RSPropertyAnimation;
    friend class RSPathAnimation;
    friend class RSKeyframeAnimation;
    friend class RSSpringAnimation;
    friend class RSTransition;
    friend class RSUIAnimationManager;
    template<typename T1>
    friend class RSAnimatableProperty;
};

template<typename T>
class RS_EXPORT RSProperty : public RSPropertyBase {
    static_assert(std::is_base_of_v<RSArithmetic<T>, T> || supports_arithmetic<T>::value);

public:
    RSProperty() : RSPropertyBase() {}
    explicit RSProperty(const T& value) : RSPropertyBase()
    {
        stagingValue_ = value;
    }
    virtual ~RSProperty() = default;

    virtual void Set(const T& value)
    {
        if (ROSEN_EQ(value, stagingValue_) || !IsValid(value)) {
            return;
        }

        stagingValue_ = value;
        auto node = target_.lock();
        if (node == nullptr) {
            return;
        }

        UpdateExtendModifierForGeometry(node);
        if (isCustom_) {
            MarkModifierDirty();
        } else {
            UpdateToRender(stagingValue_, false);
        }
    }

    virtual T Get() const
    {
        return stagingValue_;
    }

protected:
    void UpdateToRender(const T& value, bool isDelta, bool forceUpdate = false) const
    {}

    void SetValue(const std::shared_ptr<RSPropertyBase>& value) override
    {
        auto property = std::static_pointer_cast<RSProperty<T>>(value);
        if (property != nullptr) {
            stagingValue_ = property->stagingValue_;
        }
    }

    std::shared_ptr<RSPropertyBase> Clone() const override
    {
        return std::make_shared<RSProperty<T>>(stagingValue_);
    }

    bool IsValid(const T& value)
    {
        return true;
    }

    void SetIsCustom(bool isCustom) override
    {
        isCustom_ = isCustom;
    }

    bool GetIsCustom() const override
    {
        return isCustom_;
    }

    std::shared_ptr<RSRenderPropertyBase> GetRenderProperty() override
    {
        return std::make_shared<RSRenderProperty<T>>(stagingValue_, id_);
    }

    T stagingValue_ {};
    bool isCustom_ { false };

    friend class RSPathAnimation;
    friend class RSImplicitAnimator;
    friend class RSExtendedModifier;
    friend class RSModifier;
};

template<typename T>
class RS_EXPORT RSAnimatableProperty : public RSProperty<T> {
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<Color, T> ||
                  std::is_same_v<Matrix3f, T> || std::is_same_v<Vector2f, T> || std::is_same_v<Vector4f, T> ||
                  std::is_same_v<Quaternion, T> || std::is_same_v<std::shared_ptr<RSFilter>, T> ||
                  std::is_same_v<Vector4<Color>, T> || std::is_base_of_v<RSAnimatableArithmetic<T>, T> ||
                  supports_animatable_arithmetic<T>::value);

public:
    RSAnimatableProperty() : RSProperty<T>() {}
    explicit RSAnimatableProperty(const T& value) : RSProperty<T>(value)
    {
        showingValue_ = value;
    }

    virtual ~RSAnimatableProperty() = default;

    void Set(const T& value) override
    {
        if (ROSEN_EQ(value, RSProperty<T>::stagingValue_) || !RSProperty<T>::IsValid(value)) {
            return;
        }

        auto node = RSProperty<T>::target_.lock();
        if (node == nullptr) {
            RSProperty<T>::stagingValue_ = value;
            return;
        }

        RSProperty<T>::UpdateExtendModifierForGeometry(node);
        auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
        if (implicitAnimator && implicitAnimator->NeedImplicitAnimation()) {
            auto startValue = std::make_shared<RSAnimatableProperty<T>>(RSProperty<T>::stagingValue_);
            auto endValue = std::make_shared<RSAnimatableProperty<T>>(value);
            if (motionPathOption_ != nullptr) {
                implicitAnimator->BeginImplicitPathAnimation(motionPathOption_);
                implicitAnimator->CreateImplicitAnimation(
                    node, RSProperty<T>::shared_from_this(), startValue, endValue);
                implicitAnimator->EndImplicitPathAnimation();
            } else {
                implicitAnimator->CreateImplicitAnimation(
                    node, RSProperty<T>::shared_from_this(), startValue, endValue);
            }
            return;
        }

        if (runningPathNum_ > 0) {
            return;
        }

        bool hasPropertyAnimation = node->HasPropertyAnimation(RSProperty<T>::id_);
        T sendValue = value;
        if (hasPropertyAnimation) {
            sendValue = value - RSProperty<T>::stagingValue_;
        }
        RSProperty<T>::stagingValue_ = value;
        if (RSProperty<T>::isCustom_) {
            UpdateExtendedAnimatableProperty(sendValue, hasPropertyAnimation);
        } else {
            RSProperty<T>::UpdateToRender(sendValue, hasPropertyAnimation);
        }
    }

    T Get() const override
    {
        if (RSProperty<T>::isCustom_) {
            return showingValue_;
        }
        return RSProperty<T>::stagingValue_;
    }

protected:
    void UpdateOnAllAnimationFinish() override
    {
        RSProperty<T>::UpdateToRender(RSProperty<T>::stagingValue_, false, true);
    }

    void UpdateExtendedAnimatableProperty(const T& value, bool isDelta)
    {
        if (isDelta) {
            if (renderProperty_ != nullptr) {
                renderProperty_->Set(renderProperty_->Get() + value);
            }
        } else {
            showingValue_ = value;
            RSProperty<T>::MarkModifierDirty();
            if (renderProperty_ != nullptr) {
                renderProperty_->Set(value);
            }
        }
    }

    void AddPathAnimation() override
    {
        runningPathNum_ += 1;
    }

    void RemovePathAnimation() override
    {
        runningPathNum_ -= 1;
    }

    void UpdateShowingValue(const std::shared_ptr<const RSRenderPropertyBase>& property) override
    {
        auto renderProperty = std::static_pointer_cast<const RSRenderProperty<T>>(property);
        if (renderProperty != nullptr) {
            showingValue_ = renderProperty->Get();
            RSProperty<T>::MarkModifierDirty();
        }
    }

    void SetValue(const std::shared_ptr<RSPropertyBase>& value) override
    {
        auto property = std::static_pointer_cast<RSAnimatableProperty<T>>(value);
        if (property != nullptr && property->GetPropertyType() == GetPropertyType()) {
            RSProperty<T>::stagingValue_ = property->stagingValue_;
        }
    }

    std::shared_ptr<RSPropertyBase> Clone() const override
    {
        return std::make_shared<RSAnimatableProperty<T>>(RSProperty<T>::stagingValue_);
    }

    void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption) override
    {
        motionPathOption_ = motionPathOption;
    }

    std::shared_ptr<RSRenderPropertyBase> GetRenderProperty() override
    {
        if (!RSProperty<T>::isCustom_) {
            return std::make_shared<RSRenderAnimatableProperty<T>>(
                RSProperty<T>::stagingValue_, RSProperty<T>::id_, GetPropertyType());
        }

        if (renderProperty_ == nullptr) {
            renderProperty_ = std::make_shared<RSRenderAnimatableProperty<T>>(
                RSProperty<T>::stagingValue_, RSProperty<T>::id_, GetPropertyType());
            auto weak = RSProperty<T>::weak_from_this();
            renderProperty_->SetUpdateUIPropertyFunc(
                [weak](const std::shared_ptr<RSRenderPropertyBase>& renderProperty) {
                    auto property = weak.lock();
                    if (property == nullptr) {
                        return;
                    }
                    property->UpdateShowingValue(renderProperty);
                });
        }
        return renderProperty_;
    }

    T showingValue_ {};
    std::shared_ptr<RSRenderAnimatableProperty<T>> renderProperty_;
    int runningPathNum_ { 0 };
    std::shared_ptr<RSMotionPathOption> motionPathOption_ {};

private:
    RSRenderPropertyType GetPropertyType() const override
    {
        return RSRenderPropertyType::INVALID;
    }

    std::shared_ptr<RSPropertyBase> Add(const std::shared_ptr<const RSPropertyBase>& value) override
    {
        auto animatableProperty = std::static_pointer_cast<const RSAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            RSProperty<T>::stagingValue_ = RSProperty<T>::stagingValue_ + animatableProperty->stagingValue_;
        }
        return RSProperty<T>::shared_from_this();
    }

    std::shared_ptr<RSPropertyBase> Minus(const std::shared_ptr<const RSPropertyBase>& value) override
    {
        auto animatableProperty = std::static_pointer_cast<const RSAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            RSProperty<T>::stagingValue_ = RSProperty<T>::stagingValue_ - animatableProperty->stagingValue_;
        }
        return RSProperty<T>::shared_from_this();
    }

    std::shared_ptr<RSPropertyBase> Multiply(const float scale) override
    {
        RSProperty<T>::stagingValue_ = RSProperty<T>::stagingValue_ * scale;
        return RSProperty<T>::shared_from_this();
    }

    bool IsEqual(const std::shared_ptr<const RSPropertyBase>& value) const override
    {
        auto animatableProperty = std::static_pointer_cast<const RSAnimatableProperty<T>>(value);
        if (animatableProperty != nullptr) {
            return RSProperty<T>::stagingValue_ == animatableProperty->stagingValue_;
        }
        return true;
    }

    friend class RSPropertyAnimation;
    friend class RSPathAnimation;
    friend class RSUIAnimationManager;
    friend class RSExtendedModifier;
    friend class RSModifier;
};

template<>
RS_EXPORT void RSProperty<bool>::UpdateToRender(const bool& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<float>::UpdateToRender(const float& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<int>::UpdateToRender(const int& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<Color>::UpdateToRender(const Color& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<Gravity>::UpdateToRender(const Gravity& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<Matrix3f>::UpdateToRender(const Matrix3f& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<Quaternion>::UpdateToRender(const Quaternion& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<std::shared_ptr<RSFilter>>::UpdateToRender(
    const std::shared_ptr<RSFilter>& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<std::shared_ptr<RSImage>>::UpdateToRender(
    const std::shared_ptr<RSImage>& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<std::shared_ptr<RSMask>>::UpdateToRender(
    const std::shared_ptr<RSMask>& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<std::shared_ptr<RSPath>>::UpdateToRender(
    const std::shared_ptr<RSPath>& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<std::shared_ptr<RSShader>>::UpdateToRender(
    const std::shared_ptr<RSShader>& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<Vector2f>::UpdateToRender(const Vector2f& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<Vector4<uint32_t>>::UpdateToRender(
    const Vector4<uint32_t>& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<Vector4<Color>>::UpdateToRender(
    const Vector4<Color>& value, bool isDelta, bool forceUpdate) const;
template<>
RS_EXPORT void RSProperty<Vector4f>::UpdateToRender(const Vector4f& value, bool isDelta, bool forceUpdate) const;

template<>
RS_EXPORT bool RSProperty<float>::IsValid(const float& value);
template<>
RS_EXPORT bool RSProperty<Vector2f>::IsValid(const Vector2f& value);
template<>
RS_EXPORT bool RSProperty<Vector4f>::IsValid(const Vector4f& value);

template<>
RS_EXPORT RSRenderPropertyType RSAnimatableProperty<float>::GetPropertyType() const;
template<>
RS_EXPORT RSRenderPropertyType RSAnimatableProperty<Color>::GetPropertyType() const;
template<>
RS_EXPORT RSRenderPropertyType RSAnimatableProperty<Matrix3f>::GetPropertyType() const;
template<>
RS_EXPORT RSRenderPropertyType RSAnimatableProperty<Vector2f>::GetPropertyType() const;
template<>
RS_EXPORT RSRenderPropertyType RSAnimatableProperty<Vector4f>::GetPropertyType() const;
template<>
RS_EXPORT RSRenderPropertyType RSAnimatableProperty<Quaternion>::GetPropertyType() const;
template<>
RS_EXPORT RSRenderPropertyType RSAnimatableProperty<std::shared_ptr<RSFilter>>::GetPropertyType() const;
template<>
RS_EXPORT RSRenderPropertyType RSAnimatableProperty<Vector4<Color>>::GetPropertyType() const;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_H
