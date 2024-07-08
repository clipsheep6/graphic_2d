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

#include "drawing_path_effect.h"

#include <mutex>
#include <unordered_map>

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"

#include "effect/path_effect.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static std::mutex g_pathEffectLockMutex;
static std::unordered_map<void*, std::shared_ptr<PathEffect>> g_pathEffectMap;

static const Path& CastToPath(const OH_Drawing_Path& cPath)
{
    return reinterpret_cast<const Path&>(cPath);
}

static PathEffect& CastToPathEffect(OH_Drawing_PathEffect& cPathEffect)
{
    return reinterpret_cast<PathEffect&>(cPathEffect);
}

OH_Drawing_PathEffect* OH_Drawing_CreateDashPathEffect(float* intervals, int count, float phase)
{
    if (intervals == nullptr || count <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    NativeHandle<PathEffect>* pathEffectHandle = new NativeHandle<PathEffect>;
    if (pathEffectHandle == nullptr) {
        return nullptr;
    }
    pathEffectHandle->value = PathEffect::CreateDashPathEffect(intervals, count, phase);
    if (pathEffectHandle->value == nullptr) {
        delete pathEffectHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<PathEffect>*, OH_Drawing_PathEffect*>(pathEffectHandle);
}

OH_Drawing_PathEffect* OH_Drawing_CreatePathDashEffect(const OH_Drawing_Path* path,
    float advance, float phase, OH_Drawing_PathDashStyle style)
{
    if (advance <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreatePathDashEffect(
        CastToPath(*path), advance, phase, static_cast<PathDashStyle>(style));
    g_pathEffectMap.insert({pathEffect.get(), pathEffect});
    return (OH_Drawing_PathEffect*)pathEffect.get();
}

OH_Drawing_PathEffect* OH_Drawing_CreateCornerPathEffect(float radius)
{
    if (radius <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateCornerPathEffect(radius);
    g_pathEffectMap.insert({pathEffect.get(), pathEffect});
    return (OH_Drawing_PathEffect*)pathEffect.get();
}

OH_Drawing_PathEffect* OH_Drawing_CreateDiscretePathEffect(float segLength, float dev, uint32_t seedAssist)
{
    if (segLength <= 0 && seedAssist < 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateDiscretePathEffect(segLength, dev, seedAssist = 0);
    g_pathEffectMap.insert({pathEffect.get(), pathEffect});
    return (OH_Drawing_PathEffect*)pathEffect.get();
}

OH_Drawing_PathEffect* OH_Drawing_CreateSumPathEffect(OH_Drawing_PathEffect* cPathEffectOne,
    OH_Drawing_PathEffect* cPathEffectTwo)
{
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateSumPathEffect(
        CastToPathEffect(*cPathEffectOne), CastToPathEffect(*cPathEffectTwo));
    g_pathEffectMap.insert({pathEffect.get(), pathEffect});
    return (OH_Drawing_PathEffect*)pathEffect.get();
}

OH_Drawing_PathEffect* OH_Drawing_CreateComposePathEffect(OH_Drawing_PathEffect* cPathEffectOne,
    OH_Drawing_PathEffect* cPathEffectTwo)
{
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateComposePathEffect(
        CastToPathEffect(*cPathEffectOne), CastToPathEffect(*cPathEffectTwo));
    g_pathEffectMap.insert({pathEffect.get(), pathEffect});
    return (OH_Drawing_PathEffect*)pathEffect.get();
}

void OH_Drawing_PathEffectDestroy(OH_Drawing_PathEffect* cPathEffect)
{
    delete Helper::CastTo<OH_Drawing_PathEffect*, NativeHandle<PathEffect>*>(cPathEffect);
}
