//
#include "common/rs_color.h"
#include "common/rs_particle.h"
namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderParticle {
public:
    // RSRenderParticle(const std::shared_ptr<RSRenderPropertyBase>& startValue);
    // virtual ~RSRenderParticle() = default;
    // void InitProperty(const std::shared_ptr<RSRenderPropertyBase>& ParticleProperty);

   RSRenderParticle(ParticleParams particleParams);

    // Set methods
    void setPosition(Vector2f position);
    void setVelocity(Vector2f velocity);
    void setAcceleration(Vector2f acceleration);
    void setSpin(float spin);
    void setOpacity(float opacity);
    void setColor(Color color);
    void setScale(float scale);
    void setRadius(float radius);
    void setRSImage(RSImage rsImage);
    void setParticleSize(Size particleSize);
    void setParticleType(ParticleType particleType);
    void setActiveTime(int activeTime);

    // Get methods
    Vector2f getPosition();
    Vector2f getVelocity();
    Vector2f getAcceleration();
    float getSpin();
    float getOpacity();
    Color getColor();
    float getScale();
    float getRadius();
    RSImage getRSImage();
    Size getParticleSize();
    ParticleType getParticleType();
    int getActiveTime();


    // Other methods
    void initProperty(ParticleParams particleParams);
    void updateProperty(ParticleParams particleParams, int64_t deltaTime);
    void updateAccelerate(ParticleParams particleParams, int64_t deltaTime);
    void updateColor(ParticleParams particleParams, int64_t deltaTime);
    void updateOpacity(ParticleParams particleParams, int64_t deltaTime);
    void updateScale(ParticleParams particleParams, int64_t deltaTime);
    void updateSpin(ParticleParams particleParams, int64_t deltaTime);
    bool isAlive() const;
    float getRandomValue(float min, float max);
    Vector2f calculateParticlePosition(ShapeType emitShape, Drawing::Point position, Drawing::Size emitSize);
    Color lerp(const Color& start, const Color& end, float t);

private:
    Vector2f position_;
    Vector2f velocity_;
    Vector2f acceleration_;
    float scale_;
    float spin_;
    float opacity_;
    Color color_;
    float radius_;
    RSImage rsImage_;
    Drawing::Size particleSize_;
    ParticleType particleType_;
    int64_t activeTime_;
    int64_t lifeTime_;
};
} // namespace Rosen
} // namespace OHOS