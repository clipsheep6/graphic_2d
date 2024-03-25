#include "rect_bench_for_pen.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <time.h>
#include "common/log_common.h"


void RectBenchForPen::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();

    if(fStroke > 0)
    {
        OH_Drawing_PenSetWidth(pen, fStroke);
    }
    if (fPerspective) 
    {
        OH_Drawing_Matrix* perspective = OH_Drawing_MatrixCreate();
        OH_Drawing_MatrixSetMatrix(perspective, 1,0.1f,0, 0,1,0, 1e-4f,1e-3f,1);
        OH_Drawing_CanvasConcatMatrix(canvas, perspective);
    }
    
    OH_Drawing_PenSetColor(pen, colors[0]);
    OH_Drawing_PenSetAntiAlias(pen, fAA);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    onDrawPen(canvas, pen,0);
    onCleanUp();
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void RectBenchForPen::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawpenrect 测试 OH_Drawing_PenSetAntiAlias  迁移基于skia RectBench.cpp->RectBench
    // skia case name : rects_1_stroke_4_aa_persp
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    
    if(fStroke > 0)
    {
        OH_Drawing_PenSetWidth(pen, fStroke);
    }
    if (fPerspective) 
    {
        // Apply some fixed perspective to change how ops may draw the rects
        OH_Drawing_Matrix* perspective = OH_Drawing_MatrixCreate();
// 初始化为正交矩阵，并对对应项进行变换        
//        OH_Drawing_MatrixSetMatrix(perspective, 1,0,0, 0,1,0, 0,0,1);
//    SkMatrix& setAll(SkScalar scaleX, SkScalar skewX,  SkScalar transX,
//                     SkScalar skewY,  SkScalar scaleY, SkScalar transY,
//                     SkScalar persp0, SkScalar persp1, SkScalar persp2)  
        OH_Drawing_MatrixSetMatrix(perspective, 1,0.1f,0, 0,1,0, 1e-4f,1e-3f,1);
        OH_Drawing_CanvasConcatMatrix(canvas, perspective);
    }
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PenSetColor(pen, colors[i % RAND_SIZE]);
        OH_Drawing_PenSetAntiAlias(pen, fAA);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        onDrawPen(canvas, pen,i % RAND_SIZE);
    }
    onCleanUp();
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void SrcModeRectBench::onDrawPen(OH_Drawing_Canvas* canvas,OH_Drawing_Pen* pen, uint32_t index) {
    //当前用例名 drawpenrectsrcmode 测试 OH_Drawing_PenSetAlpha  迁移基于skia RectBench.cpp->SrcModeRectBench
    // skia case name : srcmode_rects_1_stroke_0_aa
    OH_Drawing_PenSetAlpha(pen, 0x80);
    OH_Drawing_PenSetBlendMode(pen, OH_Drawing_BlendMode::BLEND_MODE_SRC);    
    OH_Drawing_CanvasAttachPen(canvas, pen);    
    OH_Drawing_CanvasDrawRect(canvas, rects[index]);
}
