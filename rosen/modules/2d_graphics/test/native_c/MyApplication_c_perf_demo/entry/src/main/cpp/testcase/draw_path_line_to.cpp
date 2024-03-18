#include "draw_path_line_to.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>



void DrawPathLineTo::OnTestFunction(OH_Drawing_Canvas *canvas) {
    // 创建一个画刷pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 创建一个path对象，然后使用接口连接成一个三角形形状
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    // 指定path的起始位置
    OH_Drawing_PathMoveTo(path, 100, 100);
    OH_Drawing_PathLineTo(path, 150, 50);
    OH_Drawing_PathLineTo(path, 200, 200);
    OH_Drawing_PathClose(path);

    // 在画布上画path的形状
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void DrawPathLineTo::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // path_fill_small_triangle
    // 创建一个画刷pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 创建一个path对象，然后使用接口连接成一个三角形形状
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    // 指定path的起始位置
    OH_Drawing_PathMoveTo(path, 100, 100);
    OH_Drawing_PathLineTo(path, 150, 50);
    OH_Drawing_PathLineTo(path, 200, 200);
    OH_Drawing_PathClose(path);

    for (int i = 0; i < testCount_; i++) {
        // 在画布上画path的形状
        OH_Drawing_CanvasDrawPath(canvas, path);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}
