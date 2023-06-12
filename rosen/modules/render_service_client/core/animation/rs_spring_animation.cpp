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

#include "animation/rs_spring_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_render_spring_animation.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_property.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
RSSpringAnimation::RSSpringAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPropertyBase>& byValue) : RSPropertyAnimation(property)
{
    isDelta_ = true;
    byValue_ = byValue;
}

RSSpringAnimation::RSSpringAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue)
    : RSPropertyAnimation(property)
{
    isDelta_ = false;
    startValue_ = startValue;
    endValue_ = endValue;
}

void RSSpringAnimation::SetTimingCurve(const RSAnimationTimingCurve& timingCurve)
{
    if (timingCurve.type_ != RSAnimationTimingCurve::CurveType::SPRING) {
        ROSEN_LOGE("RSSpringAnimation::SetTimingCurve: invalid timing curve type");
        return;
    }
    timingCurve_ = timingCurve;
}

const RSAnimationTimingCurve& RSSpringAnimation::GetTimingCurve() const
{
    return timingCurve_;
}

void RSSpringAnimation::SetZeroThreshold(const float zeroThreshold)
{
    zeroThreshold_ = zeroThreshold;
}

void RSSpringAnimation::OnStart()
{
    RSPropertyAnimation::OnStart();
    auto animation = std::make_shared<RSRenderSpringAnimation>(GetId(), GetPropertyId(),
        originValue_->GetRenderProperty(), startValue_->GetRenderProperty(), endValue_->GetRenderProperty());
    // 300: placeholder for estimated duration, will be replaced by real duration on animation start.
    SetDuration(300);
    UpdateParamToRenderAnimation(animation);
    animation->SetSpringParameters(timingCurve_.response_, timingCurve_.dampingRatio_, timingCurve_.blendDuration_);
    animation->SetZeroThreshold(zeroThreshold_);
    animation->SetAdditive(GetAdditive());
    if (isCustom_) {
        animation->AttachRenderProperty(property_->GetRenderProperty());
        StartUIAnimation(animation);
    } else {
        StartRenderAnimation(animation);
    }
}

void RSSpringAnimation::StartRenderAnimation(const std::shared_ptr<RSRenderSpringAnimation>& animation)
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start spring animation, target is null!");
        return;
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreateSpring>(target->GetId(), animation);
    transactionProxy->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    if (target->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> commandForRemote =
            std::make_unique<RSAnimationCreateSpring>(target->GetId(), animation);
        transactionProxy->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
    }
}

void RSSpringAnimation::StartUIAnimation(const std::shared_ptr<RSRenderSpringAnimation>& animation)
{
    StartCustomAnimation(animation);
}
} // namespace Rosen
} // namespace OHOS
