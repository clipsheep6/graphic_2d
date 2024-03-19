#include "draw_shader_effect.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

struct GradData {
    int             fCount;
    const uint32_t*  fColors;
    const float_t* fPos;
    const char*     fName;
};

static const uint32_t gColors[] = {
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,  // 10 lines, 50 colors
};

static const uint32_t gShallowColors[] = { 0xFF555555, 0xFF444444 };
static const float_t gPos[] = {0.25f, 0.75f};
static const GradData gGradData[] = {
    { 2, gColors, nullptr, "" },
    { 50, gColors, nullptr, "_hicolor" }, // many color gradient
    { 3, gColors, nullptr, "_3color" },
    { 2, gShallowColors, nullptr, "_shallow" },
    { 2, gColors, gPos, "_pos" },
};

void DrawShaderEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    //当前用例名 drawshaderlinear_oval 测试 OH_Drawing_ShaderEffectCreateLinearGradient  迁移基于skia GradientBench.cpp->GradientBench
    // skia case name : gradient_linear_clamp
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_Point* endPt = OH_Drawing_PointCreate(kSize, kSize);
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(kSize*0.5, kSize*0.5);
    GradData data = gGradData[4];
    
    OH_Drawing_ShaderEffect* effect ;
    switch (effectType_) {
        case 0:
            effect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, data.fColors, data.fPos, data.fCount, OH_Drawing_TileMode::CLAMP);
            break;
        case 1:
            effect = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, kSize*0.5, data.fColors, data.fPos,data.fCount, OH_Drawing_TileMode::REPEAT);
            break;
        case 2:
            effect = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, data.fColors, data.fPos, data.fCount, OH_Drawing_TileMode::DECAL);
            break;
        default:
            effect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, data.fColors, data.fPos, data.fCount, OH_Drawing_TileMode::CLAMP);
            break;
    }
    
    if(gDataIndex_>=0 && gDataIndex_ < 5)
        data = gGradData[gDataIndex_];
    else
        data = gGradData[0];        

    OH_Drawing_PenSetShaderEffect(pen, effect);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0,0,kSize,kSize);

    if(bOval_)
        OH_Drawing_CanvasDrawOval(canvas, rect);
    else
        OH_Drawing_CanvasDrawRect(canvas, rect);
    
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void DrawShaderEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawshaderlinear_oval 测试 OH_Drawing_ShaderEffectCreateLinearGradient  迁移基于skia GradientBench.cpp->GradientBench
    // skia case name : gradient_linear_clamp
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_Point* endPt = OH_Drawing_PointCreate(kSize, kSize);
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(kSize*0.5, kSize*0.5);
    GradData data = gGradData[4];
    
    OH_Drawing_ShaderEffect* effect ;
    switch (effectType_) {
        case 0:
            effect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, data.fColors, data.fPos, data.fCount, OH_Drawing_TileMode::CLAMP);
            break;
        case 1:
            effect = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, kSize*0.5, data.fColors, data.fPos,data.fCount, OH_Drawing_TileMode::REPEAT);
            break;
        case 2:
            effect = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, data.fColors, data.fPos, data.fCount, OH_Drawing_TileMode::DECAL);
            break;
        default:
            effect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, data.fColors, data.fPos, data.fCount, OH_Drawing_TileMode::CLAMP);
            break;
    }
    
    if(gDataIndex_>=0 && gDataIndex_ < 5)
        data = gGradData[gDataIndex_];
    else
        data = gGradData[0];        

    OH_Drawing_PenSetShaderEffect(pen, effect);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0,0,kSize,kSize);
    for (int i = 0; i < testCount_; i++) {
        if(bOval_)
            OH_Drawing_CanvasDrawOval(canvas, rect);
        else
            OH_Drawing_CanvasDrawRect(canvas, rect);
    }

    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

