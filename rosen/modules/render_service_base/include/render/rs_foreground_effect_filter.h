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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_FOREGROUND_EFFECT_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_FOREGROUND_EFFECT_FILTER_H

#include <memory>
#include "include/core/SkColorFilter.h"
#include "include/effects/SkImageFilters.h"
#include "render/rs_skia_filter.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {
struct ForegroundEffectParam {
    Drawing::Rect src;
    Drawing::Rect dst;
    float radius{};
    float blurScale{};

    ForegroundEffectParam(const Drawing::Rect& src, const Drawing::Rect& dst, int radius,
        float blurScale)
        : src(src), dst(dst), radius(radius), blurScale(blurScale) {}
};
class RSB_EXPORT RSForegroundEffectFilter : public RSDrawingFilter {
public:
    RSForegroundEffectFilter(float blurRadius);
    RSForegroundEffectFilter(const RSForegroundEffectFilter&) = delete;
    RSForegroundEffectFilter operator=(const RSForegroundEffectFilter&) = delete;
    ~RSForegroundEffectFilter() override;

    bool IsValid() const override;
    std::string GetDescription() override;
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
    void PostProcess(RSPaintFilterCanvas& canvas) override {};

    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const override
    {
        return nullptr;
    }


private:
    float blurScale_ = 0.f;
    float blurRadius_ = 0.f;

    std::shared_ptr<Drawing::RuntimeEffect> blurEffect_;

    static constexpr float baseBlurScale = 0.5f; // base downSample radio
    static constexpr uint32_t kMaxPassesLargeRadius = 7;
    static constexpr float kDilatedConvolutionLargeRadius = 4.6f;

    void MakeForegroundEffect();
    void ComputeRadiusAndScale(int radius);
    void AdjustRadiusAndScale();
    static void CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const ForegroundEffectParam& param, std::shared_ptr<Drawing::Image>& checkedImage);
    void ApplyForegroundEffect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const ForegroundEffectParam& param) const;

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LIGHT_UP_EFFECT_FILTER_H