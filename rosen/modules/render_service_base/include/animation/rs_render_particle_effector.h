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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EFFECTOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EFFECTOR_H

#include "rs_render_particle.h"
namespace OHOS {
namespace Rosen {

class RSRenderParticleEffector {
public:
    RSRenderParticleEffector(const std::shared_ptr<ParticleRenderParams> particleParams);

    void UpdateProperty(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime);
    void UpdateAccelerate(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime);
    void UpdateAccelerationAngle(
        const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime);
    void UpdateAccelerationValue(
        const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime);
    void UpdateColor(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime);
    Color UpdateColorRandom(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, Color color);
    int16_t CalculateRedInt(
        const std::shared_ptr<RSRenderParticle>& particle, int16_t redInt, float redF, float redSpeed, float deltaTime);
    int16_t CalculateGreenInt(const std::shared_ptr<RSRenderParticle>& particle, int16_t greenInt, float greenF,
        float greenSpeed, float deltaTime);
    int16_t CalculateBlueInt(const std::shared_ptr<RSRenderParticle>& particle, int16_t blueInt, float blueF,
        float blueSpeed, float deltaTime);
    int16_t CalculateAlphaInt(const std::shared_ptr<RSRenderParticle>& particle, int16_t alphaInt, float alphaF,
        float alphaSpeed, float deltaTime);
    Color UpdateColorCurve(int64_t activeTime, Color color);
    void UpdateOpacity(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime);
    void UpdateScale(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime);
    void UpdateSpin(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime);
    // Apply effector to particle
    void ApplyEffectorToParticle(const std::shared_ptr<RSRenderParticle>& particle, int64_t deltaTime);

    template<typename T>
    T GenerateValue(T startValue, T endValue, int startTime, int endTime, int currentTime,
        std::shared_ptr<RSInterpolator> interpolator);
    float GenerateValue(float startValue, float endValue, int startTime, int endTime, int currentTime);

private:
    std::shared_ptr<ParticleRenderParams> particleParams_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EFFECTOR_H
