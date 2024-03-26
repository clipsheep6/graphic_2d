/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_DYNAMIC_DIM_EFFECT_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_DYNAMIC_DIM_EFFECT_FILTER_H

#include <memory>
#include "include/core/SkColorFilter.h"
#include "include/effects/SkImageFilters.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSDynamicDimFilter : public RSDrawingFilter {
public:
    RSDynamicDimFilter(float dynamicDimDegree);
    RSDynamicDimFilter(const RSDynamicDimFilter&) = delete;
    RSDynamicDimFilter operator=(const RSDynamicDimFilter&) = delete;
    ~RSDynamicDimFilter() override;
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const override;
    std::string GetDescription() override;
    float GetDynamicDimDegree();

    std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Multiply(float rhs) override;
    std::shared_ptr<RSFilter> Negate() override;
    bool IsNearEqual(
        const std::shared_ptr<RSFilter>& other, float threshold = std::numeric_limits<float>::epsilon()) const override;
    bool IsNearZero(float threshold = std::numeric_limits<float>::epsilon()) const override;

private:
    float dynamicDimDegree_ = 0.f;
    std::shared_ptr<Drawing::ImageFilter> CreateDynamicDimFilter(float dynamicDimDegree);

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_DYNAMIC_DIM_EFFECT_FILTER_H