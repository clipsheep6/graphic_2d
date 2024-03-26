#include "testcase/path_create_bench.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_point.h>
#include "testcase/random_path_bench.h"

void PathCreateBench::OnTestFunction(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 创建数据
    createData(10, 100, true, nullptr);
    // 生成路径
    makePath(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathReset(path);
    // 释放内存
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void PathCreateBench::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // path_create
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 创建数据
    createData(10, 100, true, nullptr);
    // 生成路径
    makePath(path);
    // OH_Drawing_CanvasDrawPath(canvas, path);
    for(int i = 0; i< testCount_; i++) {
        if(i % 10 == 0) {
            OH_Drawing_PathReset(path);
        }
        OH_Drawing_CanvasDrawPath(canvas, path);
    }
    restartMakingPaths();
    // 释放内存
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}