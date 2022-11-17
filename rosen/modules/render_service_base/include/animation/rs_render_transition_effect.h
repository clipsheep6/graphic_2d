/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_RENDER_TRANSITION_EFFECT_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSITION_RS_RENDER_TRANSITION_EFFECT_H

#ifdef ROSEN_OHOS
#include <parcel.h>
#include <refbase.h>
#endif
#include <memory>

#include "common/rs_vector2.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSRenderModifier;
template<typename T>
class RSRenderAnimatableProperty;

enum RSTransitionEffectType : uint16_t {
    FADE = 1,
    SCALE,
    TRANSLATE,
    ROTATE,
    UNDEFINED,
};

#ifdef ROSEN_OHOS
class RSRenderTransitionEffect : public Parcelable {
#else
class RSRenderTransitionEffect {
#endif
public:
    RSRenderTransitionEffect() = default;
    virtual ~RSRenderTransitionEffect() = default;
    virtual RSTransitionEffectType GetTransitionEffectType() = 0;
    const std::shared_ptr<RSRenderModifier>& GetModifier();
    virtual void UpdateFraction(float fraction) const = 0;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override = 0;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    std::shared_ptr<RSRenderModifier> modifier_;
    virtual const std::shared_ptr<RSRenderModifier> CreateModifier() = 0;
};

class RSTransitionFade : public RSRenderTransitionEffect {
public:
    explicit RSTransitionFade(float alpha) : alpha_(alpha) {}
    ~RSTransitionFade() override = default;
    RSTransitionEffectType GetTransitionEffectType() override
    {
        return RSTransitionEffectType::FADE;
    }
    void UpdateFraction(float fraction) const override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float alpha_;
    std::shared_ptr<RSRenderAnimatableProperty<float>> property_;
    const std::shared_ptr<RSRenderModifier> CreateModifier() override;
};

class RSTransitionScale : public RSRenderTransitionEffect {
public:
    RSTransitionScale(float scaleX, float scaleY, float scaleZ, float pivotX, float pivotY)
        : scaleX_(scaleX), scaleY_(scaleY), scaleZ_(scaleZ), pivotX_(pivotX), pivotY_(pivotY)
    {}
    ~RSTransitionScale() override = default;
    RSTransitionEffectType GetTransitionEffectType() override
    {
        return RSTransitionEffectType::SCALE;
    }
    void UpdateFraction(float fraction) const override;

    const std::shared_ptr<RSRenderModifier>& GetPivotModifier();

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float scaleX_;
    float scaleY_;
    float scaleZ_;
    float pivotX_;
    float pivotY_;
    std::shared_ptr<RSRenderAnimatableProperty<Vector2f>> property_;
    std::shared_ptr<RSRenderAnimatableProperty<Vector2f>> pivot_;
    std::shared_ptr<RSRenderModifier> pivotModifier_;
    const std::shared_ptr<RSRenderModifier> CreateModifier() override;
};

class RSTransitionTranslate : public RSRenderTransitionEffect {
public:
    RSTransitionTranslate(float translateX, float translateY, float translateZ)
        : translateX_(translateX), translateY_(translateY), translateZ_(translateZ)
    {}
    ~RSTransitionTranslate() override = default;
    RSTransitionEffectType GetTransitionEffectType() override
    {
        return RSTransitionEffectType::TRANSLATE;
    }
    void UpdateFraction(float fraction) const override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float translateX_;
    float translateY_;
    float translateZ_;
    std::shared_ptr<RSRenderAnimatableProperty<Vector2f>> property_;
    const std::shared_ptr<RSRenderModifier> CreateModifier() override;
};

class RSTransitionRotate : public RSRenderTransitionEffect {
public:
    RSTransitionRotate(float dx, float dy, float dz, float radian, float pivotX, float pivotY)
        : dx_(dx), dy_(dy), dz_(dz), radian_(radian), pivotX_(pivotX), pivotY_(pivotY)
    {}
    ~RSTransitionRotate() override = default;
    RSTransitionEffectType GetTransitionEffectType() override
    {
        return RSTransitionEffectType::ROTATE;
    }
    void UpdateFraction(float fraction) const override;

    const std::shared_ptr<RSRenderModifier>& GetPivotModifier();

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static RSRenderTransitionEffect* Unmarshalling(Parcel& parcel);
#endif
private:
    float dx_;
    float dy_;
    float dz_;
    float radian_;
    float pivotX_;
    float pivotY_;
    std::shared_ptr<RSRenderAnimatableProperty<Quaternion>> property_;
    std::shared_ptr<RSRenderAnimatableProperty<Vector2f>> pivot_;
    std::shared_ptr<RSRenderModifier> pivotModifier_;
    const std::shared_ptr<RSRenderModifier> CreateModifier() override;
};
} // namespace Rosen
} // namespace OHOS

#endif
