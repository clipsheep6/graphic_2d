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
#include "brush_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_space.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_rect.h>

#include "test_common.h"

#include "common/log_common.h"

void BrushReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* DemoRef = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(DemoRef, 0xff00ff00);
    OH_Drawing_CanvasAttachBrush(canvas, DemoRef);
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数
    OH_Drawing_CanvasDrawRect(canvas, rect1);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_BrushReset(DemoRef);
    }
    OH_Drawing_CanvasAttachBrush(canvas, DemoRef);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200, 200, 300, 300); // 200, 200, 300, 300 矩形参数
    OH_Drawing_CanvasDrawRect(canvas, rect2);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(DemoRef);
};

void BrushSetColor4f::OnTestPerformance(OH_Drawing_Canvas *canvas){
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(
        brush, OH_Drawing_ColorSetArgb(DRAW_COLORBLACK, DRAW_COLORBLACK, DRAW_COLORTRANSPARENT, DRAW_COLORTRANSPARENT));
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数
    OH_Drawing_ColorSpace *colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    OH_Drawing_Color4f color4f = {1, 0, 1, 0.5}; // 颜色属性red 1,green 0,blue 1,alpha 0.5
    OH_Drawing_ErrorCode code;
    for (int i = 0; i < testCount_; i++) {
        code = OH_Drawing_BrushSetColor4f(brush, &color4f, colorSpace);
    }
    DRAWING_LOGE("BrushSetColor4f:: OH_Drawing_ErrorCode is %{public}d", code);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
};