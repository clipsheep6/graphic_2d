/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_PARTICLE_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_PARTICLE_H

#include <sys/types.h>
#include <stdint.h>
#include <vector>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "animation/rs_animation_timing_curve.h"
#include "common/rs_color.h"
#include "render/rs_image.h"
#include "../../../2d_graphics/include/utils/size.h"

namespace OHOS {
namespace Rosen {
    enum ParticleUpdator {
        NONE = 0,
        RANDOM,
        CURVE
    };

    template <class T>
    class Range {
        T min;
        T max;
    };

    template <class T>
    struct ChangeInOverLife {
        T fromValue;
        T toValue;
        int startMillis;
        int endMillis;
        RSAnimationTimingCurve CURVE;
    };

    template <class T>
    class ParticleParaType {
        Range<T> val;
        ParticleUpdator updator;
        Range<float> random;
        std::vector<ChangeInOverLife<T>> valChangeOverLife;
    };

    struct PointParticleParameters {
        float radius;
    };

    enum class ShapeType {
        RECT = 0,
        CIRCLE,
        ELLIPSE,
        LINE,
        POLYGON,
        POLYLINE,
        PATH,
    };

    struct ImageParticleParameters {
        RSImage image;
        Drawing::Size size;
        ImageFit imageFit;
    };

    class particleType {
        PointParticleParameters points;
        ImageParticleParameters images;
    };
    

    struct EmitterConfig {
        int emitRate;
        ShapeType emitShape;
        Drawing::Size emitSize;
        int particleCount;
        int liftTime;
        particleType type;
    };
    
    struct ParticleVelocity{
        Range<float> velocity;
        Range<float> velocityAngle;
    };

    struct ParticleAcceleration{
        ParticleParaType<float> acceleration;
        ParticleParaType<float> accelerationAngle;
    };

    class ParticleColorParaType 
    //:: OHOS::Rosen::ParticleParaType<Color> 
    {
        Range<Color> colorVal;
        ParticleUpdator updator;
        Range<float> redRandom;
        Range<float> greenRandom;
        Range<float> blueRandom;
        Range<float> alphaRandom;
        std::vector<ChangeInOverLife<Color>> valChangeOverLife;
    };
    
struct  ParticleProperty {
    EmitterConfig emitterConfig;
    ParticleVelocity velocity;
    ParticleAcceleration acceleration;
    ParticleColorParaType color;
    ParticleParaType<float> opacity;
    ParticleParaType<float> scale;
    ParticleParaType<float> spin;
};

class ParticleSystem {
public:
    ParticleSystem(const ParticleSystem& rhs) noexcept = default;
    ~ParticleSystem() = default;

private:
    
    std::vector<ParticleProperty> particleProperties;
    

};

class Particle {
    Vector2f position;
    Color color;
    // float radius;
    // Vector2f size;
    particleType type;
};

} // namespace Rosen
} // namespace OHOS


#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_PARTICLES_H
