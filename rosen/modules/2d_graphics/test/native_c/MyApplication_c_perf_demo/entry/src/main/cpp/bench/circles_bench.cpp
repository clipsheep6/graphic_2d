#include "circles_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <time.h>
#include "common/log_common.h"
//#include "common/util.h"
#include "test_common.h"

void CirclesBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF000000);//OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Rect* rect;// = OH_Drawing_RectCreate(0, 0, 100, 100);
    TestRend rand = TestRend();
    // 在画布上画path的形状
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    float radius = rand.nextUScalar1()*3;
    float left = rand.nextUScalar1()*300;
    float top = rand.nextUScalar1()*300;
    float right = left+2*radius;
    float bottom = top+2*radius;
    rect = OH_Drawing_RectCreate(left,top,right,bottom);
    OH_Drawing_PenSetWidth(pen, rand.nextUScalar1()*5.0f);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathArcTo(path,left,top,right,bottom,0,0);
    //path.addOval 目前还没开放
//    OH_Drawing_CanvasDrawOval(canvas,rect);  OH_Drawing_Path addOval 还未启用 先不测
    OH_Drawing_PathArcTo(path,left,top,right,bottom,360,0);
//    OH_Drawing_CanvasDrawOval(canvas,rect);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void CirclesBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawpatharcto 测试 OH_Drawing_PathArcTo  迁移基于skia PathBench.cpp->CirclesBench
    // skia case name : circles_fill
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF000000);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Rect* rect;
    TestRend rand = TestRend();
    for (int i = 0; i < testCount_; i++) {
        // 在画布上画path的形状
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        float radius = rand.nextUScalar1()*3;
        float left = rand.nextUScalar1()*300;
        float top = rand.nextUScalar1()*300;
        float right = left+2*radius;
        float bottom = top+2*radius;
        rect = OH_Drawing_RectCreate(left,top,right,bottom);
        OH_Drawing_PenSetWidth(pen, rand.nextUScalar1()*5.0f);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_PathMoveTo(path, 0, 0);
        OH_Drawing_PathArcTo(path,left,top,right,bottom,0,0);
        //path.addOval 目前还没开放
//        OH_Drawing_CanvasDrawOval(canvas,rect); OH_Drawing_Path addOval 还未启用 先不测
        OH_Drawing_PathArcTo(path,left,top,right,bottom,360,0);
//        OH_Drawing_CanvasDrawOval(canvas,rect);
        OH_Drawing_PathClose(path);
        OH_Drawing_CanvasDrawPath(canvas, path);
//        DRAWING_LOGI("arcto rand value = %{public}f\n",rand.nextUScalar1());
        OH_Drawing_PathDestroy(path);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

