/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "animation/rs_render_curve_animation.h"

#include "animation/rs_value_estimator.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSRenderCurveAnimation::RSRenderCurveAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue, const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue) : RSRenderPropertyAnimation(id, propertyId, originValue),
    startValue_(startValue), endValue_(endValue)   // PT
{
    ROSEN_LOGI("RSRenderCurveAnimation_construction=");
//    auto filterStart = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSFilter>>>(startValue_);
//    auto filterEnd = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSFilter>>>(endValue_);
//    if (filterStart && filterStart->Get()) {
//        filterStart->Get()->print();
//    }
//    if (filterEnd && filterEnd->Get()) {
//        filterEnd->Get()->print();
//    }
}

void RSRenderCurveAnimation::SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator)
{
    interpolator_ = interpolator;
}

const std::shared_ptr<RSInterpolator>& RSRenderCurveAnimation::GetInterpolator() const
{
    return interpolator_;
}

#ifdef ROSEN_OHOS
bool RSRenderCurveAnimation::Marshalling(Parcel& parcel) const
{
    ROSEN_LOGI("RSRenderCurveAnimation_marshalling");
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_) && interpolator_->Marshalling(parcel))) {
        ROSEN_LOGE("RSRenderCurveAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }
    return true;
}

RSRenderCurveAnimation* RSRenderCurveAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderCurveAnimation* renderCurveAnimation = new RSRenderCurveAnimation();
    ROSEN_LOGI("RSRenderCurveAnimation_Unmarshalling");
    if (!renderCurveAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::Unmarshalling, failed");
        delete renderCurveAnimation;
        return nullptr;
    }
    return renderCurveAnimation;
}

bool RSRenderCurveAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::Parse Param, ParseParam Fail");
        return false;
    }

    // PT
    ROSEN_LOGI("RSRenderCurveAnimation_ParseParam=");
    //auto filterStart = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSFilter>>>(startValue_);
    //auto filterEnd = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSFilter>>>(endValue_);
//    if (filterStart && filterStart->Get()) {
//        filterStart->Get()->print();
//    }
//    if (filterEnd && filterEnd->Get()) {
//        filterEnd->Get()->print();
//    }
    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderCurveAnimation::Parse Param, Unmarshalling Fail");
        return false;
    }
    ROSEN_LOGI("RSRenderCurveAnimation::Parse Param");

    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    SetInterpolator(interpolator);
    return true;
}
#endif

void RSRenderCurveAnimation::OnSetFraction(float fraction)
{
    ROSEN_LOGI("on set fraction");  // x
    OnAnimateInner(fraction, linearInterpolator_);

    SetFractionInner(RSValueEstimator::EstimateFraction(interpolator_, GetLastValue(), startValue_, endValue_));
}

void RSRenderCurveAnimation::OnAnimate(float fraction)
{
    ROSEN_LOGI("RSRenderCurveAnimation On_animate");   // y
    OnAnimateInner(fraction, interpolator_);
}

void RSRenderCurveAnimation::OnAnimateInner(float fraction, const std::shared_ptr<RSInterpolator>& interpolator)
{
    if (GetPropertyId() == 0) {
        return;
    }
    ROSEN_LOGI("RSRenderCurveAnimation On_Animate_Inner S.");  // y
    auto interpolationValue =
        RSValueEstimator::Estimate(interpolator_->Interpolate(fraction), startValue_, endValue_);
    ROSEN_LOGI("RSRenderCurveAnimation On_Animate_Inner E.");  // y
    SetAnimationValue(interpolationValue);
    ROSEN_LOGI("RSRenderCurveAnimation On_Animate_Inner SET.");  // y
}
} // namespace Rosen
} // namespace OHOS
