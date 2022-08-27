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

#include "modifier/rs_property.h"

#include "command/rs_node_command.h"
#include "common/rs_color.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int PID_SHIFT = 32;

PropertyId GeneratePropertyId()
{
    static pid_t pid_ = getpid();
    static std::atomic<uint32_t> currentId_ = 1;

    ++currentId_;
    if (currentId_ == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Property Id overflow");
    }

    return ((PropertyId)pid_ << PID_SHIFT) | currentId_;
}
} // namespace

RSPropertyBase::RSPropertyBase() : id_(GeneratePropertyId()) {}

std::shared_ptr<RSPropertyBase> operator+(
    const std::shared_ptr<RSPropertyBase>& a, const std::shared_ptr<RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Add(b);
}

std::shared_ptr<RSPropertyBase> operator-(
    const std::shared_ptr<RSPropertyBase>& a, const std::shared_ptr<RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Minus(b);
}

std::shared_ptr<RSPropertyBase> operator*(const std::shared_ptr<RSPropertyBase>& value, const float scale)
{
    if (value == nullptr) {
        return {};
    }

    return value->Multiply(scale);
}

bool operator==(const std::shared_ptr<RSPropertyBase>& a, const std::shared_ptr<RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->IsEqual(b);
}

bool operator!=(const std::shared_ptr<RSPropertyBase>& a, const std::shared_ptr<RSPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return !a->IsEqual(b);
}

template<typename T>
template<typename Command>
void RSProperty<T>::UpdateToRenderImpl(const T& value, bool isDelta, bool forceUpdate) const
{
    auto node = target_.lock();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr || node == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<Command>(node->GetId(), value, id_, isDelta);
    if (forceUpdate) {
        transactionProxy->Begin();
    }
    transactionProxy->AddCommand(command, node->IsRenderServiceNode(), node->GetFollowType(), node->GetId());
    if (node->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> commandForRemote = std::make_unique<Command>(node->GetId(), value, id_, isDelta);
        transactionProxy->AddCommand(commandForRemote, true, node->GetFollowType(), node->GetId());
    }
    if (node->NeedSendExtraCommand()) {
        std::unique_ptr<RSCommand> extraCommand = std::make_unique<Command>(node->GetId(), value, id_, isDelta);
        transactionProxy->AddCommand(extraCommand, !node->IsRenderServiceNode(), node->GetFollowType(), node->GetId());
    }
    if (forceUpdate) {
        transactionProxy->Commit();
    }
}

