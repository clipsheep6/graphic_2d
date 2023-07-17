// 处理数据
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include "rs_particle.h"
#include "rs_vector2.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class ParticleProperty {
public:
    // particle property value
    Vector2f position_;
    Vector2f velocity_;
    Vector2f acceleration_;
    // float velocityX;
    // float velocityY;
    // float accelerationX;
    // float accelerationY;
    float spin_;
    float opacity_;
    Color color_;
    float radius_;
    RSImage rsImage_;
    Drawing::Size particleSize_;
    ParticleType particleType_;
    int liftTime_;

    // value change speed
    float accelerationXSpeed;
    float accelerationYSpeed;
    float spinSpeed;
    float redSpeed;
    float greenSpeed;
    float blueSpeed;
    // float alphaSpeed;
    float opacitySpeed;
    float scaleSpeed;

    // value change curve
    std::vector<ChangeInOverLife<float>> accelerationXChangeCurve;
    std::vector<ChangeInOverLife<float>> accelerationYChangeCurve;
    std::vector<ChangeInOverLife<float>> spinChangeCurve;
    std::vector<ChangeInOverLife<float>> scaleChangeCurve;
    std::vector<ChangeInOverLife<float>> opacityChangeCurve;
    std::vector<ChangeInOverLife<Color>> colorChangeCurve;

    float getRandomValue(float min, float max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
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
            return Vector2f{positionX, positionY};
        }
        if (emitShape == CIRCLE || emitShape == ELLIPSE ) {
            float rx = getRandomValue(0.f, emitSize.Width());
            float ry = getRandomValue(0.f, emitSize.Height());
            float theta = getRandomValue(0.f, 2 * PI);
            float positionX = position.x_ + rx * cos(theta);
            float positionY = position.y_ + ry * sin(theta);
            return Vector2f{positionX, positionY};
        }
        
    }

    Color lerp(const Color& start, const Color& end, float t) {
        Color result;
        result.SetRed(start.GetRed() + static_cast<int>(std::round((end.GetRed() - start.GetRed()) * t)));
        result.SetGreen(start.GetGreen() + static_cast<int>(std::round((end.GetGreen() - start.GetGreen()) * t)));
        result.SetBlue(start.GetBlue() + static_cast<int>(std::round((end.GetBlue() - start.GetBlue()) * t)));
        result.SetAlpha(start.GetAlpha() + static_cast<int>(std::round((end.GetAlpha() - start.GetAlpha()) * t)));
        return result;
    }

    void initialize(ParticleParams particleParams)
    {
       // 确定粒子属性取随机值之后的值
        position_ = calculateParticlePosition(particleParams.emitterConfig.emitShape, 
                                            particleParams.emitterConfig.position, 
                                            particleParams.emitterConfig.emitSize);

        float velocityValue = getRandomValue(particleParams.velocity.velocityValue.start_, particleParams.velocity.velocityValue.end_);
        float velocityAngle = getRandomValue(particleParams.velocity.velocityAngle.start_, particleParams.velocity.velocityAngle.end_);
        velocity_ = Vector2f{velocityValue * cos(velocityAngle), velocityValue * sin(velocityAngle)};
        float accelerationValue = getRandomValue(particleParams.acceleration.accelerationValue.val_.start_, particleParams.acceleration.accelerationValue.val_.end_);
        float accelerationAngle = getRandomValue(particleParams.acceleration.accelerationAngle.val_.start_, particleParams.acceleration.accelerationAngle.val_.end_);
        acceleration_ = Vector2f{accelerationValue * cos(accelerationAngle), accelerationValue * sin(accelerationAngle)};
        spin_ = getRandomValue(particleParams.spin.val_.start_, particleParams.spin.val_.end_);
        opacity_ = getRandomValue(particleParams.opacity.val_.start_, particleParams.opacity.val_.end_);
        particleType_ = particleParams.emitterConfig.type
        if (particleType_ == POINTS) {
            float colorRandomValue = getRandomValue(0.0f, 1.0f);  // 生成一个0.0 ~ 1.0之间的随机值
            color_ = lerp(particleParams.color.colorVal_.start_, particleParams.color.colorVal_.end_, colorRandomValue);
            radius_ = particleParams.emitterConfig.radius * getRandomValue(particleParams.scale.val_.start_, particleParams.scale.val_.end_);
        } else if (particleType_ == IMAGES) {
            rsImage_ = particleParams.emitterConfig.image;
            rsImage_.SetImageFit(particleParams.emitterConfig.imageFit);
            rsImage_.SetDstRect(RectF{0.f, 0.f, particleParams.emitterConfig.size.width_, particleParams.emitterConfig.size.height_});
        }
        liftTime_ = particleParams.emitterConfig.liftTime;
        
        // 确定变化率
        float accelerationValueSpeed = getRandomValue(particleParams.acceleration.accelerationValue.random_.start_, float particleParams.acceleration.accelerationValue.random_.end_);
        float accelerationAngleSpeed = getRandomValue(particleParams.acceleration.accelerationAngle.random_.start_, float particleParams.acceleration.accelerationAngle.random_.end_);
        float spinSpeed = getRandomValue(particleParams.spin.random_.start_, particleParams.spin.random_.end_);

        float redSpeed = getRandomValue(particleParams.color.redRandom_.start_, particleParams.color.redRandom_.end_);
        float greenSpeed = getRandomValue(particleParams.color.greenRandom_.start_, particleParams.color.greenRandom_.end_);
        float blueSpeed = getRandomValue(particleParams.color.blueRandom_.start_, particleParams.color.blueRandom_.end_);
        float opacitySpeed = getRandomValue(particleParams.opacity.redRandom_.start_, particleParams.opacity.redRandom_.end_);
        float scaleSpeed = getRandomValue(particleParams.scale.redRandom_.start_, particleParams.scale.redRandom_.end_);

        // value change curve
        // std::vector<ChangeInOverLife<float>> accelerationXChangeCurve;
        // std::vector<ChangeInOverLife<float>> accelerationYChangeCurve;
        // std::vector<ChangeInOverLife<float>> spinChangeCurve;
        // std::vector<ChangeInOverLife<float>> scaleChangeCurve;
        // std::vector<ChangeInOverLife<float>> opacityChangeCurve;
        // std::vector<ChangeInOverLife<Color>> colorChangeCurve;
        
        
        // 根据曲线变化粒子的属性值 
        // ...
    }

    void SetPosition(OHOS::Rosen::Vector2f position)
    {
        position_ = position;
    }

    Vector2f GetPosition()
    {
        return position_;
    }

    void SetColor(const Color& color)
    {
        color_ = color;
    }

    Color GetColor()
    {
        return color_;
    }

    // uint32_t GetRed() const;
    // uint32_t GetGreen() const;
    // uint32_t GetBlue() const;
    // void SetRed(uint32_t r);
    // void SetGreen(uint32_t g);
    // void SetBlue(uint32_t b);
    void SetAlpha(uint32_t a)
    {
        color_.alpha_ = a;
    }

    uint32_t GetAlpha()
    {
        return color_.alpha_;
    }

    void SetRadius(float radius)
    {
        radius_ = radius;
    }

    Color GetRadius()
    {
        return radius_;
    }

    void SetRSImage(RSImage rsImage)
    {
        rsImage_ = rsImage;
    }

    RSImage GetRSImage()
    {
        return rsImage_;
    }
};

