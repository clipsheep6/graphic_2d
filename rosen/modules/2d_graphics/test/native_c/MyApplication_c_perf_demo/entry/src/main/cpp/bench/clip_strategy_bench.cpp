#include "clip_strategy_bench.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_point.h>
//#include <native_drawing/drawing_sampling_options.h>
#include "common/log_common.h"


void ClipStrategyBench::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetBlendMode(brush, BLEND_MODE_SRC_IN);

    OH_Drawing_CanvasSaveLayer(canvas, nullptr, nullptr);
    int q = bitmapWidth_ / (count_ + 1);
    for (int i = 0; i < count_; i++) {
        int x = q * i;
        OH_Drawing_Point *point = OH_Drawing_PointCreate(x, x);
        OH_Drawing_CanvasDrawCircle(canvas, point, q/2);
        OH_Drawing_PointDestroy(point);
    }
    OH_Drawing_CanvasSaveLayer(canvas, nullptr, brush);
    OH_Drawing_CanvasClear(canvas, 0xFF00FF00);
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasRestore(canvas);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);

    DRAWING_LOGE("DrawingApiTest Started:DrawRect");
}

void ClipStrategyBench::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // clip_strategy_mask_5
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetBlendMode(brush, BLEND_MODE_SRC_IN);
    
    int q = bitmapWidth_ / (count_ + 1);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSaveLayer(canvas, nullptr, nullptr);
        for (int j = 0; j < count_; j++) {
            int x = q * j;
            OH_Drawing_Point *point = OH_Drawing_PointCreate(x, x);
            OH_Drawing_CanvasDrawCircle(canvas, point, q / 2);
            OH_Drawing_PointDestroy(point);
        }
        i = i + count_ - 1;
        OH_Drawing_CanvasSaveLayer(canvas, nullptr, brush);
        OH_Drawing_CanvasClear(canvas, 0xFF00FF00);
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasRestore(canvas);
    }
    
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
}
