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

#include "c/drawing_path_effect.h"

#include <unordered_map>
#include "effect/path_effect.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static std::unordered_map<void*, std::shared_ptr<PathEffect>> g_pathEffectMap;

OH_Drawing_PathEffect* OH_Drawing_CreateDashPathEffect(float* intervals, int count, float phase)
{
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateDashPathEffect(intervals, count, phase);
    g_pathEffectMap.insert({pathEffect.get(), pathEffect});
    return (OH_Drawing_PathEffect*)pathEffect.get();
}

void OH_Drawing_PathEffectDestroy(OH_Drawing_PathEffect* cPathEffect)
{
    auto it = g_pathEffectMap.find(cPathEffect);
    if (it == g_pathEffectMap.end()) {
        return;
    }
    g_pathEffectMap.erase(it);
}
