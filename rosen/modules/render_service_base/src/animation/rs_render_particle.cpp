//
#include "animation/rs_render_particle.h"

#include "animation/rs_interpolator.h"
#include "animation/rs_render_particle_system.h"
#include "common/rs_color.h"
#include "common/rs_particle.h"
namespace OHOS {
namespace Rosen {

RSRenderParticle::RSRenderParticle(ParticleParams particleParams)
{
    // Initialize member variables
    position_ = { 0.0f, 0.0f };
    velocity_ = { 0.0f, 0.0f };
    acceleration_ = { 0.0f, 0.0f };
    spin_ = 0.0f;
    opacity_ = 1.0f;
    color_ = { 255, 255, 255 }; // Default white color
    radius_ = 1.0f;
    rsImage_ = {}; // Default empty image
    particleSize_ = { 1.0f, 1.0f };
    particleType_ = POINTS;
    activeTime_ = 0;

    initProperty(particleParams);
}

// Set methods
void RSRenderParticle::setPosition(Vector2f position)
{
    position_ = position;
}

void RSRenderParticle::setVelocity(Vector2f velocity)
{
    velocity_ = velocity;
}

void RSRenderParticle::setAcceleration(Vector2f acceleration)
{
    acceleration_ = acceleration;
}

void RSRenderParticle::setSpin(float spin)
{
    spin_ = spin;
}

void RSRenderParticle::setOpacity(float opacity)
{
    opacity_ = opacity;
}

void RSRenderParticle::setColor(Color color)
{
    color_ = color;
}

void RSRenderParticle::setScale(float scale)
{
    scale_ = scale;
}

void RSRenderParticle::setRadius(float radius)
{
    radius_ = radius;
}

void RSRenderParticle::setRSImage(RSImage rsImage)
{
    rsImage_ = rsImage;
}

void RSRenderParticle::setParticleSize(Size particleSize)
{
    particleSize_ = particleSize;
}

void RSRenderParticle::setParticleType(ParticleType particleType)
{
    particleType_ = particleType;
}

void RSRenderParticle::setActiveTime(int activeTime)
{
    activeTime_ = activeTime;
}

// Get methods
Vector2f RSRenderParticle::getPosition()
{
    return position_;
}

Vector2f RSRenderParticle::getVelocity()
{
    return velocity_;
}

Vector2f RSRenderParticle::getAcceleration()
{
    return acceleration_;
}

float RSRenderParticle::getSpin()
{
    return spin_;
}

float RSRenderParticle::getOpacity()
{
    return opacity_;
}

Color RSRenderParticle::getColor()
{
    return color_;
}

float RSRenderParticle::getScale()
{
    return scale_;
}

float RSRenderParticle::getRadius()
{
    return radius_;
}

RSImage RSRenderParticle::getRSImage()
{
    return rsImage_;
}

Size RSRenderParticle::getParticleSize()
{
    return particleSize_;
}

ParticleType RSRenderParticle::getParticleType()
{
    return particleType_;
}

int RSRenderParticle::getActiveTime()
{
    return activeTime_;
}

// Other methods
void RSRenderParticle::initProperty(ParticleParams particleParams)
{
    // Initialize particle properties here
    // You can set default values for different properties
    // 确定粒子属性取随机值之后的值
    position_ = calculateParticlePosition(particleParams.emitterConfig.emitShape, particleParams.emitterConfig.position,
        particleParams.emitterConfig.emitSize);

    float velocityValue =
        getRandomValue(particleParams.velocity.velocityValue.start_, particleParams.velocity.velocityValue.end_);
    float velocityAngle =
        getRandomValue(particleParams.velocity.velocityAngle.start_, particleParams.velocity.velocityAngle.end_);
    velocity_ = Vector2f { velocityValue * cos(velocityAngle), velocityValue * sin(velocityAngle) };

    float accelerationValue = getRandomValue(particleParams.acceleration.accelerationValue.val_.start_,
        particleParams.acceleration.accelerationValue.val_.end_);
    float accelerationAngle = getRandomValue(particleParams.acceleration.accelerationAngle.val_.start_,
        particleParams.acceleration.accelerationAngle.val_.end_);
    acceleration_ = Vector2f { accelerationValue * cos(accelerationAngle), accelerationValue * sin(accelerationAngle) };

    spin_ = getRandomValue(particleParams.spin.val_.start_, particleParams.spin.val_.end_);
    opacity_ = getRandomValue(particleParams.opacity.val_.start_, particleParams.opacity.val_.end_);
    scale_ = getRandomValue(particleParams.scale.val_.start_, particleParams.scale.val_.end_);

    particleType_ = particleParams.emitterConfig.type;
    if (particleType_ == POINTS) {
        float colorRandomValue = getRandomValue(0.0f, 1.0f); // 生成一个0.0 ~ 1.0之间的随机值
        color_ = lerp(particleParams.color.colorVal_.start_, particleParams.color.colorVal_.end_, colorRandomValue);
        radius_ = particleParams.emitterConfig.radius * scale_;
    } else if (particleType_ == IMAGES) {
        rsImage_ = particleParams.emitterConfig.image;
        rsImage_.SetImageFit(particleParams.emitterConfig.imageFit);
        rsImage_.SetDstRect(RectF { 0.f, 0.f, particleParams.emitterConfig.size.width_ * scale_,
            particleParams.emitterConfig.size.height_ * scale_ });
    }
    activeTime_ = 0;
    lifeTime_ = particleParams.emitterConfig.lifeTime;
}

bool RSRenderParticle::isAlive() const
{
    return activeTime_ < lifeTime_;
}

float getRandomValue(float min, float max)
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

Vector2f calculateParticlePosition(ShapeType emitShape, Drawing::Point position, Drawing::Size emitSize)
{
    if (emitShape == RECT) {
        float minX = position.x_;
        float maxX = position.x_ + emitSize.Width();
        float positionX = getRandomValue(minX, maxX);
        float minY = position.y_;
        float maxY = position.y_ + emitSize.Height();
        float positionY = getRandomValue(minY, maxY);
        return Vector2f { positionX, positionY };
    }
    if (emitShape == CIRCLE || emitShape == ELLIPSE) {
        float rx = getRandomValue(0.f, emitSize.Width());
        float ry = getRandomValue(0.f, emitSize.Height());
        float theta = getRandomValue(0.f, 2 * PI);
        float positionX = position.x_ + rx * cos(theta);
        float positionY = position.y_ + ry * sin(theta);
        return Vector2f { positionX, positionY };
    }
}

Color lerp(const Color& start, const Color& end, float t)
{
    Color result;
    result.SetRed(start.GetRed() + static_cast<int>(std::round((end.GetRed() - start.GetRed()) * t)));
    result.SetGreen(start.GetGreen() + static_cast<int>(std::round((end.GetGreen() - start.GetGreen()) * t)));
    result.SetBlue(start.GetBlue() + static_cast<int>(std::round((end.GetBlue() - start.GetBlue()) * t)));
    result.SetAlpha(start.GetAlpha() + static_cast<int>(std::round((end.GetAlpha() - start.GetAlpha()) * t)));
    return result;
}

} // namespace Rosen
} // namespace OHOS