// struct EmitterConfig {
//     int emitRate;
//     ShapeType emitShape;
//     Drawing::Point position;
//     Drawing::Size emitSize;
//     int particleCount;
//     int liftTime;
//     particleType type;
// };

class ParticleEmitter {
public:
    ParticleEmitter(int numParticles, ParticleParams particleParams) : particles(numParticles) , particleParams_(particleParams)
    {
        // int emitRate = particleParams.emitterConfig.emitRate;
        // // 通过emitShape position emitSize 计算粒子的出现位置
        // position = calculateParticlePosition(particleParams.emitterConfig.
        // emitShape, Drawing::Point position, Drawing::Size emitSize)
    }

    

    void emitParticles()
    {
        //根据粒子发射速率产生粒子
        for (auto& particle : particles) {
            particle.initialize(particleParams);
        }
    }

    std::vector<ParticleProperty>& getParticles()
    {
        return particles;
    }

    float getEmitRate() const
    {
        return emitRate;
    }

    void setEmitRate(float rate)
    {
        emitRate = rate;
    }

    float getEmitterSize() const
    {
        return emitterSize;
    }

    void setEmitterSize(float size)
    {
        emitterSize = size;
    }

    float getLifeTime() const
    {
        return lifeTime;
    }

    void setLifeTime(float lifeTime)
    {
        this->lifeTime = lifeTime;
    }

    ParticleType getParticleType() const
    {
        return type;
    }

    void setParticleType(ParticleType type)
    {
        this->type = type;
    }

private:
    std::vector<ParticleProperty> particles;
    // float emitRate;
    // float emitterSize;
    // float lifeTime;
    ParticleParams particleParams_;
};
} // namespace Rosen
} // namespace OHOS