#include "draw_line.h"
#include "test_common.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_path_effect.h>

void DrawLine::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 2);
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
    float vals[2] = {1, 1};
    OH_Drawing_PathEffect *pathEffect = OH_Drawing_CreateDashPathEffect(vals, 2, 0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    // 在画布上画line
    OH_Drawing_CanvasDrawLine(canvas, 10, 10, 640, 10);

    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

void DrawLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // dashline_2_circle
    // 创建一个画刷pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 2);
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
    float vals[2] = {1, 1};
    OH_Drawing_PathEffect *pathEffect = OH_Drawing_CreateDashPathEffect(vals, 2, 0);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 在画布上画line
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawLine(canvas, 10, 10, 640, 10);
    }

    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

