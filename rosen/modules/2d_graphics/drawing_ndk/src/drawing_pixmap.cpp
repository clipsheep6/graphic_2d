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

#include "drawing_pixmap.h"

#include "image/pixmap.h"
#include "drawing_canvas_utils.h"

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

OH_Drawing_ErrorCode OH_Drawing_PixmapGetWidth(OH_Drawing_Pixmap* cPixmap, uint32_t* width)
{
    Pixmap* pixmap = CastToPixmap(cPixmap);
    if (pixmap == nullptr || width == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *width = pixmap->GetWidth();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_PixmapGetHeight(OH_Drawing_Pixmap* cPixmap, uint32_t* height)
{
    Pixmap* pixmap = CastToPixmap(cPixmap);
    if (pixmap == nullptr || height == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *height = pixmap->GetHeight();
    return OH_DRAWING_SUCCESS;
}