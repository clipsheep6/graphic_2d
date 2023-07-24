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
#include "animation/rs_render_particle_emitter.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_property.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
RSParticleAnimation::RSParticleAnimation(
    std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& endValue)
    : RSPropertyAnimation(property)
{
    endValue_ = endValue;
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

// emit --> particleRenderProperty -->RSProperties::setParticle(particle2);
void RSParticleAnimation::Emit()
{
    //particlesParams 是particleParams的数组
    auto particlesParams = property_.Get();
    auto id = property_.GetId();
    auto emitter = RSRenderParticleEmitter::RSRenderParticleEmitter(particlesParams);
    auto particles = emitter->getParticles();
    //particles 是 RSRenderParticle的数组
    auto particleRenderProperty = std::make_shared<RSRenderProperty<std::vector<RSRenderParticle>>>(particles, id);
    auto animation =
        std::make_shared<RSRenderParticleAnimation>(GetId(), GetPropertyId(), originValue_->GetRenderProperty());
	animation->AttachRenderProperty(particleRenderProperty);
}

void RSParticleAnimation::OnStart()
{
    RSPropertyAnimation::OnStart();
    auto animation =
        std::make_shared<RSRenderParticleAnimation>(GetId(), GetPropertyId(), originValue_->GetRenderProperty(), originValue_);
    //animation->SetAdditive(GetAdditive());
    // UpdateParamToRenderAnimation(animation);
    StartRenderAnimation(animation);
}
} // namespace Rosen
} // namespace OHOS
