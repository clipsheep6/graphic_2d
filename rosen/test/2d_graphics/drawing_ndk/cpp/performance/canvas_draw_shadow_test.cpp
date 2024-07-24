//
// Created on 2024/7/18.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "canvas_draw_shadow_test.h"
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
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_text_typography.h>
#include <native_drawing/drawing_mask_filter.h>
#include "test_common.h"
#include "common/drawing_type.h"
#include "performance.h"
void PerformanceCanvasDrawShadowLineOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
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
    //4.OH_Drawing_CanvasDrawShadow
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
    for(int j = 1;j<=testCount;j++){       
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, 0, 2*(j-1));
        for (int i = 0; i <= 1000; i++) {
            int x = rand.nextUScalar1() * 2;
            OH_Drawing_PathLineTo(path, i*2+x, (i%2)*2 + 2*(j-1));//i*5
    }
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_CanvasDrawShadow(
            canvas, path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);        
        OH_Drawing_PathDestroy(path);
    }
    //5.释放资源
    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);    
}
void PerformanceCanvasDrawShadowCurveOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
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
    
    //4.OH_Drawing_CanvasDrawShadow
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
        for(int i = 0; i < 100; i++){
            OH_Drawing_PathMoveTo(path, (i+1)*4, 1+j*2);
            OH_Drawing_PathArcTo(path, i*4, j*2, (i+1)*4, (j+1)*2, 0, 180);
            OH_Drawing_PathQuadTo(path,2+i*4, j*2, (i+1)*4, 1+j*2);          
        }
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_CanvasDrawShadow(
            canvas, path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);  
        OH_Drawing_PathDestroy(path);
    }
    
    //5.释放资源
    DESTROY_EFFECT;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);    
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);      
}

void CanvasDrawShadowLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawShadowLineOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawShadowLinePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawShadowLineOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasDrawShadowLineBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawShadowLineOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}

void CanvasDrawShadowCurve::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawShadowCurveOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawShadowCurvePen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawShadowCurveOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasDrawShadowCurveBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawShadowCurveOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}