template<typename T>
void RSProperty<T>::UpdateToRender(const T& value, bool isDelta, bool forceUpdate) const
{}
template<>
void RSProperty<bool>::UpdateToRender(const bool& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyBool>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<float>::UpdateToRender(const float& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyFloat>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<int>::UpdateToRender(const int& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyInt>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<Color>::UpdateToRender(const Color& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyColor>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<Gravity>::UpdateToRender(const Gravity& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyGravity>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<Matrix3f>::UpdateToRender(const Matrix3f& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyMatrix3f>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<Quaternion>::UpdateToRender(const Quaternion& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyQuaternion>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<std::shared_ptr<RSFilter>>::UpdateToRender(
    const std::shared_ptr<RSFilter>& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyFilter>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<std::shared_ptr<RSImage>>::UpdateToRender(
    const std::shared_ptr<RSImage>& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyImage>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<std::shared_ptr<RSMask>>::UpdateToRender(
    const std::shared_ptr<RSMask>& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyMask>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<std::shared_ptr<RSPath>>::UpdateToRender(
    const std::shared_ptr<RSPath>& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyPath>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<std::shared_ptr<RSShader>>::UpdateToRender(
    const std::shared_ptr<RSShader>& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyShader>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<Vector2f>::UpdateToRender(const Vector2f& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyVector2f>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<Vector4<uint32_t>>::UpdateToRender(const Vector4<uint32_t>& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyBorderStyle>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<Vector4<Color>>::UpdateToRender(const Vector4<Color>& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyVector4Color>(value, isDelta, forceUpdate);
}
template<>
void RSProperty<Vector4f>::UpdateToRender(const Vector4f& value, bool isDelta, bool forceUpdate) const
{
    UpdateToRenderImpl<RSUpdatePropertyVector4f>(value, isDelta, forceUpdate);
}

template<typename T>
bool RSProperty<T>::IsValid(const T& value)
{
    return true;
}
template<>
bool RSProperty<float>::IsValid(const float& value)
{
    return !isinf(value);
}
template<>
bool RSProperty<Vector2f>::IsValid(const Vector2f& value)
{
    return !value.IsInfinite();
}
template<>
bool RSProperty<Vector4f>::IsValid(const Vector4f& value)
{
    return !value.IsInfinite();
}

template<typename T>
RSRenderPropertyType RSAnimatableProperty<T>::GetPropertyType() const
{
    return RSRenderPropertyType::INVALID;
}
template<>
RSRenderPropertyType RSAnimatableProperty<float>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_FLOAT;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Color>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_COLOR;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Matrix3f>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_MATRIX3F;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Vector2f>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_VECTOR2F;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Vector4f>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_VECTOR4F;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Quaternion>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_QUATERNION;
}
template<>
RSRenderPropertyType RSAnimatableProperty<std::shared_ptr<RSFilter>>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_FILTER;
}
template<>
RSRenderPropertyType RSAnimatableProperty<Vector4<Color>>::GetPropertyType() const
{
    return RSRenderPropertyType::PROPERTY_VECTOR4_COLOR;
}

template<typename T>
std::shared_ptr<RSRenderPropertyBase> RSAnimatableProperty<T>::CreateRenderProperty()
{
    return std::make_shared<RSRenderAnimatableProperty<T>>(
        RSProperty<T>::stagingValue_, RSProperty<T>::id_, GetPropertyType());
}

template<typename T>
void RSAnimatableProperty<T>::SetValue(const std::shared_ptr<RSPropertyBase>& value)
{
    auto property = std::static_pointer_cast<RSAnimatableProperty<T>>(value);
    if (property != nullptr && property->GetPropertyType() == GetPropertyType()) {
        RSProperty<T>::stagingValue_ = property->stagingValue_;
    }
}

template<typename T>
void RSAnimatableProperty<T>::Set(const T& value)
{
    if (ROSEN_EQ(value, RSProperty<T>::stagingValue_) || !RSProperty<T>::IsValid(value)) {
        return;
    }

    auto node = this->target_.lock();
    if (node == nullptr) {
        RSProperty<T>::stagingValue_ = value;
        return;
    }

    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator && implicitAnimator->NeedImplicitAnimation()) {
        auto startValue = std::make_shared<RSAnimatableProperty<T>>(RSProperty<T>::stagingValue_);
        auto endValue = std::make_shared<RSAnimatableProperty<T>>(value);
        if (RSProperty<T>::motionPathOption_ != nullptr) {
            implicitAnimator->BeginImplicitPathAnimation(RSProperty<T>::motionPathOption_);
            implicitAnimator->CreateImplicitAnimation(node, RSProperty<T>::shared_from_this(), startValue, endValue);
            implicitAnimator->EndImplicitPathAnimation();
        } else {
            implicitAnimator->CreateImplicitAnimation(node, RSProperty<T>::shared_from_this(), startValue, endValue);
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
    RSProperty<T>::UpdateToRender(sendValue, hasPropertyAnimation);
}

template<typename T>
void RSProperty<T>::Set(const T& value)
{
    if (ROSEN_EQ(value, stagingValue_) || !IsValid(value)) {
        return;
    }

    stagingValue_ = value;
    if (this->target_.lock() == nullptr) {
        return;
    }

    UpdateToRender(stagingValue_, false);
}

template class RS_EXPORT RSProperty<bool>;
template class RS_EXPORT RSProperty<float>;
template class RS_EXPORT RSProperty<int>;
template class RS_EXPORT RSProperty<Color>;
template class RS_EXPORT RSProperty<Gravity>;
template class RS_EXPORT RSProperty<Matrix3f>;
template class RS_EXPORT RSProperty<Quaternion>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSFilter>>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSImage>>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSMask>>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSPath>>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSShader>>;
template class RS_EXPORT RSProperty<Vector2f>;
template class RS_EXPORT RSProperty<Vector4<uint32_t>>;
template class RS_EXPORT RSProperty<Vector4<Color>>;
template class RS_EXPORT RSProperty<Vector4f>;

template class RS_EXPORT RSAnimatableProperty<bool>;
template class RS_EXPORT RSAnimatableProperty<float>;
template class RS_EXPORT RSAnimatableProperty<Color>;
template class RS_EXPORT RSAnimatableProperty<Matrix3f>;
template class RS_EXPORT RSAnimatableProperty<Vector2f>;
template class RS_EXPORT RSAnimatableProperty<Vector4f>;
template class RS_EXPORT RSAnimatableProperty<Quaternion>;
template class RS_EXPORT RSAnimatableProperty<std::shared_ptr<RSFilter>>;
template class RS_EXPORT RSAnimatableProperty<Vector4<Color>>;
} // namespace Rosen
} // namespace OHOS
