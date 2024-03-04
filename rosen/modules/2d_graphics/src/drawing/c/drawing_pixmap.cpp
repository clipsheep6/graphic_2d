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

#include "c/drawing_pixmap.h"

#include "image/pixmap.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Pixmap* CastToPixmap(OH_Drawing_Pixmap* cPixmap)
{
    return reinterpret_cast<Pixmap*>(cPixmap);
}

OH_Drawing_Pixmap* OH_Drawing_PixmapCreate()
{
    return (OH_Drawing_Pixmap*)new Pixmap;
}

void OH_Drawing_PixmapDestroy(OH_Drawing_Pixmap* cPixmap)
{
    delete CastToPixmap(cPixmap);
}

uint32_t OH_Drawing_PixmapGetWidth(OH_Drawing_Pixmap* cPixmap)
{
    Pixmap* pixmap = CastToPixmap(cPixmap);
    if (pixmap == nullptr) {
        return 0;
    }
    return pixmap->GetWidth();
}

uint32_t OH_Drawing_PixmapGetHeight(OH_Drawing_Pixmap* cPixmap)
{
    Pixmap* pixmap = CastToPixmap(cPixmap);
    if (pixmap == nullptr) {
        return 0;
    }
    return pixmap->GetHeight();
}