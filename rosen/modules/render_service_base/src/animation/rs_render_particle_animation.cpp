/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_profiler.h"

#include "animation/rs_value_estimator.h"
#include "command/rs_animation_command.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSRenderParticleAnimation::RSRenderParticleAnimation(AnimationId id, const PropertyId& propertyId,
    const std::vector<std::shared_ptr<ParticleRenderParams>>& particlesRenderParams)
    : RSRenderPropertyAnimation(id, propertyId), particlesRenderParams_(particlesRenderParams),
      particleSystem_(std::make_shared<RSRenderParticleSystem>(particlesRenderParams_))
{}

void RSRenderParticleAnimation::DumpAnimationType(std::string& out) const
{
    out += "Type:RSRenderParticleAnimation";
}

bool RSRenderParticleAnimation::Animate(int64_t time)
{
    RS_OPTIONAL_TRACE_NAME("RSRenderParticleAnimation::Animate");
    auto target = GetTarget();
    if (!target) {
        return true;
    } else if (!target->IsOnTheTree() || !target->GetRenderProperties().GetVisible()) {
        target->RemoveModifier(property_->GetId());
        return true;
    }

    auto emitterUpdater = target->GetRenderProperties().GetEmitterUpdater();
    if (emitterUpdater) {
        UpdateEmitter(emitterUpdater);
    }

    int64_t deltaTime = time - animationFraction_.GetLastFrameTime();
    animationFraction_.SetLastFrameTime(time);
    if (particleSystem_ != nullptr) {
        auto particleNoiseField = target->GetRenderProperties().GetParticleNoiseField();
        if (particleNoiseField) {
            UpdateNoiseField(particleNoiseField);
        }
        particleSystem_->Emit(deltaTime, renderParticleVector_.renderParticleVector_);
        particleSystem_->UpdateParticle(deltaTime, renderParticleVector_.renderParticleVector_);
    }
    auto property = std::static_pointer_cast<RSRenderProperty<RSRenderParticleVector>>(property_);
    if (property) {
        property->Set(renderParticleVector_);
    }

    if (particleSystem_ == nullptr || particleSystem_->IsFinish(renderParticleVector_.renderParticleVector_)) {
        if (target) {
            target->RemoveModifier(property_->GetId());
        }
        return true;
    }
    return false;
}

void RSRenderParticleAnimation::UpdateEmitter(const std::shared_ptr<EmitterUpdater>& emitterUpdater)
{
    uint32_t index = emitterUpdater->emitterIndex_;
    if (index > particlesRenderParams_.size()) {
        return;
    }
    const auto& emitterConfig = particlesRenderParams_[index]->emitterConfig_;
    if (!ROSEN_EQ(emitterConfig.position_.x_, emitterUpdater->position_.x_) ||
        !ROSEN_EQ(emitterConfig.position_.y_, emitterUpdater->position_.y_) ||
        !ROSEN_EQ(emitterConfig.emitSize_.x_, emitterUpdater->emitSize_.x_) ||
        !ROSEN_EQ(emitterConfig.emitSize_.y_, emitterUpdater->emitSize_.y_) ||
        !ROSEN_EQ(emitterConfig.emitRate_, emitterUpdater->emitRate_)) {
        particlesRenderParams_[index]->emitterConfig_.position_ = emitterUpdater->position_;
        particlesRenderParams_[index]->emitterConfig_.emitSize_ = emitterUpdater->emitSize_;
        particlesRenderParams_[index]->emitterConfig_.emitRate_ = emitterUpdater->emitRate_;
        if (particleSystem_) {
            particleSystem_->UpdateEmitter(
                index, emitterUpdater->position_, emitterUpdater->emitSize_, emitterUpdater->emitRate_);
        } else {
            particleSystem_ = std::make_shared<RSRenderParticleSystem>(particlesRenderParams_);
        }
    }
}

void RSRenderParticleAnimation::UpdateNoiseField(const std::shared_ptr<ParticleNoiseField>& particleNoiseField)
{
    if (particleNoiseField == nullptr) {
        return;
    } else if (particleNoiseField_ != nullptr && *particleNoiseField_ == *particleNoiseField) {
        return;
    }
    particleNoiseField_ = particleNoiseField;
    particleSystem_->UpdateNoiseField(particleNoiseField);
}

void RSRenderParticleAnimation::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderParticleAnimation::OnAttach, target is nullptr");
        return;
    }
    auto particleAnimations = target->GetAnimationManager().GetParticleAnimations();
    if (!particleAnimations.empty()) {
        for (const auto& pair : particleAnimations) {
            target->RemoveModifier(pair.first);
            target->GetAnimationManager().RemoveAnimation(pair.second);
            target->GetAnimationManager().UnregisterParticleAnimation(pair.first, pair.second);
        }
    }
    target->GetAnimationManager().RegisterParticleAnimation(GetPropertyId(), GetAnimationId());
}

void RSRenderParticleAnimation::OnDetach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderParticleAnimation::OnDetach, target is nullptr");
        return;
    }
    if (particleSystem_ != nullptr) {
        particleSystem_->ClearEmitter();
        particleSystem_.reset();
    }
    auto propertyId = GetPropertyId();
    auto id = GetAnimationId();
    target->GetAnimationManager().UnregisterParticleAnimation(propertyId, id);
}

bool RSRenderParticleAnimation::Marshalling(Parcel& parcel) const
{
    auto id = GetAnimationId();
    if (!(parcel.WriteUint64(id))) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write id failed");
        return false;
    }
    if (!parcel.WriteUint64(propertyId_)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write PropertyId failed");
        return false;
    }
    if (!RSMarshallingHelper::Marshalling(parcel, particlesRenderParams_)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write particlesRenderParams failed");
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
    AnimationId id = 0;
    if (!parcel.ReadUint64(id)) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling animationId failed");
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, id);
    SetAnimationId(id);
    if (!(parcel.ReadUint64(propertyId_) && RSMarshallingHelper::Unmarshalling(parcel, particlesRenderParams_))) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling failed");
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, propertyId_);
    particleSystem_ = std::make_shared<RSRenderParticleSystem>(particlesRenderParams_);
    return true;
}

} // namespace Rosen
} // namespace OHOS
