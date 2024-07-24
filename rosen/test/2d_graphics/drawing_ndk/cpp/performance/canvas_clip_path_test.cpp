/*
Created on 2024/7/10.

Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
please include "napi/native_api.h".
*/

#include "canvas_clip_path_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
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
#include <native_drawing/drawing_mask_filter.h>
#include "test_common.h"
#include "common/drawing_type.h"
#include "performance.h"

void PerformanceCanvasClipPathLineOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    SET_PEN_AND_BRUSH_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    if(drawingType == DrawingTypeAttachBoth){
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    }else if (drawingType == DrawingTypeAttachPen){
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    }else if (drawingType == DrawingTypeAttachBrush) {
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    TestRend rand = TestRend();
    for(int j = 1;j <= 1000;j++){
        for (int i = 0; i < 1000; i++) {
            int x = rand.nextUScalar1() * 2;
            OH_Drawing_PathLineTo(path, i*2+x, (i%2)*2 + 2*(j-1));
    }
        OH_Drawing_PathMoveTo(path, 0, 2*j);
    }
    OH_Drawing_CanvasDrawPath(canvas, path);

    for(int i = 0; i < testCount; i++){
        OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, true);
    }
    
    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
}
void PerformanceCanvasClipPathCurveOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, DRAW_COLORGREEN);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_PenSetAlpha(pen,50);
    OH_Drawing_PenSetBlendMode(pen,BLEND_MODE_SRC);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
    OH_Drawing_PenSetJoin(pen,LINE_ROUND_JOIN);
    float dashPattern[] = {10.0f, 10.0f};
    float dashPhase = 0.0f;
    OH_Drawing_PathEffect *effect = OH_Drawing_CreateDashPathEffect(dashPattern, sizeof(dashPattern)/sizeof(float), dashPhase);
    OH_Drawing_PenSetPathEffect(pen, effect);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFF0000FF);
    OH_Drawing_PenSetShaderEffect(pen,shaderEffect);
    auto penStyleMask = OH_Drawing_MaskFilterCreateBlur(NORMAL,2.0,true);
    auto penStyleFilter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetMaskFilter(penStyleFilter,penStyleMask);
    OH_Drawing_PenSetFilter(pen,penStyleFilter);

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetAlpha(brush, 20);
    OH_Drawing_BrushSetBlendMode(brush, BLEND_MODE_SRC);
    OH_Drawing_BrushSetColor(brush,DRAW_COLORRED);
    auto styleMask = OH_Drawing_MaskFilterCreateBlur(NORMAL,2.0,true);
    auto styleFilter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetMaskFilter(styleFilter,styleMask);
    OH_Drawing_BrushSetFilter(brush,styleFilter);
    if(drawingType == DrawingTypeAttachBoth){
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    }else if (drawingType == DrawingTypeAttachPen){
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_CanvasAttachPen(canvas, pen);
    }else if (drawingType == DrawingTypeAttachBrush) {
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
    }

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int j = 0; j < 50; j++) {
        for(int i = 0; i < 50; i++){
            OH_Drawing_PathMoveTo(path, (i+1)*20, 5+j*10);
            OH_Drawing_PathArcTo(path, i*20, j*10, (i+1)*20, (j+1)*10, 0, 180);
            OH_Drawing_PathQuadTo(path,10+i*20, j*10, (i+1)*20, 5+j*10);    
        }
        OH_Drawing_CanvasDrawPath(canvas, path);
    }
    for(int j = 0; j < testCount; j++){
        OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, true);
    }

    OH_Drawing_PathDestroy(path);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);   
}
void CanvasClipPathLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathLineOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasClipPathLinePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathLineOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasClipPathLineBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathLineOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}

void CanvasClipPathCurve::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathCurveOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasClipPathCurvePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathCurveOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasClipPathCurveBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasClipPathCurveOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}