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

#include <stdint.h>
#include <sys/types.h>
#include <vector>

#include "../../../2d_graphics/include/utils/size.h"
#include "2d_graphics/include/utils/point.h"

#include "animation/rs_animation_timing_curve.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "render/rs_image.h"

namespace OHOS {
namespace Rosen {
enum ParticleUpdator { NONE = 0, RANDOM, CURVE };

template<typename T>
struct Range {
    T start_, end_;
    Range() : start_(), end_() {}
    Range(T a, T b) : start_(a), end_(b) {}
    bool operator==(const Range<T>& rhs) const
    {
        return start_ == rhs.start_ && end_ == rhs.end_;
    }

    T Width() const
    {
        return end_ - start_;
    }
};

template<typename T>
struct ChangeInOverLife {
    T fromValue_;
    T toValue_;
    int startMillis_;
    int endMillis_;
    RSAnimationTimingCurve& curve_;
    ChangeInOverLife(T fromValue, T toValue, int startMillis, int endMillis, RSAnimationTimingCurve& curve)
    {
        fromValue_ = fromValue;
        toValue_ = toValue;
        startMillis_ = startMillis;
        endMillis_ = endMillis;
        curve_ = curve;
    }
};

template<typename T>
struct  ParticleParaType {
    Range<T> val_;
    ParticleUpdator updator_;        //变化因子，是变化速度还是曲线
    Range<float> random_;            //随机变化速度，乘在属性值之上，用于表示沿着某一方向变化的趋势
    std::vector<ChangeInOverLife<T>> valChangeOverLife_;            //变化曲线，可以设置多段曲线，由初始值，终止值，开始时间，结束时间，沿着某曲线变化组成
    ParticleParaType(Range<T>& val, ParticleUpdator updator, Range<float> random,
        std::vector<ChangeInOverLife<T>>& valChangeOverLife)
    {
        val_ = val;
        updator_ = updator;
        random_ = random;
        valChangeOverLife_ = valChangeOverLife;
    }
};

enum ShapeType {
    RECT = 0,
    CIRCLE,
    ELLIPSE,
    LINE,
    POLYGON,
    POLYLINE,
    PATH,
};

// struct PointParticleParameters {
//     float radius_;
//     PointParticleParameters(float radius)
//     {
//         radius_ = radius;
//     }
// };

// struct ImageParticleParameters {
//     RSImage image_;
//     // Drawing::SizeF size_;
//     // ImageFit imageFit_;
//     ImageParticleParameters(RSImage image, int width, int height, ImageFit imageFit)
//     {
//         image_.SetImageFit = imageFit;
//         image_.srcRect_.SetAll(0.f, 0.f, width, height);
//     }
// };


enum ParticleType {
    POINTS,
    IMAGES,
};

struct EmitterConfig {
    int emitRate;
    ShapeType emitShape;
    Drawing::Point position;
    Drawing::Size emitSize;
    //float offsetAngle;
    int particleCount;
    int lifeTime;
    ParticleType type;
    float radius;
    RSImage image;
    Drawing::SizeF size;
    ImageFit imageFit;
};

struct ParticleVelocity {
    Range<float> velocityValue;
    Range<float> velocityAngle;
};

struct ParticleAcceleration {
    ParticleParaType<float> accelerationValue; 
    ParticleParaType<float> accelerationAngle;
};

struct ParticleColorParaType
//:: OHOS::Rosen::ParticleParaType<Color>
{
    Range<Color> colorVal_;
    ParticleUpdator updator_;
    Range<float> redRandom_;
    Range<float> greenRandom_;
    Range<float> blueRandom_;
    //Range<float> alphaRandom_;

    std::vector<ChangeInOverLife<Color>> valChangeOverLife_;
    ParticleColorParaType(Range<Color>& colorVal,
        ParticleUpdator updator, 
        Range<float> redRandom,
        Range<float> greenRandom,
        Range<float> blueRandom,
        //Range<float> alphaRandom,
        std::vector<ChangeInOverLife<Color>>& valChangeOverLife)
    {
        colorVal_ = colorVal;
        updator_ = updator;
        redRandom_ = redRandom;
        greenRandom_ = greenRandom;
        blueRandom_ = blueRandom;
        //alphaRandom_ = alphaRandom;
        valChangeOverLife_ = valChangeOverLife;
    }
};

struct ParticleParams {
    EmitterConfig emitterConfig;
    ParticleVelocity velocity;
    ParticleAcceleration acceleration;
    ParticleColorParaType color;
    ParticleParaType<float> opacity;
    ParticleParaType<float> scale;
    ParticleParaType<float> spin;
};
  
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_PARTICLES_H
