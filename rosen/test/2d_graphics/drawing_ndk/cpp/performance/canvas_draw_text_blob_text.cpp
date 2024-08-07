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

#include "canvas_draw_text_blob_text.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_text_typography.h>

#include "performance.h"
#include "test_common.h"

#include "common/drawing_type.h"
int g_five = 5;
int g_fourteen = 14;
int g_fivee = 500;
int g_ten = 10;

void CanvasDrawTextBlobMaxOnTestPerformance(
    OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    SET_PEN_AND_BRUSH_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    if (drawingType == DrawingTypeAttachBoth) {
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachBrush) {
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }

    // 2.OH_Drawing_CanvasDrawTextBlob
    std::string text = "WT";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, g_fivee);
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
    for (int i = 0; i < testCount; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, g_five, g_fivee);
    }

    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_PenDestroy(pen);
}
void CanvasDrawTextBlobLongOnTestPerformance(
    OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    SET_PEN_AND_BRUSH_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    if (drawingType == DrawingTypeAttachBoth) {
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachPen) {
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    } else if (drawingType == DrawingTypeAttachBrush) {
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }

    // 2.OH_Drawing_CanvasDrawTextBlob
    std::string text = "In the heart of the bustling city stood an old bookstore, its facade weathered by time but its "
                       "spirit undimmed.";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, g_fourteen);
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
    for (int i = 0; i < testCount; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, g_five, i * g_ten);
    }

    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_PenDestroy(pen);
}

void CanvasDrawTextBlobLong::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawTextBlobLongOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawTextBlobLongPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawTextBlobLongOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasDrawTextBlobLongBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawTextBlobLongOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}

void CanvasDrawTextBlobMax::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawTextBlobMaxOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawTextBlobMaxPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawTextBlobMaxOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasDrawTextBlobMaxBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawTextBlobMaxOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}