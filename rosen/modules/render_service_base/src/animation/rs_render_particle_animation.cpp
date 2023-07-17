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

#include "animation/rs_render_particle_animation.h"

#include <memory>

#include "animation/rs_render_particle_system.h"
#include "animation/rs_render_particle_emitter.h"
#include "animation/rs_value_estimator.h"
#include "command/rs_animation_command.h"
#include "common/rs_particle.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSRenderParticleAnimation::RSRenderParticleAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originRenderValue, const std::shared_ptr<RSPropertyBase>& originValue)
    : RSRenderPropertyAnimation(id, propertyId, originRenderValue)
{ 
    particlesParams_ = originValue.Get();
   
}


void RSRenderParticleAnimation::AttachRenderProperty(const std::shared_ptr<RSRenderPropertyBase>& renderProperty)
{
    auto particleRenderProperty = std::make_shared<RSRenderProperty<std::vector<RSRenderParticle>>>(property);
    // auto particleRenderProperty = std::make_shared<RSRenderParticle>(property);
    auto particleRenderModifier = std::make_shared<RSParticleRenderModifier>(renderProperty);
    auto target = GetTarget();
    if (target) {
        target->AddModifier(particleRenderModifier);
    }
    // 动画结束的时候将modifier清掉
    // node->RemoveModifier(PROPERTYid);
}

bool RSRenderParticleAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_) && interpolator_ != nullptr &&
            interpolator_->Marshalling(parcel))) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }
    return true;
}

RSRenderParticleAnimation* RSRenderParticleAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderParticleAnimation* renderParticleAnimation = new RSRenderParticleAnimation();
    if (!renderParticleAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Unmarshalling, failed");
        delete renderParticleAnimation;
        return nullptr;
    }
    return renderParticleAnimation;
}

bool RSRenderParticleAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling Fail");
        return false;
    }

    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    SetInterpolator(interpolator);
    return true;
}

bool RSRenderParticleAnimation::Animate(int64_t time)
{
    // time 就是当前的时间，GetLastFrameTime获取上一帧的时间，
    int64_t deltaTime = time - animationFraction_.GetLastFrameTime();
    auto particleSystem = RSRenderParticleSystem(particlesParams_);
    particleSystem->simulation(deltaTime);
    auto emitter = RSRenderParticleEmitter(particlesParams_);
    emitter->Emit(deltaTime);
    //update(time);
}

// void RSRenderParticleAnimation::OnAnimate(float fraction)
// {
//     //实现粒子动画仿真
//     //Particle simulation calculations
//     auto particle = std::static_pointer_cast<RSRenderAnimatableProperty<Particle>>(GetOriginValue());
//     if (GetOriginValue()->GetPropertyType() == RSRenderPropertyType::PROPERTY_PARTICLE) {




//         auto particleSystem = RSRenderParticleSystem(startValue_);
//         update(fraction);
//         SetAnimationValue(endValue_);
//         return;
//     }
// }




RSRenderParticleAnimation::Emit()
{
    auto emitter = RSRenderParticleEmitter::RSRenderParticleEmitter(particlesParams_);
    auto particles = emitter->getParticles();
    //particles 是 RSRenderParticle的数组
    auto particleRenderProperty = std::make_shared<RSRenderProperty<std::vector<RSRenderParticle>>>(particles, GetPropertyId());
    auto particleRenderModifier = std::make_shared<RSParticleRenderModifier>(particleRenderProperty);
    auto target = GetTarget();
    if (target) {
        target->AddModifier(particleRenderModifier);
    }
    //auto animation =
    //    std::make_shared<RSRenderParticleAnimation>(GetId(), GetPropertyId(), originValue_->GetRenderProperty());
	//animation->AttachRenderProperty(particleRenderProperty);
}


void RSRenderParticleAnimation::update(int64_t deltaTime)
{
    // 每帧更新数据
    // 设置到properties
    // drawParticle拿到properties2 画出来
    SetAnimationValue(XXX);
}

}

} // namespace Rosen
} // namespace OHOS
