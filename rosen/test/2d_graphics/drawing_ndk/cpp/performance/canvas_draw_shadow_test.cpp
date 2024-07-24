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

#include "canvas_draw_shadow_test.h"

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
int g_size = 4;
int g_two = 2;
int g_size = 1000;
int g_hundred = 100;
void CanvasDrawShadowLineOnTestPerformance(
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
    // 4.OH_Drawing_CanvasDrawShadow
    OH_Drawing_Point3D planeParams;
    OH_Drawing_Point3D devLightPos;
    planeParams.x = 0;        // 0 用于坐标计算
    planeParams.y = 0;        // 0 用于坐标计算
    planeParams.z = 50.f;     // 50.f 用于坐标计算
    devLightPos.x = 100.f;    // 100.f 用于坐标计算
    devLightPos.y = 100.f;    // 100.f 用于坐标计算
    devLightPos.z = 100.f;    // 100.f 用于坐标计算
    float lightRadius = 50.f; // 50.f 用于坐标计算
    uint32_t ambientColor = 0x19000000;
    uint32_t spotColor = 0x40000000;
    OH_Drawing_CanvasShadowFlags flag = SHADOW_FLAGS_ALL;
    TestRend rand = TestRend();
    for (int j = 1; j <= testCount; j++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, 0, g_two * (j - 1));
        for (int i = 0; i <= g_size; i++) {
            int x = rand.nextUScalar1() * g_two;
            OH_Drawing_PathLineTo(path, i * g_two + x, (i % g_two) * g_two + g_two * (j - 1));
        }
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_CanvasDrawShadow(canvas, path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
        OH_Drawing_PathDestroy(path);
    }
    // 5.释放资源
    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
}
void CanvasDrawShadowCurveOnTestPerformance(
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

    // 4.OH_Drawing_CanvasDrawShadow
    OH_Drawing_Point3D planeParams;
    OH_Drawing_Point3D devLightPos;
    planeParams.x = 0;        // 0 用于坐标计算
    planeParams.y = 0;        // 0 用于坐标计算
    planeParams.z = 50.f;     // 50.f 用于坐标计算
    devLightPos.x = 100.f;    // 100.f 用于坐标计算
    devLightPos.y = 100.f;    // 100.f 用于坐标计算
    devLightPos.z = 100.f;    // 100.f 用于坐标计算
    float lightRadius = 50.f; // 50.f 用于坐标计算
    uint32_t ambientColor = 0x19000000;
    uint32_t spotColor = 0x40000000;
    OH_Drawing_CanvasShadowFlags flag = SHADOW_FLAGS_ALL;

    for (int j = 0; j < testCount; j++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        for (int i = 0; i < g_hundred; i++) {
            OH_Drawing_PathMoveTo(path, (i + 1) * g_size, 1 + j * g_two);
            OH_Drawing_PathArcTo(path, i * g_size, j * g_two, (i + 1) * g_size, (j + 1) * g_two, 0, 180);
            OH_Drawing_PathQuadTo(path, g_two + i * g_size, j * g_two, (i + 1) * g_size, 1 + j * g_two);
        }
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_CanvasDrawShadow(canvas, path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
        OH_Drawing_PathDestroy(path);
    }

    // 5.释放资源
    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
}

void CanvasDrawShadowLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawShadowLineOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawShadowLinePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawShadowLineOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasDrawShadowLineBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawShadowLineOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}

void CanvasDrawShadowCurve::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawShadowCurveOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawShadowCurvePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawShadowCurveOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasDrawShadowCurveBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    CanvasDrawShadowCurveOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}