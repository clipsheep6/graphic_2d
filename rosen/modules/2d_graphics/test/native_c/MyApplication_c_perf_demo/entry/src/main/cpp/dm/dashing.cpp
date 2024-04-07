#include "dashing.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_path_effect.h>
#include "test_common.h"
#include "common/log_common.h"

enum{
    kW=640,
    kH=340,
};


static void drawline(OH_Drawing_Canvas* canvas, int on, int off, OH_Drawing_Pen* pen,
                     float finalX = (600), float finalY = (0),
                     float phase = (0),
                     float startX = (0), float startY = (0)) {

    float intervals[] = {
        float(on),
        float(off),
    };
    OH_Drawing_PathEffect* effect= OH_Drawing_CreateDashPathEffect(intervals,2,phase);
    OH_Drawing_PenSetPathEffect(pen, effect);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawLine(canvas, startX, startY, finalX, finalY);

}

static void show_zero_len_dash(OH_Drawing_Canvas* canvas) {
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    drawline(canvas, 2, 2, pen, (0));
    OH_Drawing_PenSetWidth(pen, 2);
    OH_Drawing_CanvasTranslate(canvas, 0, 20);
    drawline(canvas, 4, 4, pen, (0));
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

static void show_giant_dash(OH_Drawing_Canvas* canvas) {
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    drawline(canvas, 1, 1, pen, (20 * 1000));
    OH_Drawing_PenDestroy(pen);
}

Dashing::Dashing() {
    //skia dm file gm/fontregen.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "dashing";
}

void Dashing::OnTestFunction(OH_Drawing_Canvas* canvas)
{
// DRAWING_LOGI("OnTestFunction path = %{public}s",fileName_);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasTranslate(canvas, 20, 20);
    OH_Drawing_CanvasTranslate(canvas, 0, 0.5);
    struct Intervals {
        int fOnInterval;
        int fOffInterval;
    };
    for (int width = 0; width <= 2; ++width) {
        for (const Intervals& data : {Intervals{1, 1},Intervals{4, 1}}){
            for(bool aa:{false,true}){
                int w = width * width * width;
                OH_Drawing_PenSetAntiAlias(pen, aa);
                OH_Drawing_PenSetWidth(pen, w);
                int scale = w ? w : 1;
                drawline(canvas, data.fOnInterval * scale, data.fOffInterval * scale,pen);
                OH_Drawing_CanvasTranslate(canvas, 0, 20);
            }
        }
    }
    show_giant_dash(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, 20);
    show_zero_len_dash(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, 20);
    OH_Drawing_PenSetWidth(pen, 8);
    drawline(canvas, 0, 0, pen);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}
