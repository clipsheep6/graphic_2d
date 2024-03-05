/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "c/drawing_region.h"

#include "utils/region.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Region* CastToRegion(OH_Drawing_Region* cRegion)
{
    return reinterpret_cast<Region*>(cRegion);
}

static Rect* CastToRect(OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<Rect*>(cRect);
}

OH_Drawing_Region* OH_Drawing_RegionCreate()
{
    return (OH_Drawing_Region*)new Region();
}

bool OH_Drawing_RegionSetRect(OH_Drawing_Region* cRegion, OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        return false;
    }
    Rect* rect = CastToRect(cRect);
    Region* region = CastToRegion(cRegion);
    if (region == nullptr) {
        return false;
    }
    int left = rect->GetLeft();
    int right = rect->GetRight();
    int top = rect->GetTop();
    int bottom = rect->GetBottom();
    RectI rectI(left, top, right, bottom);
    return region->SetRect(rectI);
}

void OH_Drawing_RegionDestroy(OH_Drawing_Region* cRegion)
{
    delete CastToRegion(cRegion);
}