/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
struct KawaseParameter {
    Drawing::Rect src;
    Drawing::Rect dst;
    int radius;
    std::shared_ptr<Drawing::ColorFilter> colorFilter;
    float alpha = 0.f;

    KawaseParameter(const Drawing::Rect& s, const Drawing::Rect& d, int r,
        std::shared_ptr<Drawing::ColorFilter> color = nullptr, float a = 0.f)
        : src(s), dst(d), radius(r), colorFilter(color), alpha(a) {}
};
class KawaseBlurFilter {
public:
    bool ApplyKawaseBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameter& param);
    static KawaseBlurFilter* GetKawaseBlurFilter()
    {
        static thread_local KawaseBlurFilter* filter;
        if (filter == nullptr) {
            filter = new KawaseBlurFilter();
        }
        return filter;
    }

private:
    KawaseBlurFilter();
    ~KawaseBlurFilter();
    KawaseBlurFilter(const KawaseBlurFilter& filter);
    const KawaseBlurFilter &operator=(const KawaseBlurFilter& filter);
    static Drawing::Matrix GetShaderTransform(const Drawing::Canvas* canvas, const Drawing::Rect& blurRect,
        float scaleW = 1.0f, float scaleH = 1.0f);
    void CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameter& param, std::shared_ptr<Drawing::Image>& checkedImage);
    void OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameter& param);
    bool ApplyBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const std::shared_ptr<Drawing::Image>& blurImage, const KawaseParameter& param) const;
    void ComputeRadiusAndScale(int radius);
    void AdjustRadiusAndScale();
    std::string GetDescription() const;
    bool DownSample(Drawing::Canvas& canvas, std::shared_ptr<Drawing::Image>& input, const float blurScale);

    static constexpr float baseBlurScale = 0.5f; // base downSample radio
    static constexpr float scaleEpsilon = 0.01f;
    static constexpr int radiusStep1 = 50; // 50 : radius step1
    static constexpr int radiusStep2 = 150; // 150 : radius step2
    static constexpr int radiusStep3 = 400; // 400 : radius step3
    static constexpr float scaleFactor1 = 0.25f; // 0.25 : downSample scale for step1
    static constexpr float scaleFactor2 = 0.125f; // 0.125 : downSample scale for step2
    static constexpr float scaleFactor3 = 0.0625f; // 0.0625 : downSample scale for step3
    static constexpr uint32_t kMaxPasses = 4; // Maximum number of render passes
    static constexpr uint32_t kMaxPassesLargeRadius = 7;
    static constexpr float kDilatedConvolution = 2.0f;
    static constexpr float kDilatedConvolutionLargeRadius = 4.6f;
    // To avoid downscaling artifacts, interpolate the blurred fbo with the full composited image, up to this radius
    static constexpr float kMaxCrossFadeRadius = 10.0f;
    static constexpr bool supportLargeRadius = true;

    std::shared_ptr<Drawing::RuntimeEffect> blurEffect_;
    std::shared_ptr<Drawing::RuntimeEffect> mixEffect_;
    float blurRadius_ = 0.f;
    float blurScale_ = 0.25f;

    // Advanced Filter
    void setupBlurEffectAdvancedFilter();
    std::shared_ptr<Drawing::RuntimeEffect> blurEffectAF_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H
