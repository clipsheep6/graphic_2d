#include "alpha_gradients.h"
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_shader_effect.h>
#include "common/log_common.h"

enum {
    kW = 640,
    kH = 480,
};

AlphaGradients::AlphaGradients() {
    // skia dm file gm/alphagradients.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "alphagradients";
}

void AlphaGradients::draw_grad(OH_Drawing_Canvas *canvas, DrawRect &r, uint32_t c0, uint32_t c1, bool doPreMul) {
    if (doPreMul)
        c1 = 0xFFFFFFFF;
    uint32_t colors[] = {c0, c1};

    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();

    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(r.fLeft, r.fTop);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(r.fRight, r.fBottom);

    static const float_t gPos[] = {0.0f, 1.0f};
    OH_Drawing_ShaderEffect *shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, gPos, 2, OH_Drawing_TileMode::CLAMP);

    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(r.fLeft, r.fTop, r.fRight, r.fBottom);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_BrushSetShaderEffect(brush, nullptr);

    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_RectDestroy(rect);
}

void AlphaGradients::OnTestFunction(OH_Drawing_Canvas *canvas) {
    struct {
        uint32_t fColor0;
        uint32_t fColor1;
    } gRec[] = {
        {0xFFFFFFFF, 0x00000000},
        {0xFFFFFFFF, 0x00FF0000},
        {0xFFFFFFFF, 0x00FFFF00},
        {0xFFFFFFFF, 0x00FFFFFF},
        {0xFFFF0000, 0x00000000},
        {0xFFFF0000, 0x00FF0000},
        {0xFFFF0000, 0x00FFFF00},
        {0xFFFF0000, 0x00FFFFFF},
        {0xFF0000FF, 0x00000000},
        {0xFF0000FF, 0x00FF0000},
        {0xFF0000FF, 0x00FFFF00},
        {0xFF0000FF, 0x00FFFFFF},
    };

    DrawRect r = {0, 0, 300, 30};
    OH_Drawing_CanvasTranslate(canvas, 10, 10);
    size_t gRec_size = sizeof(gRec) / sizeof(gRec[0]);

    for (int doPreMul = 0; doPreMul <= 1; ++doPreMul) {
        OH_Drawing_CanvasSave(canvas);
        for (size_t i = 0; i < gRec_size; ++i) {
            draw_grad(canvas, r, gRec[i].fColor0, gRec[i].fColor1, (doPreMul));
            OH_Drawing_CanvasTranslate(canvas, 0, r.width()+8);
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, r.width()+10, 0);
    }
}
