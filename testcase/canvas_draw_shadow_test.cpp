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
#include "drawing_type.h"

void PerformanceCanvasDrawShadowLineOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    //1.pen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFFFFFF);
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetAlpha(pen,50);
    //OH_Drawing_PenSetBlendMode(pen,BLEND_MODE_SRC);
    OH_Drawing_PenSetAntiAlias(pen,true);     
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);//LINE_SQUARE_CAP
    OH_Drawing_PenSetJoin(pen,LINE_ROUND_JOIN);
    float dashPattern[] = {10.0f, 10.0f}; // 实线段长度为10像素，空白段长度为5像素
    float dashPhase = 0.0f; // 起始偏移量，这里是无偏移
    OH_Drawing_PathEffect *effect = OH_Drawing_CreateDashPathEffect(dashPattern, sizeof(dashPattern)/sizeof(float), dashPhase);
    OH_Drawing_PenSetPathEffect(pen, effect);
//     OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFF0000FF);
//     OH_Drawing_PenSetShaderEffect(pen,shaderEffect);
    auto penStyleMask = OH_Drawing_MaskFilterCreateBlur(NORMAL,2.0,true);//创建具有模糊效果的模板滤波器 有效果
    auto penStyleFilter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetMaskFilter(penStyleFilter,penStyleMask);
    OH_Drawing_PenSetFilter(pen,penStyleFilter);
    //2.设置画刷
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);//设置画刷的抗锯齿为抗锯齿
    OH_Drawing_BrushSetAlpha(brush, 20);//画刷设置透明度值 
    //OH_Drawing_BrushSetBlendMode(brush, BLEND_MODE_SRC);
    OH_Drawing_BrushSetColor(brush,DRAW_COLORRED);
    auto styleMask = OH_Drawing_MaskFilterCreateBlur(NORMAL,2.0,true);//创建具有模糊效果的模板滤波器 有效果
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
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);    
}
void PerformanceCanvasDrawShadowCurveOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    //1.pen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFFFFFF);
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetAlpha(pen,50);
    //OH_Drawing_PenSetBlendMode(pen,BLEND_MODE_SRC);
    OH_Drawing_PenSetAntiAlias(pen,true);     
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);//LINE_SQUARE_CAP
    OH_Drawing_PenSetJoin(pen,LINE_ROUND_JOIN);
    float dashPattern[] = {10.0f, 10.0f}; // 实线段长度为10像素，空白段长度为5像素
    float dashPhase = 0.0f; // 起始偏移量，这里是无偏移
    OH_Drawing_PathEffect *effect = OH_Drawing_CreateDashPathEffect(dashPattern, sizeof(dashPattern)/sizeof(float), dashPhase);
    OH_Drawing_PenSetPathEffect(pen, effect);
//     OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFF0000FF);
//     OH_Drawing_PenSetShaderEffect(pen,shaderEffect);
    auto penStyleMask = OH_Drawing_MaskFilterCreateBlur(NORMAL,2.0,true);//创建具有模糊效果的模板滤波器 有效果
    auto penStyleFilter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetMaskFilter(penStyleFilter,penStyleMask);
    OH_Drawing_PenSetFilter(pen,penStyleFilter);

    //2.设置画刷
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);//设置画刷的抗锯齿为抗锯齿
    OH_Drawing_BrushSetAlpha(brush, 20);//画刷设置透明度值 
    //OH_Drawing_BrushSetBlendMode(brush, BLEND_MODE_SRC);
    OH_Drawing_BrushSetColor(brush,DRAW_COLORRED);
    auto styleMask = OH_Drawing_MaskFilterCreateBlur(NORMAL,2.0,true);//创建具有模糊效果的模板滤波器 有效果
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