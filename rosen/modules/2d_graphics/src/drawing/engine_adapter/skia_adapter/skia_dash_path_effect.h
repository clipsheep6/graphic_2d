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

#ifndef SKIA_DASH_PATH_EFFECT_H
#define SKIA_DASH_PATH_EFFECT_H

#include <memory>

#include <include/effects/SkDashPathEffect.h>

#include "skia_path_effect.h"
#include "impl_interface/dash_path_effect_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaDashPathEffect : public DashPathEffectImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaDashPathEffect() noexcept {}
    ~SkiaDashPathEffect() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    static std::shared_ptr<SkiaPathEffect> Make(const scalar intervals[], int count, scalar phase);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif 
