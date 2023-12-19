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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LIGHT_UP_EFFECT_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LIGHT_UP_EFFECT_FILTER_H

#include <memory>
#include "include/core/SkColorFilter.h"
#include "include/effects/SkImageFilters.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSLightUpEffectFilter : public RSShaderFilter {
public:
    RSLightUpEffectFilter(float lightUpDegree);
    RSLightUpEffectFilter(const RSLightUpEffectFilter&) = delete;
    RSLightUpEffectFilter operator=(const RSLightUpEffectFilter&) = delete;
    ~RSLightUpEffectFilter() override;
    float GetLightUpDegree();
#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> MakeLightUpEffectShader(float lightUpDeg, sk_sp<SkShader> imageShader);
#else
std::shared_ptr<Drawing::ShaderEffect> MakeLightUpEffectShader(
    float lightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader);
#endif

    void PreProcess(sk_sp<SkImage> image) override;
    void PostProcess(RSPaintFilterCanvas& canvas) override;
    sk_sp<SkImage> ProcessImage(SkCanvas& canvas, const sk_sp<SkImage>& image) const override;

private:
    float lightUpDegree_ = 0.f;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LIGHT_UP_EFFECT_FILTER_H