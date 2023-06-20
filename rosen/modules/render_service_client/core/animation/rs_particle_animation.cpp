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

#include "animation/rs_particle_animation.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_render_particle_animation.h"
#include "command/rs_animation_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {
RSParticleAnimation::RSParticleAnimation(std::shared_ptr<RSPropertyBase> property) : RSPropertyAnimation(property)
{
    isDelta_ = true;
}

RSParticleAnimation::RSParticleAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue)
    : RSPropertyAnimation(property)
{
    isDelta_ = false;
    startValue_ = startValue;
    endValue_ = endValue;
}

void RSParticleAnimation::SetTimingParticle(const RSAnimationTimingParticle& timingParticle)
{
    if (timingParticle.type_ != RSAnimationTimingParticle::ParticleType::INTERPOLATING) {
        ROSEN_LOGE("RSParticleAnimation::SetTimingParticle: invalid timing particle type");
        return;
    }
    timingParticle_ = timingParticle;
}

const RSAnimationTimingParticle& RSParticleAnimation::GetTimingParticle() const
{
    return timingParticle_;
}

void RSParticleAnimation::StartRenderAnimation(const std::shared_ptr<RSRenderParticleAnimation>& animation)
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start particle animation, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreateParticle>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    }
}

void RSParticleAnimation::StartUIAnimation(const std::shared_ptr<RSRenderParticleAnimation>& animation)
{
    StartCustomAnimation(animation);
}

void RSParticleAnimation::OnStart()
{
    RSPropertyAnimation::OnStart();
    auto interpolator = timingParticle_.GetInterpolator(GetDuration());
    auto animation = std::make_shared<RSRenderParticleAnimation>(GetId(), GetPropertyId(),
        originValue_->GetRenderProperty(), startValue_->GetRenderProperty(), endValue_->GetRenderProperty());
    animation->SetInterpolator(interpolator);
    animation->SetAdditive(GetAdditive());
    UpdateParamToRenderAnimation(animation);
    if (isCustom_) {
        animation->AttachRenderProperty(property_->GetRenderProperty());
        StartUIAnimation(animation);
    } else {
        StartRenderAnimation(animation);
    }
}
} // namespace Rosen
} // namespace OHOS
