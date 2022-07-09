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

#include "animation/rs_property_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_render_path_animation.h"
#include "platform/common/rs_log.h"
#include "command/rs_animation_command.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
template<typename T>
template<typename P>
void RSPropertyAnimation<T>::OnAnimationRemoveInner()
{
    auto target = RSPropertyAnimation<T>::GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to remove animation, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<P>(target->GetId(), GetId(), GetPropertyValue());
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
        if (target->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote =
                std::make_unique<P>(target->GetId(), GetId(), GetPropertyValue());
            transactionProxy->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
        }
    }
}

template<>
void RSPropertyAnimation<float>::OnAnimationRemove()
{
    OnAnimationRemoveInner<RSOnAnimationRemoveFloat>();
}

template<>
void RSPropertyAnimation<Color>::OnAnimationRemove()
{
    OnAnimationRemoveInner<RSOnAnimationRemoveColor>();
}

template<>
void RSPropertyAnimation<Matrix3f>::OnAnimationRemove()
{
    OnAnimationRemoveInner<RSOnAnimationRemoveMatrix3f>();
}

template<>
void RSPropertyAnimation<Vector2f>::OnAnimationRemove()
{
    OnAnimationRemoveInner<RSOnAnimationRemoveVector2f>();
}

template<>
void RSPropertyAnimation<Vector4f>::OnAnimationRemove()
{
    OnAnimationRemoveInner<RSOnAnimationRemoveVector4f>();
}

template<>
void RSPropertyAnimation<Quaternion>::OnAnimationRemove()
{
    OnAnimationRemoveInner<RSOnAnimationRemoveQuaternion>();
}

template<>
void RSPropertyAnimation<std::shared_ptr<RSFilter>>::OnAnimationRemove()
{
    OnAnimationRemoveInner<RSOnAnimationRemoveFilter>();
}

template<>
void RSPropertyAnimation<Vector4<Color>>::OnAnimationRemove()
{
    OnAnimationRemoveInner<RSOnAnimationRemoveVec4Color>();
}
}
}