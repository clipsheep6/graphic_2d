#include "draw_path_transform.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_matrix.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

void DrawPathTransform::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetWidth(pen, 0.0);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    // 在画布上画path的形状
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreateScale(3,3,300,300);
    OH_Drawing_PathTransform(path, m);
    
    for(int j=0;j<100;++j)
        OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_MatrixDestroy(m);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void DrawPathTransform::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawpathtransform OH_Drawing_PathTransform  迁移基于skia HairlinePathBench.cpp->HairlinePathBench
    // skia case name : path_hairline_big_AA
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetWidth(pen, 0.0);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    // 在画布上画path的形状
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreateScale(3,3,300,300);
    OH_Drawing_PathTransform(path, m);
    
    for (int i = 0; i < testCount_; i++) {
        for(int j=0;j<100;++j)
            OH_Drawing_CanvasDrawPath(canvas, path);
    }
    OH_Drawing_MatrixDestroy(m);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

