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

#ifndef SKIA_FONT_H
#define SKIA_FONT_H

#include <include/core/SkFont.h>
#include "impl_interface/font_impl.h"
#include "skia_typeface.h"
#include "text/typeface.h"
#include "skia_font_metrics.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFont : public FontImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaFont() noexcept {};
    ~SkiaFont() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void SetTypeface(const std::shared_ptr<Typeface> tf) override;

    void SetSize(scalar textSize) override;

    scalar GetMetrics(FontMetrics *metrics) const override;

    std::shared_ptr<SkFont> GetFont() const;

private:
    std::shared_ptr<SkFont> font_ = std::make_shared<SkFont>();
};
} // Drawing
} // Rosen
} // OHOS

#endif
