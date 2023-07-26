
#include "animation/rs_render_particle_system.h"
namespace OHOS {
namespace Rosen {
RSRenderParticleSystem::RSRenderParticleSystem(std::vector<ParticleParams> particlesParams)
    : particlesParams_(particlesParams)
{}

//有几个发射器就发射几次
void RSRenderParticleSystem::Emit(int64_t deltaTime) {
    for (auto iter = 0; iter < particlesParams_.size(); iter++) { 
        auto particleParams = particlesParams_[iter];
        //创建发射器
        auto emitter = RSRenderParticleEmitter(particleParams);
        emitter->EmitParticle(deltaTime);
        // emitter->updateParticle(deltaTime);
    }
}
void RSRenderParticleSystem::Simulation(int64_t deltaTime) {
    Emit(deltaTime);
}
} // namespace Rosen
} // namespace OHOS