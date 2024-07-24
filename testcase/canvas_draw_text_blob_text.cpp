//
// Created on 2024/7/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "canvas_draw_text_blob_text.h"

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

void PerformanceCanvasDrawTextBlobMaxOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    //1.pen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, DRAW_COLORGREEN);
    OH_Drawing_PenSetWidth(pen, 3);
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
    
    //2.OH_Drawing_CanvasDrawTextBlob
    std::string text = "WT";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font,500);
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
    for (int i = 0; i < testCount; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, 5, 500);
    }
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_PenDestroy(pen);
}
void PerformanceCanvasDrawTextBlobLongOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    //1.pen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, DRAW_COLORGREEN);
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
    
    //2.OH_Drawing_CanvasDrawTextBlob
    std::string text = 
    "In the heart of the bustling city stood an old bookstore, its facade weathered by time but its spirit undimmed.";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font,14);
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
    for (int i = 0; i < testCount; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, 5, i*10);
    }
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
    PerformanceCanvasDrawTextBlobMaxOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void CanvasDrawTextBlobMaxPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawTextBlobMaxOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void CanvasDrawTextBlobMaxBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawTextBlobMaxOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}