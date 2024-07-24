//
// Created on 2024/7/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "canvas_draw_image_rect.h"
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
#include "drawing_type.h"

OH_Drawing_Image *precondItion()
{
    // 初始化位图对象
    uint32_t w = 64;
    uint32_t h = 64;
    OH_Drawing_Bitmap *bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { OH_Drawing_ColorFormat::COLOR_FORMAT_RGBA_8888,
                                       OH_Drawing_AlphaFormat::ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    OH_Drawing_Canvas *bitcanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitcanvas, bm);
    OH_Drawing_CanvasClear(bitcanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    // 创建一个画刷brush对象设置抗锯齿属性
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    // 着色器
    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0.f, 0.f);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(64.f, 64.f);
    const uint32_t Color[] = {
        0xFFFFFFFF,
        0xFF0000FF,
    };
    OH_Drawing_ShaderEffect *shaderEffect =
        OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, Color, nullptr, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(bitcanvas, brush);
    // 画圆形
    OH_Drawing_Point *center = OH_Drawing_PointCreate(32.f, 32.f);
    float radius = 32;
    OH_Drawing_CanvasDrawCircle(bitcanvas, center, radius);
    OH_Drawing_Image *image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    // 释放内存
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_CanvasDetachBrush(bitcanvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_CanvasDestroy(bitcanvas);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    OH_Drawing_PointDestroy(center);
    return image;
}
void PerformanceCanvasDrawImageRectOnTestPerformance(OH_Drawing_Canvas* canvas, DrawingType drawingType, int32_t testCount)
{
    //1.pen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, DRAW_COLORGREEN);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_PenSetAlpha(pen,255);
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
    //2.brush
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_BrushSetAlpha(brush, 200); 
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
    
    OH_Drawing_SamplingOptions *option = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST,
        OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);
    auto image = precondItion();
    // 开始创建矩形对象并平移
    OH_Drawing_Rect *dst = OH_Drawing_RectCreate(0, 0, 100, 100);      // image
    OH_Drawing_Rect *dstR = OH_Drawing_RectCreate(0, 100, 100, 100);   // image显示在这个区域上
    for (size_t i = 0; i < testCount; ++i) {
        if((i+1)%50 == 0){
            OH_Drawing_CanvasTranslate(canvas, -500, 30);
        }
        OH_Drawing_CanvasDrawImageRect(canvas, image, dst, option);
        OH_Drawing_CanvasTranslate(canvas, 10, 0);
        OH_Drawing_CanvasDrawRect(canvas, dstR);
    }
    // 释放内存
    OH_Drawing_SamplingOptionsDestroy(option);
    OH_Drawing_RectDestroy(dst);
    OH_Drawing_RectDestroy(dstR);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_BrushDestroy(brush);
}

void OHCanvasDrawImageRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawImageRectOnTestPerformance(canvas, DrawingTypeAttachBoth, testCount_);
}
void OHCanvasDrawImageRectPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawImageRectOnTestPerformance(canvas, DrawingTypeAttachPen, testCount_);
}
void OHCanvasDrawImageRectBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    PerformanceCanvasDrawImageRectOnTestPerformance(canvas, DrawingTypeAttachBrush, testCount_);
}
