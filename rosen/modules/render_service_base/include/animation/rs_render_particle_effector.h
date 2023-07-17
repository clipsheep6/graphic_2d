//
#include "rs_render_particle.h"
#include "common/rs_particle.h"
namespace OHOS {
namespace Rosen {

template<typename T>
struct ChangeInOverLife {
    float lifeRatio;
    T changeValue;
};

class RSRenderParticleEffector {
public:
    RSRenderParticleEffector(ParticleParams particleParams);

    void updateProperty(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime);
    void updateAccelerate(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime);
    void updateColor(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime);
    void updateOpacity(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime);
    void updateScale(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime);
    void updateSpin(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime);
    // Apply effector to particle
    void applyEffectorToParticle(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime) const;
    float getRandomValue(float min, float max);

private:
    ParticleParams particleParams_;
};

} // namespace Rosen
} // namespace OHOS