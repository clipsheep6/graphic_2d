#include "clear_bench.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_brush.h>

void ClearBench::OnTestFunction(OH_Drawing_Canvas *canvas) {
    // 画笔
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 2);
    OH_Drawing_PenSetAntiAlias(pen, true);

    // 创建矩形、圆角矩形
    OH_Drawing_Rect *kPartialClip = OH_Drawing_RectCreate(float(50), float(50), float(400), float(400));
    OH_Drawing_RoundRect *kComplexClip = OH_Drawing_RoundRectCreate(kPartialClip, float(15), float(15));
    OH_Drawing_Rect *kInterruptRect = OH_Drawing_RectCreate(float(200), float(200), float(203), float(203));

    // 着色器
    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(10, 10);
    const uint32_t Color[] = {
        0xFF000000,
        0xFF00FF00,
    };
    OH_Drawing_ShaderEffect *shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, Color, nullptr, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 画矩形、圆角矩形
    OH_Drawing_CanvasDrawRect(canvas, kPartialClip);
    OH_Drawing_CanvasDrawRoundRect(canvas, kComplexClip);
    OH_Drawing_CanvasDrawRect(canvas, kInterruptRect);

    OH_Drawing_Brush *Brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasSaveLayer(canvas, kPartialClip, Brush);

    OH_Drawing_CanvasSave(canvas);

    // 裁剪矩形
    switch (fType) {
    case kPartial_ClearType:
        OH_Drawing_CanvasClipRect(canvas, kPartialClip, INTERSECT, true);
        break;
    case kComplex_ClearType:
        break;
    case kFull_ClearType:
        break;
    }

    // Blue Color
    OH_Drawing_CanvasClear(canvas, 0xFF0000FF);
    OH_Drawing_CanvasRestore(canvas);

    OH_Drawing_RectCreate(float(200), float(200), float(203), float(203));

    OH_Drawing_RectDestroy(kPartialClip);
    OH_Drawing_RoundRectDestroy(kComplexClip);
    OH_Drawing_RectDestroy(kInterruptRect);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void ClearBench::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // Clear-Full
    // 画笔
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 2);
    OH_Drawing_PenSetAntiAlias(pen, true);

    // 创建矩形、圆角矩形
    OH_Drawing_Rect *kPartialClip = OH_Drawing_RectCreate(float(50), float(50), float(400), float(400));
    OH_Drawing_RoundRect *kComplexClip = OH_Drawing_RoundRectCreate(kPartialClip, float(15), float(15));
    OH_Drawing_Rect *kInterruptRect = OH_Drawing_RectCreate(float(200), float(200), float(203), float(203));

    // 着色器
    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(10, 10);
    const uint32_t Color[] = {
        0xFF000000,
        0xFF00FF00,
    };
    OH_Drawing_ShaderEffect *shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, Color, nullptr, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 画矩形、圆角矩形
    OH_Drawing_CanvasDrawRect(canvas, kPartialClip);
    OH_Drawing_CanvasDrawRoundRect(canvas, kComplexClip);
    OH_Drawing_CanvasDrawRect(canvas, kInterruptRect);

    OH_Drawing_Brush *Brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasSaveLayer(canvas, kPartialClip, Brush);

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSave(canvas);

        // 裁剪矩形
        switch (fType) {
        case kPartial_ClearType:
            OH_Drawing_CanvasClipRect(canvas, kPartialClip, INTERSECT, true);
            break;
        case kComplex_ClearType:
            break;
        case kFull_ClearType:
            break;
        }

        // Blue Color
        OH_Drawing_CanvasClear(canvas, 0xFF0000FF);
        OH_Drawing_CanvasRestore(canvas);

        OH_Drawing_RectCreate(float(200), float(200), float(203), float(203));
    }

    OH_Drawing_RectDestroy(kPartialClip);
    OH_Drawing_RoundRectDestroy(kComplexClip);
    OH_Drawing_RectDestroy(kInterruptRect);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}
