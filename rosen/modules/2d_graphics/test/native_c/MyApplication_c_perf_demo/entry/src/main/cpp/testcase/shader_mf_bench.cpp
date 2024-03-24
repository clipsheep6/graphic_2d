#include "shader_mf_bench.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
//#include <native_drawing/drawing_sampling_options.h>
#include "common/log_common.h"

void ShaderMFBench::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    uint32_t color = 0x8000ff00;
    uint32_t colorBlack = 0xFF000000;
    float pos = 100;
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Point *centerPoint = OH_Drawing_PointCreate(50, 50);
    OH_Drawing_ShaderEffect *shaderEffect = 
        OH_Drawing_ShaderEffectCreateRadialGradient(centerPoint, 50 , &colorBlack, &pos, 100, REPEAT);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    OH_Drawing_CanvasSaveLayer(canvas, nullptr, brush);
    OH_Drawing_CanvasClear(canvas, color);
    OH_Drawing_CanvasRestore(canvas);

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PointDestroy(centerPoint);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
}

void ShaderMFBench::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // shadermaskfilter_bitmap_80
    uint32_t color = 0x8000ff00;
    uint32_t colorBlack = 0xFF000000;
    float pos = 100;
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Point *centerPoint = OH_Drawing_PointCreate(50, 50);
    OH_Drawing_ShaderEffect *shaderEffect =
        OH_Drawing_ShaderEffectCreateRadialGradient(centerPoint, 50, &colorBlack, &pos, 100, REPEAT);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSaveLayer(canvas, nullptr, brush);
        OH_Drawing_CanvasClear(canvas, color);
        OH_Drawing_CanvasRestore(canvas);
    }

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PointDestroy(centerPoint);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
}
