#include "bezier_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

void BezierBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_Path* pathQuad = OH_Drawing_PathCreate();
    OH_Drawing_Path* pathCubic = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(pathQuad, 20, 20);
    OH_Drawing_PathQuadTo(pathQuad, 60, 20, 60, 60);
    OH_Drawing_PathQuadTo(pathQuad, 20, 60, 20, 100);
    OH_Drawing_PathMoveTo(pathCubic, 20, 20);
    OH_Drawing_PathCubicTo(pathCubic, 40, 20, 60, 40, 60, 60);
    OH_Drawing_PathCubicTo(pathCubic, 40, 60, 20, 80, 20, 100); 
    OH_Drawing_PenLineCapStyle cap;
    OH_Drawing_PenLineJoinStyle join;
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_PenSetCap(pen, (capType));
    OH_Drawing_PenSetJoin(pen, (joinType));
    OH_Drawing_PenSetWidth(pen, fWidth);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    if(drawType == DrawType::draw_quad){
        OH_Drawing_CanvasDrawPath(canvas, pathQuad);
    }
    else{
        OH_Drawing_CanvasDrawPath(canvas, pathCubic);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void BezierBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawenbeizer 测试 OH_Drawing_PenSetWidth OH_Drawing_PenSetCap OH_Drawing_PenSetJoin 迁移基于skia BezierBench.cpp->BezierBench
    // skia case name : draw_stroke_bezier_cubic_butt_miter_2
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_Path* pathQuad = OH_Drawing_PathCreate();
    OH_Drawing_Path* pathCubic = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(pathQuad, 20, 20);
    OH_Drawing_PathQuadTo(pathQuad, 60, 20, 60, 60);
    OH_Drawing_PathQuadTo(pathQuad, 20, 60, 20, 100);
    OH_Drawing_PathMoveTo(pathCubic, 20, 20);
    OH_Drawing_PathCubicTo(pathCubic, 40, 20, 60, 40, 60, 60);
    OH_Drawing_PathCubicTo(pathCubic, 40, 60, 20, 80, 20, 100);
    OH_Drawing_PenLineCapStyle cap;
    OH_Drawing_PenLineJoinStyle join;
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_PenSetCap(pen, (capType));
    OH_Drawing_PenSetJoin(pen, (joinType));
    OH_Drawing_PenSetWidth(pen, fWidth);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    if(drawType == DrawType::draw_quad){
        for (int i = 0; i < testCount_; i++) {
            OH_Drawing_CanvasDrawPath(canvas, pathQuad);
        }    
    }
    else{
        for (int i = 0; i < testCount_; i++) {
            OH_Drawing_CanvasDrawPath(canvas, pathCubic);
        } 
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

