#include "canvas_matrix_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

void CanvasMatrixBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF000000);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m, 1,0,0, 0,1,0, 0,0,1);
    OH_Drawing_MatrixRotate(m, 10.0, 300., 300.);
    OH_Drawing_CanvasSave(canvas);
    switch (cType) {
        case Translate:{
            OH_Drawing_CanvasTranslate(canvas, 0.0001, 0.0001);
        }break;
        case Scale:{
            OH_Drawing_CanvasScale(canvas, 1.0001, 0.9999);
        }break;
        case ConcatMatrix:{
            OH_Drawing_CanvasConcatMatrix(canvas, m);
        }break;                
    }
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void CanvasMatrixBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawcanvasscale 测试 OH_Drawing_CanvasScale  迁移基于skia GameBench.cpp->CanvasMatrixBench
    // skia case name : canvas_matrix
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF000000);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m, 1,0,0, 0,1,0, 0,0,1);
    OH_Drawing_MatrixRotate(m, 10.0, 300., 300.);
    
    for (int i = 0; i < testCount_;) {
        OH_Drawing_CanvasSave(canvas);
        for (int j = 0; j < 10000; j++) {
            switch (cType) {
                case Translate: {
                    OH_Drawing_CanvasTranslate(canvas, 0.0001, 0.0001);
                    break;
                }
                case Scale: {
                    OH_Drawing_CanvasScale(canvas, 1.0001, 0.9999);
                    break;
                }
                case ConcatMatrix: {
                    OH_Drawing_CanvasConcatMatrix(canvas, m);
                    break;
                }
            }
        }
        OH_Drawing_CanvasRestore(canvas);
        i += 1000;
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

