/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "skia_path_effect.h"
#include "skia_path_effect_data.h"
#include "effect/path_effect.h"
#include "include/core/SkPathEffect.h"
#include "include/effects/Sk1DPathEffect.h"
#include "include/effects/SkCornerPathEffect.h"
#include "include/effects/SkDashPathEffect.h"
#include <memory>

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPathEffect::SkiaPathEffect() noexcept
{
    pathEffectData_ = std::make_shared<SkiaPathEffectData>();
    pathEffectData_->type = PathEffectData::SKIA_PATH_EFFECT;
}

void SkiaPathEffect::InitWithDash(const scalar intervals[], int count, scalar phase)
{
    auto e = SkDashPathEffect::Make(intervals, count, phase);
    pathEffectData_->SetPathEffect(e.release());
}

void SkiaPathEffect::InitWithPathDash(const PathData* d, scalar advance, scalar phase, PathDashStyle style)
{
    const SkPath* p = static_cast<SkPath*>(d->GetPath());
    if (p != nullptr) {
        auto e = SkPath1DPathEffect::Make(*p, advance, phase, static_cast<SkPath1DPathEffect::Style>(style));
        pathEffectData_->SetPathEffect(e.release());
    }
}

void SkiaPathEffect::InitWithCorner(scalar radius)
{
    auto e = SkCornerPathEffect::Make(radius);
    pathEffectData_->SetPathEffect(e.release());
}

void SkiaPathEffect::InitWithSum(const PathEffectData* d1, const PathEffectData* d2)
{
    sk_sp<SkPathEffect> e1(static_cast<SkPathEffect*>(d1->GetPathEffect()));
    sk_sp<SkPathEffect> e2(static_cast<SkPathEffect*>(d2->GetPathEffect()));
    auto e = SkPathEffect::MakeSum(e1, e2);
    pathEffectData_->SetPathEffect(e.release());
}

void SkiaPathEffect::InitWithCompose(const PathEffectData* d1, const PathEffectData* d2)
{
    sk_sp<SkPathEffect> outer(static_cast<SkPathEffect*>(d1->GetPathEffect()));
    sk_sp<SkPathEffect> inner(static_cast<SkPathEffect*>(d2->GetPathEffect()));
    auto e = SkPathEffect::MakeCompose(outer, inner);
    pathEffectData_->SetPathEffect(e.release());
}

sk_sp<SkPathEffect> SkiaPathEffect::MutablePathEffect()
{
    return sk_sp<SkPathEffect>(static_cast<SkPathEffect*>(pathEffectData_->GetPathEffect()));
}
}
}
}