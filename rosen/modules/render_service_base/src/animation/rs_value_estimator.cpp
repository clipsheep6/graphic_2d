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

#include "animation/rs_value_estimator.h"

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSRenderPropertyBase> RSValueEstimator::Estimate(float fraction,
    const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue)
{
    if (endValue == nullptr) {
        ROSEN_LOGI("filter start * f2");
        return startValue->GetValue();
    }

    if (startValue == nullptr) {
        ROSEN_LOGI("filter end * f2");
        return endValue->GetValue() * fraction;
    }

    ROSEN_LOGI("RSValueEstimator_Esti PropertyType=%d", startValue->GetPropertyType());  // y   5 PROPERTY_FILTER,

    if (startValue->GetPropertyType() == RSRenderPropertyType::PROPERTY_QUATERNION) {
        auto quaternionStart = std::static_pointer_cast<RSRenderProperty<Quaternion>>(startValue);
        auto quaternionEnd = std::static_pointer_cast<RSRenderProperty<Quaternion>>(startValue);
        if (quaternionStart && quaternionEnd) {
            auto quaternionValue = EstimateQuaternion(fraction, quaternionStart->Get(), quaternionEnd->Get());
            return std::make_shared<RSRenderAnimatableProperty<Quaternion>>(quaternionValue);
        }

        return nullptr;
    }

    if (startValue->GetPropertyType() == RSRenderPropertyType::PROPERTY_FILTER) {
        auto filterStart = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSFilter>>>(startValue);
        auto filterEnd = std::static_pointer_cast<RSRenderProperty<std::shared_ptr<RSFilter>>>(endValue);
        if (filterStart && filterEnd) {
            auto filterValue = EstimateFilter(fraction, filterStart->Get(), filterEnd->Get());
            filterStart->Get()->print();
            filterEnd->Get()->print();
            filterValue->print();
            ROSEN_LOGI("filter type=%d,%d,=%d", filterStart->Get()->GetFilterType(), filterEnd->Get()->GetFilterType(), filterValue->GetFilterType());  // 1.ALGOFILTER 2.MEGERFILTER
            return std::make_shared<RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(filterValue);
        }

        ROSEN_LOGI("filter type=null");
        return nullptr;
    }

    ROSEN_LOGI("filter type= s * f1 + e * f2");
    return startValue->GetValue() * (1.0f - fraction) + endValue->GetValue() * fraction;
}

Quaternion RSValueEstimator::EstimateQuaternion(float fraction,
    const Quaternion& startValue, const Quaternion& endValue)
{
    ROSEN_LOGI("Estimate Quaternion");   // x
    auto value = startValue;
    return value.Slerp(endValue, fraction);
}

std::shared_ptr<RSFilter> RSValueEstimator::EstimateFilter(
    float fraction, const std::shared_ptr<RSFilter>& startValue, const std::shared_ptr<RSFilter>& endValue)
{
    if ((startValue == nullptr || !startValue->IsValid()) && (endValue == nullptr || !endValue->IsValid())) {
        return endValue;
    }

    if (startValue == nullptr || !startValue->IsValid()) {
        return endValue * fraction;
    }

    if (endValue == nullptr || !endValue->IsValid()) {
        return (fraction < 0.5f) ? startValue * (1.0f - fraction * 2) : endValue;
    }

    char szLog[2048] = {0};
    snprintf(szLog, 2048, "FilterType %d.%d,fract=%f", startValue->GetFilterType(), endValue->GetFilterType(), fraction);  // 1.ALGOFILTER 2.MEGERFILTER
    ROSEN_LOGI("Estimator filter [%s]", szLog);   // y

    startValue->print();
    endValue->print();
    
    if (startValue->GetFilterType() == endValue->GetFilterType()) {
//        auto v1 = ;
//        auto v2 = ;
        auto v3 = startValue * (1.0f - fraction) + endValue * fraction;
//        v1->print();
//        v2->print();
//        v3->print();
        return v3;
    } else {
        return (fraction < 0.5f) ? startValue * (1.0f - fraction * 2) : endValue * (fraction * 2 - 1.0f);
    }
}

float RSValueEstimator::EstimateFraction(
    const std::shared_ptr<RSInterpolator>& interpolator, const std::shared_ptr<RSRenderPropertyBase>& value,
    const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue)
{
    auto valueFloat = std::static_pointer_cast<RSRenderProperty<float>>(value);
    auto startFloat = std::static_pointer_cast<RSRenderProperty<float>>(startValue);
    auto endFloat = std::static_pointer_cast<RSRenderProperty<float>>(endValue);

    ROSEN_LOGI("Estimate Fraction");   // x
    if (valueFloat != nullptr && startValue != nullptr && endValue != nullptr) {
        return EstimateFloatFraction(interpolator, valueFloat->Get(), startFloat->Get(), endFloat->Get());
    }

    return 0.0f;
}

float RSValueEstimator::EstimateFloatFraction(
    const std::shared_ptr<RSInterpolator>& interpolator, const float value,
    const float startValue, const float endValue)
{
    ROSEN_LOGI("Estimate float Fraction");   // x
    float start = FRACTION_MIN;
    float end = FRACTION_MAX;
    auto byValue = endValue - startValue;
    while (end > start + EPSILON) {
        float mid = (start + end) / 2.0f;
        float fraction = interpolator->Interpolate(mid);
        auto interpolationValue = startValue * (1.0f - fraction) + endValue * fraction;
        if (value < interpolationValue) {
            (byValue > 0) ? (end = mid) : (start = mid);
        } else {
            (byValue > 0) ? (start = mid) : (end = mid);
        }

        if (std::abs(value - interpolationValue) <= EPSILON) {
            return mid;
        }
    }

    return FRACTION_MIN;
}
} // namespace Rosen
} // namespace OHOS
