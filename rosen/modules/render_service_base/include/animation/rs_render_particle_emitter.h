#include "rs_render_particle.h"
#include "rs_render_particle_effector.h"
#include "common/rs_particle.h"
namespace OHOS {
namespace Rosen {

class RSRenderParticleEmitter {
public:
    RSRenderParticleEmitter(ParticleParams particleParams_);

    void EmitParticle(int64_t deltaTime);
    void UpdateParticle(int64_t deltaTime);
    // Setters
    void setLiveTime(int liveTime);
    void setEmitRate(int emitRate);
    void setEmitShape(ShapeType emitShape);
    void setPosition(const Vector2f& position);
    void setEmitSize(const Size& emitSize);
    void setParticleType(ParticleType type);
    void setRadius(float radius);
    void setImage(const RSImage& image);
    void setSize(const Size& size);
    void setImageFit(ImageFit imageFit);

    // Getters
    int getMaxParticle() const;
    int getActiveParticle() const;
    static const std::vector<RSRenderParticle>& getParticles() const;
    const std::vector<RSRenderParticle*>& getActiveParticles() const;
    int getLiveTime() const;
    bool isEmitting() const;
    int getEmitRate() const;
    ShapeType getEmitShape() const;
    const Vector2f& getPosition() const;
    const Size& getEmitSize() const;
    ParticleType getParticleType() const;
    float getRadius() const;
    const RSImage& getImage() const;
    const Size& getSize() const;
    ImageFit getImageFit() const;

private:
    int maxParticle_;
    int activeParticle_;
    std::vector<RSRenderParticle> particles_;
    //std::vector<RSRenderParticle*> activeParticles_;
    int liveTime_;
    bool isEmitting_;
    int emitRate_;
    ShapeType emitShape_;   //发射区域的形状
    Vector2f position_;     //发射区域的位置
    Size emitSize_;         //发射区域大小
    ParticleType type_;
    float radius_;
    RSImage image_;
    Size size_;
    ImageFit imageFit_;
    ParticleParams particleParams_;
    int64_t m_accumulatedTime;
};

} // namespace Rosen
} // namespace OHOS