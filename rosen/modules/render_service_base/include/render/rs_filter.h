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

#ifndef ROSEN_ENGINE_CORE_RENDER_RS_FILTER_H
#define ROSEN_ENGINE_CORE_RENDER_RS_FILTER_H

#include <memory>
#include <stdint.h>

#include "common/rs_color.h"
#include "common/rs_macros.h"
#ifndef USE_ROSEN_DRAWING
#include "include/gpu/GrDirectContext.h"
#else
#include "image/gpu_context.h"
#endif

namespace OHOS {
namespace Rosen {
enum BLUR_COLOR_MODE : int {
    PRE_DEFINED = 0,           // use the pre-defined mask color
    AVERAGE     = 1,           // use the average color of the blurred area as mask color
    FASTAVERAGE = 2,
    DEFAULT     = PRE_DEFINED
};

class RSB_EXPORT RSFilter : public std::enable_shared_from_this<RSFilter> {
public:
    class RSFilterTask {
    public:
#ifndef USE_ROSEN_DRAWING
        virtual bool InitSurface(GrRecordingContext* grContext);
#else
        virtual bool InitSurface(Drawing::GPUContext* grContext);
#endif
        virtual bool Render();
    };
    static std::function<void(std::weak_ptr<RSFilter::RSFilterTask>)> postTask;

    virtual ~RSFilter();
    RSFilter(const RSFilter&) = delete;
    RSFilter(const RSFilter&&) = delete;
    RSFilter& operator=(const RSFilter&) = delete;
    RSFilter& operator=(const RSFilter&&) = delete;
    virtual std::string GetDescription();
    static std::shared_ptr<RSFilter> CreateBlurFilter(float blurRadiusX, float blurRadiusY);
    static std::shared_ptr<RSFilter> CreateMaterialFilter(
        int style, float dipScale, BLUR_COLOR_MODE mode = DEFAULT, float ratio = 1.0);
    static std::shared_ptr<RSFilter> CreateMaterialFilter(
        float radius, float saturation, float brightness, uint32_t colorValue,
        BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT);
    static std::shared_ptr<RSFilter> CreateLightUpEffectFilter(float lightUpDegree);

    enum FilterType {
        NONE = 0,
        BLUR,
        MATERIAL,
        LIGHT_UP_EFFECT,
    };
    FilterType GetFilterType() const
    {
        return type_;
    }
    virtual bool IsValid() const
    {
        return type_ != FilterType::NONE;
    }

    uint32_t Hash() const
    {
        return hash_;
    }

    virtual bool IsNearEqual(
        const std::shared_ptr<RSFilter>& other, float threshold = std::numeric_limits<float>::epsilon()) const
    {
        return true;
    }

    virtual bool IsNearZero(float threshold = std::numeric_limits<float>::epsilon()) const
    {
        return true;
    }

protected:
    FilterType type_;
    uint32_t hash_ = 0;
    RSFilter();
    virtual std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) { return nullptr; }
    virtual std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) { return nullptr; }
    virtual std::shared_ptr<RSFilter> Multiply(float rhs) { return nullptr; }
    virtual std::shared_ptr<RSFilter> Negate() { return nullptr; }
    friend RSB_EXPORT std::shared_ptr<RSFilter> operator+(const std::shared_ptr<RSFilter>& lhs,
                                                         const std::shared_ptr<RSFilter>& rhs);
    friend RSB_EXPORT std::shared_ptr<RSFilter> operator-(const std::shared_ptr<RSFilter>& lhs,
                                                         const std::shared_ptr<RSFilter>& rhs);
    friend RSB_EXPORT std::shared_ptr<RSFilter> operator*(const std::shared_ptr<RSFilter>& lhs, float rhs);
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_RENDER_RS_FILTER_H
