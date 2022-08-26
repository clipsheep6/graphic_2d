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

#include "modifier/rs_render_property.h"
#include "command/rs_node_command.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {

#ifdef ROSEN_OHOS
bool RSRenderPropertyBase::Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderPropertyBase>& val)
{
    RSRenderPropertyType type = val->GetPropertyType();
    if (!(parcel.WriteInt16(static_cast<int16_t>(type)))) {
        return false;
    }
    switch (type) {
        case RSRenderPropertyType::PROPERTY_FLOAT: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_MATRIX3F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Matrix3f>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_QUATERNION: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Quaternion>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_FILTER: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_VECTOR2F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4<Color>>>(val);
            if (property == nullptr) {
                return false;
            }
            return parcel.WriteUint64(property->GetId()) && RSMarshallingHelper::Marshalling(parcel, property->Get());
        }
        default: {
            return false;
        }
    }
    return true;
}

bool RSRenderPropertyBase::Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderPropertyBase>& val)
{
    int16_t typeId = 0;
    if (!parcel.ReadInt16(typeId)) {
        return false;
    }
    RSRenderPropertyType type = static_cast<RSRenderPropertyType>(typeId);
    PropertyId id = 0;
    if (!parcel.ReadUint64(id)) {
        return false;
    }
    switch (type) {
        case RSRenderPropertyType::PROPERTY_FLOAT: {
            float value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<float>(value, id, type));
            break;
        }
        case RSRenderPropertyType::PROPERTY_COLOR: {
            Color value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Color>(value, id, type));
            break;
        }
        case RSRenderPropertyType::PROPERTY_MATRIX3F: {
            Matrix3f value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Matrix3f>(value, id, type));
            break;
        }
        case RSRenderPropertyType::PROPERTY_QUATERNION: {
            Quaternion value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Quaternion>(value, id, type));
            break;
        }
        case RSRenderPropertyType::PROPERTY_FILTER: {
            std::shared_ptr<RSFilter> value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>(value, id, type));
            break;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR2F: {
            Vector2f value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Vector2f>(value, id, type));
            break;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4F: {
            Vector4f value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Vector4f>(value, id, type));
            break;
        }
        case RSRenderPropertyType::PROPERTY_VECTOR4_COLOR: {
            Vector4<Color> value;
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                return false;
            }
            val.reset(new RSRenderAnimatableProperty<Vector4<Color>>(value, id, type));
            break;
        }
        default: {
            return false;
        }
    }
    return val != nullptr;
}
#endif

template<>
void RSRenderProperty<bool>::SendIfNeed(const bool& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyBool>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<float>::SendIfNeed(const float& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyFloat>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<int>::SendIfNeed(const int& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyInt>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<Color>::SendIfNeed(const Color& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyColor>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<Gravity>::SendIfNeed(const Gravity& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyGravity>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<Matrix3f>::SendIfNeed(const Matrix3f& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyMatrix3f>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<Quaternion>::SendIfNeed(const Quaternion& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyQuaternion>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<std::shared_ptr<RSFilter>>::SendIfNeed(const std::shared_ptr<RSFilter>& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyFilter>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<std::shared_ptr<RSImage>>::SendIfNeed(const std::shared_ptr<RSImage>& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyImage>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<std::shared_ptr<RSMask>>::SendIfNeed(const std::shared_ptr<RSMask>& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyMask>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<std::shared_ptr<RSPath>>::SendIfNeed(const std::shared_ptr<RSPath>& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyPath>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<std::shared_ptr<RSShader>>::SendIfNeed(const std::shared_ptr<RSShader>& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyShader>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<Vector2f>::SendIfNeed(const Vector2f& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyVector2f>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<Vector4<uint32_t>>::SendIfNeed(const Vector4<uint32_t>& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyBorderStyle>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<Vector4<Color>>::SendIfNeed(const Vector4<Color>& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyVector4Color>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

template<>
void RSRenderProperty<Vector4f>::SendIfNeed(const Vector4f& value)
{
    if (node_.lock()) {
        auto node = (node_.lock())->ReinterpretCastTo<RSRenderNode>();
        std::unique_ptr<RSCommand> cmd =
            std::make_unique<RSUpdatePropertyVector4f>(node->GetId(), value, id_, false);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(cmd, true, FollowType::FOLLOW_TO_PARENT, node->GetId());
        }
    }
}

// Attention: SurfaceNode created by RosenRenderTexture/RosenRenderWeb/RosenRenderXComponent,
// has not supported DrawCmdList yet.
template<>
void RSRenderProperty<std::shared_ptr<DrawCmdList>>::SendIfNeed(
    const std::shared_ptr<DrawCmdList>& value)
{
    return;
}

template<>
float RSRenderAnimatableProperty<float>::toFloat() const
{
    return std::fabs(RSRenderProperty<float>::stagingValue_);
}

template<>
float RSRenderAnimatableProperty<Vector4f>::toFloat() const
{
    return RSRenderProperty<Vector4f>::stagingValue_.GetLength();
}

template<>
float RSRenderAnimatableProperty<Quaternion>::toFloat() const
{
    return RSRenderProperty<Quaternion>::stagingValue_.GetLength();
}

template<>
float RSRenderAnimatableProperty<Vector2f>::toFloat() const
{
    return RSRenderProperty<Vector2f>::stagingValue_.GetLength();
}

std::shared_ptr<RSRenderPropertyBase> operator+(const std::shared_ptr<RSRenderPropertyBase>& a,
    const std::shared_ptr<RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Add(b);
}

std::shared_ptr<RSRenderPropertyBase> operator-(const std::shared_ptr<RSRenderPropertyBase>& a,
    const std::shared_ptr<RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->Minus(b);
}

std::shared_ptr<RSRenderPropertyBase> operator*(const std::shared_ptr<RSRenderPropertyBase>& value,
    const float scale)
{
    if (value == nullptr) {
        return {};
    }

    return value->Multiply(scale);
}

bool operator==(const std::shared_ptr<RSRenderPropertyBase>& a, const std::shared_ptr<RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return a->IsEqual(b);
}

bool operator!=(const std::shared_ptr<RSRenderPropertyBase>& a, const std::shared_ptr<RSRenderPropertyBase>& b)
{
    if (a == nullptr) {
        return {};
    }

    return !a->IsEqual(b);
}

} // namespace Rosen
} // namespace OHOS
