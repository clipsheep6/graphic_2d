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

#include "canvas_clip_path_test.h"

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
#include "performance.h"
#include "test_common.h"
#include "common/drawing_type.h"
int g_sizex = 1000;
int g_sizey = 1000;
int g_fifty = 50;
int g_ten = 10;
int g_two = 2;
int g_twenty = 20;
int g_five = 5;
int g_eight = 180;
void PerformanceCanvasClipPathLine(
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

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    TestRend rand = TestRend();
    for (int j = 1; j <= g_sizex; j++) {
        for (int i = 0; i < g_sizey; i++) {
            int x = rand.nextUScalar1() * g_two;
            OH_Drawing_PathLineTo(path, i * g_two + x, (i % g_two) * g_two + g_two * (j - 1));
        }
        OH_Drawing_PathMoveTo(path, 0, g_two * j);
    }
    OH_Drawing_CanvasDrawPath(canvas, path);

    for (int i = 0; i < testCount; i++) {
        OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, true);
    }

    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
}
void PerformanceCanvasClipPathCurve(
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

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int j = 0; j < g_fifty; j++) {
        for (int i = 0; i < g_fifty; i++) {
            OH_Drawing_PathMoveTo(path, (i + 1) * g_twenty, g_five + j * g_ten);
            OH_Drawing_PathArcTo(path, i * g_twenty, j * g_ten, (i + 1) * g_twenty, (j + 1) * g_ten, 0, g_eight);
            OH_Drawing_PathQuadTo(path, g_ten + i * g_twenty, j * g_ten, (i + 1) * g_twenty, g_five + j * g_ten);
        }
        OH_Drawing_CanvasDrawPath(canvas, path);
    }
    for (int j = 0; j < testCount; j++) {
        OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, true);
    }

    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
}
void CanvasClipPathLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathLine(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasClipPathLinePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathLine(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasClipPathLineBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathLine(canvas, DrawingTypeAttachBrush, testCount_);
}

void CanvasClipPathCurve::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathCurve(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasClipPathCurvePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathCurve(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasClipPathCurveBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathCurve(canvas, DrawingTypeAttachBrush, testCount_);
}