//
#include "animation/rs_render_particle_emitter.h"
#include <vector>
namespace OHOS {
namespace Rosen {
    
constexpr int64_t NANOSECONDS_PER_ONE_SECOND = 1000000000;
RSRenderParticleEmitter::RSRenderParticleEmitter(ParticleParams particleParams)
    : particleParams_(particleParams)
{}

//一个发射器 发射粒子
void RSRenderParticleEmitter::EmitParticle(int64_t deltaTime)
{
    //configureEmitter 不用配置发射器属性，直接从emitterConfig中获取
    auto emittersConfig = particleParams_.emitterConfig;
    int count = emittersConfig.particleCount;
    int emitRate = emittersConfig.emitRate;
    m_accumulatedTime = 0; // 单位纳秒
    int particleNum = 0;
    m_accumulatedTime += deltaTime;

    // EmitParticle 发射粒子循环，加入粒子数组
    while (m_accumulatedTime >= (1.0 / emitRate) * NANOSECONDS_PER_ONE_SECOND && particleNum < count) {
        //创建粒子
        //particle.initProperty(particleParams);
        //此时粒子属性已经是初始属性，取过随机值的。将粒子加入粒子数组
        auto particle = RSRenderParticle(particleParams_); 
        //particle.getActiveTime();
        particles_.push_back(particle);
        particleNum++;
        m_accumulatedTime += deltaTime;
    }

    //将此时的粒子数组作为particleRenderProperty
    // 更新并移除已消亡的粒子
    //update particle
    for (auto it = particles_.begin(); it != particles_.end();) {
        auto effect = RSRenderParticleEffector(particleParams_);
        effect.applyEffectorToParticle(it, deltaTime);
        if (!(it->isAlive())) {
            it = particles_.erase(it);
            particleNum--;
        } else {
            ++it;
        }
    }
}

// Setters
void RSRenderParticleEmitter::setLiveTime(int liveTime)
{
    liveTime_ = liveTime;
}

void RSRenderParticleEmitter::setEmitRate(int emitRate)
{
    emitRate_ = emitRate;
}

void RSRenderParticleEmitter::setEmitShape(ShapeType emitShape)
{
    emitShape_ = emitShape;
}

void RSRenderParticleEmitter::setPosition(const Vector2f& position)
{
    position_ = position;
}

void RSRenderParticleEmitter::setEmitSize(const Size& emitSize)
{
    emitSize_ = emitSize;
}

void RSRenderParticleEmitter::setParticleType(ParticleType type)
{
    type_ = type;
}

void RSRenderParticleEmitter::setRadius(float radius)
{
    radius_ = radius;
}

void RSRenderParticleEmitter::setImage(const RSImage& image)
{
    image_ = image;
}

void RSRenderParticleEmitter::setSize(const Size& size)
{
    size_ = size;
}

void RSRenderParticleEmitter::setImageFit(ImageFit imageFit)
{
    imageFit_ = imageFit;
}

// Getters
int RSRenderParticleEmitter::getMaxParticle() const
{
    return maxParticle_;
}

int RSRenderParticleEmitter::getActiveParticle() const
{
    return activeParticle_;
}

const std::vector<RSRenderParticle>& RSRenderParticleEmitter::getParticles() const
{
    return particles_;
}

const std::vector<RSRenderParticle*>& RSRenderParticleEmitter::getActiveParticles() const
{
    return activeParticles_;
}

int RSRenderParticleEmitter::getLiveTime() const
{
    return liveTime_;
}

bool RSRenderParticleEmitter::isEmitting() const
{
    return isEmitting_;
}

int RSRenderParticleEmitter::getEmitRate() const
{
    return emitRate_;
}

ShapeType RSRenderParticleEmitter::getEmitShape() const
{
    return emitShape_;
}

const Vector2f& RSRenderParticleEmitter::getPosition() const
{
    return position_;
}

const Size& RSRenderParticleEmitter::getEmitSize() const
{
    return emitSize_;
}

ParticleType RSRenderParticleEmitter::getParticleType() const
{
    return type_;
}

float RSRenderParticleEmitter::getRadius() const
{
    return radius_;
}

const RSImage& RSRenderParticleEmitter::getImage() const
{
    return image_;
}

const Size& RSRenderParticleEmitter::getSize() const
{
    return size_;
}

ImageFit RSRenderParticleEmitter::getImageFit() const
{
    return imageFit_;
}
} // namespace Rosen
} // namespace OHOS