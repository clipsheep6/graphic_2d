/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_DYNAMIC_LIGHT_UP_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_DYNAMIC_LIGHT_UP_FILTER_H

#include <memory>
#include "include/core/SkColorFilter.h"
#include "include/effects/SkImageFilters.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSDynamicLightUpFilter : public RSShaderFilter {
public:
    RSDynamicLightUpFilter(float dynamicLightUpRate, float dynamicLightUpDegree);
    RSDynamicLightUpFilter(const RSDynamicLightUpFilter&) = delete;
    RSDynamicLightUpFilter operator=(const RSDynamicLightUpFilter&) = delete;
    ~RSDynamicLightUpFilter() override;
    float GetDynamicLightUpRate() const;
    float GetDynamicLightUpDegree() const;
#ifndef USE_ROSEN_DRAWING
sk_sp<SkBlender> MakeDynamicLightUpBlender(float dynamicLightUpRate, float dynamicLightUpDeg);
#else
std::shared_ptr<Drawing::ShaderEffect> MakeDynamicLightUpShader(
    float dynamicLightUpRate, float dynamicLightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader);
#endif

    void PreProcess(sk_sp<SkImage> image) override;
    void PostProcess(RSPaintFilterCanvas& canvas) override;
    sk_sp<SkImage> ProcessImage(SkCanvas& canvas, const sk_sp<SkImage>& image) const override;

private:
    float dynamicLightUpRate_ = 0.f;
    float dynamicLightUpDegree_ = 0.f;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_DYNAMIC_LIGHT_UP_FILTER_H