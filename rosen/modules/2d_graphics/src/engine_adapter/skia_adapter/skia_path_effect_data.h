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

#ifndef SKIA_PATH_EFFECT_DATA_H
#define SKIA_PATH_EFFECT_DATA_H

#include "include/core/SkPathEffect.h"

#include "impl_interface/path_effect_data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct SkiaPathEffectData : PathEffectData {
public:
    SkiaPathEffectData() noexcept : PathEffectData(), effect(nullptr) {}
    void SetPathEffect(void* e) override { effect.reset(static_cast<SkPathEffect*>(e)); }
    void* GetPathEffect() const override { return effect.get(); }
    sk_sp<SkPathEffect> effect;
};
}
}
}
#endif
