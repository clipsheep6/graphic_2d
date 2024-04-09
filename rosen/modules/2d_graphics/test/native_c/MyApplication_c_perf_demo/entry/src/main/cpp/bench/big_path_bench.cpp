#include "big_path_bench.h"
#include "big_path.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

//struct DrawRect {
//    float fLeft;   //!< smaller x-axis bounds
//    float fTop;    //!< smaller y-axis bounds
//    float fRight;  //!< larger x-axis bounds
//    float fBottom; //!< larger y-axis bounds
//    bool contains(float x, float y) const { return x >= fLeft && x < fRight && y >= fTop && y < fBottom; }
//};

BigPathBench::BigPathBench(Align align,bool round) : fAlign(align),fRound(round)
{
    fPath = OH_Drawing_PathCreate();
    BenchUtils::make_big_path(fPath);
    
}

BigPathBench::~BigPathBench()
{
    OH_Drawing_PathDestroy(fPath);
}

void BigPathBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_PenSetWidth(pen, 2);
    if(fRound)
        OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    DrawRect r = {-1113.0, 0, 1511.0, 92.0};//OH没有实现path->GetBounds()接口，先用bigpath中手动计算出

    switch (fAlign) {
        case kLeft_Align:
            OH_Drawing_CanvasTranslate(canvas, -r.fLeft, 0);
            break;
        case kMiddle_Align:
            break;
        case kRight_Align:
            OH_Drawing_CanvasTranslate(canvas, 640 - r.fRight, 0);
            break;
    }
    
    OH_Drawing_CanvasDrawPath(canvas, fPath);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void BigPathBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawbigpath 测试 OH_Drawing_CanvasTranslate 迁移基于skia BigPathBench.cpp->BigPathBench
    // skia case name : bigpath_left_round
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_PenSetWidth(pen, 2);
    if(fRound)
        OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    DrawRect r = {-1113.0, 0, 1511.0, 92.0};//OH没有实现path->GetBounds()接口，先用bigpath中手动计算出

    switch (fAlign) {
        case kLeft_Align:
            OH_Drawing_CanvasTranslate(canvas, -r.fLeft, 0);
            break;
        case kMiddle_Align:
            break;
        case kRight_Align:
            OH_Drawing_CanvasTranslate(canvas, 640 - r.fRight, 0);
            break;
    }
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPath(canvas, fPath);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

