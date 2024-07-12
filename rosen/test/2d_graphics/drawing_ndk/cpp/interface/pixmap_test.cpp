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
#include "pixmap_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_pixmap.h>

#include "common/log_common.h"

void PixmapGetWidth::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Pixmap *pixmap = OH_Drawing_PixmapCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形 // 0, 0, 100, 100 创建矩形
    if (pixmap == nullptr) {
        DRAWING_LOGE("PixmapGetWidth:: test OH_Drawing_PixmapCreate crete failed");
    }
    uint32_t width;
    OH_Drawing_ErrorCode code;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_PixmapGetWidth(pixmap, &width);
    }
    DRAWING_LOGE("PixmapGetWidth:: OH_Drawing_ErrorCode is %{public}d ,width is %{public}d", code, width);
    OH_Drawing_PixmapDestroy(pixmap);
    DRAWING_LOGE("PixmapGetWidth:: test OH_Drawing_PixmapDestroy ");
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}

void PixmapGetHeight::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Pixmap *pixmap = OH_Drawing_PixmapCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形 // 0, 0, 100, 100 创建矩形
    if (pixmap == nullptr) {
        DRAWING_LOGE("PixmapGetHeight:: test OH_Drawing_PixmapCreate crete failed");
    }
    uint32_t height;
    OH_Drawing_ErrorCode code;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_PixmapGetHeight(pixmap, &height);
    }
    DRAWING_LOGE("PixmapGetHeight::OH_Drawing_ErrorCode is %{public}d , height is %{public}d", code, height);
    OH_Drawing_PixmapDestroy(pixmap);
    DRAWING_LOGE("PixmapGetHeight:: test OH_Drawing_PixmapDestroy ");
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
}
