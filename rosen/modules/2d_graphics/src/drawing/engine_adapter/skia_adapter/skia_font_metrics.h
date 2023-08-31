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

#ifndef SKIA_FONT_STRING_H
#define SKIA_FONT_STRING_H

#include <memory>
#include <include/core/SkFontMetrics.h>
#include "impl_interface/font_metrics_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontMetrics : public FontMetricsImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaFontMetrics() noexcept;
    ~SkiaFontMetrics() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    std::shared_ptr<SkFontMetrics> GetSkFontMetrics() const;

    void SetSkFontMetrics(const std::shared_ptr<SkFontMetrics> metrics);

    uint32_t *fFlags_ = nullptr;
    scalar *fTop_ = nullptr;
    scalar *fAscent_ = nullptr;
    scalar *fDescent_ = nullptr;
    scalar *fBottom_ = nullptr;
    scalar *fLeading_ = nullptr;
    scalar *fAvgCharWidth_ = nullptr;
    scalar *fMaxCharWidth_ = nullptr;
    scalar *fXMin_ = nullptr;
    scalar *fXMax_ = nullptr;
    scalar *fXHeight_ = nullptr;
    scalar *fCapHeight_ = nullptr;
    scalar *fUnderlineThickness_ = nullptr;
    scalar *fUnderlinePosition_ = nullptr;
    scalar *fStrikeoutThickness_ = nullptr;
    scalar *fStrikeoutPosition_ = nullptr;

private:
    std::shared_ptr<SkFontMetrics> skFontMetrics_ = std::make_shared<SkFontMetrics>();
};
} // Drawing
} // Rosen
} // OHOS

#endif
