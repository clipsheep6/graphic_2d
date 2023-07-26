//
#include "animation/rs_render_particle_effector.h"

#include <cmath>
#include <iostream>
#include <random>
#include "common/rs_particle.h"
namespace OHOS {
namespace Rosen {
RSRenderParticleEffector::RSRenderParticleEffector(ParticleParams> particleParams)
{
    particleParams_ = particleParams;
}

void RSRenderParticleEffector::updateColor(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime)
{
    auto colorUpdator = particleParams_.color.updator_;
    if (colorUpdator == RANDOM) {
    float redSpeed = getRandomValue(particleParams_.color.redRandom_.start_, particleParams_.color.redRandom_.end_);
    float greenSpeed = getRandomValue(particleParams_.color.greenRandom_.start_, particleParams_.color.greenRandom_.end_);
    float blueSpeed = getRandomValue(particleParams_.color.blueRandom_.start_, particleParams_.color.blueRandom_.end_);
    Color color = particle.getColor();
    color.SetRed(color.GetRed() + redSpeed * deltaTime);
    color.SetGreen(color.GetGreen() + greenSpeed * deltaTime);
    color.SetBlue(color.GetBlue() + blueSpeed * deltaTime);
    particle.setColor(color);
    } else if (colorUpdator == CURVE) {
        auto valChangeOverLife = particleParams_.color.valChangeOverLife_;
        for (int i = 0; i < valChangeOverLife.size(); i++) {
            auto colorCurve = valChangeOverLife[i].curve;
            Color startValue = valChangeOverLife[i].fromValue_;
            Color endValue = valChangeOverLife[i].toValue_;
            int startTime = valChangeOverLife[i].startMillis_;
            int endTime = valChangeOverLife[i].endMillis_;
            if (activeTime >= startTime && activeTime < endTime) {
                Color value = generateValue(startValue, endValue, startTime, endTime, activeTime, colorCurve);
                particle.setColor(value);
            }
        }
    }
}

void RSRenderParticleEffector::updateOpacity(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime)
{
    auto opacityUpdator = particleParams_.opacity.updator_;
    if (opacityUpdator == RANDOM) {
        float opacitySpeed = getRandomValue(particleParams_.opacity.random_.start_, particleParams_.opacity.random_.end_);
        auto opacity = particle.getOpacity();
        opacity += opacitySpeed * deltaTime;
        particle.setOpacity(opacity);
    } else if (opacityUpdator == CURVE) {
        auto valChangeOverLife = particleParams_.opacity.valChangeOverLife_;
        for (int i = 0; i < valChangeOverLife.size(); i++) {
            auto colorCurve = valChangeOverLife[i].curve;
            float startValue = valChangeOverLife[i].fromValue_;
            float endValue = valChangeOverLife[i].toValue_;
            int startTime = valChangeOverLife[i].startMillis_;
            int endTime = valChangeOverLife[i].endMillis_;
            if (activeTime >= startTime && activeTime < endTime) {
                float value = generateValue(startValue, endValue, startTime, endTime, activeTime, colorCurve);
                particle.setOpacity(value);
            }
        }
    } 
    //color_.SetAlpha(color_.GetAlpha() * opacity_);
}

void RSRenderParticleEffector::updateScale(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime)
{
    auto scaleUpdator = particleParams_.scale.updator_;
    if (scaleUpdator == RANDOM) {
        float scaleSpeed = getRandomValue(particleParams_.scale.random_.start_, particleParams_.scale.random_.end_);
        auto scale = particle.getScale();
        scale += scaleSpeed * deltaTime;
        particle.setScale(scale);
    } else if (scaleUpdator == CURVE) {
        auto valChangeOverLife = particleParams_.scale.valChangeOverLife_;
        for (int i = 0; i < valChangeOverLife.size(); i++) {
            auto scaleCurve = valChangeOverLife[i].curve;
            float startValue = valChangeOverLife[i].fromValue_;
            float endValue = valChangeOverLife[i].toValue_;
            int startTime = valChangeOverLife[i].startMillis_;
            int endTime = valChangeOverLife[i].endMillis_;
            if (activeTime >= startTime && activeTime < endTime) {
                float value = generateValue(startValue, endValue, startTime, endTime, activeTime, scaleCurve);
                particle.setScale(value);
            }
        }
    }
}
void RSRenderParticleEffector::updateSpin(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime)
{
    auto spinUpdator = particleParams_.spin.updator_;
    if (spinUpdator == RANDOM) {
        float spinSpeed = getRandomValue(particleParams_.spin.random_.start_, particleParams_.spin.random_.end_);
        auto spin = particle.getSpin();
        spin += spinSpeed * deltaTime;
        particle.setSpin(spin);
    } else if (spinUpdator == CURVE) {
        auto valChangeOverLife = particleParams_.spin.valChangeOverLife_;
        for (int i = 0; i < valChangeOverLife.size(); i++) {
            auto spinCurve = valChangeOverLife[i].curve;
            float startValue = valChangeOverLife[i].fromValue_;
            float endValue = valChangeOverLife[i].toValue_;
            int startTime = valChangeOverLife[i].startMillis_;
            int endTime = valChangeOverLife[i].endMillis_;
            if (activeTime >= startTime && activeTime < endTime) {
                float value = generateValue(startValue, endValue, startTime, endTime, activeTime, spinCurve);
                particle.setSpin(value);
            }
        }
    }
}

void RSRenderParticleEffector::updateAccelerate(RSRenderParticle& particle, int64_t deltaTime, int64_t activeTime)
{
    auto acceValueUpdator = particleParams_.acceleration.accelerationValue.updator_;
    auto acceAngleUpdator = particleParams_.acceleration.accelerationValue.updator_;
    float acceValueChange = 0.f;
    float acceAngleChange = 0.f;
    float value = 0.f;
    float Angle = 0.f;
    if (acceValueUpdator == RANDOM) {
        float acceValueSpeed = getRandomValue(particleParams_.acceleration.accelerationValue.random_.start_,
            particleParams_.acceleration.accelerationValue.random_.end_);
        float acceValueChange = acceValueSpeed * deltaTime;
    } else if (acceValueUpdator == CURVE) {
        auto valChangeOverLife = particleParams_.acceleration.accelerationValue.valChangeOverLife_;
        for (int i = 0; i < valChangeOverLife.size(); i++) {
            auto acceValCurve = valChangeOverLife[i].curve;
            float startValue = valChangeOverLife[i].fromValue_;
            float endValue = valChangeOverLife[i].toValue_;
            int startTime = valChangeOverLife[i].startMillis_;
            int endTime = valChangeOverLife[i].endMillis_;
            if (activeTime >= startTime && activeTime < endTime) {
                value = generateValue(startValue, endValue, startTime, endTime, activeTime, acceValCurve);
            }
        }
    }
    if (acceAngleUpdator == RANDOM) {
        float acceAngleSpeed = getRandomValue(particleParams_.acceleration.accelerationAngle.random_.start_,
            particleParams_.acceleration.accelerationAngle.random_.end_);
        float acceAngleChange = acceAngleSpeed * deltaTime;

    } else if (acceAngleUpdator == CURVE) {
        auto valChangeOverLife = particleParams_.acceleration.accelerationAngle.valChangeOverLife_;
        for (int i = 0; i < valChangeOverLife.size(); i++) {
            auto acceAngleCurve = valChangeOverLife[i].curve;
            float startValue = valChangeOverLife[i].fromValue_;
            float endValue = valChangeOverLife[i].toValue_;
            int startTime = valChangeOverLife[i].startMillis_;
            int endTime = valChangeOverLife[i].endMillis_;
            if (activeTime >= startTime && activeTime < endTime) {
                Angle = generateValue(startValue, endValue, startTime, endTime, activeTime, acceAngleCurve);
            }
        }
    }
    if (acceValueUpdator == RANDOM && acceAngleUpdator == RANDOM) {
        auto acceleration = particle.getAcceleration();
        acceleration.x_ += acceValueChange * cos(acceAngleChange);
        acceleration.y_ += acceValueChange * sin(acceAngleChange);
        particle.setAcceleration(acceleration);
    } else if (acceValueUpdator == CURVE && acceAngleUpdator == CURVE) {
        // acceleration_.x_ = value * cos(Angle);
        // acceleration_.y_ = value * sin(Angle);
        particle.setAcceleration({value * cos(Angle), value * sin(Angle)});
    } else if (acceValueUpdator == RANDOM && acceAngleUpdator == CURVE) {
        //待实现
    } else if (acceValueUpdator == CURVE && acceAngleUpdator == RANDOM) {
        //待实现
    }
}

// Apply effector to particle
void RSRenderParticleEffector::applyEffectorToParticle(RSRenderParticle& particle, int64_t deltaTime) const
{
    int64_t activeTime = particle.getActiveTime();
    updateAccelerate(particle, deltaTime, activeTime);
    updateColor(particle, deltaTime, activeTime);
    updateOpacity(particle, deltaTime, activeTime);
    updateScale(particle, deltaTime, activeTime);
    updateSpin(particle, deltaTime, activeTime);

    auto acceleration = particle.getAcceleration();
    Vector2f velocity = particle.getVelocity();
    velocity.x_ += acceleration.x_ * deltaTime;
    velocity.y_ += acceleration.y_ * deltaTime;
    Vector2f position = particle.getPosition();
    position.x_ += velocity.x_ * deltaTime;
    position.y_ += velocity.y_ * deltaTime;
    particle.setVelocity(velocity);
    particle.setPosition(position);
    float opacity = particle.getOpacity();
    Color color = particle.getColor();
    color.SetAlpha(color.GetAlpha() * opacity);

    auto scale = particle.getScale();
    if (particle.getParticleType() == POINTS) {
        auto radius = particle.getRadius();
        radius *= scale;
    } else if (particle.getParticleType() == IMAGES) {
        auto rsImage = particle.getRSImage();
        auto width = rsImage.width_;
        auto height = rsImage.height_;
        width *= scale;
        height *= scale;
        rsImage.SetDstRect(RectF { 0.f, 0.f, width, height });
        particle.setRSImage(rsImage);
    }
    // 更新生存时间
    activeTime += deltaTime;
    particle.setActiveTime(activeTime);
}

// 根据曲线类型和时间参数生成每帧的值
template<typename T>
T generateValue(T startValue, T endValue, int startTime, int endTime, int currentTime,
    RSAnimationTimingCurve curveType)
{
    // 计算时间比例
    float t = (currentTime - startTime) / (endTime - startTime);

    // 根据曲线类型进行插值
    if (curveType == LINEAR){
        if ( std::is_same<T, Color>::value ) {
            Color startColor = *reinterpret_cast<Color*>(&startValue);
            Color endColor = *reinterpret_cast<Color*>(&endValue);
            
            Color result;
            result.SetRed(startColor.GetRed() + t * (endColor.GetRed() - startColor.GetRed()));
            result.SetGreen(startColor.GetGreen() + t * (endColor.GetGreen() - startColor.GetGreen()));
            result.SetBlue(startColor.GetBlue() + t * (endColor.GetBlue() - startColor.GetBlue()));
            result.SetAlpha(startColor.GetAlpha() + t * (endColor.GetAlpha() - startColor.GetAlpha()));
            return *reinterpret_cast<T*>(&result);
        }
        return startValue + t * (endValue - startValue);
    } 
    // else if (/*其他类型的曲线*/ ){
    // }
}

float getRandomValue(float min, float max)
{
    static std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}


//属性之间的联动变化效果器待实现

} // namespace Rosen
} // namespace OHOS