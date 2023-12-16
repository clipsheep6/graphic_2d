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

#include "c/drawing_bitmap.h"

#include "draw/color.h"
#include "utils/round_rect.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static RoundRect* CastToRoundRect(OH_Drawing_RoundRect* cRoundRect)
{
    return reinterpret_cast<RoundRect*>(cRoundRect);
}

OH_Drawing_RoundRect* OH_Drawing_RoundRectCreate()
{
    return (OH_Drawing_RoundRect*)new RoundRect();
}

OH_Drawing_RoundRect* OH_Drawing_RoundRectCreateWithParam(float left, float top, float right,
    float bottom, float xRad, float yRad)
{
    Rect rect{left, top, right, bottom};
    return (OH_Drawing_RoundRect*)new RoundRect(rect, xRad, yRad);
}

void OH_Drawing_RoundRectDestroy(OH_Drawing_RoundRect* cRoundRect)
{
    delete CastToRoundRect(cRoundRect);
}