#include "draw_path.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>

void DrawPath::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 创建一个path对象
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathLineTo(path, 300, 300);
    
    // 在画布上画path的形状
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

void DrawPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 创建一个path对象
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathLineTo(path, 300, 300);

    // 在画布上画path的形状
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPath(canvas, path);
    }

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

