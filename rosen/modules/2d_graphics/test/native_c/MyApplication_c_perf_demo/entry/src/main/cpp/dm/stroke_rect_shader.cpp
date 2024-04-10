//
// Created on 2024/4/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "stroke_rect_shader.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include "bench/hardstopgradientbench_scalenumcolors.h"
#include "test_common.h"
#include "common/log_common.h"


// Draw stroked rects (both AA and nonAA) with all the types of joins:
//    bevel, miter, miter-limited-to-bevel, round
// and as a hairline.
void StrokeRectShader::OnTestFunction(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Rect* kRect = OH_Drawing_RectCreate(0, 0, 100, 100);
    DrawRect Rect = {0, 0, 100, 100}; 
    OH_Drawing_Point* startPts = OH_Drawing_PointCreate(Rect.fLeft, Rect.fTop);
    OH_Drawing_Point* endPts = OH_Drawing_PointCreate(Rect.fRight, Rect.fBottom);
    const uint32_t kColors[] = {
        0xFFFF0000,
        0xFF0000FF,
    };
    constexpr float pos[] = {0.2f, 1.0f};
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_ShaderEffect *shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPts, endPts, kColors, pos, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);
    // Do a large initial translate so that local coords disagree with device coords significantly
    // for the first rect drawn.
    OH_Drawing_CanvasTranslate(canvas, 0.5 * (Rect.fLeft + Rect.fRight), 0.5 * (Rect.fTop + Rect.fBottom));
    float kPad = 20;
    for (auto aa : {false, true}) {
        OH_Drawing_PenSetAntiAlias(pen, aa);
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_PenSetWidth(pen, 10);
        OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);
        OH_Drawing_CanvasDrawRect(canvas, kRect);
        OH_Drawing_CanvasTranslate(canvas, Rect.width() + kPad, 0);
        
        OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
        OH_Drawing_CanvasDrawRect(canvas, kRect);
        OH_Drawing_CanvasTranslate(canvas, Rect.width() + kPad, 0);
        
        // This miter limit should effectively produce a bevel join.
        OH_Drawing_PenSetMiterLimit(pen, 0.01f);
        OH_Drawing_CanvasDrawRect(canvas, kRect);
        OH_Drawing_CanvasTranslate(canvas, Rect.width() + kPad, 0);
        
        OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
        OH_Drawing_CanvasDrawRect(canvas, kRect);
        OH_Drawing_CanvasTranslate(canvas, Rect.width() + kPad, 0);
        
        OH_Drawing_PenSetWidth(pen, 0);
        OH_Drawing_CanvasDrawRect(canvas, kRect);

        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, 0, Rect.height() + kPad);
    }

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PenDestroy(pen);
}