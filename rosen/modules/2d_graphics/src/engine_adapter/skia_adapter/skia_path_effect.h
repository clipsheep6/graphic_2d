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

#ifndef SKIA_PATH_EFFECT_H
#define SKIA_PATH_EFFECT_H

#include "include/core/SkPathEffect.h"

#include "impl_interface/path_effect_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPathEffect : public PathEffectImpl {
public:
    SkiaPathEffect() noexcept;
    ~SkiaPathEffect() {};

    void InitWithDash(const scalar intervals[], int count, scalar phase) override;
    void InitWithPathDash(const PathData* d, scalar advance, scalar phase, PathDashStyle style) override;
    void InitWithCorner(scalar radius) override;
    void InitWithSum(const PathEffectData* d1, const PathEffectData* d2) override;
    void InitWithCompose(const PathEffectData* d1, const PathEffectData* d2) override;
private:
    sk_sp<SkPathEffect> MutablePathEffect();
};
}
}
}
#endif
